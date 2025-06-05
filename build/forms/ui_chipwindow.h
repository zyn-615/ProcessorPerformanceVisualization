/********************************************************************************
** Form generated from reading UI file 'chipwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHIPWINDOW_H
#define UI_CHIPWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChipWindow
{
public:
    QWidget *centralwidget;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ChipWindow)
    {
        if (ChipWindow->objectName().isEmpty())
            ChipWindow->setObjectName("ChipWindow");
        ChipWindow->resize(800, 600);
        centralwidget = new QWidget(ChipWindow);
        centralwidget->setObjectName("centralwidget");
        ChipWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(ChipWindow);
        statusbar->setObjectName("statusbar");
        ChipWindow->setStatusBar(statusbar);

        retranslateUi(ChipWindow);

        QMetaObject::connectSlotsByName(ChipWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ChipWindow)
    {
        ChipWindow->setWindowTitle(QCoreApplication::translate("ChipWindow", "ChipWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChipWindow: public Ui_ChipWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHIPWINDOW_H
