/********************************************************************************
** Form generated from reading UI file 'distanceSearchResultsDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISTANCESEARCHRESULTSDIALOG_H
#define UI_DISTANCESEARCHRESULTSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_distanceSearchResultsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QTableWidget *resultTable;

    void setupUi(QDialog *distanceSearchResultsDialog)
    {
        if (distanceSearchResultsDialog->objectName().isEmpty())
            distanceSearchResultsDialog->setObjectName(QString::fromUtf8("distanceSearchResultsDialog"));
        distanceSearchResultsDialog->resize(770, 494);
        buttonBox = new QDialogButtonBox(distanceSearchResultsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 460, 751, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Save);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(distanceSearchResultsDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 751, 441));
        resultTable = new QTableWidget(groupBox);
        if (resultTable->columnCount() < 6)
            resultTable->setColumnCount(6);
        if (resultTable->rowCount() < 15)
            resultTable->setRowCount(15);
        resultTable->setObjectName(QString::fromUtf8("resultTable"));
        resultTable->setGeometry(QRect(20, 30, 711, 401));
        QFont font;
        font.setPointSize(8);
        resultTable->setFont(font);
        resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        resultTable->setSelectionMode(QAbstractItemView::NoSelection);
        resultTable->setSelectionBehavior(QAbstractItemView::SelectItems);
        resultTable->setGridStyle(Qt::DotLine);
        resultTable->setCornerButtonEnabled(true);
        resultTable->setRowCount(15);
        resultTable->setColumnCount(6);
        resultTable->horizontalHeader()->setDefaultSectionSize(122);
        resultTable->horizontalHeader()->setMinimumSectionSize(20);
        resultTable->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
        resultTable->horizontalHeader()->setStretchLastSection(true);
        resultTable->verticalHeader()->setDefaultSectionSize(25);
        resultTable->verticalHeader()->setMinimumSectionSize(25);
        resultTable->verticalHeader()->setStretchLastSection(true);

        retranslateUi(distanceSearchResultsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), distanceSearchResultsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(distanceSearchResultsDialog);
    } // setupUi

    void retranslateUi(QDialog *distanceSearchResultsDialog)
    {
        distanceSearchResultsDialog->setWindowTitle(QApplication::translate("distanceSearchResultsDialog", "Distance Search Results", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("distanceSearchResultsDialog", "Significant Pair List", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class distanceSearchResultsDialog: public Ui_distanceSearchResultsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISTANCESEARCHRESULTSDIALOG_H
