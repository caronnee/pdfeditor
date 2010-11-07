/********************************************************************************
** Form generated from reading UI file 'showPage.ui'
**
** Created: Sun Nov 7 16:07:51 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWPAGE_H
#define UI_SHOWPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QCommandLinkButton>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QWidget>
#include <MyWidget.h>

QT_BEGIN_NAMESPACE

class Ui_TabUI
{
public:
    QGridLayout *gridLayout_2;
    QScrollArea *scrollArea;
    MyWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QLabel *label;
    QGroupBox *pageManipulation;
    QHBoxLayout *horizontalLayout;
    QComboBox *Revision;
    QCommandLinkButton *previous;
    QLineEdit *pageInfo;
    QCommandLinkButton *next;
    QPushButton *commit;
    QPushButton *branchRevision;

    void setupUi(QWidget *TabUI)
    {
        if (TabUI->objectName().isEmpty())
            TabUI->setObjectName(QString::fromUtf8("TabUI"));
        TabUI->resize(459, 339);
        gridLayout_2 = new QGridLayout(TabUI);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        scrollArea = new QScrollArea(TabUI);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new MyWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 439, 246));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout_2->addWidget(scrollArea, 0, 0, 1, 1);

        pageManipulation = new QGroupBox(TabUI);
        pageManipulation->setObjectName(QString::fromUtf8("pageManipulation"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pageManipulation->sizePolicy().hasHeightForWidth());
        pageManipulation->setSizePolicy(sizePolicy);
        pageManipulation->setMinimumSize(QSize(14, 40));
        pageManipulation->setContextMenuPolicy(Qt::NoContextMenu);
        pageManipulation->setAlignment(Qt::AlignCenter);
        pageManipulation->setFlat(true);
        pageManipulation->setCheckable(false);
        horizontalLayout = new QHBoxLayout(pageManipulation);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Revision = new QComboBox(pageManipulation);
        Revision->setObjectName(QString::fromUtf8("Revision"));
        sizePolicy.setHeightForWidth(Revision->sizePolicy().hasHeightForWidth());
        Revision->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(Revision);

        previous = new QCommandLinkButton(pageManipulation);
        previous->setObjectName(QString::fromUtf8("previous"));

        horizontalLayout->addWidget(previous);

        pageInfo = new QLineEdit(pageManipulation);
        pageInfo->setObjectName(QString::fromUtf8("pageInfo"));
        pageInfo->setContextMenuPolicy(Qt::NoContextMenu);
        pageInfo->setAlignment(Qt::AlignCenter);
        pageInfo->setReadOnly(true);

        horizontalLayout->addWidget(pageInfo);

        next = new QCommandLinkButton(pageManipulation);
        next->setObjectName(QString::fromUtf8("next"));
        next->setBaseSize(QSize(10, 30));

        horizontalLayout->addWidget(next);

        commit = new QPushButton(pageManipulation);
        commit->setObjectName(QString::fromUtf8("commit"));

        horizontalLayout->addWidget(commit);

        branchRevision = new QPushButton(pageManipulation);
        branchRevision->setObjectName(QString::fromUtf8("branchRevision"));

        horizontalLayout->addWidget(branchRevision);


        gridLayout_2->addWidget(pageManipulation, 1, 0, 1, 1);


        retranslateUi(TabUI);

        QMetaObject::connectSlotsByName(TabUI);
    } // setupUi

    void retranslateUi(QWidget *TabUI)
    {
        TabUI->setWindowTitle(QApplication::translate("TabUI", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TabUI", "TextLabel", 0, QApplication::UnicodeUTF8));
        pageManipulation->setTitle(QApplication::translate("TabUI", "Page Info", 0, QApplication::UnicodeUTF8));
        previous->setText(QApplication::translate("TabUI", "Previous", 0, QApplication::UnicodeUTF8));
        next->setText(QApplication::translate("TabUI", "Next", 0, QApplication::UnicodeUTF8));
        commit->setText(QApplication::translate("TabUI", "Commit", 0, QApplication::UnicodeUTF8));
        branchRevision->setText(QApplication::translate("TabUI", "export revision", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TabUI: public Ui_TabUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWPAGE_H
