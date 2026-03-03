#ifndef ADDDOCTOR_H
#define ADDDOCTOR_H

#include <QDialog>
#include <QTimeEdit>
#include <QCheckBox>
#include "doctor.h"

namespace Ui { class AddDoctor; }

class AddDoctor : public QDialog
{
    Q_OBJECT
public:
    explicit AddDoctor(QWidget *parent = nullptr);
    ~AddDoctor();


    const Doctor& resultDoctor() const { return tmp_; }


    QString name();
    QString id();
    QString sex();
    int     age();
    QString title();
    QString subject();
    QTime   stime();
    QTime   etime();
    void presetForEdit(const Doctor &d);
    void preset(const Doctor& d);
    void applyTo(Doctor& d) const;
    void presetDoctor(const Doctor& d, bool editMode = true);

    bool dayEnabled(int i) const;   // 是否勾选启用
    QTime dayStart (int i) const;   // 当天开始
    QTime dayEnd   (int i) const;   // 当天结束


private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_chkMon_toggled(bool on);
    void on_chkTue_toggled(bool on);
    void on_chkWed_toggled(bool on);
    void on_chkThu_toggled(bool on);
    void on_chkFri_toggled(bool on);
    void on_chkSat_toggled(bool on);
    void on_chkSun_toggled(bool on);

private:
    Ui::AddDoctor *ui;
    Doctor tmp_;

    // 根据复选框是否勾选，取某一天的上/下班时间 未勾选则置为 invalid()
    void getPair(QTime &start, QTime &end,QTimeEdit *editStart, QTimeEdit *editEnd, QCheckBox *chk);
    void setDayUI(int idx, QCheckBox* chk, QTimeEdit* s, QTimeEdit* e,const Doctor& d);

};

#endif // ADDDOCTOR_H
