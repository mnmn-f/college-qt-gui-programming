#ifndef DOCTORINFORTABLE_H
#define DOCTORINFORTABLE_H

#include<QTextStream>
#include<doctor.h>
#include <qfile.h>
#include <QTextStream>
#include <QTextCodec>
class DoctorInfortable
{
public:
    DoctorInfortable();
    virtual ~DoctorInfortable();


    bool readDoctorFromFile(const QString &readFileName);
    bool saveDoctorToFile(const QString &saveFileName);

    void addDoctor(Doctor & doc);
    Doctor & getDoctor(int index);
    int getDoctorNum();
    bool removeDoctorAt(int index);

protected:
    QVector <Doctor> docs;//医生列表
    int docs_num;//医生总数


};

#endif // DOCTORINFORTABLE_H
