// doctortablemodel.h
#ifndef DOCTORTABLEMODEL_H
#define DOCTORTABLEMODEL_H

#include <QAbstractTableModel>
#include "doctorinfortable.h"

class DoctorTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit DoctorTableModel(DoctorInfortable* ds, QObject* parent=nullptr): QAbstractTableModel(parent), ds_(ds) {}

    enum Col {
        C_NAME, C_ID, C_SEX, C_AGE, C_TITLE, C_DEPT, C_START, C_END,
        C_COUNT
    };

    int rowCount(const QModelIndex&) const override { return ds_ ? ds_->getDoctorNum() : 0; }
    int columnCount(const QModelIndex&) const override { return C_COUNT; }
    QVariant data(const QModelIndex& i, int role) const override;
    QVariant headerData(int s, Qt::Orientation o, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& i) const override;
    bool setData(const QModelIndex& i, const QVariant& v, int role) override;
    void refresh(){ beginResetModel(); endResetModel(); }


signals:
    void doctorsEdited();

private:
    DoctorInfortable* ds_;
};

#endif
