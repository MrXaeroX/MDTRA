/********************************************************************************
** Form generated from reading UI file 'torsionSearchResultsDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TORSIONSEARCHRESULTSDIALOG_H
#define UI_TORSIONSEARCHRESULTSDIALOG_H

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

class Ui_torsionSearchResultsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QTableWidget *resultTable;

    void setupUi(QDialog *torsionSearchResultsDialog)
    {
        if (torsionSearchResultsDialog->objectName().isEmpty())
            torsionSearchResultsDialog->setObjectName(QString::fromUtf8("torsionSearchResultsDialog"));
        torsionSearchResultsDialog->resize(822, 494);
        buttonBox = new QDialogButtonBox(torsionSearchResultsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 460, 751, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Save);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(torsionSearchResultsDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 801, 441));
        resultTable = new QTableWidget(groupBox);
        if (resultTable->columnCount() < 7)
            resultTable->setColumnCount(7);
        if (resultTable->rowCount() < 15)
            resultTable->setRowCount(15);
        resultTable->setObjectName(QString::fromUtf8("resultTable"));
        resultTable->setGeometry(QRect(20, 30, 761, 401));
        QFont font;
        font.setPointSize(8);
        resultTable->setFont(font);
        resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        resultTable->setSelectionMode(QAbstractItemView::NoSelection);
        resultTable->setSelectionBehavior(QAbstractItemView::SelectItems);
        resultTable->setGridStyle(Qt::DotLine);
        resultTable->setCornerButtonEnabled(true);
        resultTable->setRowCount(15);
        resultTable->setColumnCount(7);
        resultTable->horizontalHeader()->setDefaultSectionSize(110);
        resultTable->horizontalHeader()->setMinimumSectionSize(20);
        resultTable->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
        resultTable->horizontalHeader()->setStretchLastSection(true);
        resultTable->verticalHeader()->setDefaultSectionSize(25);
        resultTable->verticalHeader()->setMinimumSectionSize(25);
        resultTable->verticalHeader()->setStretchLastSection(true);

        retranslateUi(torsionSearchResultsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), torsionSearchResultsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(torsionSearchResultsDialog);
    } // setupUi

    void retranslateUi(QDialog *torsionSearchResultsDialog)
    {
        torsionSearchResultsDialog->setWindowTitle(QApplication::translate("torsionSearchResultsDialog", "Torsion Search Results", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("torsionSearchResultsDialog", "Significant Torsion Angles", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class torsionSearchResultsDialog: public Ui_torsionSearchResultsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TORSIONSEARCHRESULTSDIALOG_H
