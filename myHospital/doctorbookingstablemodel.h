// doctorbookingstablemodel.h
#ifndef DOCTORBOOKINGSTABLEMODEL_H
#define DOCTORBOOKINGSTABLEMODEL_H
#include <QAbstractTableModel>
#include "doctorinfortable.h"
#include "patient.h"

class DoctorBookingsTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit DoctorBookingsTableModel(QObject *p, DoctorInfortable *table, int doctorIndex);
    int rowCount(const QModelIndex& = {}) const override { return m_rows.size(); }
    int columnCount(const QModelIndex& = {}) const override { return 9; }
    QVariant data(const QModelIndex&, int) const override;
    QVariant headerData(int, Qt::Orientation, int) const override;
    void refresh();

private:
    DoctorInfortable *m_table=nullptr;
    int m_docIndex=-1;
    struct Row { Patient p; QString dept; QString doctor; };
    QVector<Row> m_rows;
    static constexpr int kSlotSecs = 30*60;
};
#endif
