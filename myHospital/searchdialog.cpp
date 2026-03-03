#include "searchdialog.h"
#include "ui_searchdialog.h"
#include "doctor.h"
#include "patient.h"
#include <QMessageBox>
#include <QHeaderView>



SearchDialog::SearchDialog(QWidget *parent, void *table, SearchType type)
    : QDialog(parent), ui(new Ui::SearchDialog), dataTable(table), searchType(type)
{
    ui->setupUi(this);
    setSizeGripEnabled(true);

    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    setWindowTitle(searchType==SearchDoctor ? "查询医生信息" : "查询预约信息");
    ui->searchLabel->setText("请输入关键词：");

    setupTable();
    setupFieldCombo();
    connect(ui->searchButton, &QPushButton::clicked, this, &SearchDialog::on_searchButton_clicked);
    connect(ui->closeButton,  &QPushButton::clicked, this, &SearchDialog::on_closeButton_clicked);
}

SearchDialog::~SearchDialog() { delete ui; }

void SearchDialog::setupTable()
{
    model = new SearchTableModel(this, searchType==SearchDoctor ? SearchTableModel::Doctors: SearchTableModel::Patients);
    ui->tableViewSearch->setModel(model);
    auto *hv = ui->tableViewSearch->horizontalHeader();
    hv->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewSearch->verticalHeader()->setVisible(false);
    ui->tableViewSearch->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewSearch->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void SearchDialog::setupFieldCombo()
{
    ui->comboField->clear();
    if (searchType == SearchDoctor)
    {
        ui->comboField->addItem("姓名", "name");
        ui->comboField->addItem("编号", "id");
        ui->comboField->addItem("科室", "dept");
        ui->comboField->addItem("职称", "title");
        ui->comboField->setCurrentIndex(0);

    }
    else
    {
        ui->comboField->addItem("姓名", "name");
        ui->comboField->addItem("身份证号", "pid");
        ui->comboField->addItem("电话", "phone");
        ui->comboField->addItem("症状", "symptom");
        ui->comboField->setCurrentIndex(0);

    }
}

void SearchDialog::on_searchButton_clicked()
{
    const QString keyword = ui->searchEdit->text().trimmed();
    if (keyword.isEmpty()) { QMessageBox::warning(this,"输入错误","请输入搜索关键词"); return; }

    const QString fieldKey = ui->comboField->currentData().toString();
    if (fieldKey.isEmpty()) { QMessageBox::warning(this,"输入错误","请选择要搜索的项"); return; }

    if (searchType == SearchDoctor)
        searchDoctors(keyword, fieldKey);

    else
        searchPatients(keyword, fieldKey);
}

void SearchDialog::searchDoctors(const QString &keyword, const QString &field)
{
    if (!dataTable) return;
    auto *doctorTable = static_cast<DoctorInfortable*>(dataTable);
    //dataTable转换为DoctorInfortable

    QVector<Doctor> results;

    const Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    for (int i = 0; i < doctorTable->getDoctorNum(); ++i)
    {
        const Doctor d = doctorTable->getDoctor(i);
        bool ok = false;
        if (field=="name")
            ok = d.d_name.contains(keyword, cs);
        else if (field=="id")
            ok = d.d_id.contains(keyword, cs);
        else if (field=="dept")
            ok = d.d_subject.contains(keyword, cs);
        else if (field=="title")
            ok = d.d_title.contains(keyword, cs);
        if (ok) results.push_back(d);
    }

    model->setDoctorResults(results);
    ui->resultCountLabel->setText(QString("找到 %1 条结果").arg(results.size()));
}

void SearchDialog::searchPatients(const QString &keyword, const QString &field)
{
    if (!dataTable) return;
    auto *patientTable = static_cast<PatInforTable*>(dataTable);
    QVector<Patient> results;

    const Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    for (int i = 0; i < patientTable->getPatNum(); ++i)
    {
        const Patient p = patientTable->getPat(i);
        bool ok = false;
        if (field=="name")
            ok = p.p_name.contains(keyword, cs);
        else if (field=="pid")
            ok = p.p_id.contains(keyword, cs);
        else if (field=="phone")
            ok = QString::number(p.p_phone).contains(keyword, cs);
        else if (field=="symptom")
            ok = p.p_symptom.contains(keyword, cs);
        if (ok) results.push_back(p);
    }

    model->setPatientResults(results);
    ui->resultCountLabel->setText(QString("找到 %1 条结果").arg(results.size()));
}

void SearchDialog::on_closeButton_clicked() { accept(); }
