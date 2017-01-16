/********************************************************************************
** Form generated from reading UI file 'progressDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROGRESSDIALOG_H
#define UI_PROGRESSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_progressDialog
{
public:
    QGroupBox *groupBox;
    QLabel *lblStreams;
    QProgressBar *streamProgress;
    QLabel *lblCurrent;
    QProgressBar *currentProgress;
    QPushButton *pushButton;

    void setupUi(QDialog *progressDialog)
    {
        if (progressDialog->objectName().isEmpty())
            progressDialog->setObjectName(QString::fromUtf8("progressDialog"));
        progressDialog->resize(521, 203);
        groupBox = new QGroupBox(progressDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 501, 151));
        lblStreams = new QLabel(groupBox);
        lblStreams->setObjectName(QString::fromUtf8("lblStreams"));
        lblStreams->setGeometry(QRect(20, 15, 471, 21));
        streamProgress = new QProgressBar(groupBox);
        streamProgress->setObjectName(QString::fromUtf8("streamProgress"));
        streamProgress->setGeometry(QRect(30, 40, 451, 16));
        streamProgress->setValue(24);
        streamProgress->setTextVisible(false);
        streamProgress->setTextDirection(QProgressBar::TopToBottom);
        lblCurrent = new QLabel(groupBox);
        lblCurrent->setObjectName(QString::fromUtf8("lblCurrent"));
        lblCurrent->setGeometry(QRect(20, 70, 471, 21));
        currentProgress = new QProgressBar(groupBox);
        currentProgress->setObjectName(QString::fromUtf8("currentProgress"));
        currentProgress->setGeometry(QRect(30, 100, 451, 16));
        currentProgress->setValue(24);
        currentProgress->setTextVisible(false);
        currentProgress->setTextDirection(QProgressBar::TopToBottom);
        pushButton = new QPushButton(progressDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(220, 170, 75, 23));

        retranslateUi(progressDialog);

        QMetaObject::connectSlotsByName(progressDialog);
    } // setupUi

    void retranslateUi(QDialog *progressDialog)
    {
        progressDialog->setWindowTitle(QApplication::translate("progressDialog", "Build Progress", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        lblStreams->setText(QApplication::translate("progressDialog", "Stream: 0/0", 0, QApplication::UnicodeUTF8));
        lblCurrent->setText(QApplication::translate("progressDialog", "Current Stream: 0/0", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("progressDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class progressDialog: public Ui_progressDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROGRESSDIALOG_H
