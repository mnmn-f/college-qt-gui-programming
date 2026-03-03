/ doctorbookingstablemodel.cpp
#include "doctorbookingstablemodel.h"
#include <algorithm>

DoctorBookingsTableModel::DoctorBookingsTableModel(QObject *p, DoctorInfortable *table, int doctorIndex)
    : QAbstractTableModel(p), m_table(table), m_docIndex(doctorIndex) {}

void DoctorBookingsTableModel::refresh()
{
    beginResetModel();
    m_rows.clear();
    if (!m_table || m_docIndex<0 || m_docIndex>=m_table->getDoctorNum())
    {
        endResetModel();
        return;
    }

    const Doctor &doc = m_table->getDoctor(m_docIndex);
    QVector<Patient> bookings = doc.d_patient;
    std::sort(bookings.begin(), bookings.end(),[](const Patient&a,const Patient&b)
    {
        return (a.p_booking_date==b.p_booking_date)? (a.p_booking_time<b.p_booking_time): (a.p_booking_date<b.p_booking_date);
    });//先按预约日期升序排序,同一天内按预约时间升序

    for (const Patient &p: bookings)
        m_rows.push_back(Row{p, doc.d_subject, doc.d_name});

    endResetModel();
}

QVariant DoctorBookingsTableModel::headerData(int s, Qt::Orientation o, int r) const
{
    if (r!=Qt::DisplayRole || o!=Qt::Horizontal) return {};
    static const QStringList H={"医生","科室","姓名","性别","年龄","预约日期","预约时段","联系电话","症状描述"};
    return H.value(s);
}

QVariant DoctorBookingsTableModel::data(const QModelIndex &i, int r) const
{
    if (!i.isValid() || r!=Qt::DisplayRole) return {};

    const Row &row = m_rows[i.row()];
    switch (i.column()) {
    case 0: return row.doctor;
    case 1: return row.dept;
    case 2: return row.p.p_name;
    case 3: return (row.p.p_sex==1? "男":"女");
    case 4: return row.p.p_age;
    case 5: return row.p.p_booking_date.toString("yyyy-MM-dd");
    case 6: { const QTime e=row.p.p_booking_time.addSecs(kSlotSecs);
              return QString("%1-%2").arg(row.p.p_booking_time.toString("hh:mm"), e.toString("hh:mm")); }
    case 7: return QString::number(row.p.p_phone);
    case 8: return row.p.p_symptom;
    default: return {};
    }
}
