#include "patinfortable.h"
#include <QTextCodec>

PatInforTable::PatInforTable() {}
PatInforTable::~PatInforTable() {}

bool PatInforTable::readPatFromFile(const QString &readFileName)
{
    QFile file(readFileName);
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&file);
    in.setCodec("UTF-8");               // 统一 UTF-8
    in.setAutoDetectUnicode(true);

    pats.clear();

    const QString nLine = in.readLine();
    bool ok = false;
    int n = nLine.trimmed().toInt(&ok);
    if (!ok || n < 0) n = 0;

    pats.reserve(n);
    for (int i = 0; i < n; ++i) {
        Patient p;
        p.readPatient(in);
        pats.push_back(p);
    }

    file.close();
    return true;
}
bool PatInforTable::updateAt(int row, const Patient& p)
{
    if (row < 0 || row >= pats.size()) return false;
    pats[row] = p;
    return true;
}


bool PatInforTable::savePatToFile(const QString &saveFileName)
{
    QFile file(saveFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);
    out.setCodec("UTF-8");

    const int n = pats.size();
    out << n << "\n";

    for (int i = 0; i < n; ++i)
    {
        pats[i].savePatient(out);
    }

    file.close();
    return true;
}


bool PatInforTable::removeAt(int row)
{
    if (row < 0 || row >= pats.size()) return false;
    pats.removeAt(row);
    pats_num = pats.size();
    return true;
}


void PatInforTable::replaceAll(const QVector<Patient>& all)
{
    pats = all;
    pats_num = pats.size();
}

void PatInforTable::setPat(int row, const Patient& p)
{
    if (row>=0 && row<pats.size()) pats[row]=p;
}


void PatInforTable::addPat(const Patient &addPat)
{
    pats.push_back(addPat);
}

Patient &PatInforTable::getPat(int index)
{
    return pats[index];
}

int PatInforTable::getPatNum() const
{
    return pats.size();
}
