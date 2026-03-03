#ifndef DOCTOR_H
#define DOCTOR_H

#include <QString>
#include <QTime>
#include <QDate>
#include <QVector>
#include <QTextStream>

#include "patient.h"

class Doctor
{
public:
    Doctor();
    Doctor(const Doctor &doc);
    Doctor& operator=(const Doctor &doc);
    ~Doctor();


    QString d_name;
    QString d_id;
    QString d_sex;
    int     d_age;
    QString d_title;
    QString d_subject;
    QTime   d_time_s;
    QTime   d_time_e;

    QVector<Patient> d_patient;
    int d_pat_num;

    QTime weekStart[7];
    QTime weekEnd[7];
    bool   weekEnabled[7];


    void saveDoctor(QTextStream &s) const;
    void readDoctor(QTextStream &s);
    void workTimeFor(const QDate &day, QTime &outStart, QTime &outEnd) const;


};

#endif // DOCTOR_H
