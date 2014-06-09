/********************************************************************************
** Form generated from reading UI file 'rfid.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RFID_H
#define UI_RFID_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rfid
{
public:
    QWidget *centralWidget;
    QStackedWidget *stackedWidget;
    QWidget *Scan;
    QPushButton *pushButton_scan;
    QLabel *label;
    QWidget *Options;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QWidget *Read;
    QLineEdit *lineEdit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *rfid)
    {
        if (rfid->objectName().isEmpty())
            rfid->setObjectName(QStringLiteral("rfid"));
        rfid->resize(685, 517);
        centralWidget = new QWidget(rfid);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setGeometry(QRect(20, 0, 631, 441));
        Scan = new QWidget();
        Scan->setObjectName(QStringLiteral("Scan"));
        pushButton_scan = new QPushButton(Scan);
        pushButton_scan->setObjectName(QStringLiteral("pushButton_scan"));
        pushButton_scan->setGeometry(QRect(60, 40, 99, 27));
        label = new QLabel(Scan);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(280, 50, 221, 17));
        stackedWidget->addWidget(Scan);
        Options = new QWidget();
        Options->setObjectName(QStringLiteral("Options"));
        pushButton_2 = new QPushButton(Options);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(30, 30, 99, 27));
        pushButton_3 = new QPushButton(Options);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(40, 70, 99, 27));
        pushButton_4 = new QPushButton(Options);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setGeometry(QRect(50, 100, 99, 27));
        pushButton_5 = new QPushButton(Options);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setGeometry(QRect(50, 140, 99, 27));
        stackedWidget->addWidget(Options);
        Read = new QWidget();
        Read->setObjectName(QStringLiteral("Read"));
        lineEdit = new QLineEdit(Read);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(110, 90, 113, 27));
        stackedWidget->addWidget(Read);
        rfid->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(rfid);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 685, 25));
        rfid->setMenuBar(menuBar);
        mainToolBar = new QToolBar(rfid);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        rfid->addToolBar(Qt::TopToolBarArea, mainToolBar);
        rfid->insertToolBarBreak(mainToolBar);
        statusBar = new QStatusBar(rfid);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        rfid->setStatusBar(statusBar);

        retranslateUi(rfid);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(rfid);
    } // setupUi

    void retranslateUi(QMainWindow *rfid)
    {
        rfid->setWindowTitle(QApplication::translate("rfid", "rfid", 0));
        pushButton_scan->setText(QApplication::translate("rfid", "Scan...", 0));
        label->setText(QApplication::translate("rfid", "No RFID Tag Detected!!", 0));
        pushButton_2->setText(QApplication::translate("rfid", "Init", 0));
        pushButton_3->setText(QApplication::translate("rfid", "Install", 0));
        pushButton_4->setText(QApplication::translate("rfid", "Reuse", 0));
        pushButton_5->setText(QApplication::translate("rfid", "Read", 0));
    } // retranslateUi

};

namespace Ui {
    class rfid: public Ui_rfid {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RFID_H
