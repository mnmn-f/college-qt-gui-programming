#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include "doctorinfortable.h"
#include "patinfortable.h"
#include "searchtablemodel.h"

namespace Ui { class SearchDialog; }

class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    enum SearchType { SearchDoctor, SearchPatient };
    explicit SearchDialog(QWidget *parent, void *table, SearchType type);
    ~SearchDialog();

private slots:
    void on_searchButton_clicked();
    void on_closeButton_clicked();

private:
    Ui::SearchDialog *ui;
    void *dataTable = nullptr;
    SearchType searchType;

    SearchTableModel *model = nullptr;

    void setupTable();
    void setupFieldCombo();

    void searchDoctors(const QString &keyword, const QString &field);
    void searchPatients(const QString &keyword, const QString &field);
};

#endif // SEARCHDIALOG_H
