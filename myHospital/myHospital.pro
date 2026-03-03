QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addbooking.cpp \
    adddoctor.cpp \
    bookingtablemodel.cpp \
    doctor.cpp \
    doctorbookingsmodel.cpp \
    doctorinfortable.cpp \
    doctortablemodel.cpp \
    main.cpp \
    mainwindow.cpp \
    patient.cpp \
    patinfortable.cpp \
    searchdialog.cpp \
    searchtablemodel.cpp \
    suspend.cpp \
    suspenddialog.cpp \
    suspendmodel.cpp

HEADERS += \
    addbooking.h \
    adddoctor.h \
    bookingtablemodel.h \
    doctor.h \
    doctorbookingsmodel.h \
    doctorinfortable.h \
    doctortablemodel.h \
    mainwindow.h \
    patient.h \
    patinfortable.h \
    searchdialog.h \
    searchtablemodel.h \
    suspend.h \
    suspenddialog.h \
    suspendmodel.h

FORMS += \
    addbooking.ui \
    adddoctor.ui \
    mainwindow.ui \
    searchdialog.ui \
    suspenddialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
