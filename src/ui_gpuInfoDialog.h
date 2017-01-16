/********************************************************************************
** Form generated from reading UI file 'gpuInfoDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GPUINFODIALOG_H
#define UI_GPUINFODIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_gpuInfoDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QGroupBox *groupBox;
    QComboBox *comboBox;
    QLabel *label_2;
    QTextEdit *textEdit;
    QLabel *label_3;
    QCheckBox *checkBox;

    void setupUi(QDialog *gpuInfoDialog)
    {
        if (gpuInfoDialog->objectName().isEmpty())
            gpuInfoDialog->setObjectName(QString::fromUtf8("gpuInfoDialog"));
        gpuInfoDialog->resize(520, 339);
        buttonBox = new QDialogButtonBox(gpuInfoDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 300, 501, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);
        buttonBox->setCenterButtons(true);
        label = new QLabel(gpuInfoDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 131, 111));
        groupBox = new QGroupBox(gpuInfoDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(150, 10, 361, 281));
        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(90, 20, 251, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 20, 71, 21));
        textEdit = new QTextEdit(groupBox);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(20, 80, 321, 161));
        QPalette palette;
        QBrush brush(QColor(224, 223, 227, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        textEdit->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        font.setPointSize(10);
        textEdit->setFont(font);
        textEdit->setAutoFillBackground(true);
        textEdit->setFrameShape(QFrame::Panel);
        textEdit->setFrameShadow(QFrame::Sunken);
        textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit->setUndoRedoEnabled(false);
        textEdit->setReadOnly(true);
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 55, 321, 21));
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(20, 250, 331, 20));
        checkBox->setCheckable(true);
        checkBox->setChecked(false);
#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(comboBox);
#endif // QT_NO_SHORTCUT

        retranslateUi(gpuInfoDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), gpuInfoDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(gpuInfoDialog);
    } // setupUi

    void retranslateUi(QDialog *gpuInfoDialog)
    {
        gpuInfoDialog->setWindowTitle(QApplication::translate("gpuInfoDialog", "GPU Information", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        groupBox->setTitle(QString());
        label_2->setText(QApplication::translate("gpuInfoDialog", "&GPU:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("gpuInfoDialog", "Information:", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("gpuInfoDialog", "MDTRA will use this GPU in CUDA computations", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class gpuInfoDialog: public Ui_gpuInfoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GPUINFODIALOG_H
