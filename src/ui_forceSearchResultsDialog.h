/********************************************************************************
** Form generated from reading UI file 'forceSearchResultsDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORCESEARCHRESULTSDIALOG_H
#define UI_FORCESEARCHRESULTSDIALOG_H

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

class Ui_forceSearchResultsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QTableWidget *resultTable;

    void setupUi(QDialog *forceSearchResultsDialog)
    {
        if (forceSearchResultsDialog->objectName().isEmpty())
            forceSearchResultsDialog->setObjectName(QString::fromUtf8("forceSearchResultsDialog"));
        forceSearchResultsDialog->resize(651, 494);
        buttonBox = new QDialogButtonBox(forceSearchResultsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 460, 631, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Save);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(forceSearchResultsDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 631, 441));
        resultTable = new QTableWidget(groupBox);
        if (resultTable->columnCount() < 5)
            resultTable->setColumnCount(5);
        if (resultTable->rowCount() < 15)
            resultTable->setRowCount(15);
        resultTable->setObjectName(QString::fromUtf8("resultTable"));
        resultTable->setGeometry(QRect(20, 30, 591, 401));
        QFont font;
        font.setPointSize(8);
        resultTable->setFont(font);
        resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        resultTable->setSelectionMode(QAbstractItemView::NoSelection);
        resultTable->setSelectionBehavior(QAbstractItemView::SelectItems);
        resultTable->setGridStyle(Qt::DotLine);
        resultTable->setCornerButtonEnabled(true);
        resultTable->setRowCount(15);
        resultTable->setColumnCount(5);
        resultTable->horizontalHeader()->setDefaultSectionSize(122);
        resultTable->horizontalHeader()->setMinimumSectionSize(20);
        resultTable->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
        resultTable->horizontalHeader()->setStretchLastSection(true);
        resultTable->verticalHeader()->setDefaultSectionSize(25);
        resultTable->verticalHeader()->setMinimumSectionSize(25);
        resultTable->verticalHeader()->setStretchLastSection(true);

        retranslateUi(forceSearchResultsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), forceSearchResultsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(forceSearchResultsDialog);
    } // setupUi

    void retranslateUi(QDialog *forceSearchResultsDialog)
    {
        forceSearchResultsDialog->setWindowTitle(QApplication::translate("forceSearchResultsDialog", "Force Search Results", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("forceSearchResultsDialog", "Significant Atom List", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class forceSearchResultsDialog: public Ui_forceSearchResultsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORCESEARCHRESULTSDIALOG_H
