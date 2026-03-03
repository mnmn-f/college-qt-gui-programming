#include "bookingtablemodel.h"
#include <QTime>


static constexpr int kSlotSecs = 30*60;

static inline QTime ceilToHalf(const QTime& t)
{
    int m=t.minute();
    if(m==0||m==30) return QTime(t.hour(),m,0);
    if(m<30) return QTime(t.hour(),30,0);
    return QTime(t.hour()+1,0,0);
}
static inline QTime endOfSlot(const QTime& s){ return s.addSecs(kSlotSecs); }
static inline bool overlap30(const QTime& a, const QTime& b)
{
    return (a < endOfSlot(b)) && (b < endOfSlot(a));
}

BookingTableModel::BookingTableModel(DoctorInfortable* dt,PatInforTable* pt,QObject* parent)
    : QAbstractTableModel(parent), dt_(dt), pt_(pt)
{}

int BookingTableModel::rowCount(const QModelIndex&) const
{
    return pt_ ? pt_->getPatNum() : 0;
}

QVariant BookingTableModel::headerData(int s, Qt::Orientation o, int role) const
{
    if (o==Qt::Horizontal && role==Qt::DisplayRole)
    {
        static const char* H[] = {"姓名","身份证号","性别","年龄","预约日期","预约时段","医生","科室","联系电话","症状描述"};
        return QString::fromUtf8(H[s]);
    }
    return {};
}

QVariant BookingTableModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || !pt_)
        return {};
    const int r = idx.row(), c = idx.column();
    if (r<0 || r>=pt_->getPatNum())
        return {};
    const Patient p = pt_->getPat(r);

    if (role==Qt::DisplayRole || role==Qt::EditRole)
    {
        switch (c)
        {
        case C_Name:
            return p.p_name;
        case C_Id:
            return p.p_id;
        case C_Sex:
            return (p.p_sex==1 ? "男" : "女");
        case C_Age:
            return p.p_age;
        case C_Date:
            return p.p_booking_date.toString("yyyy-MM-dd");
        case C_Slot:
            return QString("%1-%2").arg(p.p_booking_time.toString("hh:mm"),endOfSlot(p.p_booking_time).toString("hh:mm"));
        case C_Phone:
            return QString::number(p.p_phone);
        case C_Symptom:
            return p.p_symptom;
        case C_Doctor:
        {
            const int di = findDoctorIndexFor(p);
            return (di>=0 ? dt_->getDoctor(di).d_name : "");
        }
        case C_Dept:
        {
            const int di = findDoctorIndexFor(p);
            return (di>=0 ? dt_->getDoctor(di).d_subject : "");
        }
        default:
            break;
        }
    }
    return {};
}

Qt::ItemFlags BookingTableModel::flags(const QModelIndex &idx) const
{
    if (!idx.isValid())
        return Qt::NoItemFlags;
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;   
    //医生和科室在行内不改
    if (idx.column()!=C_Doctor && idx.column()!=C_Dept)
        f |= Qt::ItemIsEditable;//添加可编辑标志
    return f;
}


Patient BookingTableModel::patientAt(int row) const
{
    if (!pt_ || row<0 || row>=pt_->getPatNum()) return Patient();
    return pt_->getPat(row);
}

QString BookingTableModel::doctorNameAt(int row) const
{
    const Patient p = patientAt(row);
    const int di = findDoctorIndexFor(p);
    return (di>=0 ? dt_->getDoctor(di).d_name : "");
}

bool BookingTableModel::patientTimeClash(const Patient& p, QString* reason) const //检测病人自己的预约冲突
{
    if (!pt_)
        return false;
    for (int i=0;i<pt_->getPatNum();++i)
    {
        const Patient q = pt_->getPat(i);
        if (q.p_id != p.p_id)
            continue;
        if (q.p_booking_date != p.p_booking_date)
            continue;
        if (overlap30(ceilToHalf(p.p_booking_time), ceilToHalf(q.p_booking_time)))
        {
            if (reason)
                *reason = QString("因患者[%1] %2 与现有 %3-%4 冲突").arg(p.p_name, p.p_booking_date.toString("yyyy-MM-dd"),
                                        q.p_booking_time.toString("hh:mm"),endOfSlot(q.p_booking_time).toString("hh:mm"));
            return true;
        }
    }
    return false;
}

bool BookingTableModel::addBooking(const Patient& p, int doctorIndex, QString* whyAdjust)
{
    if (!pt_ || !dt_)
        return false;

    QString why;
    QTime s = p.p_booking_time;
    fitShift( (doctorIndex>=0 && doctorIndex<dt_->getDoctorNum()) ? dt_->getDoctor(doctorIndex).d_name : "",
              p.p_booking_date, s, why );
    Patient np = p;
    np.p_booking_time = s;
    if (whyAdjust)
        *whyAdjust = why;

    if (patientTimeClash(np, whyAdjust))
        return false;

    pt_->addPat(np);

    if (doctorIndex>=0 && doctorIndex<dt_->getDoctorNum())
        appendToDoctor(doctorIndex, np);

    beginResetModel(); endResetModel();
    return true;
}

bool BookingTableModel::removeBooking(int row)
{
    if (!pt_ || row<0 || row>=pt_->getPatNum()) return false;
    const Patient p = pt_->getPat(row);

    removeFromAllDoctors(p);

    if (!pt_->removeAt(row)) return false;

    beginResetModel(); endResetModel();
    return true;
}


bool BookingTableModel::updateBooking(int row, const Patient& np, int newDocIdx, QString* whyAdjust)
{
    if (!pt_ || row<0 || row>=pt_->getPatNum()) return false;

    QString why;
    QTime s = np.p_booking_time;
    fitShift( (newDocIdx>=0 && newDocIdx<dt_->getDoctorNum()) ? dt_->getDoctor(newDocIdx).d_name : "",np.p_booking_date, s, why );
    Patient np2 = np; np2.p_booking_time = s;
    if (whyAdjust)
      *whyAdjust = why;

    if (patientTimeClash(np2, whyAdjust))//检测
        return false;

    const Patient old = pt_->getPat(row);
    pt_->getPat(row) = np2;

    removeFromAllDoctors(old);
    if (newDocIdx>=0 && newDocIdx<dt_->getDoctorNum())
        appendToDoctor(newDocIdx, np2);

    emit dataChanged(index(row,0), index(row,C_COUNT-1));
    return true;
}

bool BookingTableModel::fitShift(const QString& doctorName,const QDate& date,QTime& start,QString& why) const
{
    if (!dt_)
    { why="医生表缺失"; return false; }
    const Doctor* doc = nullptr;

    for (int i=0;i<dt_->getDoctorNum();++i)
        if (dt_->getDoctor(i).d_name == doctorName)
        {
            doc=&dt_->getDoctor(i);
            break;
        }
    if (!doc)
    {
        why="医生不存在";
        return false;
    }

    QTime s0,e0;
    doc->workTimeFor(date, s0, e0);
    QTime latest = e0.addSecs(-kSlotSecs);

    start = ceilToHalf(start);
    if (start < s0) { start = s0; why = "因早于上班时间"; return true; }
    if (start > latest) { start = latest; why = "因晚于下班时间"; return true; }

    why.clear(); return true;
}

void BookingTableModel::refresh()
{
    beginResetModel();
    endResetModel();
}


int BookingTableModel::findDoctorIndexFor(const Patient& p) const
{
    if (!dt_)
        return -1;
    for (int i=0;i<dt_->getDoctorNum();++i)
    {
        const Doctor& d = dt_->getDoctor(i);
        for (const Patient& x : d.d_patient){
            if (x.p_id==p.p_id &&
                x.p_booking_date==p.p_booking_date && x.p_booking_time==p.p_booking_time)
                return i;
        }
    }
    return -1;
}

void BookingTableModel::removeFromAllDoctors(const Patient& p)
{
    if (!dt_) return;
    for (int i=0;i<dt_->getDoctorNum();++i)
    {
        Doctor& d = dt_->getDoctor(i);
        for (int k=d.d_patient.size()-1; k>=0; --k)
        {
            const Patient& x = d.d_patient[k];
            if (x.p_id==p.p_id &&x.p_booking_date==p.p_booking_date &&x.p_booking_time==p.p_booking_time)
            {
                d.d_patient.removeAt(k);
                d.d_pat_num = d.d_patient.size();
            }
        }
    }
}

void BookingTableModel::appendToDoctor(int docIdx, const Patient& p)
{
    if (!dt_ || docIdx<0 || docIdx>=dt_->getDoctorNum())
        return;
    Doctor& d = dt_->getDoctor(docIdx);
    d.d_patient.append(p);
    d.d_pat_num = d.d_patient.size();
}
