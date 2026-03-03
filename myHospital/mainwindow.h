#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include "doctortablemodel.h"
#include "bookingtablemodel.h"
#include "doctorbookingsmodel.h"

#include "adddoctor.h"
#include "addbooking.h"
#include "suspenddialog.h"
#include "searchdialog.h"




#include "doctorinfortable.h"
#include "patinfortable.h"
#include "bookingtablemodel.h"
#include "doctortablemodel.h"
#include "doctorbookingsmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;



    DoctorInfortable doctorTable;
    PatInforTable    patientTable;

    QStackedWidget   *stacked_          = nullptr;
    QTableView       *viewBookings_     = nullptr;  // 总预约
    QTableView       *viewDoctors_      = nullptr;  // 医生总表
    QTableView       *viewDocBookings_  = nullptr;  // 某医生的预约

    BookingTableModel     *modelBookings_     = nullptr;
    DoctorTableModel      *modelDoctors_      = nullptr;
    DoctorBookingsModel   *modelDocBookings_  = nullptr;

    int pickBookingIndex(QWidget* parent);
    void loadData();
    void saveData();
    void updateStatusBar();

    QVector<SuspendItem> m_suspends;

    void buildCentralViews();
    void bindModels();

private slots:

    void on_actionDoctorEdit_triggered();     // 修改医生
    void on_actionDoctorDelete_triggered();   // 删除医生

    void on_actionViewBookings_triggered();
    void on_actionViewDoctors_triggered();
    void on_actionViewDoctorBookings_triggered();


    void on_actionSearchBookings_triggered();
    void on_actionSearchDoctors_triggered();

    void on_actionBookingAdd_triggered();
    void on_actionBookingEdit_triggered();
    void on_actionBookingDel_triggered();
    void on_actionDoctorAdd_triggered();

    void on_actionManageSuspend_triggered();

    void on_action_O_triggered();
    void on_action_S_triggered();

};

#endif
