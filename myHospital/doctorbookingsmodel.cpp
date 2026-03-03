#include "doctorbookingsmodel.h"
#include <algorithm>

QTime DoctorBookingsModel::ceilToHalf(const QTime& t)
{
    const int m = t.minute();
    if (m == 0 || m == 30)
        return QTime(t.hour(), m, 0);
    if (m < 30)
        return QTime(t.hour(), 30, 0);
    return QTime(t.hour()+1, 0, 0);
}

void DoctorBookingsModel::setDoctorIndex(int idx)
{
    if (idx == docIndex_) return;
    beginResetModel();
    docIndex_ = idx;
    order_.clear();
    endResetModel();
    refresh();
}

void DoctorBookingsModel::refresh()
{
    if (!dt_ || docIndex_ < 0 || docIndex_ >= dt_->getDoctorNum()) {
        beginResetModel(); order_.clear(); endResetModel();
        return;
    }

    const Doctor& d = dt_->getDoctor(docIndex_);
    const int n = d.d_patient.size();

    QVector<int> newOrder(n);
    for (int i = 0; i < n; ++i) newOrder[i] = i;

    //时间升序
    std::sort(newOrder.begin(), newOrder.end(),
              [&d](int a, int b){
                  const Patient& pa = d.d_patient[a];
                  const Patient& pb = d.d_patient[b];
                  if (pa.p_booking_date == pb.p_booking_date)
                  {
                      const QTime ta = ceilToHalf(pa.p_booking_time);
                      const QTime tb = ceilToHalf(pb.p_booking_time);
                      return ta < tb;
                  }
                  return pa.p_booking_date < pb.p_booking_date;
              });

    beginResetModel();
    order_.swap(newOrder);
    endResetModel();
}

int DoctorBookingsModel::rowCount(const QModelIndex&) const
{
    if (!dt_ || docIndex_ < 0 || docIndex_ >= dt_->getDoctorNum()) return 0;
    return order_.size();   // 用排序后的行数
}

QVariant DoctorBookingsModel::data(const QModelIndex& i, int role) const
{
    if (!dt_ || !i.isValid() || role != Qt::DisplayRole)
        return {};

    const Doctor& d = dt_->getDoctor(docIndex_);
    if (i.row() < 0 || i.row() >= order_.size())
        return {};

    const int real = order_[i.row()];
    if (real < 0 || real >= d.d_patient.size())
        return {};

    const Patient& p = d.d_patient[real];

    switch (i.column())
    {
    case 0: return p.p_name;
    case 1: return p.p_id;
    case 2: return (p.p_sex == 1 ? "男" : "女");
    case 3: return p.p_age;
    case 4: return p.p_booking_date.toString("yyyy-MM-dd");
    case 5: {
        const QTime s = ceilToHalf(p.p_booking_time);
        return QString("%1-%2")
               .arg(s.toString("hh:mm"))
               .arg(s.addSecs(kSlotSecs).toString("hh:mm"));
    }
    case 6: return QString::number(p.p_phone);
    case 7: return p.p_symptom;
    default: return {};
    }
}

QVariant DoctorBookingsModel::headerData(int s, Qt::Orientation o, int role) const
{
    if (o == Qt::Horizontal && role == Qt::DisplayRole)
    {
        static const char* H[] = { "姓名","身份证","性别","年龄","日期","时段","电话","症状" };
        if (s >= 0 && s < 8) return QString::fromUtf8(H[s]);
    }
    return QAbstractTableModel::headerData(s, o, role);
}
