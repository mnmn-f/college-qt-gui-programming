#ifndef BOOKINGTABLEMODEL_H
#define BOOKINGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QSet>
#include "doctorinfortable.h"
#include "patinfortable.h"
#include "patient.h"

class BookingTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BookingTableModel(DoctorInfortable* dt,PatInforTable* pt,QObject* parent = nullptr);

    enum Col {
        C_Name = 0, C_Id, C_Sex, C_Age, C_Date, C_Slot, C_Doctor, C_Dept, C_Phone, C_Symptom,
        C_COUNT
    };

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override { return C_COUNT; }
    QVariant data(const QModelIndex &, int role) const override;
    QVariant headerData(int s, Qt::Orientation o, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &idx) const override;


    Patient patientAt(int row) const;

    QString doctorNameAt(int row) const;


    bool patientTimeClash(const Patient& p, QString* reason = nullptr) const;

    bool addBooking(const Patient& p, int doctorIndex, QString* whyAdjust = nullptr);
    bool removeBooking(int row);

    // 修改预约
    bool updateBooking(int row, const Patient& np, int newDocIdx, QString* whyAdjust = nullptr);

    // 让时间对齐并落在医生当日班次边界内
    bool fitShift(const QString& doctorName, const QDate& date, QTime& start, QString& why) const;

    void refresh();

private:
    DoctorInfortable* dt_;
    PatInforTable*    pt_;

    //找到 p 对应的医生索引
    int findDoctorIndexFor(const Patient& p) const;
    void removeFromAllDoctors(const Patient& p);
    void appendToDoctor(int docIdx, const Patient& p);
};

#endif // BOOKINGTABLEMODEL_H
