#include "addbooking.h"
#include "ui_addbooking.h"
#include <QMessageBox>
#include <QSet>
#include <QDebug>

// 对齐到整点/半点
static inline QTime ceilToHalfHour(const QTime& t) {
    int m = t.minute();
    if (m == 0 || m == 30) return QTime(t.hour(), m, 0);
    if (m < 30) return QTime(t.hour(), 30, 0);
    int h = t.hour() + 1;
    return QTime(h, 0, 0);
}

//已知开始返回结束
static inline QTime slotEnd(const QTime& start) { return start.addSecs(kSlotSecs); }

//时间重合判断
static inline bool overlap30min(const QTime& a, const QTime& b) {
    QTime aEnd = slotEnd(a), bEnd = slotEnd(b);
    return (a < bEnd) && (b < aEnd);
}



//从指定时间开始找
static inline QTime findFirstFreeSlot(const Doctor &doctor, const QDate &day, const QTime &from)
{
    QTime workS, workE;
    doctor.workTimeFor(day, workS, workE);
    QTime probe = ceilToHalfHour(from);
    if (probe < workS) probe = workS;

    const QTime latestStart = workE.addSecs(-kSlotSecs);
    while (probe <= latestStart)
    {
        bool taken = false;
        for (const Patient &p : doctor.d_patient)
        {
            if (p.p_booking_date != day)
                continue;
            if (overlap30min(probe, ceilToHalfHour(p.p_booking_time)))
            {
                taken = true;
                break;
            }
        }
        if (!taken)
            return probe;
        probe = probe.addSecs(kSlotSecs);
    }
    return {}; // 无空档
}

AddBooking::AddBooking(QWidget *parent,DoctorInfortable *docTable,const QVector<SuspendItem> *suspends)
    : QDialog(parent),ui(new Ui::AddBooking),doctorTable(docTable),m_suspends(suspends)
{
    ui->setupUi(this);
    ui->dateTimeEdit_time->setDisplayFormat("yyyy-MM-dd HH:mm");
    ui->dateTimeEdit_time->setCalendarPopup(true);
    ui->dateTimeEdit_time->setReadOnly(false);
    ui->dateTimeEdit_time->setKeyboardTracking(false);//关闭跟踪
    ui->dateTimeEdit_time->setFocusPolicy(Qt::StrongFocus);

    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setSizeGripEnabled(true);
    resetForm();

    connect(ui->dateTimeEdit_time, &QDateTimeEdit::editingFinished,this, &AddBooking::on_timeEdit_userEditingFinished);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddBooking::on_buttonBox_accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddBooking::on_buttonBox_rejected);
    setSizeGripEnabled(true);
    setMinimumSize(400, 300);

}

AddBooking::~AddBooking() { delete ui; }


void AddBooking::preset(const Patient& p, int doctorIndex)
{
    hasPreset_  = true;//有预设数据
    presetting_ = true;// 预填期间屏蔽联动

    ui->lineEdit_name->setText(p.p_name);
    ui->lineEdit_id->setText(p.p_id);
    ui->comboBox_sex->setCurrentText(p.p_sex==1 ? "男" : "女");
    ui->spinBox_age->setValue(p.p_age);
    ui->lineEdit_phone->setText(QString::number(p.p_phone));
    ui->textEdit_symptom->setPlainText(p.p_symptom);

    initDepartmentsAndDoctors();

    QString dept;
    if (doctorIndex>=0 && doctorIndex<doctorTable->getDoctorNum())
        dept = doctorTable->getDoctor(doctorIndex).d_subject.trimmed();

    ui->comboBox_department->blockSignals(true);
    for (int i=0;i<ui->comboBox_department->count();++i)
    {
        if (ui->comboBox_department->itemText(i).trimmed() == dept)
        {
            ui->comboBox_department->setCurrentIndex(i);//找到并选中科室
            break;
        }
    }
    ui->comboBox_department->blockSignals(false);

    updateDoctorsComboBox(ui->comboBox_department->currentText());

    ui->comboBox_doctor->blockSignals(true);
    for (int i=0;i<ui->comboBox_doctor->count();++i)
    {
        bool ok=false;
        int g = ui->comboBox_doctor->itemData(i).toInt(&ok);
        if (ok && g==doctorIndex)
        {
            ui->comboBox_doctor->setCurrentIndex(i);
            break;
        }//找到了就加上这个医生
    }
    ui->comboBox_doctor->blockSignals(false);
    ui->dateTimeEdit_time->setEnabled(true);
    ui->dateTimeEdit_time->blockSignals(true);
    ui->dateTimeEdit_time->setDate(p.p_booking_date);
    ui->dateTimeEdit_time->setTime(p.p_booking_time);
    ui->dateTimeEdit_time->blockSignals(false);

    presetting_ = false;
    suppressNextHint_ = true;//防止重复提示
}

void AddBooking::on_comboBox_doctor_currentIndexChanged(int index)
{
    if (index <= 0)
    {
        ui->dateTimeEdit_time->setEnabled(false);
        return;
    }
    initAdjust_ = true;
    updateDateTimeRange();
    initAdjust_ = false;
    userEditing_ = false;           //   初始化之后第一次变更不认为是用户触发
}

void AddBooking::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    if (!hasPreset_) //如果是新建的
    {
        resetForm();
        initDepartmentsAndDoctors();
        initAdjust_ = true;
        updateDateTimeRange();
        initAdjust_ = false;
        userEditing_ = false;
    }
    else
    {
        ui->comboBox_doctor->setEnabled(true);
        ui->dateTimeEdit_time->setEnabled(true);
    }
}


//from起，找最近的有班且未停诊的时间
static bool findNearestAvailable(const Doctor &doctor,int doctorIndex,const QVector<SuspendItem> *suspends,
                                 QDate fromDate, QTime fromTime,QDate &outDate, QTime &outStart,QString &reasonText)
{
    auto isSusp = [&](const QDate &d)->bool
    {
        if (!suspends)
            return false;
        for (const auto &x : *suspends)
            if (x.doctorIndex == doctorIndex && x.date == d)
                return true;
        return false;
    };

    for (int dayOff = 0; dayOff < 60; ++dayOff)
    {
        const QDate d = fromDate.addDays(dayOff);

        QTime ws, we; doctor.workTimeFor(d, ws, we);
        if (!ws.isValid() || !we.isValid() || ws >= we)
            continue; // 当天不上班
        if (isSusp(d))
            continue; // 当天停诊

        QTime base = (dayOff == 0 ? fromTime : ws);
        if (base < ws)
            base = ws;

        const QTime start = findFirstFreeSlot(doctor, d, base);
        if (!start.isValid())
            continue; // 当天已满

        outDate  = d;
        outStart = start;

        // 和用户请求的日期/时间不同，设置 reason
        const bool changed = (d != fromDate) || (start != fromTime);
        if (!changed) { reasonText.clear(); return true; }

        const QTime lastStart = we.addSecs(-kSlotSecs);
        if (d != fromDate)
        {
            reasonText = QString("因【%1停诊或超过当日工作时间】已为你切换到 %2 %3")
                           .arg(fromDate.toString("yyyy-MM-dd"))
                           .arg(d.toString("yyyy-MM-dd"))
                           .arg(start.toString("hh:mm"));
        }
        else if (fromTime < ws)
        {
            reasonText = QString("因【未到上班时间（班次 %1-%2）】已调整到 %3")
                           .arg(ws.toString("hh:mm")).arg(we.toString("hh:mm"))
                           .arg(start.toString("hh:mm"));
        }
        else if (fromTime > lastStart)
        {
            reasonText = QString("因【已超出可预约上限（班次 %1-%2）】已调整到 %3")
                           .arg(ws.toString("hh:mm")).arg(we.toString("hh:mm"))
                           .arg(start.toString("hh:mm"));
        } else
        {
            reasonText = QString("因【所选时段与已有预约冲突】已调整到 %1")
                           .arg(start.toString("hh:mm"));
        }
        return true;
    }
    return false;
}




void AddBooking::resetForm()
{
    ui->lineEdit_name->clear();
    ui->lineEdit_id->clear();
    ui->lineEdit_phone->clear();
    ui->textEdit_symptom->clear();
    ui->comboBox_sex->setCurrentIndex(0);
    ui->spinBox_age->setValue(0);

    ui->comboBox_department->clear();
    ui->comboBox_doctor->clear();
    ui->comboBox_department->addItem(tr("请选择科室"));
    ui->comboBox_doctor->addItem(tr("请选择医生"));
    ui->comboBox_doctor->setEnabled(false);
    ui->dateTimeEdit_time->setEnabled(false);

}

bool AddBooking::isSuspended(int docIndex, const QDate &day) const
{
    if (!m_suspends)
        return false;
    for (const auto &x : *m_suspends)
        if (x.doctorIndex == docIndex && x.date == day)
            return true;

    return false;
}

void AddBooking::initDepartmentsAndDoctors()
{
    if (!doctorTable) return;

    departmentDoctorsMap.clear();
    departmentDoctorIndicesMap.clear();

    QSet<QString> departments;//QSet可以去重
    for (int i = 0; i < doctorTable->getDoctorNum(); ++i)
    {
        const Doctor &d = doctorTable->getDoctor(i);
        departments.insert(d.d_subject.trimmed());
    }//找到已经有医生的科室

    ui->comboBox_department->clear();
    ui->comboBox_department->addItem("请选择科室");

    for (const QString &dept : departments)
        ui->comboBox_department->addItem(dept);

    for (int i = 0; i < doctorTable->getDoctorNum(); ++i)
    {
        const Doctor &d = doctorTable->getDoctor(i);
        const QString dept = d.d_subject.trimmed();
        departmentDoctorsMap[dept].append(d);//d添加到科室
        departmentDoctorIndicesMap[dept].append(i);//d的索引也添加
    }
}


void AddBooking::updateDoctorsComboBox(const QString &department)
{
    ui->comboBox_doctor->clear();
    ui->comboBox_doctor->addItem("请选择医生");

    const QString dept = department.trimmed();
    if (dept.isEmpty() || dept == "请选择科室")
    {
        ui->comboBox_doctor->setEnabled(false);
        ui->dateTimeEdit_time->setEnabled(false);
        return;
    }

    if (departmentDoctorsMap.contains(dept) && departmentDoctorIndicesMap.contains(dept))
    {
        const auto &docs = departmentDoctorsMap[dept];
        const auto &idxs = departmentDoctorIndicesMap[dept];
        for (int i = 0; i < docs.size(); ++i)
        {
            const Doctor &d = docs[i];
            int globalIndex = idxs[i];
            QString s = QString("%1 (%2) - %3").arg(d.d_name, d.d_title, d.d_id);
            ui->comboBox_doctor->addItem(s, globalIndex); // data=全局索引
        }
        ui->comboBox_doctor->setEnabled(true);
    }
    else
    {
        ui->comboBox_doctor->setEnabled(false);
        ui->dateTimeEdit_time->setEnabled(false);
    }
}

int AddBooking::currentDoctorIndex() const
{
    int idx = ui->comboBox_doctor->currentIndex();
    if (idx <= 0)
        return -1;
    bool ok = false;
    int globalIndex = ui->comboBox_doctor->currentData().toInt(&ok);
    return ok ? globalIndex : -1;
}//获取医生的全局i大学

Doctor AddBooking::getCurrentDoctor() const
{
    int g = currentDoctorIndex();
    if (g >= 0 && g < doctorTable->getDoctorNum())
        return doctorTable->getDoctor(g);

    return Doctor();
}

void AddBooking::updateDateTimeRange()
{
    const int docIdx = currentDoctorIndex();
    const Doctor doctor = getCurrentDoctor();
    if (docIdx < 0 || doctor.d_name.isEmpty()) { ui->dateTimeEdit_time->setEnabled(false); return; }

    ui->dateTimeEdit_time->setEnabled(true);

    const QDate today = QDate::currentDate();
    QDate useDate = today;

    ui->dateTimeEdit_time->setMinimumDate(today);
    ui->dateTimeEdit_time->setMaximumDate(today.addDays(60));

    QTime ws, we; doctor.workTimeFor(useDate, ws, we);
    QTime base = ws.isValid()? qMax(ws, ceilToHalfHour(QTime::currentTime()))
                             : ceilToHalfHour(QTime::currentTime());


    QDate finalDate; QTime finalStart; QString reason;
    const bool ok = findNearestAvailable(doctor, docIdx, m_suspends, useDate, base,
                                         finalDate, finalStart, reason);
    if (!ok)
    {
        ui->dateTimeEdit_time->setEnabled(false);
        QMessageBox::warning(this, "提示", "未来 60 天内均无可预约时段。");
        suppressNextHint_ = false;
        return;
    }

    doctor.workTimeFor(finalDate, ws, we);
    ui->dateTimeEdit_time->setMinimumTime(ws);
    ui->dateTimeEdit_time->setMaximumTime(we.addSecs(-kSlotSecs));


    const QDate prevD = ui->dateTimeEdit_time->date();
    const QTime prevT = ui->dateTimeEdit_time->time();
    ui->dateTimeEdit_time->blockSignals(true);
    ui->dateTimeEdit_time->setDate(finalDate);
    ui->dateTimeEdit_time->setTime(finalStart);
    ui->dateTimeEdit_time->blockSignals(false);

    // 时间发生了变化才提示
    if (!suppressNextHint_ && (finalDate!=prevD || finalStart!=prevT) && !reason.isEmpty())
        QMessageBox::information(this, "提示", reason);

    suppressNextHint_ = false;
}





void AddBooking::on_comboBox_department_currentIndexChanged(int index)
{
    if (index <= 0)
    {
        ui->comboBox_doctor->setEnabled(false);
        ui->dateTimeEdit_time->setEnabled(false);
        return;
    }
    updateDoctorsComboBox(ui->comboBox_department->currentText());
}

void AddBooking::on_dateTimeEdit_time_dateTimeChanged(const QDateTime &dt)
{

    if (presetting_)
        return;

    const int docIdx = currentDoctorIndex();
    const Doctor doctor = getCurrentDoctor();
    if (docIdx < 0 || doctor.d_name.isEmpty()) return;

    const QDate d = dt.date();
    QTime ws, we; doctor.workTimeFor(d, ws, we);

    QTime s = ceilToHalfHour(dt.time());

    //看时间要不要调整
    bool needAdjust = false;

    if (!ws.isValid() || !we.isValid() || isSuspended(docIdx, d))
        needAdjust = true;
    else {
        if (s < ws || s > we.addSecs(-kSlotSecs)) needAdjust = true;
        else {
            for (const Patient& p : doctor.d_patient){
                if (p.p_booking_date!=d) continue;
                if (overlap30min(s, ceilToHalfHour(p.p_booking_time))) { needAdjust = true; break; }
            }
        }
    }

    if (!needAdjust)
    {
        ui->dateTimeEdit_time->blockSignals(true);
        ui->dateTimeEdit_time->setTime(s);
        ui->dateTimeEdit_time->blockSignals(false);
        return;
    }

    QDate finalDate; QTime finalStart; QString reason;
    const bool ok = findNearestAvailable(doctor, docIdx, m_suspends, d, s,finalDate, finalStart, reason);
    if (!ok)
    {
        QMessageBox::warning(this, "提示", "未来 60 天内均无可预约时段。");
        return;
    }


    ui->dateTimeEdit_time->blockSignals(true);
    ui->dateTimeEdit_time->setDate(finalDate);
    ui->dateTimeEdit_time->setTime(finalStart);
    ui->dateTimeEdit_time->blockSignals(false);

    //if ( (finalDate!=d || finalStart!=s) && !reason.isEmpty() )
        //QMessageBox::information(this, "提示", reason);
}

void AddBooking::on_timeEdit_userEditingFinished()
{
    if (presetting_)
        return; // 预填阶段不提示

    const int docIdx = currentDoctorIndex();
    const Doctor doctor = getCurrentDoctor();
    if (docIdx < 0 || doctor.d_name.isEmpty()) return;

    // 用户刚输入的目标
    const QDate dWant = ui->dateTimeEdit_time->date();
    const QTime tWant = ui->dateTimeEdit_time->time();

    QDate dFinal;
    QTime tFinal;
    QString reason;
    if (!findNearestAvailable(doctor, docIdx, m_suspends,dWant, ceilToHalfHour(tWant),dFinal, tFinal, reason))
    {
        QMessageBox::warning(this, "提示", "未来 60 天内均无可预约时段。");
        return;
    }

    QTime ws, we; doctor.workTimeFor(dFinal, ws, we);
    ui->dateTimeEdit_time->blockSignals(true);
    ui->dateTimeEdit_time->setMinimumTime(ws);
    ui->dateTimeEdit_time->setMaximumTime(we.addSecs(-kSlotSecs));
    ui->dateTimeEdit_time->setDate(dFinal);
    ui->dateTimeEdit_time->setTime(tFinal);
    ui->dateTimeEdit_time->blockSignals(false);

    if (dFinal != dWant || tFinal != ceilToHalfHour(tWant)) {
        QMessageBox::information(this, "提示", reason);
    }
}









bool AddBooking::validateBookingInfo()
{
    if (ui->lineEdit_name->text().isEmpty()) { QMessageBox::warning(this,"输入错误","请输入姓名"); return false; }
    if (ui->lineEdit_id->text().isEmpty()) { QMessageBox::warning(this,"输入错误","请输入身份证号"); return false; }
    if (ui->comboBox_department->currentIndex()<=0) { QMessageBox::warning(this,"输入错误","请选择科室"); return false; }
    if (ui->comboBox_doctor->currentIndex()<=0) { QMessageBox::warning(this,"输入错误","请选择医生"); return false; }
    if (ui->lineEdit_phone->text().isEmpty()) { QMessageBox::warning(this,"输入错误","请输入联系电话"); return false; }

    const int docIdx = currentDoctorIndex();
    if (isSuspended(docIdx, ui->dateTimeEdit_time->date()))
    {
        QMessageBox::warning(this, "停诊", "该医生当天停诊，不能预约。");
        return false;
    }
    return true;
}

Patient AddBooking::getBookingInfo()
{
    Patient p;
    p.p_name = ui->lineEdit_name->text();
    p.p_id = ui->lineEdit_id->text();
    p.p_sex = (ui->comboBox_sex->currentText() == "男") ? 1 : 0;
    p.p_age = ui->spinBox_age->value();
    p.p_booking_date = ui->dateTimeEdit_time->date();
    p.p_booking_time = ui->dateTimeEdit_time->time();
    p.p_phone = ui->lineEdit_phone->text().toInt();
    p.p_symptom = ui->textEdit_symptom->toPlainText();
    return p;
}

void AddBooking::on_buttonBox_accepted()
{
    if (!validateBookingInfo()) return;
    accept();
}

void AddBooking::on_buttonBox_rejected()
{
    resetForm();
    reject();
}
