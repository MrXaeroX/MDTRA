/********************************************************************************
** Form generated from reading UI file 'selectionDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTIONDIALOG_H
#define UI_SELECTIONDIALOG_H

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

QT_BEGIN_NAMESPACE

class Ui_selectionDialog
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
    QLabel *label_2;
    QComboBox *sCombo;

    void setupUi(QDialog *selectionDialog)
    {
        if (selectionDialog->objectName().isEmpty())
            selectionDialog->setObjectName(QString::fromUtf8("selectionDialog"));
        selectionDialog->resize(571, 314);
        buttonBox = new QDialogButtonBox(selectionDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 280, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);
        buttonBox->setCenterButtons(true);
        groupBox_2 = new QGroupBox(selectionDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 10, 551, 261));
        sel_label = new QLabel(groupBox_2);
        sel_label->setObjectName(QString::fromUtf8("sel_label"));
        sel_label->setGeometry(QRect(20, 70, 131, 21));
        sel_string = new QLineEdit(groupBox_2);
        sel_string->setObjectName(QString::fromUtf8("sel_string"));
        sel_string->setGeometry(QRect(150, 70, 281, 20));
        sel_atCount = new QLabel(groupBox_2);
        sel_atCount->setObjectName(QString::fromUtf8("sel_atCount"));
        sel_atCount->setGeometry(QRect(470, 70, 61, 21));
        sel_atCount->setAlignment(Qt::AlignCenter);
        sel_label2 = new QLabel(groupBox_2);
        sel_label2->setObjectName(QString::fromUtf8("sel_label2"));
        sel_label2->setGeometry(QRect(20, 110, 131, 20));
        sel_label2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults = new QLabel(groupBox_2);
        sel_labelResults->setObjectName(QString::fromUtf8("sel_labelResults"));
        sel_labelResults->setGeometry(QRect(150, 110, 341, 141));
        sel_labelResults->setTextFormat(Qt::AutoText);
        sel_labelResults->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults->setWordWrap(true);
        sel_parse = new QPushButton(groupBox_2);
        sel_parse->setObjectName(QString::fromUtf8("sel_parse"));
        sel_parse->setGeometry(QRect(430, 70, 22, 22));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 30, 131, 21));
        sCombo = new QComboBox(groupBox_2);
        sCombo->setObjectName(QString::fromUtf8("sCombo"));
        sCombo->setGeometry(QRect(150, 30, 381, 22));
#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(sCombo);
#endif // QT_NO_SHORTCUT

        retranslateUi(selectionDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), selectionDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(selectionDialog);
    } // setupUi

    void retranslateUi(QDialog *selectionDialog)
    {
        selectionDialog->setWindowTitle(QString());
        groupBox_2->setTitle(QApplication::translate("selectionDialog", "Selection", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("selectionDialog", "Selection Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("selectionDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("selectionDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
        label_2->setText(QApplication::translate("selectionDialog", "&Stream Source:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class selectionDialog: public Ui_selectionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTIONDIALOG_H
