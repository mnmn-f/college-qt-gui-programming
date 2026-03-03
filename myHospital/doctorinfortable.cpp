#include "doctorinfortable.h"


DoctorInfortable::DoctorInfortable(){docs_num=0;}

DoctorInfortable::~DoctorInfortable(){}

bool DoctorInfortable::removeDoctorAt(int index)
{
    if (index < 0 || index >= docs.size()) return false;
    docs.removeAt(index);
    return true;
}

bool DoctorInfortable::readDoctorFromFile(const QString &readFileName)
{
    QFile file(readFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("UTF-8"));
    docs.clear();

    docs_num=in.readLine().toInt();

    for (int i = 0; i < docs_num; ++i)
    {
        Doctor doctor;
        doctor.readDoctor(in);

        if (!doctor.d_name.isEmpty() && !doctor.d_id.isEmpty())
        {
            docs.append(doctor);
        }
    }
    docs_num = docs.size();
    file.close();
    return true;
}

bool DoctorInfortable::saveDoctorToFile(const QString &saveFileName)
{
    QFile file(saveFileName);
    if (!file.open(QFile::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);
    // 原来是 "system" 必须统一为 UTF-8
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    out << docs_num << "\n";
    for (int i = 0; i < docs_num; ++i)
    {
        docs[i].saveDoctor(out);
    }
    return true;
}


void DoctorInfortable::addDoctor(Doctor & addDoc)
{
    docs.push_back(addDoc);
    docs_num++;
}
Doctor& DoctorInfortable::getDoctor(int index)
{
    return docs[index];
}
int DoctorInfortable::getDoctorNum()
{
    docs_num=docs.size();
    return docs_num;
}

