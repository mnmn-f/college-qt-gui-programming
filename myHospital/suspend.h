// suspend.h
#ifndef SUSPEND_H
#define SUSPEND_H
#include <QDate>
#include <QString>

struct SuspendItem
{
    int doctorIndex = -1;
    QDate date;
    QString reason;
};
#endif
