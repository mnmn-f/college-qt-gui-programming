#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include "searchdialog.h"
#include "bookingtablemodel.h"
#include "doctortablemodel.h"
#include "doctorbookingsmodel.h"
#include "addbooking.h"

namespace {

// 病人同身份证
bool findPatientNameById(PatInforTable& pt, const QString& id, QString* nameOut)
{
    for (int i = 0; i < pt.getPatNum(); ++i) {
        const Patient& p = pt.getPat(i);
        if (p.p_id == id) {
            if (nameOut) *nameOut = p.p_name;
            return true;
        }
    }
    return false;
}

// 医生同编号
bool doctorIdExists( DoctorInfortable& dt, const QString& id)
{
    for (int i = 0; i < dt.getDoctorNum(); ++i) {
        if (dt.getDoctor(i).d_id == id) return true;
    }
    return false;
}

} // namespace


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setStyleSheet(
        "QMainWindow {"
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "               stop: 0 #a1c4fd, stop: 1 #c2e9fb);"
        "}"

        "QTableView {"
        "   background: transparent;"
        "   border: 1px solid #8faee0;"
        "   color: #2c3e50;"
        "   gridline-color: rgba(143, 174, 224, 50);"
        "   alternate-background-color: rgba(255, 255, 255, 30);"
        "}"

        "QTableView::item {"
        "   padding: 6px;"
        "   border-bottom: 1px solid rgba(143, 174, 224, 30);"
        "}"
        "QTableView::item:selected {"
        "   background: #8faee0;"
        "   color: white;"
        "}"

        "QHeaderView::section {"
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "               stop: 0 #6c8cd5, stop: 1 #8faee0);"
        "   color: white;"
        "   padding: 8px;"
        "   border: 1px solid #5a7bc2;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "}"
        "QHeaderView::section:hover {"
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "               stop: 0 #5a7bc2, stop: 1 #7a9bd8);"
        "}"
        "QHeaderView::section:pressed {"
        "   background: #4a6bb0;"
        "}"
    );

    //容纳多个控件，每次显示一个
    stacked_ = new QStackedWidget(this);
    setCentralWidget(stacked_);


    viewBookings_    = new QTableView(stacked_);
    viewDoctors_     = new QTableView(stacked_);
    viewDocBookings_ = new QTableView(stacked_);

    modelBookings_     = new BookingTableModel(&doctorTable, &patientTable, this);
    modelDoctors_      = new DoctorTableModel(&doctorTable, this);
    modelDocBookings_  = new DoctorBookingsModel(&doctorTable, this);



    viewBookings_->setModel(modelBookings_);
    viewDoctors_->setModel(modelDoctors_);
    viewDocBookings_->setModel(modelDocBookings_);


    auto setupView = [](QTableView* v)
    {
        v->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//均匀拉伸
        v->verticalHeader()->setVisible(false);
        v->setSelectionBehavior(QAbstractItemView::SelectRows);
        v->setSelectionMode(QAbstractItemView::SingleSelection);
    };
    setupView(viewBookings_);
    setupView(viewDoctors_);
    setupView(viewDocBookings_);


    stacked_->addWidget(viewBookings_);
    stacked_->addWidget(viewDoctors_);
    stacked_->addWidget(viewDocBookings_);

    stacked_->setCurrentWidget(viewBookings_);

    setWindowTitle("门诊预约管理系统");

    buildCentralViews();
    bindModels();

    loadData();
    updateStatusBar();


    /*connect(ui->actionViewBookings, &QAction::triggered, this, &MainWindow::on_actionViewBookings_triggered);
    connect(ui->actionViewDoctors,  &QAction::triggered, this, &MainWindow::on_actionViewDoctors_triggered);
    connect(ui->actionViewDoctorBookings, &QAction::triggered, this, &MainWindow::on_actionViewDoctorBookings_triggered);

    connect(ui->actionBookingAdd,  &QAction::triggered, this, &MainWindow::on_actionBookingAdd_triggered);
    connect(ui->actionBookingEdit, &QAction::triggered, this, &MainWindow::on_actionBookingEdit_triggered);
    connect(ui->actionBookingDel,  &QAction::triggered, this, &MainWindow::on_actionBookingDel_triggered);

    connect(ui->actionDoctorAdd,  &QAction::triggered, this, &MainWindow::on_actionDoctorAdd_triggered);
    connect(ui->actionDoctorSave, &QAction::triggered, this, &MainWindow::on_actionDoctorSave_triggered);

    connect(ui->actionManageSuspend, &QAction::triggered, this, &MainWindow::on_actionManageSuspend_triggered);

    connect(ui->actionSearchBookings, &QAction::triggered, this, &MainWindow::on_actionSearchBookings_triggered);
    connect(ui->actionSearchDoctors,  &QAction::triggered, this, &MainWindow::on_actionSearchDoctors_triggered);
    */

    on_actionViewBookings_triggered();
}




void MainWindow::on_actionBookingAdd_triggered()
{
    if (doctorTable.getDoctorNum() == 0) {
        QMessageBox::warning(this, "提示", "请先添加医生信息");
        return;
    }

    AddBooking dlg(this, &doctorTable, &m_suspends);
    if (dlg.exec() != QDialog::Accepted) return;

    Patient newP = dlg.getBookingInfo();

    QString existedName;
    if (findPatientNameById(patientTable, newP.p_id, &existedName) && existedName != newP.p_name) {
        QMessageBox::warning(this, "身份证冲突",
            QString("身份证号 %1 已被病人“%2”使用，不能再登记为“%3”。")
            .arg(newP.p_id, existedName, newP.p_name));
        return;
    }


    auto ceilToHalf = [](QTime t)
    {
        int m=t.minute();
        if (m==0 || m==30) return QTime(t.hour(),m,0);
        if (m<30) return QTime(t.hour(),30,0);
        int h=t.hour()+1; return QTime(h,0,0);
    };

    auto overlap30 = [&](const QTime& a, const QTime& b){
        QTime aEnd = a.addSecs(30*60), bEnd = b.addSecs(30*60);
        return (a < bEnd) && (b < aEnd);
    };

    for (int i=0; i<patientTable.getPatNum(); ++i) {
        const Patient& p = patientTable.getPat(i);
        if (p.p_id != newP.p_id)
            continue;
        if (p.p_booking_date != newP.p_booking_date)
            continue;
        if (overlap30(ceilToHalf(p.p_booking_time), ceilToHalf(newP.p_booking_time)))
        {
            QMessageBox::warning(this, "时间冲突",
                "该病人此日期已经有预约，时间段发生重叠，不能重复预约。");
            return;
        }
    }


    patientTable.addPat(newP);


    const int docIdx = dlg.currentDoctorIndex();
    if (docIdx >= 0 && docIdx < doctorTable.getDoctorNum())
    {
        Doctor& d = doctorTable.getDoctor(docIdx);
        d.d_patient.append(newP);
        d.d_pat_num = d.d_patient.size();
    }

    if (modelBookings_) modelBookings_->refresh();
    if (modelDoctors_)  modelDoctors_->refresh();
    updateStatusBar();
    saveData();
    QMessageBox::information(this, "成功", "预约已添加");
}



//弹出一个下拉选择预约
int MainWindow::pickBookingIndex(QWidget* parent)
{
    if (patientTable.getPatNum()==0)
        return -1;
    QStringList items;
    items.reserve(patientTable.getPatNum());//分配空间
    for (int i=0;i<patientTable.getPatNum();++i)
    {
        const Patient& p = patientTable.getPat(i);
        QString doc="-", dept="-";
        for (int k=0;k<doctorTable.getDoctorNum();++k)
        {
            const Doctor& d = doctorTable.getDoctor(k);
            for (const Patient& dp : d.d_patient)
            {
                if (dp.p_name==p.p_name && dp.p_id==p.p_id &&
                    dp.p_booking_date==p.p_booking_date &&
                    dp.p_booking_time==p.p_booking_time) { doc=d.d_name; dept=d.d_subject; goto out; }
            }
        }
        out:;
        items << QString("%1  %2 %3  %4/%5")
                  .arg(p.p_name).arg(p.p_booking_date.toString("yyyy-MM-dd"))
                  .arg(p.p_booking_time.toString("hh:mm")).arg(doc, dept);
    }
    bool ok=false;
    const QString choice = QInputDialog::getItem(parent, tr("选择预约"), tr("预约："),items, 0, false, &ok);//显示选择
    if (!ok || choice.isEmpty()) return -1;
    return items.indexOf(choice);
}

void MainWindow::on_actionBookingEdit_triggered()
{

    const int idx = pickBookingIndex(this);
    if (idx < 0) return;

    const Patient oldP = patientTable.getPat(idx);

    // 找旧医生索引
    int oldDoc = -1;
    for (int k = 0; k < doctorTable.getDoctorNum(); ++k)
    {
        const Doctor &d = doctorTable.getDoctor(k);
        for (const Patient &dp : d.d_patient)
        {
            if (dp.p_name == oldP.p_name && dp.p_id == oldP.p_id &&
                dp.p_booking_date == oldP.p_booking_date &&
                dp.p_booking_time == oldP.p_booking_time)
            {
                oldDoc = k; goto foundOldDoc;
            }
        }
    }
foundOldDoc:;


    AddBooking dlg(this, &doctorTable, &m_suspends);
    dlg.preset(oldP, oldDoc);                // 预填
    if (dlg.exec() != QDialog::Accepted) return;

    const Patient np  = dlg.getBookingInfo();
    const int     newDoc = dlg.currentDoctorIndex();


    for (int i = 0; i < patientTable.getPatNum(); ++i)
    {
        if (i == idx)
            continue;
        const Patient &p = patientTable.getPat(i);
        if (p.p_id == np.p_id && p.p_name != np.p_name)
        {
            QMessageBox::warning(this, tr("身份证冲突"),tr("身份证号 %1 已登记为“%2”，不能再登记为“%3”。").arg(np.p_id, p.p_name, np.p_name));

            return;
        }
    }

    //同一病人不能重叠 =====
    auto ceilToHalf = [](QTime t)
    {
        const int m = t.minute();
        if (m == 0 || m == 30) return QTime(t.hour(), m, 0);
        if (m < 30) return QTime(t.hour(), 30, 0);
        return QTime(t.hour() + 1, 0, 0);
    };
    auto overlap30 = [&](const QTime &a, const QTime &b)
    {
        const QTime aEnd = a.addSecs(30 * 60), bEnd = b.addSecs(30 * 60);
        return (a < bEnd) && (b < aEnd);
    };

    for (int i = 0; i < patientTable.getPatNum(); ++i)
    {
        if (i == idx) continue;
        const Patient &p = patientTable.getPat(i);
        if (p.p_id != np.p_id)
            continue;
        if (p.p_booking_date != np.p_booking_date)
            continue;
        if (overlap30(ceilToHalf(p.p_booking_time), ceilToHalf(np.p_booking_time)))
        {
            QMessageBox::warning(this, tr("时间冲突"),
                                 tr("该病人在 %1 已有其它预约，时间段发生重叠。")
                                 .arg(np.p_booking_date.toString("yyyy-MM-dd")));
            return;
        }
    }


    patientTable.getPat(idx) = np;

    // 从老医生删除
    if (oldDoc >= 0)
    {
        auto &v = doctorTable.getDoctor(oldDoc).d_patient;
        v.erase(std::remove_if(v.begin(), v.end(), [&](const Patient &x)
        {
            return x.p_name == oldP.p_name && x.p_id == oldP.p_id &&
                   x.p_booking_date == oldP.p_booking_date && x.p_booking_time == oldP.p_booking_time;
        }), v.end());
        doctorTable.getDoctor(oldDoc).d_pat_num = v.size();
    }//遍历容器 v，将所有不满足条件的元素移动到容器前部,删除从返回迭代器到 end() 的所有元素


    if (newDoc >= 0)
    {
        auto &v2 = doctorTable.getDoctor(newDoc).d_patient;
        v2.push_back(np);
        doctorTable.getDoctor(newDoc).d_pat_num = v2.size();
    }


    if (modelBookings_)     modelBookings_->refresh();
    if (modelDoctors_)      modelDoctors_->refresh();
    if (modelDocBookings_)  modelDocBookings_->refresh();
    saveData();

    QMessageBox::information(this, tr("成功"), tr("预约信息已更新。"));
}


void MainWindow::on_actionBookingDel_triggered()
{
    const int idx = pickBookingIndex(this);
    if (idx < 0) return;

    const Patient p = patientTable.getPat(idx);


    patientTable.removeAt(idx);


    for (int k=0;k<doctorTable.getDoctorNum();++k)
    {
        auto &v = doctorTable.getDoctor(k).d_patient;
        v.erase(std::remove_if(v.begin(), v.end(), [&](const Patient& x)
        {
            return x.p_name==p.p_name && x.p_id==p.p_id &&
                   x.p_booking_date==p.p_booking_date &&
                   x.p_booking_time==p.p_booking_time;
        }), v.end());
        doctorTable.getDoctor(k).d_pat_num = v.size();
    }

    modelBookings_->refresh();
    modelDoctors_->refresh();
    if (modelDocBookings_) modelDocBookings_->refresh();
    saveData();
}


void MainWindow::on_actionDoctorEdit_triggered()
{
    if (!viewDoctors_) return;
    const QModelIndex idx = viewDoctors_->currentIndex();
    if (!idx.isValid()) { QMessageBox::information(this,"提示","请先选中一位医生"); return; }

    const int row = idx.row();
    if (row < 0 || row >= doctorTable.getDoctorNum()) return;

    Doctor dOld = doctorTable.getDoctor(row);

    AddDoctor dlg(this);
    dlg.presetDoctor(dOld, /*editMode=*/true);
    if (dlg.exec() != QDialog::Accepted)
        return;

    Doctor dNew = dOld;
    dNew.d_age     = dlg.age();
    dNew.d_title   = dlg.title();
    dNew.d_subject = dlg.subject();
    dNew.d_time_s  = dlg.stime();
    dNew.d_time_e  = dlg.etime();

    for (int i=0;i<7;++i)
    {
        dNew.weekEnabled[i] = dlg.dayEnabled(i);
        if (dNew.weekEnabled[i])
        {
            dNew.weekStart[i] = dlg.dayStart(i);
            dNew.weekEnd[i]   = dlg.dayEnd(i);
        }
        else
        {
            dNew.weekStart[i] = QTime();
            dNew.weekEnd[i]   = QTime();
        }
    }


    doctorTable.getDoctor(row) = dNew;
    if (modelDoctors_) modelDoctors_->refresh();
    saveData();
    QMessageBox::information(this,"成功","医生信息已更新。");
}

void MainWindow::on_actionDoctorDelete_triggered()
{
    QModelIndex idx = viewDoctors_->currentIndex();
    if (!idx.isValid())
        return;
    int row = idx.row();
    if (row < 0 || row >= doctorTable.getDoctorNum())
        return;

    const Doctor d = doctorTable.getDoctor(row);

    //删掉医生预约
    QVector<Patient> kept;
    kept.reserve(patientTable.getPatNum());
    for (int i=0;i<patientTable.getPatNum();++i)
    {
        const Patient &p = patientTable.getPat(i);
        bool belong = false;
        for (const Patient& dp : d.d_patient)
        {
            if (dp.p_name==p.p_name && dp.p_id==p.p_id &&
                dp.p_booking_date==p.p_booking_date &&
                dp.p_booking_time==p.p_booking_time)
            { belong=true;
                break;
            }
        }
        if (!belong) kept.push_back(p);
    }
    patientTable.replaceAll(kept);


    doctorTable.removeDoctorAt(row);

    modelBookings_->refresh();
    modelDoctors_->refresh();
    if (modelDocBookings_) modelDocBookings_->refresh();
    saveData();
}





void MainWindow::on_actionViewBookings_triggered()
{
    stacked_->setCurrentWidget(viewBookings_);
}

void MainWindow::on_actionViewDoctors_triggered()
{
    stacked_->setCurrentWidget(viewDoctors_);
}

void MainWindow::on_actionViewDoctorBookings_triggered()
{
    if (doctorTable.getDoctorNum() == 0)
    {
        QMessageBox::information(this, "提示", "暂无医生信息");
        return;
    }

    // 选医生
    QStringList items;
    QVector<int> mapIdx;
    items.reserve(doctorTable.getDoctorNum());
    mapIdx.reserve(doctorTable.getDoctorNum());

    for (int i = 0; i < doctorTable.getDoctorNum(); ++i)
    {
        const Doctor &d = doctorTable.getDoctor(i);
        if (d.d_name.isEmpty()) continue;
        items << QString("%1（%2）").arg(d.d_name, d.d_subject);
        mapIdx << i;
    }
    if (items.isEmpty())
    {
        QMessageBox::information(this, "提示", "暂无可选择的医生");
        return;
    }

    bool ok = false;
    const QString choice = QInputDialog::getItem(this, "选择医生", "医生：", items, 0, false, &ok);
    if (!ok || choice.isEmpty()) return;

    const int row = items.indexOf(choice);
    if (row < 0 || row >= mapIdx.size())
        return;

    const int docIndex = mapIdx[row];


    modelDocBookings_->setDoctorIndex(docIndex);

    // 切换医生预约数据页
    stacked_->setCurrentWidget(viewDocBookings_);
}



MainWindow::~MainWindow()
{
    saveData();
    delete ui;
}

void MainWindow::buildCentralViews()
{
    if (stacked_) return;

    stacked_ = new QStackedWidget(this);

    viewBookings_    = new QTableView(stacked_);
    viewDoctors_     = new QTableView(stacked_);
    viewDocBookings_ = new QTableView(stacked_);


    for (auto v : {viewBookings_, viewDoctors_, viewDocBookings_})
    {
        v->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        v->verticalHeader()->setVisible(false);
        v->setSelectionBehavior(QAbstractItemView::SelectRows);
        v->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    stacked_->addWidget(viewBookings_);
    stacked_->addWidget(viewDoctors_);
    stacked_->addWidget(viewDocBookings_);

    setCentralWidget(stacked_);
}


void MainWindow::bindModels()
{
    if (!modelBookings_)    modelBookings_    = new BookingTableModel(&doctorTable, &patientTable, this);
    if (!modelDoctors_)     modelDoctors_     = new DoctorTableModel(&doctorTable, this);
    if (!modelDocBookings_) modelDocBookings_ = new DoctorBookingsModel(&doctorTable, this);

    viewBookings_->setModel(modelBookings_);
    viewDoctors_->setModel(modelDoctors_);
    viewDocBookings_->setModel(modelDocBookings_);


    stacked_->setCurrentWidget(viewBookings_);
}



void MainWindow::updateStatusBar()
{
    int doctorCount = doctorTable.getDoctorNum();
    int patientCount = patientTable.getPatNum();
    ui->statusbar->showMessage(QString("医生: %1 人 | 预约: %2 个").arg(doctorCount).arg(patientCount));
}


void MainWindow::on_actionDoctorAdd_triggered()
{
    AddDoctor dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    Doctor nd;
    nd.d_name    = dlg.name();
    nd.d_id      = dlg.id();
    nd.d_sex     = dlg.sex();
    nd.d_age     = dlg.age();
    nd.d_title   = dlg.title();
    nd.d_subject = dlg.subject();
    nd.d_time_s  = dlg.stime();
    nd.d_time_e  = dlg.etime();


    for (int i=0;i<7;++i)
    {
        nd.weekEnabled[i] = dlg.dayEnabled(i);
        if (nd.weekEnabled[i])
        {
            nd.weekStart[i] = dlg.dayStart(i);
            nd.weekEnd[i]   = dlg.dayEnd(i);
        } else
        {
            nd.weekStart[i] = QTime(); nd.weekEnd[i] = QTime();
        }
    }

    if (doctorIdExists(doctorTable, nd.d_id))
    {
        QMessageBox::warning(this, "编号冲突",
            QString("医生编号/身份证 %1 已存在，请更换。").arg(nd.d_id));
        return;
    }

    doctorTable.addDoctor(nd);
    if (modelDoctors_) modelDoctors_->refresh();
    updateStatusBar();
    saveData();
    QMessageBox::information(this, "成功", "医生已添加");
}


void MainWindow::on_actionManageSuspend_triggered()
{
    SuspendDialog dlg(this, &doctorTable, m_suspends);
    if (dlg.exec()==QDialog::Accepted){
        m_suspends = dlg.result();

    }
}


void MainWindow::on_actionSearchBookings_triggered()
{
    SearchDialog dlg(this, &patientTable, SearchDialog::SearchPatient);
    dlg.exec();
}
void MainWindow::on_actionSearchDoctors_triggered()
{
    SearchDialog dlg(this, &doctorTable, SearchDialog::SearchDoctor);
    dlg.exec();
}




static QString findDoctorDat(const QDir& dir)
{

    if (dir.exists("doctors.dat"))
        return dir.absoluteFilePath("doctors.dat");
    if (dir.exists("doctor.dat"))
        return dir.absoluteFilePath("doctor.dat");

    const QStringList cands = dir.entryList(QStringList() << "doctor*.dat", QDir::Files);
    if (!cands.isEmpty()) return dir.absoluteFilePath(cands.first());
    return QString();
}
static QString findPatientDat(const QDir& dir)
{
    if (dir.exists("patients.dat")) return dir.absoluteFilePath("patients.dat");
    if (dir.exists("patient.dat"))  return dir.absoluteFilePath("patient.dat");
    const QStringList cands = dir.entryList(QStringList() << "patient*.dat", QDir::Files);
    if (!cands.isEmpty()) return dir.absoluteFilePath(cands.first());
    return QString();
}

void MainWindow::on_action_O_triggered()
{
    const QString dirPath = QFileDialog::getExistingDirectory(this, "选择数据文件夹");
    if (dirPath.isEmpty()) return;

    QDir dir(dirPath);
    if (!dir.exists()) {
        QMessageBox::warning(this, "打开失败", "所选文件夹不存在。");
        return;
    }

    const QString docf = findDoctorDat(dir);
    const QString patf = findPatientDat(dir);

    bool doctorLoaded = false, patientLoaded = false;
    QStringList msgLines;

    if (!docf.isEmpty())
    {
        doctorLoaded = doctorTable.readDoctorFromFile(docf);
        msgLines << QString("医生文件：%1 —— %2").arg(QFileInfo(docf).fileName(),
                                              doctorLoaded ? "加载成功" : "加载失败");
    }
    else
    {
        msgLines << "医生文件：未找到（期望 doctors.dat / doctor.dat 或 doctor*.dat）";
    }

    if (!patf.isEmpty())
    {
        patientLoaded = patientTable.readPatFromFile(patf);
        msgLines << QString("病人文件：%1 —— %2").arg(QFileInfo(patf).fileName(),
                                              patientLoaded ? "加载成功" : "加载失败");
    }
    else
    {
        msgLines << "病人文件：未找到（期望 patients.dat / patient.dat 或 patient*.dat）";
    }

    if (doctorLoaded || patientLoaded)
    {
        modelDoctors_->layoutChanged();
        modelBookings_->layoutChanged();
        updateStatusBar();
        QMessageBox::information(this, "打开结果", msgLines.join("\n"));
    } else
    {
        QMessageBox::warning(this, "打开失败", msgLines.join("\n"));
    }
}

void MainWindow::on_action_S_triggered()
{

    const QString baseDir = QFileDialog::getExistingDirectory(this, "选择保存父目录");
    if (baseDir.isEmpty()) return;

    QDir parent(baseDir);
    if (!parent.exists()) {
        QMessageBox::warning(this, "保存失败", "所选父目录不存在。");
        return;
    }


    bool ok = false;
    const QString defaultName = "data_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString subName = QInputDialog::getText(this, "保存到新文件夹","请输入新文件夹名（留空则自动生成）：",QLineEdit::Normal, defaultName, &ok);

    if (!ok || subName.trimmed().isEmpty()) subName = defaultName;


    if (!parent.mkpath(subName))
    {
        QMessageBox::warning(this, "保存失败", "无法创建保存目录：" + subName);
        return;
    }

    QDir saveDir(parent.absoluteFilePath(subName));
    const QString docf = saveDir.absoluteFilePath("doctors.dat");
    const QString patf = saveDir.absoluteFilePath("patients.dat");

    const bool a = doctorTable.saveDoctorToFile(docf);
    const bool b = patientTable.savePatToFile(patf);

    QString title = (a && b) ? "保存成功" : "保存部分或失败";
    QStringList lines;
    lines << QString("医生文件：%1 —— %2").arg(QFileInfo(docf).fileName(), a ? "保存成功" : "保存失败");
    lines << QString("病人文件：%1 —— %2").arg(QFileInfo(patf).fileName(), b ? "保存成功" : "保存失败");
    lines << "保存位置：" + saveDir.absolutePath();

    QMessageBox::information(this, title, lines.join("\n"));
}

void MainWindow::loadData()
{
    doctorTable.readDoctorFromFile("doctors.dat");
    patientTable.readPatFromFile("patients.dat");
    if (modelDoctors_)  modelDoctors_->layoutChanged();
    if (modelBookings_) modelBookings_->layoutChanged();
}

void MainWindow::saveData()
{
    doctorTable.saveDoctorToFile("doctors.dat");
    patientTable.savePatToFile("patients.dat");
}
