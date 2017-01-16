/********************************************************************************
** Form generated from reading UI file 'distanceSearchDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISTANCESEARCHDIALOG_H
#define UI_DISTANCESEARCHDIALOG_H

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
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_distanceSearchDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_2;
    QLabel *sel_label;
    QLineEdit *sel_string;
    QLabel *sel_atCount;
    QLabel *sel_label2;
    QLabel *sel_labelResults;
    QPushButton *sel_parse;
    QCheckBox *selIgnore;
    QGroupBox *groupBox;
    QComboBox *sCombo;
    QLabel *label_2;
    QComboBox *sCombo2;
    QLabel *label_3;
    QGroupBox *trajectoryRange;
    QLabel *label_4;
    QSpinBox *eIndex;
    QSpinBox *sIndex;
    QLabel *label_5;
    QGroupBox *groupBox_4;
    QLabel *label;
    QComboBox *sCriterion;
    QLabel *label_6;
    QDoubleSpinBox *spinCriterion;
    QLabel *label_7;
    QLabel *label_8;
    QDoubleSpinBox *spinMin;
    QLabel *label_9;
    QDoubleSpinBox *spinMax;
    QLabel *label_10;
    QLabel *label_11;

    void setupUi(QDialog *distanceSearchDialog)
    {
        if (distanceSearchDialog->objectName().isEmpty())
            distanceSearchDialog->setObjectName(QString::fromUtf8("distanceSearchDialog"));
        distanceSearchDialog->resize(571, 603);
        buttonBox = new QDialogButtonBox(distanceSearchDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 570, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox_2 = new QGroupBox(distanceSearchDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 190, 551, 151));
        sel_label = new QLabel(groupBox_2);
        sel_label->setObjectName(QString::fromUtf8("sel_label"));
        sel_label->setGeometry(QRect(20, 20, 131, 21));
        sel_string = new QLineEdit(groupBox_2);
        sel_string->setObjectName(QString::fromUtf8("sel_string"));
        sel_string->setGeometry(QRect(150, 20, 281, 20));
        sel_atCount = new QLabel(groupBox_2);
        sel_atCount->setObjectName(QString::fromUtf8("sel_atCount"));
        sel_atCount->setGeometry(QRect(470, 20, 61, 21));
        sel_atCount->setAlignment(Qt::AlignCenter);
        sel_label2 = new QLabel(groupBox_2);
        sel_label2->setObjectName(QString::fromUtf8("sel_label2"));
        sel_label2->setGeometry(QRect(20, 60, 111, 20));
        sel_label2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults = new QLabel(groupBox_2);
        sel_labelResults->setObjectName(QString::fromUtf8("sel_labelResults"));
        sel_labelResults->setGeometry(QRect(150, 60, 341, 51));
        sel_labelResults->setTextFormat(Qt::AutoText);
        sel_labelResults->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults->setWordWrap(true);
        sel_parse = new QPushButton(groupBox_2);
        sel_parse->setObjectName(QString::fromUtf8("sel_parse"));
        sel_parse->setGeometry(QRect(430, 20, 22, 22));
        selIgnore = new QCheckBox(groupBox_2);
        selIgnore->setObjectName(QString::fromUtf8("selIgnore"));
        selIgnore->setGeometry(QRect(20, 120, 511, 20));
        selIgnore->setChecked(true);
        groupBox = new QGroupBox(distanceSearchDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 551, 171));
        sCombo = new QComboBox(groupBox);
        sCombo->setObjectName(QString::fromUtf8("sCombo"));
        sCombo->setGeometry(QRect(150, 30, 381, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 30, 111, 21));
        sCombo2 = new QComboBox(groupBox);
        sCombo2->setObjectName(QString::fromUtf8("sCombo2"));
        sCombo2->setGeometry(QRect(150, 60, 381, 22));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 60, 111, 21));
        trajectoryRange = new QGroupBox(groupBox);
        trajectoryRange->setObjectName(QString::fromUtf8("trajectoryRange"));
        trajectoryRange->setGeometry(QRect(20, 100, 511, 61));
        trajectoryRange->setFlat(false);
        trajectoryRange->setCheckable(true);
        trajectoryRange->setChecked(false);
        label_4 = new QLabel(trajectoryRange);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(290, 30, 121, 21));
        eIndex = new QSpinBox(trajectoryRange);
        eIndex->setObjectName(QString::fromUtf8("eIndex"));
        eIndex->setGeometry(QRect(410, 30, 61, 22));
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
        groupBox_4 = new QGroupBox(distanceSearchDialog);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 350, 551, 211));
        label = new QLabel(groupBox_4);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 131, 21));
        sCriterion = new QComboBox(groupBox_4);
        sCriterion->setObjectName(QString::fromUtf8("sCriterion"));
        sCriterion->setGeometry(QRect(150, 20, 301, 22));
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 120, 131, 21));
        spinCriterion = new QDoubleSpinBox(groupBox_4);
        spinCriterion->setObjectName(QString::fromUtf8("spinCriterion"));
        spinCriterion->setGeometry(QRect(150, 120, 61, 22));
        spinCriterion->setDecimals(3);
        spinCriterion->setMaximum(9999.99);
        spinCriterion->setSingleStep(0.1);
        spinCriterion->setValue(0.5);
        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(220, 60, 331, 21));
        label_7->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_8 = new QLabel(groupBox_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 60, 131, 21));
        spinMin = new QDoubleSpinBox(groupBox_4);
        spinMin->setObjectName(QString::fromUtf8("spinMin"));
        spinMin->setGeometry(QRect(150, 60, 61, 22));
        spinMin->setDecimals(3);
        spinMin->setMaximum(9999.99);
        spinMin->setSingleStep(0.1);
        label_9 = new QLabel(groupBox_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(220, 90, 331, 21));
        label_9->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        spinMax = new QDoubleSpinBox(groupBox_4);
        spinMax->setObjectName(QString::fromUtf8("spinMax"));
        spinMax->setGeometry(QRect(150, 90, 61, 22));
        spinMax->setDecimals(3);
        spinMax->setMaximum(9999.99);
        spinMax->setSingleStep(0.1);
        spinMax->setValue(4);
        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(20, 90, 131, 21));
        label_11 = new QLabel(groupBox_4);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(220, 124, 331, 81));
        label_11->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_11->setWordWrap(true);
#ifndef QT_NO_SHORTCUT
        sel_label->setBuddy(sel_string);
        label_2->setBuddy(sCombo);
        label_3->setBuddy(sCombo2);
        label_4->setBuddy(eIndex);
        label_5->setBuddy(sIndex);
        label->setBuddy(sCriterion);
        label_6->setBuddy(spinCriterion);
        label_8->setBuddy(spinMin);
        label_10->setBuddy(spinMax);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sCombo, sCombo2);
        QWidget::setTabOrder(sCombo2, trajectoryRange);
        QWidget::setTabOrder(trajectoryRange, sIndex);
        QWidget::setTabOrder(sIndex, eIndex);
        QWidget::setTabOrder(eIndex, sel_string);
        QWidget::setTabOrder(sel_string, sel_parse);
        QWidget::setTabOrder(sel_parse, sCriterion);
        QWidget::setTabOrder(sCriterion, spinMin);
        QWidget::setTabOrder(spinMin, spinMax);
        QWidget::setTabOrder(spinMax, spinCriterion);
        QWidget::setTabOrder(spinCriterion, buttonBox);

        retranslateUi(distanceSearchDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), distanceSearchDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(distanceSearchDialog);
    } // setupUi

    void retranslateUi(QDialog *distanceSearchDialog)
    {
        distanceSearchDialog->setWindowTitle(QApplication::translate("distanceSearchDialog", "Distance Search", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("distanceSearchDialog", "Selection", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("distanceSearchDialog", "Selection &Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("distanceSearchDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("distanceSearchDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
        selIgnore->setText(QApplication::translate("distanceSearchDialog", "&Ignore Pairs within the Same Residue", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("distanceSearchDialog", "Streams", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("distanceSearchDialog", "&Stream Source 1:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("distanceSearchDialog", "&Stream Source 2:", 0, QApplication::UnicodeUTF8));
        trajectoryRange->setTitle(QApplication::translate("distanceSearchDialog", "Trajectory Range", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("distanceSearchDialog", "&End Index:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("distanceSearchDialog", "&Start Index:", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("distanceSearchDialog", "Significance Criterion", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("distanceSearchDialog", "&Statistical Parameter:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("distanceSearchDialog", "Minimum Di&fference:", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("distanceSearchDialog", "(Discard pairs with parameter less than this value, in A)", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("distanceSearchDialog", "&Minimum Value:", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("distanceSearchDialog", "(Discard pairs with parameter greater than this value, in A)", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("distanceSearchDialog", "Ma&ximum Value:", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("distanceSearchDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">(Discard pairs with parameter delta</span><span style=\" font-size:8pt; vertical-align:super;\">1</span><span style=\" font-size:8pt;\"> less than this value, in A)</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; vertical-align:super;\">1</"
                        "span><span style=\" font-size:8pt;\"> Delta is a difference between statistical parameters measured between different Stream Sources</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class distanceSearchDialog: public Ui_distanceSearchDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISTANCESEARCHDIALOG_H
