/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *MainWindownSubTitle;
    QFrame *MainWindownProcessorSystemArea;
    QLabel *MainWindowComputerDisplay;
    QLabel *MainWindowComputerStand;
    QFrame *line;
    QLabel *label;
    QFrame *line_2;
    QFrame *line_3;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QFrame *line_4;
    QFrame *line_5;
    QLabel *label_5;
    QFrame *line_6;
    QFrame *line_7;
    QFrame *line_8;
    QFrame *line_9;
    QFrame *line_10;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(39, 19, 652, 511));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        MainWindownSubTitle = new QLabel(verticalLayoutWidget);
        MainWindownSubTitle->setObjectName("MainWindownSubTitle");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindownSubTitle->sizePolicy().hasHeightForWidth());
        MainWindownSubTitle->setSizePolicy(sizePolicy);
        MainWindownSubTitle->setMinimumSize(QSize(639, 30));
        MainWindownSubTitle->setLayoutDirection(Qt::LeftToRight);
        MainWindownSubTitle->setTextFormat(Qt::AutoText);
        MainWindownSubTitle->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        MainWindownSubTitle->setWordWrap(false);
        MainWindownSubTitle->setMargin(10);

        verticalLayout_2->addWidget(MainWindownSubTitle);

        MainWindownProcessorSystemArea = new QFrame(verticalLayoutWidget);
        MainWindownProcessorSystemArea->setObjectName("MainWindownProcessorSystemArea");
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(MainWindownProcessorSystemArea->sizePolicy().hasHeightForWidth());
        MainWindownProcessorSystemArea->setSizePolicy(sizePolicy1);
        MainWindownProcessorSystemArea->setMinimumSize(QSize(650, 0));
        MainWindownProcessorSystemArea->setFrameShape(QFrame::StyledPanel);
        MainWindownProcessorSystemArea->setFrameShadow(QFrame::Raised);
        MainWindowComputerDisplay = new QLabel(MainWindownProcessorSystemArea);
        MainWindowComputerDisplay->setObjectName("MainWindowComputerDisplay");
        MainWindowComputerDisplay->setGeometry(QRect(70, 130, 171, 91));
        MainWindowComputerDisplay->setStyleSheet(QString::fromUtf8("border: 200px solid black;"));
        MainWindowComputerStand = new QLabel(MainWindownProcessorSystemArea);
        MainWindowComputerStand->setObjectName("MainWindowComputerStand");
        MainWindowComputerStand->setGeometry(QRect(150, 220, 16, 41));
        MainWindowComputerStand->setStyleSheet(QString::fromUtf8("border: 2px solid black;"));
        line = new QFrame(MainWindownProcessorSystemArea);
        line->setObjectName("line");
        line->setGeometry(QRect(100, 260, 118, 3));
        line->setStyleSheet(QString::fromUtf8("border: 2px solid black;"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label = new QLabel(MainWindownProcessorSystemArea);
        label->setObjectName("label");
        label->setGeometry(QRect(110, 300, 91, 21));
        label->setStyleSheet(QString::fromUtf8("border: 2px solid black;"));
        label->setAlignment(Qt::AlignCenter);
        label->setMargin(0);
        line_2 = new QFrame(MainWindownProcessorSystemArea);
        line_2->setObjectName("line_2");
        line_2->setGeometry(QRect(80, 220, 1, 93));
        line_2->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(MainWindownProcessorSystemArea);
        line_3->setObjectName("line_3");
        line_3->setGeometry(QRect(80, 310, 30, 1));
        line_3->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        label_2 = new QLabel(MainWindownProcessorSystemArea);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(350, 190, 201, 181));
        label_2->setStyleSheet(QString::fromUtf8("border: 2px solid black;"));
        label_3 = new QLabel(MainWindownProcessorSystemArea);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(400, 280, 91, 41));
        label_3->setStyleSheet(QString::fromUtf8("border: 22px solid green;"));
        label_4 = new QLabel(MainWindownProcessorSystemArea);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(430, 285, 30, 31));
        label_4->setStyleSheet(QString::fromUtf8("border: 3px solid black;"));
        label_4->setAlignment(Qt::AlignCenter);
        line_4 = new QFrame(MainWindownProcessorSystemArea);
        line_4->setObjectName("line_4");
        line_4->setGeometry(QRect(470, 285, 1, 30));
        line_4->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_4->setFrameShape(QFrame::VLine);
        line_4->setFrameShadow(QFrame::Sunken);
        line_5 = new QFrame(MainWindownProcessorSystemArea);
        line_5->setObjectName("line_5");
        line_5->setGeometry(QRect(480, 285, 1, 30));
        line_5->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_5->setFrameShape(QFrame::VLine);
        line_5->setFrameShadow(QFrame::Sunken);
        label_5 = new QLabel(MainWindownProcessorSystemArea);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(400, 200, 71, 41));
        label_5->setStyleSheet(QString::fromUtf8("border:5px solid black;"));
        label_5->setAlignment(Qt::AlignCenter);
        line_6 = new QFrame(MainWindownProcessorSystemArea);
        line_6->setObjectName("line_6");
        line_6->setGeometry(QRect(410, 230, 1, 61));
        line_6->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_6->setFrameShape(QFrame::VLine);
        line_6->setFrameShadow(QFrame::Sunken);
        line_7 = new QFrame(MainWindownProcessorSystemArea);
        line_7->setObjectName("line_7");
        line_7->setGeometry(QRect(200, 310, 210, 1));
        line_7->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_7->setFrameShape(QFrame::HLine);
        line_7->setFrameShadow(QFrame::Sunken);
        line_8 = new QFrame(MainWindownProcessorSystemArea);
        line_8->setObjectName("line_8");
        line_8->setGeometry(QRect(200, 210, 118, 1));
        line_8->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_8->setFrameShape(QFrame::HLine);
        line_8->setFrameShadow(QFrame::Sunken);
        line_9 = new QFrame(MainWindownProcessorSystemArea);
        line_9->setObjectName("line_9");
        line_9->setGeometry(QRect(315, 210, 1, 86));
        line_9->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_9->setFrameShape(QFrame::VLine);
        line_9->setFrameShadow(QFrame::Sunken);
        line_10 = new QFrame(MainWindownProcessorSystemArea);
        line_10->setObjectName("line_10");
        line_10->setGeometry(QRect(316, 295, 95, 1));
        line_10->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        line_10->setFrameShape(QFrame::HLine);
        line_10->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(MainWindownProcessorSystemArea);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Overview", nullptr));
        MainWindownSubTitle->setText(QCoreApplication::translate("MainWindow", "Overview", nullptr));
        MainWindowComputerDisplay->setText(QString());
        MainWindowComputerStand->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "KeyBoard", nullptr));
        label_2->setText(QString());
        label_3->setText(QString());
        label_4->setText(QCoreApplication::translate("MainWindow", "CPU", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "SSD", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
