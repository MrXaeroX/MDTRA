/********************************************************************************
** Form generated from reading UI file 'torsionSearchDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TORSIONSEARCHDIALOG_H
#define UI_TORSIONSEARCHDIALOG_H

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

class Ui_torsionSearchDialog
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
    QDoubleSpinBox *spinCriterion;
    QLabel *label_11;

    void setupUi(QDialog *torsionSearchDialog)
    {
        if (torsionSearchDialog->objectName().isEmpty())
            torsionSearchDialog->setObjectName(QString::fromUtf8("torsionSearchDialog"));
        torsionSearchDialog->resize(571, 533);
        buttonBox = new QDialogButtonBox(torsionSearchDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 500, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox_2 = new QGroupBox(torsionSearchDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 190, 551, 131));
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
        groupBox = new QGroupBox(torsionSearchDialog);
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
        groupBox_4 = new QGroupBox(torsionSearchDialog);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 330, 551, 161));
        label = new QLabel(groupBox_4);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 131, 21));
        sCriterion = new QComboBox(groupBox_4);
        sCriterion->setObjectName(QString::fromUtf8("sCriterion"));
        sCriterion->setGeometry(QRect(150, 20, 301, 22));
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 60, 131, 21));
        spinCriterion = new QDoubleSpinBox(groupBox_4);
        spinCriterion->setObjectName(QString::fromUtf8("spinCriterion"));
        spinCriterion->setGeometry(QRect(150, 60, 61, 22));
        spinCriterion->setDecimals(2);
        spinCriterion->setMaximum(180);
        spinCriterion->setSingleStep(5);
        spinCriterion->setValue(36);
        label_11 = new QLabel(groupBox_4);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(220, 60, 331, 101));
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
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sCombo, sCombo2);
        QWidget::setTabOrder(sCombo2, trajectoryRange);
        QWidget::setTabOrder(trajectoryRange, sIndex);
        QWidget::setTabOrder(sIndex, eIndex);
        QWidget::setTabOrder(eIndex, sel_string);
        QWidget::setTabOrder(sel_string, sel_parse);
        QWidget::setTabOrder(sel_parse, sCriterion);
        QWidget::setTabOrder(sCriterion, spinCriterion);
        QWidget::setTabOrder(spinCriterion, buttonBox);

        retranslateUi(torsionSearchDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), torsionSearchDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(torsionSearchDialog);
    } // setupUi

    void retranslateUi(QDialog *torsionSearchDialog)
    {
        torsionSearchDialog->setWindowTitle(QApplication::translate("torsionSearchDialog", "Torsion Search", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("torsionSearchDialog", "Selection", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("torsionSearchDialog", "Selection &Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("torsionSearchDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("torsionSearchDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
        groupBox->setTitle(QApplication::translate("torsionSearchDialog", "Streams", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("torsionSearchDialog", "&Stream Source 1:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("torsionSearchDialog", "&Stream Source 2:", 0, QApplication::UnicodeUTF8));
        trajectoryRange->setTitle(QApplication::translate("torsionSearchDialog", "Trajectory Range", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("torsionSearchDialog", "&End Index:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("torsionSearchDialog", "&Start Index:", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("torsionSearchDialog", "Significance Criterion", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("torsionSearchDialog", "&Statistical Parameter:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("torsionSearchDialog", "Meaningful Di&fference:", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("torsionSearchDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">(Discard torsion angle pairs with delta</span><span style=\" font-size:8pt; vertical-align:super;\">1</span><span style=\" font-size:8pt;\"> less than this value, in Degrees)</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; vertical-align:su"
                        "per;\">1</span><span style=\" font-size:8pt;\"> Delta is a difference between statistical parameters measured between different Stream Sources</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class torsionSearchDialog: public Ui_torsionSearchDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TORSIONSEARCHDIALOG_H
