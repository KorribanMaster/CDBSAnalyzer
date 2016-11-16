/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *mpaFileLabel;
    QToolButton *mpaLoadTool;
    QLabel *outputFileLabel;
    QPushButton *loadButton;
    QToolButton *saveToolButton;
    QLineEdit *outputFileEdit;
    QLineEdit *mpaFileEdit;
    QPushButton *saveButton;
    QToolButton *settingsToolButton;
    QLineEdit *settingsEdit;
    QLabel *settingsLabel;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QLineEdit *roiEdit;
    QLineEdit *binEdit;
    QLabel *binWidthLabel;
    QLabel *roiWidthLabel;
    QPushButton *computeButton;
    QMenuBar *menubar;
    QMenu *menuSettings;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(20, 20, 301, 83));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        mpaFileLabel = new QLabel(gridLayoutWidget);
        mpaFileLabel->setObjectName(QStringLiteral("mpaFileLabel"));

        gridLayout->addWidget(mpaFileLabel, 0, 0, 1, 1);

        mpaLoadTool = new QToolButton(gridLayoutWidget);
        mpaLoadTool->setObjectName(QStringLiteral("mpaLoadTool"));

        gridLayout->addWidget(mpaLoadTool, 0, 2, 1, 1);

        outputFileLabel = new QLabel(gridLayoutWidget);
        outputFileLabel->setObjectName(QStringLiteral("outputFileLabel"));

        gridLayout->addWidget(outputFileLabel, 1, 0, 1, 1);

        loadButton = new QPushButton(gridLayoutWidget);
        loadButton->setObjectName(QStringLiteral("loadButton"));

        gridLayout->addWidget(loadButton, 0, 3, 1, 1);

        saveToolButton = new QToolButton(gridLayoutWidget);
        saveToolButton->setObjectName(QStringLiteral("saveToolButton"));

        gridLayout->addWidget(saveToolButton, 1, 2, 1, 1);

        outputFileEdit = new QLineEdit(gridLayoutWidget);
        outputFileEdit->setObjectName(QStringLiteral("outputFileEdit"));

        gridLayout->addWidget(outputFileEdit, 1, 1, 1, 1);

        mpaFileEdit = new QLineEdit(gridLayoutWidget);
        mpaFileEdit->setObjectName(QStringLiteral("mpaFileEdit"));

        gridLayout->addWidget(mpaFileEdit, 0, 1, 1, 1);

        saveButton = new QPushButton(gridLayoutWidget);
        saveButton->setObjectName(QStringLiteral("saveButton"));

        gridLayout->addWidget(saveButton, 1, 3, 1, 1);

        settingsToolButton = new QToolButton(gridLayoutWidget);
        settingsToolButton->setObjectName(QStringLiteral("settingsToolButton"));

        gridLayout->addWidget(settingsToolButton, 2, 2, 1, 1);

        settingsEdit = new QLineEdit(gridLayoutWidget);
        settingsEdit->setObjectName(QStringLiteral("settingsEdit"));

        gridLayout->addWidget(settingsEdit, 2, 1, 1, 1);

        settingsLabel = new QLabel(gridLayoutWidget);
        settingsLabel->setObjectName(QStringLiteral("settingsLabel"));

        gridLayout->addWidget(settingsLabel, 2, 0, 1, 1);

        gridLayoutWidget_2 = new QWidget(centralwidget);
        gridLayoutWidget_2->setObjectName(QStringLiteral("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(20, 120, 221, 80));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        roiEdit = new QLineEdit(gridLayoutWidget_2);
        roiEdit->setObjectName(QStringLiteral("roiEdit"));

        gridLayout_2->addWidget(roiEdit, 0, 1, 1, 1);

        binEdit = new QLineEdit(gridLayoutWidget_2);
        binEdit->setObjectName(QStringLiteral("binEdit"));

        gridLayout_2->addWidget(binEdit, 1, 1, 1, 1);

        binWidthLabel = new QLabel(gridLayoutWidget_2);
        binWidthLabel->setObjectName(QStringLiteral("binWidthLabel"));

        gridLayout_2->addWidget(binWidthLabel, 1, 0, 1, 1);

        roiWidthLabel = new QLabel(gridLayoutWidget_2);
        roiWidthLabel->setObjectName(QStringLiteral("roiWidthLabel"));

        gridLayout_2->addWidget(roiWidthLabel, 0, 0, 1, 1);

        computeButton = new QPushButton(gridLayoutWidget_2);
        computeButton->setObjectName(QStringLiteral("computeButton"));

        gridLayout_2->addWidget(computeButton, 0, 2, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 20));
        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName(QStringLiteral("menuSettings"));
        menuSettings->setEnabled(true);
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuSettings->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        mpaFileLabel->setText(QApplication::translate("MainWindow", "mpa file", 0));
        mpaLoadTool->setText(QApplication::translate("MainWindow", "...", 0));
        outputFileLabel->setText(QApplication::translate("MainWindow", "output file", 0));
        loadButton->setText(QApplication::translate("MainWindow", "Load", 0));
        saveToolButton->setText(QApplication::translate("MainWindow", "...", 0));
        saveButton->setText(QApplication::translate("MainWindow", "Save", 0));
        settingsToolButton->setText(QApplication::translate("MainWindow", "...", 0));
        settingsLabel->setText(QApplication::translate("MainWindow", "settings", 0));
        binWidthLabel->setText(QApplication::translate("MainWindow", "bin width", 0));
        roiWidthLabel->setText(QApplication::translate("MainWindow", "roi width", 0));
        computeButton->setText(QApplication::translate("MainWindow", "Compute", 0));
        menuSettings->setTitle(QApplication::translate("MainWindow", "Settings", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
