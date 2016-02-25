/********************************************************************************
** Form generated from reading UI file 'hbSearchDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HBSEARCHDIALOG_H
#define UI_HBSEARCHDIALOG_H

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
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_hbSearchDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QComboBox *sCombo;
    QLabel *label_2;
    QGroupBox *trajectoryRange;
    QLabel *label_4;
    QSpinBox *eIndex;
    QSpinBox *sIndex;
    QLabel *label_5;
    QGroupBox *groupBox_2;
    QCheckBox *cbEnergyTreshold;
    QDoubleSpinBox *spinEnergy;
    QLabel *label;
    QCheckBox *cbPercentTreshold;
    QSpinBox *spinPercent;
    QLabel *label_3;
    QGroupBox *groupBox_3;
    QCheckBox *cbGrouping;

    void setupUi(QDialog *hbSearchDialog)
    {
        if (hbSearchDialog->objectName().isEmpty())
            hbSearchDialog->setObjectName(QString::fromUtf8("hbSearchDialog"));
        hbSearchDialog->resize(511, 443);
        buttonBox = new QDialogButtonBox(hbSearchDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 410, 491, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(hbSearchDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 491, 141));
        sCombo = new QComboBox(groupBox);
        sCombo->setObjectName(QString::fromUtf8("sCombo"));
        sCombo->setGeometry(QRect(150, 30, 321, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 30, 111, 21));
        trajectoryRange = new QGroupBox(groupBox);
        trajectoryRange->setObjectName(QString::fromUtf8("trajectoryRange"));
        trajectoryRange->setGeometry(QRect(20, 60, 451, 61));
        trajectoryRange->setFlat(false);
        trajectoryRange->setCheckable(true);
        trajectoryRange->setChecked(false);
        label_4 = new QLabel(trajectoryRange);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(240, 30, 121, 21));
        eIndex = new QSpinBox(trajectoryRange);
        eIndex->setObjectName(QString::fromUtf8("eIndex"));
        eIndex->setGeometry(QRect(360, 30, 61, 22));
        eIndex->setMinimum(1);
        eIndex->setMaximum(999999);
        eIndex->setValue(9999);
        sIndex = new QSpinBox(trajectoryRange);
        sIndex->setObjectName(QString::fromUtf8("sIndex"));
        sIndex->setGeometry(QRect(150, 30, 61, 22));
        sIndex->setMinimum(1);
        sIndex->setMaximum(999999);
        label_5 = new QLabel(trajectoryRange);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(30, 30, 121, 21));
        groupBox_2 = new QGroupBox(hbSearchDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 230, 491, 171));
        cbEnergyTreshold = new QCheckBox(groupBox_2);
        cbEnergyTreshold->setObjectName(QString::fromUtf8("cbEnergyTreshold"));
        cbEnergyTreshold->setGeometry(QRect(30, 30, 451, 17));
        spinEnergy = new QDoubleSpinBox(groupBox_2);
        spinEnergy->setObjectName(QString::fromUtf8("spinEnergy"));
        spinEnergy->setEnabled(false);
        spinEnergy->setGeometry(QRect(60, 60, 62, 22));
        spinEnergy->setMaximum(10);
        spinEnergy->setSingleStep(0.1);
        spinEnergy->setValue(1);
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(false);
        label->setGeometry(QRect(130, 60, 151, 21));
        cbPercentTreshold = new QCheckBox(groupBox_2);
        cbPercentTreshold->setObjectName(QString::fromUtf8("cbPercentTreshold"));
        cbPercentTreshold->setGeometry(QRect(30, 100, 451, 17));
        spinPercent = new QSpinBox(groupBox_2);
        spinPercent->setObjectName(QString::fromUtf8("spinPercent"));
        spinPercent->setEnabled(false);
        spinPercent->setGeometry(QRect(60, 130, 61, 22));
        spinPercent->setMaximum(100);
        spinPercent->setSingleStep(5);
        spinPercent->setValue(90);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setEnabled(false);
        label_3->setGeometry(QRect(130, 130, 151, 21));
        groupBox_3 = new QGroupBox(hbSearchDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 160, 491, 61));
        cbGrouping = new QCheckBox(groupBox_3);
        cbGrouping->setObjectName(QString::fromUtf8("cbGrouping"));
        cbGrouping->setGeometry(QRect(30, 26, 441, 21));
#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(sCombo);
        label_4->setBuddy(eIndex);
        label_5->setBuddy(sIndex);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sCombo, trajectoryRange);
        QWidget::setTabOrder(trajectoryRange, sIndex);
        QWidget::setTabOrder(sIndex, eIndex);
        QWidget::setTabOrder(eIndex, buttonBox);

        retranslateUi(hbSearchDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), hbSearchDialog, SLOT(reject()));
        QObject::connect(cbEnergyTreshold, SIGNAL(toggled(bool)), spinEnergy, SLOT(setEnabled(bool)));
        QObject::connect(cbEnergyTreshold, SIGNAL(toggled(bool)), label, SLOT(setEnabled(bool)));
        QObject::connect(cbPercentTreshold, SIGNAL(toggled(bool)), spinPercent, SLOT(setEnabled(bool)));
        QObject::connect(cbPercentTreshold, SIGNAL(toggled(bool)), label_3, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(hbSearchDialog);
    } // setupUi

    void retranslateUi(QDialog *hbSearchDialog)
    {
        hbSearchDialog->setWindowTitle(QApplication::translate("hbSearchDialog", "H-Bonds Search", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("hbSearchDialog", "Streams", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("hbSearchDialog", "&Stream Source:", 0, QApplication::UnicodeUTF8));
        trajectoryRange->setTitle(QApplication::translate("hbSearchDialog", "Trajectory Range", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("hbSearchDialog", "&End Index:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("hbSearchDialog", "&Start Index:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("hbSearchDialog", "Significance Criterion", 0, QApplication::UnicodeUTF8));
        cbEnergyTreshold->setText(QApplication::translate("hbSearchDialog", "Ignore Bonds with Absolute Energy Value Less Than:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("hbSearchDialog", "kcal/mol", 0, QApplication::UnicodeUTF8));
        cbPercentTreshold->setText(QApplication::translate("hbSearchDialog", "Ignore Bonds Existing in Less Than:", 0, QApplication::UnicodeUTF8));
        spinPercent->setSuffix(QApplication::translate("hbSearchDialog", "%", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("hbSearchDialog", "of the Trajectory", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("hbSearchDialog", "Search Options", 0, QApplication::UnicodeUTF8));
        cbGrouping->setText(QApplication::translate("hbSearchDialog", "&Group Bonds Formed by Equivalent Donor and/or Acceptor", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class hbSearchDialog: public Ui_hbSearchDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HBSEARCHDIALOG_H
