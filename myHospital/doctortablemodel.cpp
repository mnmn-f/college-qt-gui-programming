// doctortablemodel.cpp
#include "doctortablemodel.h"

QVariant DoctorTableModel::data(const QModelIndex& i, int role) const
{
    if (!ds_ || !i.isValid()) return {};
    const Doctor& d = ds_->getDoctor(i.row());
    if (role==Qt::DisplayRole || role==Qt::EditRole)
    {
        switch(i.column())
        {
        case C_NAME:  return d.d_name;
        case C_ID:    return d.d_id;
        case C_SEX:   return d.d_sex;
        case C_AGE:   return d.d_age;
        case C_TITLE: return d.d_title;
        case C_DEPT:  return d.d_subject;
        case C_START: return d.d_time_s.toString("hh:mm");
        case C_END:   return d.d_time_e.toString("hh:mm");
        }
    }
    return {};
}

QVariant DoctorTableModel::headerData(int s, Qt::Orientation o, int role) const
{
    if (o==Qt::Horizontal && role==Qt::DisplayRole){
        static const char* H[]={"姓名","编号","性别","年龄","职称","科室","上班","下班"};
        return H[s];
    }
    return QAbstractTableModel::headerData(s,o,role);
}

Qt::ItemFlags DoctorTableModel::flags(const QModelIndex& i) const
{
    if (!i.isValid()) return Qt::NoItemFlags;
    if (i.column()==C_AGE || i.column()==C_TITLE)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool DoctorTableModel::setData(const QModelIndex& i, const QVariant& v, int role){
    if (!ds_ || role!=Qt::EditRole || !i.isValid()) return false;
    Doctor& d = ds_->getDoctor(i.row());
    if (i.column()==C_AGE)   d.d_age   = v.toInt();
    else if (i.column()==C_TITLE) d.d_title = v.toString();
    else
        return false;
    emit dataChanged(i,i);
    emit doctorsEdited();
    return true;
}
