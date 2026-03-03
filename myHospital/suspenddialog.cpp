// suspenddialog.cpp
#include "suspenddialog.h"
#include "ui_suspenddialog.h"
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>

SuspendDialog::SuspendDialog(QWidget *p, DoctorInfortable *doc, const QVector<SuspendItem>& initial)
    : QDialog(p), ui(new Ui::SuspendDialog), docTable(doc)
{
    setSizeGripEnabled(true);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    ui->setupUi(this);
    setWindowTitle("管理停诊");

    model = new SuspendModel(this, docTable);
    model->setAll(initial);

    setupTable();

    connect(ui->btnAdd, &QPushButton::clicked, this, &SuspendDialog::onAdd);
    connect(ui->btnDel, &QPushButton::clicked, this, &SuspendDialog::onDel);
    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

SuspendDialog::~SuspendDialog(){ delete ui; }

void SuspendDialog::setupTable()
{
    ui->tableViewSuspend->setModel(model);
    ui->tableViewSuspend->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewSuspend->verticalHeader()->setVisible(false);
    ui->tableViewSuspend->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewSuspend->setEditTriggers(QAbstractItemView::NoEditTriggers);
}



QVector<SuspendItem> SuspendDialog::result() const { return model->all(); }

void SuspendDialog::onAdd()
{
    if (!docTable || docTable->getDoctorNum()==0)
    {
        QMessageBox::warning(this,"提示","暂无医生信息"); return;
    }

    QStringList items; QVector<int> idx;
    for (int i=0;i<docTable->getDoctorNum();++i)
    {
        const Doctor &d = docTable->getDoctor(i);
        if (d.d_name.isEmpty()) continue;
        items << QString("%1（%2）").arg(d.d_name, d.d_subject);
        idx << i;
    }

    bool ok=false;
    QString choice = QInputDialog::getItem(this,"选择医生","医生：",items,0,false,&ok);
    if (!ok || choice.isEmpty())
        return;
    int sel = items.indexOf(choice);
    //选项列表items中查找choice的位置索引。
    int docIndex = idx[sel];

    QString ds = QInputDialog::getText(this,"停诊日期","格式：yyyy-MM-dd");
    QDate date = QDate::fromString(ds,"yyyy-MM-dd");
    if (!date.isValid())
    { QMessageBox::warning(this,"输入错误","日期格式不正确"); return; }

    QString reason = QInputDialog::getText(this,"停诊原因","可不填");

    SuspendItem it; it.doctorIndex=docIndex; it.date=date; it.reason=reason;
    model->add(it);

}

void SuspendDialog::onDel()
{
    auto sel = ui->tableViewSuspend->selectionModel()->selectedRows();
    if (sel.isEmpty()) return;
    model->removeRowAt(sel.first().row());

}
