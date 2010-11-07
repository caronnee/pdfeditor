/********************************************************************************
** Form generated from reading UI file 'pdfgui.ui'
**
** Created: Sun Nov 7 16:07:51 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PDFGUI_H
#define UI_PDFGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <openpdf.h>

QT_BEGIN_NAMESPACE

class Ui_pdfGuiClass
{
public:
    QAction *actionOpen;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *FileManipulation;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *openFile;
    QPushButton *saveButton;
    QPushButton *SaveAs;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QPushButton *display;
    QPushButton *rotate;
    QGroupBox *pageManipulation;
    QGridLayout *gridLayout;
    QPushButton *pageUp;
    QPushButton *pageDown;
    QPushButton *insertEmpty;
    QPushButton *deletePage;
    QPushButton *insertRange;
    OpenPdf *openedPdfs;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *pdfGuiClass)
    {
        if (pdfGuiClass->objectName().isEmpty())
            pdfGuiClass->setObjectName(QString::fromUtf8("pdfGuiClass"));
        pdfGuiClass->resize(444, 401);
        pdfGuiClass->setMinimumSize(QSize(33, 30));
        pdfGuiClass->setAutoFillBackground(false);
        actionOpen = new QAction(pdfGuiClass);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        centralWidget = new QWidget(pdfGuiClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        FileManipulation = new QGroupBox(centralWidget);
        FileManipulation->setObjectName(QString::fromUtf8("FileManipulation"));
        FileManipulation->setAlignment(Qt::AlignCenter);
        horizontalLayout_2 = new QHBoxLayout(FileManipulation);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        openFile = new QPushButton(FileManipulation);
        openFile->setObjectName(QString::fromUtf8("openFile"));

        horizontalLayout_2->addWidget(openFile);

        saveButton = new QPushButton(FileManipulation);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        horizontalLayout_2->addWidget(saveButton);

        SaveAs = new QPushButton(FileManipulation);
        SaveAs->setObjectName(QString::fromUtf8("SaveAs"));

        horizontalLayout_2->addWidget(SaveAs);


        verticalLayout->addWidget(FileManipulation);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setAlignment(Qt::AlignCenter);
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        display = new QPushButton(groupBox);
        display->setObjectName(QString::fromUtf8("display"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(display->sizePolicy().hasHeightForWidth());
        display->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(display);

        rotate = new QPushButton(groupBox);
        rotate->setObjectName(QString::fromUtf8("rotate"));

        horizontalLayout->addWidget(rotate);


        verticalLayout->addWidget(groupBox);

        pageManipulation = new QGroupBox(centralWidget);
        pageManipulation->setObjectName(QString::fromUtf8("pageManipulation"));
        pageManipulation->setAlignment(Qt::AlignCenter);
        gridLayout = new QGridLayout(pageManipulation);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pageUp = new QPushButton(pageManipulation);
        pageUp->setObjectName(QString::fromUtf8("pageUp"));

        gridLayout->addWidget(pageUp, 0, 1, 1, 1);

        pageDown = new QPushButton(pageManipulation);
        pageDown->setObjectName(QString::fromUtf8("pageDown"));

        gridLayout->addWidget(pageDown, 0, 3, 1, 1);

        insertEmpty = new QPushButton(pageManipulation);
        insertEmpty->setObjectName(QString::fromUtf8("insertEmpty"));

        gridLayout->addWidget(insertEmpty, 0, 4, 1, 1);

        deletePage = new QPushButton(pageManipulation);
        deletePage->setObjectName(QString::fromUtf8("deletePage"));

        gridLayout->addWidget(deletePage, 0, 6, 1, 1);

        insertRange = new QPushButton(pageManipulation);
        insertRange->setObjectName(QString::fromUtf8("insertRange"));

        gridLayout->addWidget(insertRange, 0, 5, 1, 1);


        verticalLayout->addWidget(pageManipulation);

        openedPdfs = new OpenPdf(centralWidget);
        openedPdfs->setObjectName(QString::fromUtf8("openedPdfs"));
        openedPdfs->setAcceptDrops(true);
        openedPdfs->setTabPosition(QTabWidget::North);
        openedPdfs->setTabShape(QTabWidget::Triangular);
        openedPdfs->setTabsClosable(true);
        openedPdfs->setMovable(true);

        verticalLayout->addWidget(openedPdfs);

        pdfGuiClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(pdfGuiClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 444, 20));
        pdfGuiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(pdfGuiClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        pdfGuiClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(pdfGuiClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        pdfGuiClass->setStatusBar(statusBar);

        retranslateUi(pdfGuiClass);
        QObject::connect(openedPdfs, SIGNAL(tabCloseRequested(int)), openedPdfs, SLOT(closeAndRemoveTab(int)));
        QObject::connect(openFile, SIGNAL(clicked()), openedPdfs, SLOT(openAnotherPdf()));
        QObject::connect(saveButton, SIGNAL(clicked()), openedPdfs, SLOT(save()));
        QObject::connect(SaveAs, SIGNAL(clicked()), openedPdfs, SLOT(saveAs()));
        QObject::connect(display, SIGNAL(clicked()), openedPdfs, SLOT(display()));
        QObject::connect(rotate, SIGNAL(clicked()), openedPdfs, SLOT(rotate()));
        QObject::connect(insertEmpty, SIGNAL(clicked()), openedPdfs, SLOT(insertEmoty()));
        QObject::connect(insertRange, SIGNAL(clicked()), openedPdfs, SLOT(insertRange()));
        QObject::connect(deletePage, SIGNAL(clicked()), openedPdfs, SLOT(deletePage()));

        openedPdfs->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(pdfGuiClass);
    } // setupUi

    void retranslateUi(QMainWindow *pdfGuiClass)
    {
        pdfGuiClass->setWindowTitle(QApplication::translate("pdfGuiClass", "pdfGui", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("pdfGuiClass", "Open", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        FileManipulation->setToolTip(QApplication::translate("pdfGuiClass", "FileManupulation", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        FileManipulation->setTitle(QApplication::translate("pdfGuiClass", "File Manipulation", 0, QApplication::UnicodeUTF8));
        openFile->setText(QApplication::translate("pdfGuiClass", "Open", 0, QApplication::UnicodeUTF8));
        saveButton->setText(QApplication::translate("pdfGuiClass", "Save", 0, QApplication::UnicodeUTF8));
        SaveAs->setText(QApplication::translate("pdfGuiClass", "Save as", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("pdfGuiClass", "Display manipulation", 0, QApplication::UnicodeUTF8));
        display->setText(QApplication::translate("pdfGuiClass", "Display", 0, QApplication::UnicodeUTF8));
        rotate->setText(QApplication::translate("pdfGuiClass", "Rotate", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pageManipulation->setToolTip(QApplication::translate("pdfGuiClass", "manipulation with page", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pageManipulation->setTitle(QApplication::translate("pdfGuiClass", "Page manipulation", 0, QApplication::UnicodeUTF8));
        pageUp->setText(QApplication::translate("pdfGuiClass", "Up", 0, QApplication::UnicodeUTF8));
        pageDown->setText(QApplication::translate("pdfGuiClass", "Down", 0, QApplication::UnicodeUTF8));
        insertEmpty->setText(QApplication::translate("pdfGuiClass", "Insert Empty", 0, QApplication::UnicodeUTF8));
        deletePage->setText(QApplication::translate("pdfGuiClass", "DeletePage", 0, QApplication::UnicodeUTF8));
        insertRange->setText(QApplication::translate("pdfGuiClass", "Insert Range", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class pdfGuiClass: public Ui_pdfGuiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PDFGUI_H
