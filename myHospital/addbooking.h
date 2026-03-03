#ifndef ADDBOOKING_H
#define ADDBOOKING_H

#include <QDialog>
#include <QMap>
#include <QVector>
#include <QDateTime>
#include "doctorinfortable.h"
#include "patinfortable.h"
#include "patient.h"
#include "suspend.h"

namespace Ui { class AddBooking; }

// 30分钟时段
static constexpr int kSlotSecs = 30 * 60;

class AddBooking : public QDialog {
    Q_OBJECT
public:
    void preset(const Patient& p, int doctorIndex);
    explicit AddBooking(QWidget *parent,
                        DoctorInfortable *docTable,
                        const QVector<SuspendItem> *suspends);
    ~AddBooking();
    Patient getBookingInfo();
    int     currentDoctorIndex() const;

    void setInitialPatient(const Patient& p);
    void preselectDoctorByName(const QString&);


protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void on_comboBox_department_currentIndexChanged(int index);
    void on_comboBox_doctor_currentIndexChanged(int index);
    void on_dateTimeEdit_time_dateTimeChanged(const QDateTime &dt);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_timeEdit_userEditingFinished();


private:
    bool presetting_ = false; // 正在预填，屏蔽自动跳转
    bool hasPreset_  = false;
    Ui::AddBooking *ui;
    DoctorInfortable *doctorTable = nullptr;
    const QVector<SuspendItem> *m_suspends = nullptr;


    QMap<QString, QVector<Doctor>> departmentDoctorsMap;
    QMap<QString, QVector<int>>    departmentDoctorIndicesMap;
    void resetForm();
    void initDepartmentsAndDoctors();
    void updateDoctorsComboBox(const QString &department);
    void updateDateTimeRange();
    Doctor getCurrentDoctor() const;
    QTime  findNextAvailableTime(const Doctor &doctor, const QDate &date, const QTime &currentTime);
    bool   validateBookingInfo();
    bool isSuspended(int docIndex, const QDate &day) const;
    bool suppressHint_ = false;
    QString lastHint_;
    bool initAdjust_  = false;
    bool userEditing_ = false;
    void applyTo(Doctor& d) const;
    bool suppressNextHint_= false;


};

#endif // ADDBOOKING_H
