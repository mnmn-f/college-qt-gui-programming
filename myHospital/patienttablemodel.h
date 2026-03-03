#ifndef PATIENTTABLEMODEL_H
#define PATIENTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QSet>
#include "patinfortable.h"
#include "doctorinfortable.h"

class PatientTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit PatientTableModel(QObject *parent,
                               PatInforTable *patTable,
                               DoctorInfortable *docTable);

    int rowCount(const QModelIndex &parent = {}) const override { Q_UNUSED(parent); return m_rows.size(); }
    int columnCount(const QModelIndex &parent = {}) const override { Q_UNUSED(parent); return 10; }
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;

    void refresh();

private:
    PatInforTable    *m_patTable = nullptr;
    DoctorInfortable *m_docTable = nullptr;

    struct Row { Patient p; QString doctorName; QString department; };
    QVector<Row> m_rows;

    static constexpr int kSlotSecs = 30 * 60;
    static bool isValid(const Patient &p) {
        return !p.p_name.trimmed().isEmpty()
            && !p.p_booking_date.isNull()
            && p.p_booking_time.isValid();
    }
    void findDoctorFor(const Patient &p, QString &doctorName, QString &department) const;
};

#endif // PATIENTTABLEMODEL_H
