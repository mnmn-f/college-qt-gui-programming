#include "adddoctor.h"
#include "ui_adddoctor.h"

// adddoctor.cpp
#include "adddoctor.h"
#include "ui_adddoctor.h"

AddDoctor::AddDoctor(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddDoctor)
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->setupUi(this);
    setSizeGripEnabled(true);
}

AddDoctor::~AddDoctor() { delete ui; }


QString AddDoctor::name() { return ui->lineEdit_DName->text(); }
QString AddDoctor::id() { return ui->lineEdit_DId->text().trimmed(); }
QString AddDoctor::sex() { return ui->comboBox_DSex->currentText(); }
int     AddDoctor::age() { return ui->spinBox_DAge->value(); }
QString AddDoctor::title() { return ui->comboBox_DTitle->currentText(); }
QString AddDoctor::subject() { return ui->comboBox_D_subject->currentText(); }
QTime   AddDoctor::stime() { return ui->timeEdit_DSTime->time(); }
QTime   AddDoctor::etime() { return ui->timeEdit_DETime->time(); }

//修改的预填
void AddDoctor::presetDoctor(const Doctor &d, bool editMode)
{

    ui->lineEdit_DName->setText(d.d_name);
    ui->lineEdit_DId->setText(d.d_id);
    ui->comboBox_DSex->setCurrentText(d.d_sex);
    ui->spinBox_DAge->setValue(d.d_age);
    ui->comboBox_DTitle->setCurrentText(d.d_title);
    ui->comboBox_D_subject->setCurrentText(d.d_subject);
    ui->timeEdit_DSTime->setTime(d.d_time_s.isValid() ? d.d_time_s : QTime(9,0));
    ui->timeEdit_DETime->setTime(d.d_time_e.isValid() ? d.d_time_e : QTime(18,0));

    //姓名/编号/性别禁改
    ui->lineEdit_DName->setEnabled(!editMode);
    ui->lineEdit_DId  ->setEnabled(!editMode);
    ui->comboBox_DSex ->setEnabled(!editMode);

    auto setDay = [&](int i, QCheckBox* chk, QTimeEdit* s, QTimeEdit* e)
    {
        const bool en = d.weekEnabled[i] && d.weekStart[i].isValid() && d.weekEnd[i].isValid();
        chk->setChecked(en);
        s->setTime(en ? d.weekStart[i] : QTime(0,0));
        e->setTime(en ? d.weekEnd[i]   : QTime(0,0));
        s->setEnabled(en);
        e->setEnabled(en);
        connect(chk, &QCheckBox::toggled, s, &QWidget::setEnabled);
        connect(chk, &QCheckBox::toggled, e, &QWidget::setEnabled);
    };
    setDay(0, ui->chkMon, ui->timeMonStart, ui->timeMonEnd);
    setDay(1, ui->chkTue, ui->timeTueStart, ui->timeTueEnd);
    setDay(2, ui->chkWed, ui->timeWedStart, ui->timeWedEnd);
    setDay(3, ui->chkThu, ui->timeThuStart, ui->timeThuEnd);
    setDay(4, ui->chkFri, ui->timeFriStart, ui->timeFriEnd);
    setDay(5, ui->chkSat, ui->timeSatStart, ui->timeSatEnd);
    setDay(6, ui->chkSun, ui->timeSunStart, ui->timeSunEnd);
}


bool  AddDoctor::dayEnabled(int i) const
{
    switch (i)
    {
    case 0: return ui->chkMon->isChecked();
    case 1: return ui->chkTue->isChecked();
    case 2: return ui->chkWed->isChecked();
    case 3: return ui->chkThu->isChecked();
    case 4: return ui->chkFri->isChecked();
    case 5: return ui->chkSat->isChecked();
    case 6: return ui->chkSun->isChecked();
    }
    return false;
}

QTime AddDoctor::dayStart(int i) const
{
    switch (i)
    {
    case 0: return ui->timeMonStart->time();
    case 1: return ui->timeTueStart->time();
    case 2: return ui->timeWedStart->time();
    case 3: return ui->timeThuStart->time();
    case 4: return ui->timeFriStart->time();
    case 5: return ui->timeSatStart->time();
    case 6: return ui->timeSunStart->time();
    }
    return QTime();
}

QTime AddDoctor::dayEnd(int i) const
{
    switch (i)
    {
    case 0: return ui->timeMonEnd->time();
    case 1: return ui->timeTueEnd->time();
    case 2: return ui->timeWedEnd->time();
    case 3: return ui->timeThuEnd->time();
    case 4: return ui->timeFriEnd->time();
    case 5: return ui->timeSatEnd->time();
    case 6: return ui->timeSunEnd->time();
    }
    return QTime();
}


void AddDoctor::on_chkMon_toggled(bool on){ ui->timeMonStart->setEnabled(on); ui->timeMonEnd->setEnabled(on); }
void AddDoctor::on_chkTue_toggled(bool on){ ui->timeTueStart->setEnabled(on); ui->timeTueEnd->setEnabled(on); }
void AddDoctor::on_chkWed_toggled(bool on){ ui->timeWedStart->setEnabled(on); ui->timeWedEnd->setEnabled(on); }
void AddDoctor::on_chkThu_toggled(bool on){ ui->timeThuStart->setEnabled(on); ui->timeThuEnd->setEnabled(on); }
void AddDoctor::on_chkFri_toggled(bool on){ ui->timeFriStart->setEnabled(on); ui->timeFriEnd->setEnabled(on); }
void AddDoctor::on_chkSat_toggled(bool on){ ui->timeSatStart->setEnabled(on); ui->timeSatEnd->setEnabled(on); }
void AddDoctor::on_chkSun_toggled(bool on){ ui->timeSunStart->setEnabled(on); ui->timeSunEnd->setEnabled(on); }






void AddDoctor::on_buttonBox_accepted(){ accept(); }
void AddDoctor::on_buttonBox_rejected(){ reject(); }



