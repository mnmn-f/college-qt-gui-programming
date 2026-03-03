#ifndef SEARCHTABLEMODEL_H
#define SEARCHTABLEMODEL_H

#include <QAbstractTableModel>
#include "doctorinfortable.h"
#include "patinfortable.h"

class SearchTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Mode { Doctors, Patients };

    explicit SearchTableModel(QObject *parent, Mode m);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override { Q_UNUSED(parent); return m_rows; }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;

    // 构建/更新数据
    void setDoctorResults(const QVector<Doctor> &list);
    void setPatientResults(const QVector<Patient> &list);

private:
    Mode m_mode;
    int m_rows = 0;

    QVector<Doctor>  m_docs;
    QVector<Patient> m_pats;
    static constexpr int kSlotSecs = 30 * 60;
};

#endif // SEARCHTABLEMODEL_H
