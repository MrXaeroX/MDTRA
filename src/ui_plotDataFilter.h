/********************************************************************************
** Form generated from reading UI file 'plotDataFilter.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOTDATAFILTER_H
#define UI_PLOTDATAFILTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_plotDataFilterDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_2;
    QFrame *line;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *plotDataFilterDialog)
    {
        if (plotDataFilterDialog->objectName().isEmpty())
            plotDataFilterDialog->setObjectName(QString::fromUtf8("plotDataFilterDialog"));
        plotDataFilterDialog->resize(561, 587);
        plotDataFilterDialog->setMinimumSize(QSize(560, 500));
        plotDataFilterDialog->setSizeGripEnabled(true);
        verticalLayout_3 = new QVBoxLayout(plotDataFilterDialog);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        groupBox = new QGroupBox(plotDataFilterDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(0, 0));
        verticalLayout_4 = new QVBoxLayout(groupBox);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_2);


        verticalLayout_2->addLayout(horizontalLayout);

        line = new QFrame(groupBox);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        scrollArea = new QScrollArea(groupBox);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 519, 473));
        scrollArea->setWidget(scrollAreaWidgetContents_2);

        verticalLayout_2->addWidget(scrollArea);


        verticalLayout_4->addLayout(verticalLayout_2);


        verticalLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(plotDataFilterDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        verticalLayout->addWidget(buttonBox);


        verticalLayout_3->addLayout(verticalLayout);


        retranslateUi(plotDataFilterDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), plotDataFilterDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), plotDataFilterDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(plotDataFilterDialog);
    } // setupUi

    void retranslateUi(QDialog *plotDataFilterDialog)
    {
        plotDataFilterDialog->setWindowTitle(QApplication::translate("plotDataFilterDialog", "Plot Data Visibility", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("plotDataFilterDialog", "Data Filter", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("plotDataFilterDialog", "Data", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("plotDataFilterDialog", "Estimator", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class plotDataFilterDialog: public Ui_plotDataFilterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOTDATAFILTER_H
