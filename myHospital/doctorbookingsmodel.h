#ifndef DOCTORBOOKINGSMODEL_H
#define DOCTORBOOKINGSMODEL_H

#include <QAbstractTableModel>
#include "doctorinfortable.h"
#include "patient.h"


class DoctorBookingsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DoctorBookingsModel(DoctorInfortable* dt, QObject* parent=nullptr)
        : QAbstractTableModel(parent), dt_(dt) {}

    void setDoctorIndex(int idx);

    int  doctorIndex() const { return docIndex_; }

    int rowCount(const QModelIndex& = QModelIndex()) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 8; }
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation o, int role) const override;

    void refresh();

private:
    static constexpr int kSlotSecs = 30 * 60;
    static QTime ceilToHalf(const QTime& t);

    DoctorInfortable* dt_ = nullptr;
    int docIndex_ = -1;

    QVector<int> order_;
};

#endif // DOCTORBOOKINGSMODEL_H
