#include "patient.h"

// patient.cpp
#include "patient.h"

Patient::Patient() : p_sex(0), p_age(0), p_phone(0)
{
}

Patient::Patient(const Patient &pat)
{
    p_name = pat.p_name;
    p_id = pat.p_id;
    p_sex = pat.p_sex;
    p_age = pat.p_age;
    p_booking_date = pat.p_booking_date;
    p_booking_time = pat.p_booking_time;
    p_phone = pat.p_phone;
    p_symptom = pat.p_symptom;
}

Patient& Patient::operator=(const Patient &pat)
{
    if (this != &pat) {
        p_name = pat.p_name;
        p_id = pat.p_id;
        p_sex = pat.p_sex;
        p_age = pat.p_age;
        p_booking_date = pat.p_booking_date;
        p_booking_time = pat.p_booking_time;
        p_phone = pat.p_phone;
        p_symptom = pat.p_symptom;
    }
    return *this;
}

Patient::~Patient()
{
}


void Patient::savePatient(QTextStream &s) const
{

    s << p_name << "\n";
    s << p_id << "\n";
    s << p_sex << "\n";
    s << p_age << "\n";
    s << p_booking_date.toString("yyyy-MM-dd") << "\n";
    s << p_booking_time.toString("hh:mm") << "\n";
    s << QString::number(p_phone) << "\n";
    s << QString::fromUtf8(p_symptom.toUtf8().toBase64()) << "\n";
}

void Patient::readPatient(QTextStream &s)
{
    // read
    p_name = s.readLine();
    p_id   = s.readLine();
    p_sex  = s.readLine().toInt();
    p_age  = s.readLine().toInt();
    p_booking_date = QDate::fromString(s.readLine(), "yyyy-MM-dd");
    p_booking_time = QTime::fromString(s.readLine(), "hh:mm");
    p_phone = s.readLine().toInt();
    p_symptom = QString::fromUtf8(QByteArray::fromBase64(s.readLine().toUtf8()));
}
