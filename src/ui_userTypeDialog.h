/********************************************************************************
** Form generated from reading UI file 'userTypeDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERTYPEDIALOG_H
#define UI_USERTYPEDIALOG_H

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
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_userTypeDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_2;
    QWidget *codeEditHolder;
    QLabel *label_3;
    QTextEdit *compilerOutput;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QLabel *label;
    QLineEdit *sUnitTitle;
    QLabel *label_2;
    QLabel *label_4;
    QComboBox *cUnitType;

    void setupUi(QDialog *userTypeDialog)
    {
        if (userTypeDialog->objectName().isEmpty())
            userTypeDialog->setObjectName(QString::fromUtf8("userTypeDialog"));
        userTypeDialog->resize(621, 615);
        buttonBox = new QDialogButtonBox(userTypeDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 580, 601, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox_2 = new QGroupBox(userTypeDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 140, 601, 431));
        codeEditHolder = new QWidget(groupBox_2);
        codeEditHolder->setObjectName(QString::fromUtf8("codeEditHolder"));
        codeEditHolder->setGeometry(QRect(10, 50, 581, 241));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        font.setPointSize(10);
        codeEditHolder->setFont(font);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 300, 581, 20));
        compilerOutput = new QTextEdit(groupBox_2);
        compilerOutput->setObjectName(QString::fromUtf8("compilerOutput"));
        compilerOutput->setGeometry(QRect(10, 320, 581, 101));
        QPalette palette;
        QBrush brush(QColor(224, 223, 227, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        compilerOutput->setPalette(palette);
        compilerOutput->setFrameShape(QFrame::Panel);
        compilerOutput->setFrameShadow(QFrame::Sunken);
        compilerOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        compilerOutput->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        compilerOutput->setUndoRedoEnabled(false);
        compilerOutput->setReadOnly(true);
        compilerOutput->setTabStopWidth(32);
        verticalLayoutWidget = new QWidget(groupBox_2);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 20, 581, 281));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 4);
        label_3->raise();
        compilerOutput->raise();
        verticalLayoutWidget->raise();
        codeEditHolder->raise();
        groupBox = new QGroupBox(userTypeDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 601, 121));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 131, 21));
        sUnitTitle = new QLineEdit(groupBox);
        sUnitTitle->setObjectName(QString::fromUtf8("sUnitTitle"));
        sUnitTitle->setGeometry(QRect(150, 30, 431, 20));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(320, 60, 261, 61));
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_2->setWordWrap(true);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 70, 131, 21));
        cUnitType = new QComboBox(groupBox);
        cUnitType->setObjectName(QString::fromUtf8("cUnitType"));
        cUnitType->setGeometry(QRect(150, 70, 151, 22));
#ifndef QT_NO_SHORTCUT
        label->setBuddy(sUnitTitle);
        label_4->setBuddy(cUnitType);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sUnitTitle, cUnitType);
        QWidget::setTabOrder(cUnitType, compilerOutput);
        QWidget::setTabOrder(compilerOutput, buttonBox);

        retranslateUi(userTypeDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), userTypeDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(userTypeDialog);
    } // setupUi

    void retranslateUi(QDialog *userTypeDialog)
    {
        userTypeDialog->setWindowTitle(QString());
        groupBox_2->setTitle(QApplication::translate("userTypeDialog", "Program Source &Code", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("userTypeDialog", "Compiler Messages:", 0, QApplication::UnicodeUTF8));
        compilerOutput->setDocumentTitle(QString());
        compilerOutput->setHtml(QApplication::translate("userTypeDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("userTypeDialog", "Type &Description", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("userTypeDialog", "&Unit Title:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("userTypeDialog", "Data Sources of user-defined type with equal Unit Titles may be added to the same Result Collector.", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("userTypeDialog", "Unit T&ype:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class userTypeDialog: public Ui_userTypeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERTYPEDIALOG_H
