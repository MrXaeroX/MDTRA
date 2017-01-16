/********************************************************************************
** Form generated from reading UI file 'inputTextDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INPUTTEXTDIALOG_H
#define UI_INPUTTEXTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MDTRA_InputTextDialog
{
public:
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox;
    QLabel *label;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QDialog *MDTRA_InputTextDialog)
    {
        if (MDTRA_InputTextDialog->objectName().isEmpty())
            MDTRA_InputTextDialog->setObjectName(QString::fromUtf8("MDTRA_InputTextDialog"));
        MDTRA_InputTextDialog->setWindowModality(Qt::NonModal);
        MDTRA_InputTextDialog->resize(531, 406);
        MDTRA_InputTextDialog->setModal(true);
        layoutWidget1 = new QWidget(MDTRA_InputTextDialog);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(180, 370, 171, 31));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        okButton = new QPushButton(layoutWidget1);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setMaximumSize(QSize(100, 16777215));
        okButton->setDefault(true);
        okButton->setFlat(false);

        horizontalLayout_2->addWidget(okButton);

        cancelButton = new QPushButton(layoutWidget1);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_2->addWidget(cancelButton);

        groupBox = new QGroupBox(MDTRA_InputTextDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 511, 351));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 471, 31));
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label->setWordWrap(true);
        plainTextEdit = new QPlainTextEdit(groupBox);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(20, 50, 471, 281));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        plainTextEdit->setFont(font);
        plainTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        QWidget::setTabOrder(plainTextEdit, okButton);
        QWidget::setTabOrder(okButton, cancelButton);

        retranslateUi(MDTRA_InputTextDialog);

        QMetaObject::connectSlotsByName(MDTRA_InputTextDialog);
    } // setupUi

    void retranslateUi(QDialog *MDTRA_InputTextDialog)
    {
        MDTRA_InputTextDialog->setWindowTitle(QString());
        okButton->setText(QApplication::translate("MDTRA_InputTextDialog", "&OK", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("MDTRA_InputTextDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        label->setText(QApplication::translate("MDTRA_InputTextDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MDTRA_InputTextDialog: public Ui_MDTRA_InputTextDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INPUTTEXTDIALOG_H
