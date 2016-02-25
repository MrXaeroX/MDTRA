/********************************************************************************
** Form generated from reading UI file 'labelDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LABELDIALOG_H
#define UI_LABELDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_labelDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_3;
    QLabel *label;
    QPlainTextEdit *teLabel;
    QLabel *label_2;
    QComboBox *dsCombo;
    QLabel *label_3;
    QSpinBox *spinBox;
    QGroupBox *groupBox;
    QCheckBox *cbBorder;
    QCheckBox *cbSolid;
    QCheckBox *cbConnect;
    QCheckBox *cbVertline;

    void setupUi(QDialog *labelDialog)
    {
        if (labelDialog->objectName().isEmpty())
            labelDialog->setObjectName(QString::fromUtf8("labelDialog"));
        labelDialog->resize(391, 363);
        buttonBox = new QDialogButtonBox(labelDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 330, 371, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox_3 = new QGroupBox(labelDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 10, 371, 191));
        label = new QLabel(groupBox_3);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 90, 91, 21));
        teLabel = new QPlainTextEdit(groupBox_3);
        teLabel->setObjectName(QString::fromUtf8("teLabel"));
        teLabel->setGeometry(QRect(110, 90, 241, 91));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 30, 91, 21));
        dsCombo = new QComboBox(groupBox_3);
        dsCombo->setObjectName(QString::fromUtf8("dsCombo"));
        dsCombo->setGeometry(QRect(110, 30, 241, 22));
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 60, 91, 21));
        spinBox = new QSpinBox(groupBox_3);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(110, 60, 241, 22));
        spinBox->setMaximum(99999999);
        groupBox = new QGroupBox(labelDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 210, 371, 111));
        cbBorder = new QCheckBox(groupBox);
        cbBorder->setObjectName(QString::fromUtf8("cbBorder"));
        cbBorder->setGeometry(QRect(20, 20, 251, 17));
        cbSolid = new QCheckBox(groupBox);
        cbSolid->setObjectName(QString::fromUtf8("cbSolid"));
        cbSolid->setGeometry(QRect(20, 40, 251, 17));
        cbSolid->setAutoRepeatInterval(100);
        cbConnect = new QCheckBox(groupBox);
        cbConnect->setObjectName(QString::fromUtf8("cbConnect"));
        cbConnect->setGeometry(QRect(20, 60, 251, 17));
        cbVertline = new QCheckBox(groupBox);
        cbVertline->setObjectName(QString::fromUtf8("cbVertline"));
        cbVertline->setGeometry(QRect(20, 80, 251, 17));
#ifndef QT_NO_SHORTCUT
        label->setBuddy(teLabel);
        label_2->setBuddy(dsCombo);
        label_3->setBuddy(spinBox);
#endif // QT_NO_SHORTCUT

        retranslateUi(labelDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), labelDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), labelDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(labelDialog);
    } // setupUi

    void retranslateUi(QDialog *labelDialog)
    {
        labelDialog->setWindowTitle(QString());
        groupBox_3->setTitle(QApplication::translate("labelDialog", "Label Data", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("labelDialog", "&Title:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("labelDialog", "&Data Source:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("labelDialog", "S&napshot:", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("labelDialog", "Label Flags", 0, QApplication::UnicodeUTF8));
        cbBorder->setText(QApplication::translate("labelDialog", "Display &Border", 0, QApplication::UnicodeUTF8));
        cbSolid->setText(QApplication::translate("labelDialog", "&Solid Background", 0, QApplication::UnicodeUTF8));
        cbConnect->setText(QApplication::translate("labelDialog", "&Connect to Marker", 0, QApplication::UnicodeUTF8));
        cbVertline->setText(QApplication::translate("labelDialog", "&Vertical Line", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class labelDialog: public Ui_labelDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LABELDIALOG_H
