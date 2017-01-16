/********************************************************************************
** Form generated from reading UI file 'occluderDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OCCLUDERDIALOG_H
#define UI_OCCLUDERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_occluderDialog
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

    void setupUi(QDialog *occluderDialog)
    {
        if (occluderDialog->objectName().isEmpty())
            occluderDialog->setObjectName(QString::fromUtf8("occluderDialog"));
        occluderDialog->resize(571, 214);
        buttonBox = new QDialogButtonBox(occluderDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 180, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox_2 = new QGroupBox(occluderDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 10, 551, 161));
        sel_label = new QLabel(groupBox_2);
        sel_label->setObjectName(QString::fromUtf8("sel_label"));
        sel_label->setGeometry(QRect(20, 30, 131, 21));
        sel_string = new QLineEdit(groupBox_2);
        sel_string->setObjectName(QString::fromUtf8("sel_string"));
        sel_string->setGeometry(QRect(150, 30, 281, 20));
        sel_atCount = new QLabel(groupBox_2);
        sel_atCount->setObjectName(QString::fromUtf8("sel_atCount"));
        sel_atCount->setGeometry(QRect(470, 30, 61, 21));
        sel_atCount->setAlignment(Qt::AlignCenter);
        sel_label2 = new QLabel(groupBox_2);
        sel_label2->setObjectName(QString::fromUtf8("sel_label2"));
        sel_label2->setGeometry(QRect(20, 70, 131, 20));
        sel_label2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults = new QLabel(groupBox_2);
        sel_labelResults->setObjectName(QString::fromUtf8("sel_labelResults"));
        sel_labelResults->setGeometry(QRect(150, 70, 341, 71));
        sel_labelResults->setTextFormat(Qt::AutoText);
        sel_labelResults->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults->setWordWrap(true);
        sel_parse = new QPushButton(groupBox_2);
        sel_parse->setObjectName(QString::fromUtf8("sel_parse"));
        sel_parse->setGeometry(QRect(430, 30, 22, 22));
        QWidget::setTabOrder(sel_string, sel_parse);
        QWidget::setTabOrder(sel_parse, buttonBox);

        retranslateUi(occluderDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), occluderDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), occluderDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(occluderDialog);
    } // setupUi

    void retranslateUi(QDialog *occluderDialog)
    {
        occluderDialog->setWindowTitle(QString());
        groupBox_2->setTitle(QApplication::translate("occluderDialog", "Occluder", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("occluderDialog", "Selection Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("occluderDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("occluderDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class occluderDialog: public Ui_occluderDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OCCLUDERDIALOG_H
