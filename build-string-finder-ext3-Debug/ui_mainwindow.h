/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSelect_and_index_directory;
    QAction *actionSearch;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *directory_hlayout;
    QLineEdit *directoryLineEdit;
    QPushButton *directoryButton;
    QFrame *line;
    QHBoxLayout *search_hlayout;
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
    QTreeWidget *treeWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(512, 512);
        actionSelect_and_index_directory = new QAction(MainWindow);
        actionSelect_and_index_directory->setObjectName(QStringLiteral("actionSelect_and_index_directory"));
        actionSearch = new QAction(MainWindow);
        actionSearch->setObjectName(QStringLiteral("actionSearch"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        directory_hlayout = new QHBoxLayout();
        directory_hlayout->setSpacing(6);
        directory_hlayout->setObjectName(QStringLiteral("directory_hlayout"));
        directoryLineEdit = new QLineEdit(centralWidget);
        directoryLineEdit->setObjectName(QStringLiteral("directoryLineEdit"));
        directoryLineEdit->setAlignment(Qt::AlignCenter);
        directoryLineEdit->setReadOnly(true);

        directory_hlayout->addWidget(directoryLineEdit);

        directoryButton = new QPushButton(centralWidget);
        directoryButton->setObjectName(QStringLiteral("directoryButton"));

        directory_hlayout->addWidget(directoryButton);


        verticalLayout->addLayout(directory_hlayout);

        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        search_hlayout = new QHBoxLayout();
        search_hlayout->setSpacing(6);
        search_hlayout->setObjectName(QStringLiteral("search_hlayout"));
        searchLineEdit = new QLineEdit(centralWidget);
        searchLineEdit->setObjectName(QStringLiteral("searchLineEdit"));
        searchLineEdit->setAlignment(Qt::AlignCenter);

        search_hlayout->addWidget(searchLineEdit);

        searchButton = new QPushButton(centralWidget);
        searchButton->setObjectName(QStringLiteral("searchButton"));

        search_hlayout->addWidget(searchButton);


        verticalLayout->addLayout(search_hlayout);

        treeWidget = new QTreeWidget(centralWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->header()->setVisible(true);

        verticalLayout->addWidget(treeWidget);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(directoryButton, SIGNAL(clicked()), actionSelect_and_index_directory, SLOT(trigger()));
        QObject::connect(searchButton, SIGNAL(clicked()), actionSearch, SLOT(trigger()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        actionSelect_and_index_directory->setText(QApplication::translate("MainWindow", "Select and index directory", Q_NULLPTR));
        actionSearch->setText(QApplication::translate("MainWindow", "Search", Q_NULLPTR));
        directoryLineEdit->setPlaceholderText(QApplication::translate("MainWindow", "choose a directory", Q_NULLPTR));
        directoryButton->setText(QApplication::translate("MainWindow", "Select and index directory", Q_NULLPTR));
        searchLineEdit->setPlaceholderText(QApplication::translate("MainWindow", "enter what you want to find", Q_NULLPTR));
        searchButton->setText(QApplication::translate("MainWindow", "Search", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
