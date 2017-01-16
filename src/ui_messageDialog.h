/********************************************************************************
** Form generated from reading UI file 'messageDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEDIALOG_H
#define UI_MESSAGEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_messageDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;

    void setupUi(QDialog *messageDialog)
    {
        if (messageDialog->objectName().isEmpty())
            messageDialog->setObjectName(QString::fromUtf8("messageDialog"));
        messageDialog->setWindowModality(Qt::ApplicationModal);
        messageDialog->resize(381, 321);
        messageDialog->setModal(true);
        buttonBox = new QDialogButtonBox(messageDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 280, 361, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);
        buttonBox->setCenterButtons(false);
        textEdit = new QTextEdit(messageDialog);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 10, 361, 271));
        QPalette palette;
        QBrush brush(QColor(224, 223, 227, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        textEdit->setPalette(palette);
        QFont font;
        font.setPointSize(8);
        textEdit->setFont(font);
        textEdit->setUndoRedoEnabled(false);
        textEdit->setReadOnly(true);

        retranslateUi(messageDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), messageDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(messageDialog);
    } // setupUi

    void retranslateUi(QDialog *messageDialog)
    {
        messageDialog->setWindowTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class messageDialog: public Ui_messageDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEDIALOG_H
