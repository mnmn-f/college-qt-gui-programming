#ifndef DOCINFORTABLE_H
#define DOCINFORTABLE_H

#include <QDialog>

namespace Ui {
class DocInforTable;
}

class DocInforTable : public QDialog
{
    Q_OBJECT

public:
    explicit DocInforTable(QWidget *parent = nullptr);
    ~DocInforTable();

    /*
    bool readSportsmanFromFile(const QString &readFileName);
    bool saveSportsmanToFile(const QString &saveFileName);

    void addSportman(CSportMan & sportman);
    CSportMan & getSportMan(int index);
    int getSportsmanNum();

protected:
    QVector <CSportMan> m_sportmans; //运动员的列表
    int m_number; //运动员的总数
     */

    bool readDocFromFile
};

#endif // DOCINFORTABLE_H
