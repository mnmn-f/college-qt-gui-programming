#include "doctor.h"

Doctor::Doctor() : d_age(0), d_pat_num(0)
{
    // 默认周表不启用
    for (int i = 0; i < 7; ++i) {
        weekStart[i]   = QTime();
        weekEnd[i]     = QTime();
        weekEnabled[i] = false;
    }
}

Doctor::Doctor(const Doctor &doc)
{
    d_name    = doc.d_name;
    d_id      = doc.d_id;
    d_sex     = doc.d_sex;
    d_age     = doc.d_age;
    d_title   = doc.d_title;
    d_subject = doc.d_subject;
    d_time_s  = doc.d_time_s;
    d_time_e  = doc.d_time_e;

    d_patient = doc.d_patient;
    d_pat_num = doc.d_pat_num;

    for (int i = 0; i < 7; ++i)
    {
        weekStart[i]   = doc.weekStart[i];
        weekEnd[i]     = doc.weekEnd[i];
        weekEnabled[i] = doc.weekEnabled[i];
    }
}

Doctor& Doctor::operator=(const Doctor &doc)
{
    if (this != &doc) {
        d_name    = doc.d_name;
        d_id      = doc.d_id;
        d_sex     = doc.d_sex;
        d_age     = doc.d_age;
        d_title   = doc.d_title;
        d_subject = doc.d_subject;
        d_time_s  = doc.d_time_s;
        d_time_e  = doc.d_time_e;

        d_patient = doc.d_patient;
        d_pat_num = doc.d_pat_num;

        for (int i = 0; i < 7; ++i) {
            weekStart[i]   = doc.weekStart[i];
            weekEnd[i]     = doc.weekEnd[i];
            weekEnabled[i] = doc.weekEnabled[i];
        }
    }
    return *this;
}

Doctor::~Doctor() {}

void Doctor::saveDoctor(QTextStream &s) const
{
    s << d_name    << "\n";
    s << d_id      << "\n";
    s << d_sex     << "\n";
    s << d_age     << "\n";
    s << d_title   << "\n";
    s << d_subject << "\n";
    s << d_time_s.toString("hh:mm") << "\n";
    s << d_time_e.toString("hh:mm") << "\n";


    s << d_patient.size() << "\n";
    for (const Patient &p : d_patient)
    {
        p.savePatient(s);
    }

    //无效用 "-" 占位，保证行数固定
    for (int i = 0; i < 7; ++i) {
        s << (weekStart[i].isValid() ? weekStart[i].toString("hh:mm") : "-") << "\n";
    }
    for (int i = 0; i < 7; ++i) {
        s << (weekEnd[i].isValid() ? weekEnd[i].toString("hh:mm") : "-") << "\n";
    }
}

void Doctor::readDoctor(QTextStream &s)
{
    d_name    = s.readLine();
    d_id      = s.readLine();
    d_sex     = s.readLine();
    d_age     = s.readLine().toInt();
    d_title   = s.readLine();
    d_subject = s.readLine();

    d_time_s  = QTime::fromString(s.readLine(), "hh:mm");
    d_time_e  = QTime::fromString(s.readLine(), "hh:mm");
    const int n = s.readLine().toInt();
    d_patient.clear();
    d_patient.reserve(n);
    for (int i = 0; i < n; ++i) {
        Patient p; p.readPatient(s);
        d_patient.push_back(p);
    }
    d_pat_num = d_patient.size();

    for (int i = 0; i < 7; ++i) {
        if (s.atEnd()) { weekStart[i] = QTime(); continue; }
        const QString t = s.readLine();
        weekStart[i] = (t.isEmpty() || t == "-") ? QTime() : QTime::fromString(t, "hh:mm");
    }
    for (int i = 0; i < 7; ++i) {
        if (s.atEnd()) { weekEnd[i] = QTime(); continue; }
        const QString t = s.readLine();
        weekEnd[i] = (t.isEmpty() || t == "-") ? QTime() : QTime::fromString(t, "hh:mm");
    }

    // 读取完后推断 weekEnabled
    for (int i = 0; i < 7; ++i) {
        weekEnabled[i] = (weekStart[i].isValid() &&
                          weekEnd[i].isValid()   &&
                          weekStart[i] < weekEnd[i]);
    }
}


void Doctor::workTimeFor(const QDate &day, QTime &outStart, QTime &outEnd) const
{
    outStart = QTime();
    outEnd   = QTime();

    const int idx = day.dayOfWeek() - 1; // 0..6 (Mon..Sun)
    if (idx < 0 || idx >= 7) return;


    if (weekEnabled[idx])
    {
        const bool hasValidCustom =
            weekStart[idx].isValid() &&
            weekEnd[idx].isValid() &&
            weekStart[idx] < weekEnd[idx];

        if (hasValidCustom)
        {

            outStart = weekStart[idx];
            outEnd   = weekEnd[idx];
            return;
        } else
        {
            // 勾选但自定义时段无效（如 00:00~00:00）→ 视为当天不上班
            return;
        }
    }


    if (d_time_s.isValid() && d_time_e.isValid() && d_time_s < d_time_e) {
        outStart = d_time_s;
        outEnd   = d_time_e;
        return;
    }

}

