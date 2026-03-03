#ifndef PATIENT_H
#define PATIENT_H

#include<QString>
#include<QTextStream>
#include<QTime>
#include <QMetaType>
//、门诊预约信息：病人姓名、身份证号、性别、年龄、预约时间、联系电话、症状描述等。

class Patient
{
public:
    Patient();

    Patient(const Patient & pat);
    Patient& operator= (const Patient & man);
    virtual ~Patient();

    void savePatient(QTextStream &aStream) const;
    void readPatient(QTextStream &aStream);

    QString p_name;
    QString p_id;
    int p_sex;
    int p_age;
    QDate p_booking_date;
    QTime p_booking_time;//默认问诊时间持续30分钟，即从booking_time后30分钟为问诊时间
    int p_phone;
    QString p_symptom;


};
Q_DECLARE_METATYPE(Patient)

#endif // PATIENT_H
