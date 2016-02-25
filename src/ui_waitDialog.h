/********************************************************************************
** Form generated from reading UI file 'waitDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAITDIALOG_H
#define UI_WAITDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_waitDialog
{
public:
    QGroupBox *groupBox;
    QLabel *label;
    QPushButton *pushButton;

    void setupUi(QDialog *waitDialog)
    {
        if (waitDialog->objectName().isEmpty())
            waitDialog->setObjectName(QString::fromUtf8("waitDialog"));
        waitDialog->resize(283, 173);
        groupBox = new QGroupBox(waitDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 261, 121));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 221, 81));
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);
        pushButton = new QPushButton(waitDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(100, 140, 75, 23));

        retranslateUi(waitDialog);

        QMetaObject::connectSlotsByName(waitDialog);
    } // setupUi

    void retranslateUi(QDialog *waitDialog)
    {
        waitDialog->setWindowTitle(QApplication::translate("waitDialog", "Please Wait...", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        label->setText(QString());
        pushButton->setText(QApplication::translate("waitDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class waitDialog: public Ui_waitDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAITDIALOG_H
