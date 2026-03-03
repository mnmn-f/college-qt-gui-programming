#include "searchtablemodel.h"

SearchTableModel::SearchTableModel(QObject *parent, Mode m)
    : QAbstractTableModel(parent), m_mode(m) {}

int SearchTableModel::columnCount(const QModelIndex &) const {
    return (m_mode == Doctors) ? 8 : 8;
}

QVariant SearchTableModel::headerData(int section, Qt::Orientation ori, int role) const
{
    if (role != Qt::DisplayRole || ori != Qt::Horizontal) return {};
    if (m_mode == Doctors)
    {
        static const QStringList h = {"姓名","编号","性别","年龄","职称","科室","上班时间","下班时间"};
        return h.value(section);
    }
    else
    {
        static const QStringList h = {"姓名","身份证号","性别","年龄","预约日期","预约时段","联系电话","症状描述"};
        return h.value(section);
    }
}

QVariant SearchTableModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || role != Qt::DisplayRole) return {};

    const int r = idx.row(), c = idx.column();
    if (m_mode == Doctors) {
        const Doctor &d = m_docs[r];
        switch (c) {
        case 0: return d.d_name;
        case 1: return d.d_id;
        case 2: return d.d_sex;
        case 3: return d.d_age;
        case 4: return d.d_title;
        case 5: return d.d_subject;
        case 6: return d.d_time_s.toString("hh:mm");
        case 7: return d.d_time_e.toString("hh:mm");
        default: return {};
        }
    }
    else
    {
        const Patient &p = m_pats[r];
        switch (c) {
        case 0: return p.p_name;
        case 1: return p.p_id;
        case 2: return (p.p_sex == 1 ? QStringLiteral("男") : QStringLiteral("女"));
        case 3: return p.p_age;
        case 4: return p.p_booking_date.toString("yyyy-MM-dd");
        case 5:
        {
            const QTime end = p.p_booking_time.addSecs(kSlotSecs);
            return QString("%1-%2").arg(p.p_booking_time.toString("hh:mm"), end.toString("hh:mm"));
        }
        case 6: return QString::number(p.p_phone);
        case 7: return p.p_symptom;
        default: return {};
        }
    }
}

void SearchTableModel::setDoctorResults(const QVector<Doctor> &list)
{
    beginResetModel();
    m_mode = Doctors;
    m_docs = list;
    m_pats.clear();//设置为医生的时候清空病人的结果
    m_rows = m_docs.size();
    endResetModel();
}

void SearchTableModel::setPatientResults(const QVector<Patient> &list) {
    beginResetModel();
    m_mode = Patients;
    m_pats = list;
    m_docs.clear();
    m_rows = m_pats.size();
    endResetModel();
}
