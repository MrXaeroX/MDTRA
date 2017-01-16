/********************************************************************************
** Form generated from reading UI file 'forceSearchDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORCESEARCHDIALOG_H
#define UI_FORCESEARCHDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
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

class Ui_forceSearchDialog
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
    QDoubleSpinBox *spinCriterionDiff;
    QLabel *label_11;
    QLabel *label_7;
    QDoubleSpinBox *spinCriterionDot;
    QLabel *label_12;

    void setupUi(QDialog *forceSearchDialog)
    {
        if (forceSearchDialog->objectName().isEmpty())
            forceSearchDialog->setObjectName(QString::fromUtf8("forceSearchDialog"));
        forceSearchDialog->resize(571, 533);
        buttonBox = new QDialogButtonBox(forceSearchDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 500, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox_2 = new QGroupBox(forceSearchDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 190, 551, 121));
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
        groupBox = new QGroupBox(forceSearchDialog);
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
        groupBox_4 = new QGroupBox(forceSearchDialog);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 320, 551, 171));
        label = new QLabel(groupBox_4);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 131, 21));
        sCriterion = new QComboBox(groupBox_4);
        sCriterion->setObjectName(QString::fromUtf8("sCriterion"));
        sCriterion->setGeometry(QRect(150, 20, 301, 22));
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 56, 131, 21));
        spinCriterionDiff = new QDoubleSpinBox(groupBox_4);
        spinCriterionDiff->setObjectName(QString::fromUtf8("spinCriterionDiff"));
        spinCriterionDiff->setGeometry(QRect(150, 56, 61, 22));
        spinCriterionDiff->setDecimals(3);
        spinCriterionDiff->setMaximum(9999.99);
        spinCriterionDiff->setSingleStep(0.1);
        spinCriterionDiff->setValue(3.5);
        label_11 = new QLabel(groupBox_4);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(220, 110, 321, 41));
        label_11->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_11->setWordWrap(true);
        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(20, 110, 131, 21));
        spinCriterionDot = new QDoubleSpinBox(groupBox_4);
        spinCriterionDot->setObjectName(QString::fromUtf8("spinCriterionDot"));
        spinCriterionDot->setGeometry(QRect(150, 110, 61, 22));
        spinCriterionDot->setDecimals(3);
        spinCriterionDot->setMaximum(9999.99);
        spinCriterionDot->setSingleStep(0.1);
        spinCriterionDot->setValue(30);
        label_12 = new QLabel(groupBox_4);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(220, 60, 321, 41));
        label_12->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_12->setWordWrap(true);
#ifndef QT_NO_SHORTCUT
        sel_label->setBuddy(sel_string);
        label_2->setBuddy(sCombo);
        label_3->setBuddy(sCombo2);
        label_4->setBuddy(eIndex);
        label_5->setBuddy(sIndex);
        label->setBuddy(sCriterion);
        label_6->setBuddy(spinCriterionDiff);
        label_7->setBuddy(spinCriterionDiff);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sCombo, sCombo2);
        QWidget::setTabOrder(sCombo2, trajectoryRange);
        QWidget::setTabOrder(trajectoryRange, sIndex);
        QWidget::setTabOrder(sIndex, eIndex);
        QWidget::setTabOrder(eIndex, sel_string);
        QWidget::setTabOrder(sel_string, sel_parse);
        QWidget::setTabOrder(sel_parse, sCriterion);
        QWidget::setTabOrder(sCriterion, spinCriterionDiff);
        QWidget::setTabOrder(spinCriterionDiff, buttonBox);

        retranslateUi(forceSearchDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), forceSearchDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(forceSearchDialog);
    } // setupUi

    void retranslateUi(QDialog *forceSearchDialog)
    {
        forceSearchDialog->setWindowTitle(QApplication::translate("forceSearchDialog", "Force Search", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("forceSearchDialog", "Selection", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("forceSearchDialog", "Selection &Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("forceSearchDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("forceSearchDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
        groupBox->setTitle(QApplication::translate("forceSearchDialog", "Streams", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("forceSearchDialog", "&Stream Source 1:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("forceSearchDialog", "&Stream Source 2:", 0, QApplication::UnicodeUTF8));
        trajectoryRange->setTitle(QApplication::translate("forceSearchDialog", "Trajectory Range", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("forceSearchDialog", "&End Index:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("forceSearchDialog", "&Start Index:", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("forceSearchDialog", "Significance Criterion", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("forceSearchDialog", "&Statistical Parameter:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("forceSearchDialog", "Minimum Di&fference:", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("forceSearchDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">(Accept atoms with difference between statistical parameters for angle between force vectors greater than or equal to this value, in degrees)</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("forceSearchDialog", "Minimum Do&t:", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("forceSearchDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">(Accept atoms with difference between statistical parameters for length of force vectors greater than or equal to this value, in kcal/A)</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class forceSearchDialog: public Ui_forceSearchDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORCESEARCHDIALOG_H
