/********************************************************************************
** Form generated from reading UI file 'pcaResultsDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PCARESULTSDIALOG_H
#define UI_PCARESULTSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_pcaResultsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QTextEdit *textEdit;

    void setupUi(QDialog *pcaResultsDialog)
    {
        if (pcaResultsDialog->objectName().isEmpty())
            pcaResultsDialog->setObjectName(QString::fromUtf8("pcaResultsDialog"));
        pcaResultsDialog->resize(531, 494);
        buttonBox = new QDialogButtonBox(pcaResultsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 460, 511, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Save);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(pcaResultsDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 511, 441));
        textEdit = new QTextEdit(groupBox);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(20, 30, 471, 391));
        QPalette palette;
        QBrush brush(QColor(224, 223, 227, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        textEdit->setPalette(palette);
        textEdit->setReadOnly(true);
        QWidget::setTabOrder(textEdit, buttonBox);

        retranslateUi(pcaResultsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), pcaResultsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(pcaResultsDialog);
    } // setupUi

    void retranslateUi(QDialog *pcaResultsDialog)
    {
        pcaResultsDialog->setWindowTitle(QApplication::translate("pcaResultsDialog", "PCA Results", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("pcaResultsDialog", "Eigenvectors and eigenvalues", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class pcaResultsDialog: public Ui_pcaResultsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCARESULTSDIALOG_H
