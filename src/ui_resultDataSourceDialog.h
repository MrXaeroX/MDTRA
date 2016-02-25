/********************************************************************************
** Form generated from reading UI file 'resultDataSourceDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESULTDATASOURCEDIALOG_H
#define UI_RESULTDATASOURCEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_resultDataSourceDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLabel *label;
    QComboBox *dsCombo;
    QGroupBox *groupBox_2;
    QLabel *label_2;
    QLabel *label_3;
    QDoubleSpinBox *dsScale;
    QDoubleSpinBox *dsBias;
    QCheckBox *cbVis;

    void setupUi(QDialog *resultDataSourceDialog)
    {
        if (resultDataSourceDialog->objectName().isEmpty())
            resultDataSourceDialog->setObjectName(QString::fromUtf8("resultDataSourceDialog"));
        resultDataSourceDialog->resize(431, 249);
        buttonBox = new QDialogButtonBox(resultDataSourceDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 210, 411, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(resultDataSourceDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 411, 80));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 111, 21));
        dsCombo = new QComboBox(groupBox);
        dsCombo->setObjectName(QString::fromUtf8("dsCombo"));
        dsCombo->setGeometry(QRect(130, 30, 261, 22));
        groupBox_2 = new QGroupBox(resultDataSourceDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 100, 411, 71));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 30, 111, 21));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(220, 30, 111, 21));
        dsScale = new QDoubleSpinBox(groupBox_2);
        dsScale->setObjectName(QString::fromUtf8("dsScale"));
        dsScale->setGeometry(QRect(130, 30, 62, 22));
        dsScale->setMinimum(-100000);
        dsScale->setMaximum(100000);
        dsScale->setValue(1);
        dsBias = new QDoubleSpinBox(groupBox_2);
        dsBias->setObjectName(QString::fromUtf8("dsBias"));
        dsBias->setGeometry(QRect(330, 30, 62, 22));
        dsBias->setMinimum(-100000);
        dsBias->setMaximum(100000);
        dsBias->setValue(0);
        cbVis = new QCheckBox(resultDataSourceDialog);
        cbVis->setObjectName(QString::fromUtf8("cbVis"));
        cbVis->setGeometry(QRect(30, 180, 371, 17));
        cbVis->setChecked(true);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(dsCombo);
        label_2->setBuddy(dsScale);
        label_3->setBuddy(dsBias);
#endif // QT_NO_SHORTCUT

        retranslateUi(resultDataSourceDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), resultDataSourceDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), resultDataSourceDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(resultDataSourceDialog);
    } // setupUi

    void retranslateUi(QDialog *resultDataSourceDialog)
    {
        resultDataSourceDialog->setWindowTitle(QString());
        groupBox->setTitle(QApplication::translate("resultDataSourceDialog", "Result Data Source", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("resultDataSourceDialog", "&Data Source:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("resultDataSourceDialog", "Data Mapping", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("resultDataSourceDialog", "&Scale:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("resultDataSourceDialog", "&Bias:", 0, QApplication::UnicodeUTF8));
        cbVis->setText(QApplication::translate("resultDataSourceDialog", "Visible on the plot", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class resultDataSourceDialog: public Ui_resultDataSourceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESULTDATASOURCEDIALOG_H
