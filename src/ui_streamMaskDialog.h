/********************************************************************************
** Form generated from reading UI file 'streamMaskDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STREAMMASKDIALOG_H
#define UI_STREAMMASKDIALOG_H

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
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_streamMaskDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLineEdit *lbStreamMask;
    QLabel *label;
    QPushButton *buttonBrowse;
    QGroupBox *valueRangeBox;
    QSpinBox *eIndex;
    QLabel *label_2;
    QLabel *label_3;
    QSpinBox *sIndex;

    void setupUi(QDialog *streamMaskDialog)
    {
        if (streamMaskDialog->objectName().isEmpty())
            streamMaskDialog->setObjectName(QString::fromUtf8("streamMaskDialog"));
        streamMaskDialog->resize(590, 242);
        buttonBox = new QDialogButtonBox(streamMaskDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 200, 571, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(streamMaskDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 571, 181));
        lbStreamMask = new QLineEdit(groupBox);
        lbStreamMask->setObjectName(QString::fromUtf8("lbStreamMask"));
        lbStreamMask->setGeometry(QRect(160, 30, 371, 21));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 141, 21));
        buttonBrowse = new QPushButton(groupBox);
        buttonBrowse->setObjectName(QString::fromUtf8("buttonBrowse"));
        buttonBrowse->setGeometry(QRect(530, 30, 21, 21));
        valueRangeBox = new QGroupBox(groupBox);
        valueRangeBox->setObjectName(QString::fromUtf8("valueRangeBox"));
        valueRangeBox->setGeometry(QRect(20, 70, 531, 91));
        valueRangeBox->setCheckable(true);
        valueRangeBox->setChecked(false);
        eIndex = new QSpinBox(valueRangeBox);
        eIndex->setObjectName(QString::fromUtf8("eIndex"));
        eIndex->setGeometry(QRect(410, 40, 61, 22));
        eIndex->setMaximum(999999);
        eIndex->setValue(9999);
        label_2 = new QLabel(valueRangeBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 40, 121, 21));
        label_3 = new QLabel(valueRangeBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(290, 40, 121, 21));
        sIndex = new QSpinBox(valueRangeBox);
        sIndex->setObjectName(QString::fromUtf8("sIndex"));
        sIndex->setGeometry(QRect(150, 40, 61, 22));
        sIndex->setMaximum(999999);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(lbStreamMask);
        label_2->setBuddy(sIndex);
        label_3->setBuddy(eIndex);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(lbStreamMask, buttonBox);

        retranslateUi(streamMaskDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), streamMaskDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), streamMaskDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(streamMaskDialog);
    } // setupUi

    void retranslateUi(QDialog *streamMaskDialog)
    {
        streamMaskDialog->setWindowTitle(QApplication::translate("streamMaskDialog", "Add Files By Mask", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        label->setText(QApplication::translate("streamMaskDialog", "Stream File &Mask:", 0, QApplication::UnicodeUTF8));
        buttonBrowse->setText(QApplication::translate("streamMaskDialog", "...", 0, QApplication::UnicodeUTF8));
        valueRangeBox->setTitle(QApplication::translate("streamMaskDialog", "Use Mask Value Range:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("streamMaskDialog", "&Start Index:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("streamMaskDialog", "&End Index:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class streamMaskDialog: public Ui_streamMaskDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STREAMMASKDIALOG_H
