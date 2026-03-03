#include "patienttablemodel.h"

PatientTableModel::PatientTableModel(QObject *parent,
                                     PatInforTable *patTable,
                                     DoctorInfortable *docTable)
    : QAbstractTableModel(parent),
      m_patTable(patTable),
      m_docTable(docTable)
{}

void PatientTableModel::refresh()
{
    beginResetModel();
    m_rows.clear();

    if (m_patTable) {
        QSet<QString> seen;
        for (int i=0;i<m_patTable->getPatNum();++i) {
            const Patient p = m_patTable->getPat(i);
            if (!isValid(p)) continue;

            const QString key = p.p_id + "|" + p.p_booking_date.toString("yyyy-MM-dd")
                                + "|" + p.p_booking_time.toString("hh:mm");
            if (seen.contains(key)) continue;  // 去重
            seen.insert(key);

            QString doc, dept;
            findDoctorFor(p, doc, dept);
            m_rows.push_back(Row{p, doc, dept});
        }
    }
    endResetModel();
}

void PatientTableModel::findDoctorFor(const Patient &p, QString &doctorName, QString &department) const
{
    doctorName.clear(); department.clear();
    if (!m_docTable) return;
    for (int i=0;i<m_docTable->getDoctorNum();++i)
    {
        const Doctor &d = m_docTable->getDoctor(i);
        for (const Patient &dp : d.d_patient)
        {
            const bool idMatch   = (!p.p_id.isEmpty() && dp.p_id == p.p_id);
            const bool infoMatch = (dp.p_name==p.p_name && dp.p_booking_date==p.p_booking_date &&
                                    dp.p_booking_time==p.p_booking_time);
            if (idMatch || infoMatch)
            { doctorName=d.d_name; department=d.d_subject; return; }
        }
    }
}

QVariant PatientTableModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || role != Qt::DisplayRole) return {};
    const Row &r = m_rows[idx.row()];
    const Patient &p = r.p;

    switch (idx.column()) {
    case 0: return p.p_name;
    case 1: return p.p_id;
    case 2: return (p.p_sex==1? QStringLiteral("男"):QStringLiteral("女"));
    case 3: return p.p_age;
    case 4: return p.p_booking_date.toString("yyyy-MM-dd");
    case 5: {
        const QTime end = p.p_booking_time.addSecs(kSlotSecs);
        return QString("%1-%2").arg(p.p_booking_time.toString("hh:mm"), end.toString("hh:mm"));
    }
    case 6: return r.doctorName;
    case 7: return r.department;
    case 8: return QString::number(p.p_phone);
    case 9: return p.p_symptom;
    default: return {};
    }
}

QVariant PatientTableModel::headerData(int section, Qt::Orientation ori, int role) const
{
    if (role != Qt::DisplayRole || ori != Qt::Horizontal) return {};
    static const QStringList H = {
        "姓名","身份证号","性别","年龄","预约日期","预约时段","医生","科室","联系电话","症状描述"
    };
    return H.value(section);
}
