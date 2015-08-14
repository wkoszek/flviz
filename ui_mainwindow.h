/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Tue 23. Jun 10:43:53 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpacerItem>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <flvizgv.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *buttonOpen;
    QPushButton *buttonExit;
    QFrame *lineSimulation;
    QLabel *labelSimulation;
    QPushButton *buttonRestart;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    FLVizGv *grv;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(749, 466);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setMargin(11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        buttonOpen = new QPushButton(centralWidget);
        buttonOpen->setObjectName(QString::fromUtf8("buttonOpen"));

        verticalLayout->addWidget(buttonOpen);

        buttonExit = new QPushButton(centralWidget);
        buttonExit->setObjectName(QString::fromUtf8("buttonExit"));

        verticalLayout->addWidget(buttonExit);

        lineSimulation = new QFrame(centralWidget);
        lineSimulation->setObjectName(QString::fromUtf8("lineSimulation"));
        lineSimulation->setFrameShape(QFrame::HLine);
        lineSimulation->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(lineSimulation);

        labelSimulation = new QLabel(centralWidget);
        labelSimulation->setObjectName(QString::fromUtf8("labelSimulation"));
        labelSimulation->setTextFormat(Qt::RichText);
        labelSimulation->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelSimulation);

        buttonRestart = new QPushButton(centralWidget);
        buttonRestart->setObjectName(QString::fromUtf8("buttonRestart"));

        verticalLayout->addWidget(buttonRestart);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 2);

        grv = new FLVizGv(centralWidget);
        grv->setObjectName(QString::fromUtf8("grv"));

        gridLayout->addWidget(grv, 0, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        buttonOpen->setText(QApplication::translate("MainWindow", "Otw\303\263rz", 0));
        buttonExit->setText(QApplication::translate("MainWindow", "Wyjd\305\272", 0));
        labelSimulation->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">Symulacja</span></p></body></html>", 0));
        buttonRestart->setText(QApplication::translate("MainWindow", "Restart", 0));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
