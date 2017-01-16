/********************************************************************************
** Form generated from reading UI file 'dataSourceDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATASOURCEDIALOG_H
#define UI_DATASOURCEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_dataSourceDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QLabel *label_2;
    QComboBox *sCombo;
    QGroupBox *groupBox_2;
    QLabel *label_3;
    QComboBox *dataTypeCombo;
    QLabel *a_label;
    QLabel *a_atResidueLabel;
    QComboBox *a_atResidue;
    QLabel *a_atNameLabel;
    QComboBox *a_atName;
    QLabel *a_atIndex;
    QLabel *b_atResidueLabel;
    QComboBox *b_atResidue;
    QLabel *b_atIndex;
    QLabel *b_atNameLabel;
    QComboBox *b_atName;
    QLabel *b_label;
    QLabel *c_atResidueLabel;
    QComboBox *c_atResidue;
    QLabel *c_atIndex;
    QLabel *c_atNameLabel;
    QComboBox *c_atName;
    QLabel *c_label;
    QLabel *d_atResidueLabel;
    QComboBox *d_atResidue;
    QLabel *d_atIndex;
    QLabel *d_atNameLabel;
    QComboBox *d_atName;
    QLabel *d_label;
    QLabel *e_label;
    QLabel *f_label;
    QLabel *e_atResidueLabel;
    QLabel *e_atIndex;
    QComboBox *e_atResidue;
    QComboBox *e_atName;
    QLabel *e_atNameLabel;
    QLabel *f_atResidueLabel;
    QLabel *f_atIndex;
    QComboBox *f_atResidue;
    QComboBox *f_atName;
    QLabel *f_atNameLabel;
    QLabel *sel_label;
    QLineEdit *sel_string;
    QLabel *sel_atCount;
    QLabel *sel_label2;
    QLabel *sel_labelResults;
    QFrame *line;
    QPushButton *sel_parse;
    QLabel *adv_label;
    QLabel *adv_message;
    QPushButton *adv_button;
    QFrame *line_2;

    void setupUi(QDialog *dataSourceDialog)
    {
        if (dataSourceDialog->objectName().isEmpty())
            dataSourceDialog->setObjectName(QString::fromUtf8("dataSourceDialog"));
        dataSourceDialog->resize(571, 592);
        buttonBox = new QDialogButtonBox(dataSourceDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 560, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(dataSourceDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 551, 121));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(130, 30, 401, 20));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 111, 21));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 70, 111, 21));
        sCombo = new QComboBox(groupBox);
        sCombo->setObjectName(QString::fromUtf8("sCombo"));
        sCombo->setGeometry(QRect(130, 70, 401, 22));
        groupBox_2 = new QGroupBox(dataSourceDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 140, 551, 411));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 30, 111, 21));
        dataTypeCombo = new QComboBox(groupBox_2);
        dataTypeCombo->setObjectName(QString::fromUtf8("dataTypeCombo"));
        dataTypeCombo->setGeometry(QRect(130, 30, 401, 22));
        a_label = new QLabel(groupBox_2);
        a_label->setObjectName(QString::fromUtf8("a_label"));
        a_label->setGeometry(QRect(20, 70, 91, 21));
        a_atResidueLabel = new QLabel(groupBox_2);
        a_atResidueLabel->setObjectName(QString::fromUtf8("a_atResidueLabel"));
        a_atResidueLabel->setGeometry(QRect(130, 70, 61, 21));
        a_atResidue = new QComboBox(groupBox_2);
        a_atResidue->setObjectName(QString::fromUtf8("a_atResidue"));
        a_atResidue->setGeometry(QRect(200, 70, 91, 22));
        a_atNameLabel = new QLabel(groupBox_2);
        a_atNameLabel->setObjectName(QString::fromUtf8("a_atNameLabel"));
        a_atNameLabel->setGeometry(QRect(310, 70, 46, 21));
        a_atName = new QComboBox(groupBox_2);
        a_atName->setObjectName(QString::fromUtf8("a_atName"));
        a_atName->setGeometry(QRect(360, 70, 91, 22));
        a_atIndex = new QLabel(groupBox_2);
        a_atIndex->setObjectName(QString::fromUtf8("a_atIndex"));
        a_atIndex->setGeometry(QRect(470, 70, 61, 21));
        a_atIndex->setAlignment(Qt::AlignCenter);
        b_atResidueLabel = new QLabel(groupBox_2);
        b_atResidueLabel->setObjectName(QString::fromUtf8("b_atResidueLabel"));
        b_atResidueLabel->setGeometry(QRect(130, 100, 61, 21));
        b_atResidue = new QComboBox(groupBox_2);
        b_atResidue->setObjectName(QString::fromUtf8("b_atResidue"));
        b_atResidue->setGeometry(QRect(200, 100, 91, 22));
        b_atIndex = new QLabel(groupBox_2);
        b_atIndex->setObjectName(QString::fromUtf8("b_atIndex"));
        b_atIndex->setGeometry(QRect(470, 100, 61, 21));
        b_atIndex->setAlignment(Qt::AlignCenter);
        b_atNameLabel = new QLabel(groupBox_2);
        b_atNameLabel->setObjectName(QString::fromUtf8("b_atNameLabel"));
        b_atNameLabel->setGeometry(QRect(310, 100, 46, 21));
        b_atName = new QComboBox(groupBox_2);
        b_atName->setObjectName(QString::fromUtf8("b_atName"));
        b_atName->setGeometry(QRect(360, 100, 91, 22));
        b_label = new QLabel(groupBox_2);
        b_label->setObjectName(QString::fromUtf8("b_label"));
        b_label->setGeometry(QRect(20, 100, 91, 21));
        c_atResidueLabel = new QLabel(groupBox_2);
        c_atResidueLabel->setObjectName(QString::fromUtf8("c_atResidueLabel"));
        c_atResidueLabel->setGeometry(QRect(130, 130, 61, 21));
        c_atResidue = new QComboBox(groupBox_2);
        c_atResidue->setObjectName(QString::fromUtf8("c_atResidue"));
        c_atResidue->setGeometry(QRect(200, 130, 91, 22));
        c_atIndex = new QLabel(groupBox_2);
        c_atIndex->setObjectName(QString::fromUtf8("c_atIndex"));
        c_atIndex->setGeometry(QRect(470, 130, 61, 21));
        c_atIndex->setAlignment(Qt::AlignCenter);
        c_atNameLabel = new QLabel(groupBox_2);
        c_atNameLabel->setObjectName(QString::fromUtf8("c_atNameLabel"));
        c_atNameLabel->setGeometry(QRect(310, 130, 46, 21));
        c_atName = new QComboBox(groupBox_2);
        c_atName->setObjectName(QString::fromUtf8("c_atName"));
        c_atName->setGeometry(QRect(360, 130, 91, 22));
        c_label = new QLabel(groupBox_2);
        c_label->setObjectName(QString::fromUtf8("c_label"));
        c_label->setGeometry(QRect(20, 130, 91, 21));
        d_atResidueLabel = new QLabel(groupBox_2);
        d_atResidueLabel->setObjectName(QString::fromUtf8("d_atResidueLabel"));
        d_atResidueLabel->setGeometry(QRect(130, 160, 61, 21));
        d_atResidue = new QComboBox(groupBox_2);
        d_atResidue->setObjectName(QString::fromUtf8("d_atResidue"));
        d_atResidue->setGeometry(QRect(200, 160, 91, 22));
        d_atIndex = new QLabel(groupBox_2);
        d_atIndex->setObjectName(QString::fromUtf8("d_atIndex"));
        d_atIndex->setGeometry(QRect(470, 160, 61, 21));
        d_atIndex->setAlignment(Qt::AlignCenter);
        d_atNameLabel = new QLabel(groupBox_2);
        d_atNameLabel->setObjectName(QString::fromUtf8("d_atNameLabel"));
        d_atNameLabel->setGeometry(QRect(310, 160, 46, 21));
        d_atName = new QComboBox(groupBox_2);
        d_atName->setObjectName(QString::fromUtf8("d_atName"));
        d_atName->setGeometry(QRect(360, 160, 91, 22));
        d_label = new QLabel(groupBox_2);
        d_label->setObjectName(QString::fromUtf8("d_label"));
        d_label->setGeometry(QRect(20, 160, 91, 21));
        e_label = new QLabel(groupBox_2);
        e_label->setObjectName(QString::fromUtf8("e_label"));
        e_label->setGeometry(QRect(20, 190, 91, 21));
        f_label = new QLabel(groupBox_2);
        f_label->setObjectName(QString::fromUtf8("f_label"));
        f_label->setGeometry(QRect(20, 220, 91, 21));
        e_atResidueLabel = new QLabel(groupBox_2);
        e_atResidueLabel->setObjectName(QString::fromUtf8("e_atResidueLabel"));
        e_atResidueLabel->setGeometry(QRect(130, 190, 61, 21));
        e_atIndex = new QLabel(groupBox_2);
        e_atIndex->setObjectName(QString::fromUtf8("e_atIndex"));
        e_atIndex->setGeometry(QRect(470, 190, 61, 21));
        e_atIndex->setAlignment(Qt::AlignCenter);
        e_atResidue = new QComboBox(groupBox_2);
        e_atResidue->setObjectName(QString::fromUtf8("e_atResidue"));
        e_atResidue->setGeometry(QRect(200, 190, 91, 22));
        e_atName = new QComboBox(groupBox_2);
        e_atName->setObjectName(QString::fromUtf8("e_atName"));
        e_atName->setGeometry(QRect(360, 190, 91, 22));
        e_atNameLabel = new QLabel(groupBox_2);
        e_atNameLabel->setObjectName(QString::fromUtf8("e_atNameLabel"));
        e_atNameLabel->setGeometry(QRect(310, 190, 46, 21));
        f_atResidueLabel = new QLabel(groupBox_2);
        f_atResidueLabel->setObjectName(QString::fromUtf8("f_atResidueLabel"));
        f_atResidueLabel->setGeometry(QRect(130, 220, 61, 21));
        f_atIndex = new QLabel(groupBox_2);
        f_atIndex->setObjectName(QString::fromUtf8("f_atIndex"));
        f_atIndex->setGeometry(QRect(470, 220, 61, 21));
        f_atIndex->setAlignment(Qt::AlignCenter);
        f_atResidue = new QComboBox(groupBox_2);
        f_atResidue->setObjectName(QString::fromUtf8("f_atResidue"));
        f_atResidue->setGeometry(QRect(200, 220, 91, 22));
        f_atName = new QComboBox(groupBox_2);
        f_atName->setObjectName(QString::fromUtf8("f_atName"));
        f_atName->setGeometry(QRect(360, 220, 91, 22));
        f_atNameLabel = new QLabel(groupBox_2);
        f_atNameLabel->setObjectName(QString::fromUtf8("f_atNameLabel"));
        f_atNameLabel->setGeometry(QRect(310, 220, 46, 21));
        sel_label = new QLabel(groupBox_2);
        sel_label->setObjectName(QString::fromUtf8("sel_label"));
        sel_label->setGeometry(QRect(20, 280, 131, 21));
        sel_string = new QLineEdit(groupBox_2);
        sel_string->setObjectName(QString::fromUtf8("sel_string"));
        sel_string->setGeometry(QRect(150, 280, 281, 20));
        sel_atCount = new QLabel(groupBox_2);
        sel_atCount->setObjectName(QString::fromUtf8("sel_atCount"));
        sel_atCount->setGeometry(QRect(470, 280, 61, 21));
        sel_atCount->setAlignment(Qt::AlignCenter);
        sel_label2 = new QLabel(groupBox_2);
        sel_label2->setObjectName(QString::fromUtf8("sel_label2"));
        sel_label2->setGeometry(QRect(20, 310, 131, 21));
        sel_label2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults = new QLabel(groupBox_2);
        sel_labelResults->setObjectName(QString::fromUtf8("sel_labelResults"));
        sel_labelResults->setGeometry(QRect(150, 310, 341, 51));
        sel_labelResults->setTextFormat(Qt::AutoText);
        sel_labelResults->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults->setWordWrap(true);
        line = new QFrame(groupBox_2);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(20, 250, 511, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        sel_parse = new QPushButton(groupBox_2);
        sel_parse->setObjectName(QString::fromUtf8("sel_parse"));
        sel_parse->setGeometry(QRect(430, 280, 22, 22));
        adv_label = new QLabel(groupBox_2);
        adv_label->setObjectName(QString::fromUtf8("adv_label"));
        adv_label->setGeometry(QRect(20, 374, 131, 21));
        adv_label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        adv_message = new QLabel(groupBox_2);
        adv_message->setObjectName(QString::fromUtf8("adv_message"));
        adv_message->setGeometry(QRect(150, 374, 271, 31));
        adv_message->setTextFormat(Qt::AutoText);
        adv_message->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        adv_message->setWordWrap(true);
        adv_button = new QPushButton(groupBox_2);
        adv_button->setObjectName(QString::fromUtf8("adv_button"));
        adv_button->setGeometry(QRect(430, 374, 101, 23));
        line_2 = new QFrame(groupBox_2);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(20, 358, 511, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(lineEdit);
        label_2->setBuddy(sCombo);
        label_3->setBuddy(dataTypeCombo);
        a_atResidueLabel->setBuddy(a_atResidue);
        a_atNameLabel->setBuddy(a_atName);
        b_atResidueLabel->setBuddy(b_atResidue);
        b_atNameLabel->setBuddy(b_atName);
        c_atResidueLabel->setBuddy(c_atResidue);
        c_atNameLabel->setBuddy(c_atName);
        d_atResidueLabel->setBuddy(d_atResidue);
        d_atNameLabel->setBuddy(d_atName);
        e_atResidueLabel->setBuddy(e_atResidue);
        e_atNameLabel->setBuddy(e_atName);
        f_atResidueLabel->setBuddy(f_atResidue);
        f_atNameLabel->setBuddy(f_atName);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(lineEdit, sCombo);
        QWidget::setTabOrder(sCombo, buttonBox);

        retranslateUi(dataSourceDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), dataSourceDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(dataSourceDialog);
    } // setupUi

    void retranslateUi(QDialog *dataSourceDialog)
    {
        dataSourceDialog->setWindowTitle(QString());
        groupBox->setTitle(QApplication::translate("dataSourceDialog", "Data Source Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("dataSourceDialog", "Data Source &Title:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("dataSourceDialog", "&Stream Source:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("dataSourceDialog", "Data Declaration", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("dataSourceDialog", "Data &Type:", 0, QApplication::UnicodeUTF8));
        a_label->setText(QApplication::translate("dataSourceDialog", "A:", 0, QApplication::UnicodeUTF8));
        a_atResidueLabel->setText(QApplication::translate("dataSourceDialog", "&Residue:", 0, QApplication::UnicodeUTF8));
        a_atNameLabel->setText(QApplication::translate("dataSourceDialog", "&Atom:", 0, QApplication::UnicodeUTF8));
        a_atIndex->setText(QApplication::translate("dataSourceDialog", "0", 0, QApplication::UnicodeUTF8));
        b_atResidueLabel->setText(QApplication::translate("dataSourceDialog", "&Residue:", 0, QApplication::UnicodeUTF8));
        b_atIndex->setText(QApplication::translate("dataSourceDialog", "0", 0, QApplication::UnicodeUTF8));
        b_atNameLabel->setText(QApplication::translate("dataSourceDialog", "&Atom:", 0, QApplication::UnicodeUTF8));
        b_label->setText(QApplication::translate("dataSourceDialog", "B:", 0, QApplication::UnicodeUTF8));
        c_atResidueLabel->setText(QApplication::translate("dataSourceDialog", "&Residue:", 0, QApplication::UnicodeUTF8));
        c_atIndex->setText(QApplication::translate("dataSourceDialog", "0", 0, QApplication::UnicodeUTF8));
        c_atNameLabel->setText(QApplication::translate("dataSourceDialog", "&Atom:", 0, QApplication::UnicodeUTF8));
        c_label->setText(QApplication::translate("dataSourceDialog", "C:", 0, QApplication::UnicodeUTF8));
        d_atResidueLabel->setText(QApplication::translate("dataSourceDialog", "&Residue:", 0, QApplication::UnicodeUTF8));
        d_atIndex->setText(QApplication::translate("dataSourceDialog", "0", 0, QApplication::UnicodeUTF8));
        d_atNameLabel->setText(QApplication::translate("dataSourceDialog", "&Atom:", 0, QApplication::UnicodeUTF8));
        d_label->setText(QApplication::translate("dataSourceDialog", "D:", 0, QApplication::UnicodeUTF8));
        e_label->setText(QApplication::translate("dataSourceDialog", "E:", 0, QApplication::UnicodeUTF8));
        f_label->setText(QApplication::translate("dataSourceDialog", "F:", 0, QApplication::UnicodeUTF8));
        e_atResidueLabel->setText(QApplication::translate("dataSourceDialog", "&Residue:", 0, QApplication::UnicodeUTF8));
        e_atIndex->setText(QApplication::translate("dataSourceDialog", "0", 0, QApplication::UnicodeUTF8));
        e_atNameLabel->setText(QApplication::translate("dataSourceDialog", "&Atom:", 0, QApplication::UnicodeUTF8));
        f_atResidueLabel->setText(QApplication::translate("dataSourceDialog", "&Residue:", 0, QApplication::UnicodeUTF8));
        f_atIndex->setText(QApplication::translate("dataSourceDialog", "0", 0, QApplication::UnicodeUTF8));
        f_atNameLabel->setText(QApplication::translate("dataSourceDialog", "&Atom:", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("dataSourceDialog", "Selection Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("dataSourceDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("dataSourceDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
        adv_label->setText(QString());
        adv_message->setText(QString());
        adv_button->setText(QApplication::translate("dataSourceDialog", "Ad&vanced...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dataSourceDialog: public Ui_dataSourceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATASOURCEDIALOG_H
