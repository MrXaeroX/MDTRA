/********************************************************************************
** Form generated from reading UI file 'pcaDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PCADIALOG_H
#define UI_PCADIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_pcaDialog
{
public:
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
    QGroupBox *trajectoryRange;
    QLabel *label_4;
    QSpinBox *eIndex;
    QSpinBox *sIndex;
    QLabel *label_5;
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_3;
    QLabel *label;
    QSpinBox *numPC;

    void setupUi(QDialog *pcaDialog)
    {
        if (pcaDialog->objectName().isEmpty())
            pcaDialog->setObjectName(QString::fromUtf8("pcaDialog"));
        pcaDialog->resize(571, 379);
        groupBox_2 = new QGroupBox(pcaDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 130, 551, 101));
        sel_label = new QLabel(groupBox_2);
        sel_label->setObjectName(QString::fromUtf8("sel_label"));
        sel_label->setGeometry(QRect(20, 20, 111, 21));
        sel_string = new QLineEdit(groupBox_2);
        sel_string->setObjectName(QString::fromUtf8("sel_string"));
        sel_string->setGeometry(QRect(150, 20, 281, 20));
        sel_atCount = new QLabel(groupBox_2);
        sel_atCount->setObjectName(QString::fromUtf8("sel_atCount"));
        sel_atCount->setGeometry(QRect(470, 20, 61, 21));
        sel_atCount->setAlignment(Qt::AlignCenter);
        sel_label2 = new QLabel(groupBox_2);
        sel_label2->setObjectName(QString::fromUtf8("sel_label2"));
        sel_label2->setGeometry(QRect(20, 50, 111, 20));
        sel_label2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults = new QLabel(groupBox_2);
        sel_labelResults->setObjectName(QString::fromUtf8("sel_labelResults"));
        sel_labelResults->setGeometry(QRect(150, 50, 381, 41));
        sel_labelResults->setTextFormat(Qt::AutoText);
        sel_labelResults->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults->setWordWrap(true);
        sel_parse = new QPushButton(groupBox_2);
        sel_parse->setObjectName(QString::fromUtf8("sel_parse"));
        sel_parse->setGeometry(QRect(430, 20, 22, 22));
        groupBox = new QGroupBox(pcaDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 551, 111));
        sCombo = new QComboBox(groupBox);
        sCombo->setObjectName(QString::fromUtf8("sCombo"));
        sCombo->setGeometry(QRect(150, 20, 381, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 21, 121, 20));
        trajectoryRange = new QGroupBox(groupBox);
        trajectoryRange->setObjectName(QString::fromUtf8("trajectoryRange"));
        trajectoryRange->setGeometry(QRect(20, 50, 511, 51));
        trajectoryRange->setFlat(false);
        trajectoryRange->setCheckable(true);
        trajectoryRange->setChecked(false);
        label_4 = new QLabel(trajectoryRange);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(290, 20, 101, 21));
        eIndex = new QSpinBox(trajectoryRange);
        eIndex->setObjectName(QString::fromUtf8("eIndex"));
        eIndex->setGeometry(QRect(390, 20, 61, 22));
        eIndex->setMinimum(1);
        eIndex->setMaximum(999999);
        eIndex->setValue(9999);
        sIndex = new QSpinBox(trajectoryRange);
        sIndex->setObjectName(QString::fromUtf8("sIndex"));
        sIndex->setGeometry(QRect(150, 20, 61, 22));
        sIndex->setMinimum(1);
        sIndex->setMaximum(999999);
        label_5 = new QLabel(trajectoryRange);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(50, 20, 101, 21));
        buttonBox = new QDialogButtonBox(pcaDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 330, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox_3 = new QGroupBox(pcaDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 240, 551, 71));
        label = new QLabel(groupBox_3);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 231, 31));
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        numPC = new QSpinBox(groupBox_3);
        numPC->setObjectName(QString::fromUtf8("numPC"));
        numPC->setGeometry(QRect(250, 30, 61, 22));
        numPC->setMinimum(1);
        numPC->setValue(9);
#ifndef QT_NO_SHORTCUT
        sel_label->setBuddy(sel_string);
        label_2->setBuddy(sCombo);
        label_4->setBuddy(eIndex);
        label_5->setBuddy(sIndex);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sCombo, trajectoryRange);
        QWidget::setTabOrder(trajectoryRange, sIndex);
        QWidget::setTabOrder(sIndex, eIndex);
        QWidget::setTabOrder(eIndex, sel_string);
        QWidget::setTabOrder(sel_string, sel_parse);

        retranslateUi(pcaDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), pcaDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(pcaDialog);
    } // setupUi

    void retranslateUi(QDialog *pcaDialog)
    {
        pcaDialog->setWindowTitle(QApplication::translate("pcaDialog", "Principal Component Analysis", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("pcaDialog", "Selection", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("pcaDialog", "Selection &Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("pcaDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("pcaDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
        groupBox->setTitle(QApplication::translate("pcaDialog", "Stream", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("pcaDialog", "&Stream Source:", 0, QApplication::UnicodeUTF8));
        trajectoryRange->setTitle(QApplication::translate("pcaDialog", "Trajectory Range", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("pcaDialog", "&End Index:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("pcaDialog", "&Start Index:", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("pcaDialog", "PCA Options", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("pcaDialog", "Number of principal components to display:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class pcaDialog: public Ui_pcaDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCADIALOG_H
