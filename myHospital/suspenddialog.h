// suspenddialog.h
#ifndef SUSPENDDIALOG_H
#define SUSPENDDIALOG_H
#include <QDialog>
#include "suspendmodel.h"

namespace Ui { class SuspendDialog; }

class SuspendDialog : public QDialog {
    Q_OBJECT
public:
    explicit SuspendDialog(QWidget *p, DoctorInfortable *doc, const QVector<SuspendItem>& initial);
    ~SuspendDialog();
    QVector<SuspendItem> result() const;

private slots:
    void onAdd();
    void onDel();

private:
    Ui::SuspendDialog *ui;
    DoctorInfortable *docTable=nullptr;
    SuspendModel *model=nullptr;

    void setupTable();
    void updateStatus();
};
#endif
