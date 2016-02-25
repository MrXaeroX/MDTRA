/********************************************************************************
** Form generated from reading UI file 'resultDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESULTDIALOG_H
#define UI_RESULTDIALOG_H

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
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_resultDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QGroupBox *groupBox_2;
    QComboBox *dsTypeCombo;
    QLabel *label_2;
    QListWidget *dsList;
    QLabel *label_3;
    QPushButton *dsAdd;
    QPushButton *dsEdit;
    QPushButton *dsRemove;
    QPushButton *dsDown;
    QPushButton *dsUp;
    QLabel *label_4;
    QComboBox *dsScaleUnitsCombo;
    QPushButton *dsAddMulti;
    QLabel *rbLabel;
    QRadioButton *rbTime;
    QRadioButton *rbRes;

    void setupUi(QDialog *resultDialog)
    {
        if (resultDialog->objectName().isEmpty())
            resultDialog->setObjectName(QString::fromUtf8("resultDialog"));
        resultDialog->resize(569, 534);
        buttonBox = new QDialogButtonBox(resultDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 500, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(resultDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 551, 80));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(130, 30, 391, 20));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 111, 21));
        groupBox_2 = new QGroupBox(resultDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 100, 551, 391));
        dsTypeCombo = new QComboBox(groupBox_2);
        dsTypeCombo->setObjectName(QString::fromUtf8("dsTypeCombo"));
        dsTypeCombo->setGeometry(QRect(130, 30, 391, 22));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 30, 111, 21));
        dsList = new QListWidget(groupBox_2);
        dsList->setObjectName(QString::fromUtf8("dsList"));
        dsList->setGeometry(QRect(20, 100, 411, 201));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 70, 411, 21));
        dsAdd = new QPushButton(groupBox_2);
        dsAdd->setObjectName(QString::fromUtf8("dsAdd"));
        dsAdd->setGeometry(QRect(450, 100, 81, 23));
        dsEdit = new QPushButton(groupBox_2);
        dsEdit->setObjectName(QString::fromUtf8("dsEdit"));
        dsEdit->setGeometry(QRect(450, 160, 81, 23));
        dsRemove = new QPushButton(groupBox_2);
        dsRemove->setObjectName(QString::fromUtf8("dsRemove"));
        dsRemove->setGeometry(QRect(450, 190, 81, 23));
        dsDown = new QPushButton(groupBox_2);
        dsDown->setObjectName(QString::fromUtf8("dsDown"));
        dsDown->setGeometry(QRect(450, 280, 81, 23));
        dsUp = new QPushButton(groupBox_2);
        dsUp->setObjectName(QString::fromUtf8("dsUp"));
        dsUp->setGeometry(QRect(450, 250, 81, 23));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 330, 111, 20));
        dsScaleUnitsCombo = new QComboBox(groupBox_2);
        dsScaleUnitsCombo->setObjectName(QString::fromUtf8("dsScaleUnitsCombo"));
        dsScaleUnitsCombo->setGeometry(QRect(130, 330, 301, 22));
        dsAddMulti = new QPushButton(groupBox_2);
        dsAddMulti->setObjectName(QString::fromUtf8("dsAddMulti"));
        dsAddMulti->setGeometry(QRect(450, 130, 81, 23));
        rbLabel = new QLabel(groupBox_2);
        rbLabel->setObjectName(QString::fromUtf8("rbLabel"));
        rbLabel->setGeometry(QRect(20, 360, 111, 16));
        rbTime = new QRadioButton(groupBox_2);
        rbTime->setObjectName(QString::fromUtf8("rbTime"));
        rbTime->setGeometry(QRect(130, 360, 151, 17));
        rbRes = new QRadioButton(groupBox_2);
        rbRes->setObjectName(QString::fromUtf8("rbRes"));
        rbRes->setGeometry(QRect(290, 360, 251, 17));
#ifndef QT_NO_SHORTCUT
        label->setBuddy(lineEdit);
        label_2->setBuddy(dsTypeCombo);
        label_3->setBuddy(dsList);
        label_4->setBuddy(dsScaleUnitsCombo);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(lineEdit, dsTypeCombo);
        QWidget::setTabOrder(dsTypeCombo, dsList);
        QWidget::setTabOrder(dsList, dsAdd);
        QWidget::setTabOrder(dsAdd, dsAddMulti);
        QWidget::setTabOrder(dsAddMulti, dsEdit);
        QWidget::setTabOrder(dsEdit, dsRemove);
        QWidget::setTabOrder(dsRemove, dsUp);
        QWidget::setTabOrder(dsUp, dsDown);
        QWidget::setTabOrder(dsDown, dsScaleUnitsCombo);
        QWidget::setTabOrder(dsScaleUnitsCombo, buttonBox);

        retranslateUi(resultDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), resultDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(resultDialog);
    } // setupUi

    void retranslateUi(QDialog *resultDialog)
    {
        resultDialog->setWindowTitle(QString());
        groupBox->setTitle(QApplication::translate("resultDialog", "Result Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("resultDialog", "Result &Title:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("resultDialog", "Data Sources", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("resultDialog", "Data Source &Type:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("resultDialog", "Data Source &List:", 0, QApplication::UnicodeUTF8));
        dsAdd->setText(QApplication::translate("resultDialog", "Add...", 0, QApplication::UnicodeUTF8));
        dsEdit->setText(QApplication::translate("resultDialog", "Edit...", 0, QApplication::UnicodeUTF8));
        dsRemove->setText(QApplication::translate("resultDialog", "Remove", 0, QApplication::UnicodeUTF8));
        dsDown->setText(QApplication::translate("resultDialog", "Down", 0, QApplication::UnicodeUTF8));
        dsUp->setText(QApplication::translate("resultDialog", "Up", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("resultDialog", "&Scale Units:", 0, QApplication::UnicodeUTF8));
        dsAddMulti->setText(QApplication::translate("resultDialog", "Multiple...", 0, QApplication::UnicodeUTF8));
        rbLabel->setText(QApplication::translate("resultDialog", "Layout:", 0, QApplication::UnicodeUTF8));
        rbTime->setText(QApplication::translate("resultDialog", "&Time Based", 0, QApplication::UnicodeUTF8));
        rbRes->setText(QApplication::translate("resultDialog", "&Residue Based", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class resultDialog: public Ui_resultDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESULTDIALOG_H
