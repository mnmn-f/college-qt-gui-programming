#ifndef PATINFORTABLE_H
#define PATINFORTABLE_H

#include<QTextStream>
#include<doctor.h>
#include <qfile.h>
#include <QTextStream>
#include <QTextCodec>

class PatInforTable
{
public:
    PatInforTable();
    virtual ~PatInforTable();

    bool updateAt(int row, const Patient& p);

    bool readPatFromFile(const QString &readFileName);
    bool savePatToFile(const QString &saveFileName);

    void addPat(const Patient & doc);
    Patient & getPat(int index);
    int getPatNum()const;

    void setPat(int row, const Patient& p);
    bool removeAt(int row);

    void replaceAll(const QVector<Patient>& all);




protected:
    QVector <Patient> pats;
    int pats_num;

};

#endif // PATINFORTABLE_H
