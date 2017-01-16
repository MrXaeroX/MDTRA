/********************************************************************************
** Form generated from reading UI file 'histogramDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTOGRAMDIALOG_H
#define UI_HISTOGRAMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_histogramDialog
{
public:
    QGroupBox *groupBox;
    QComboBox *rCombo;
    QLabel *label_2;
    QLabel *label_4;
    QComboBox *dCombo;
    QCheckBox *cbAuto;
    QGroupBox *groupBox_3;
    QLabel *progressBarMsg;
    QProgressBar *progressBar;
    QPushButton *btnClose;
    QPushButton *btnRebuild;
    QPushButton *btnSave;
    QGroupBox *groupBox_4;
    QLabel *label;
    QSpinBox *bandSpin;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QComboBox *aCombo;
    QLabel *label_3;
    QWidget *widget;
    QToolButton *colorChangeButton;
    QRadioButton *rbCAuto;
    QRadioButton *rbCCustom;
    QLabel *lblColorValue;
    QRadioButton *rbCGray;
    QPushButton *btnExport;

    void setupUi(QDialog *histogramDialog)
    {
        if (histogramDialog->objectName().isEmpty())
            histogramDialog->setObjectName(QString::fromUtf8("histogramDialog"));
        histogramDialog->resize(621, 609);
        groupBox = new QGroupBox(histogramDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 601, 111));
        rCombo = new QComboBox(groupBox);
        rCombo->setObjectName(QString::fromUtf8("rCombo"));
        rCombo->setGeometry(QRect(150, 20, 431, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 21, 121, 20));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 50, 121, 20));
        dCombo = new QComboBox(groupBox);
        dCombo->setObjectName(QString::fromUtf8("dCombo"));
        dCombo->setGeometry(QRect(150, 50, 431, 22));
        cbAuto = new QCheckBox(groupBox);
        cbAuto->setObjectName(QString::fromUtf8("cbAuto"));
        cbAuto->setGeometry(QRect(150, 80, 431, 21));
        cbAuto->setChecked(true);
        groupBox_3 = new QGroupBox(histogramDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 220, 601, 351));
        progressBarMsg = new QLabel(groupBox_3);
        progressBarMsg->setObjectName(QString::fromUtf8("progressBarMsg"));
        progressBarMsg->setGeometry(QRect(50, 190, 501, 31));
        progressBarMsg->setAlignment(Qt::AlignCenter);
        progressBar = new QProgressBar(groupBox_3);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(180, 170, 271, 16));
        progressBar->setValue(0);
        btnClose = new QPushButton(histogramDialog);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));
        btnClose->setGeometry(QRect(390, 580, 71, 23));
        btnRebuild = new QPushButton(histogramDialog);
        btnRebuild->setObjectName(QString::fromUtf8("btnRebuild"));
        btnRebuild->setGeometry(QRect(150, 580, 71, 23));
        btnSave = new QPushButton(histogramDialog);
        btnSave->setObjectName(QString::fromUtf8("btnSave"));
        btnSave->setEnabled(false);
        btnSave->setGeometry(QRect(230, 580, 71, 23));
        groupBox_4 = new QGroupBox(histogramDialog);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 130, 601, 81));
        label = new QLabel(groupBox_4);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 131, 21));
        bandSpin = new QSpinBox(groupBox_4);
        bandSpin->setObjectName(QString::fromUtf8("bandSpin"));
        bandSpin->setEnabled(false);
        bandSpin->setGeometry(QRect(490, 20, 91, 22));
        bandSpin->setMinimum(2);
        bandSpin->setMaximum(100);
        bandSpin->setValue(16);
        radioButton = new QRadioButton(groupBox_4);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setGeometry(QRect(400, 20, 91, 21));
        radioButton_2 = new QRadioButton(groupBox_4);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
        radioButton_2->setGeometry(QRect(150, 20, 81, 21));
        radioButton_2->setChecked(true);
        aCombo = new QComboBox(groupBox_4);
        aCombo->setObjectName(QString::fromUtf8("aCombo"));
        aCombo->setGeometry(QRect(230, 20, 151, 22));
        label_3 = new QLabel(groupBox_4);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 50, 131, 21));
        widget = new QWidget(groupBox_4);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(130, 50, 461, 21));
        colorChangeButton = new QToolButton(widget);
        colorChangeButton->setObjectName(QString::fromUtf8("colorChangeButton"));
        colorChangeButton->setEnabled(false);
        colorChangeButton->setGeometry(QRect(430, 0, 21, 21));
        rbCAuto = new QRadioButton(widget);
        rbCAuto->setObjectName(QString::fromUtf8("rbCAuto"));
        rbCAuto->setGeometry(QRect(18, 0, 81, 21));
        rbCAuto->setChecked(true);
        rbCCustom = new QRadioButton(widget);
        rbCCustom->setObjectName(QString::fromUtf8("rbCCustom"));
        rbCCustom->setGeometry(QRect(268, 0, 91, 21));
        lblColorValue = new QLabel(widget);
        lblColorValue->setObjectName(QString::fromUtf8("lblColorValue"));
        lblColorValue->setGeometry(QRect(358, 0, 71, 21));
        lblColorValue->setAutoFillBackground(true);
        lblColorValue->setFrameShape(QFrame::StyledPanel);
        rbCGray = new QRadioButton(widget);
        rbCGray->setObjectName(QString::fromUtf8("rbCGray"));
        rbCGray->setGeometry(QRect(128, 0, 101, 21));
        btnExport = new QPushButton(histogramDialog);
        btnExport->setObjectName(QString::fromUtf8("btnExport"));
        btnExport->setEnabled(false);
        btnExport->setGeometry(QRect(310, 580, 71, 23));
#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(rCombo);
        label_4->setBuddy(dCombo);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(rCombo, dCombo);
        QWidget::setTabOrder(dCombo, radioButton_2);
        QWidget::setTabOrder(radioButton_2, aCombo);
        QWidget::setTabOrder(aCombo, radioButton);
        QWidget::setTabOrder(radioButton, bandSpin);
        QWidget::setTabOrder(bandSpin, btnRebuild);
        QWidget::setTabOrder(btnRebuild, btnSave);
        QWidget::setTabOrder(btnSave, btnExport);
        QWidget::setTabOrder(btnExport, btnClose);

        retranslateUi(histogramDialog);
        QObject::connect(radioButton_2, SIGNAL(toggled(bool)), aCombo, SLOT(setEnabled(bool)));
        QObject::connect(radioButton, SIGNAL(toggled(bool)), bandSpin, SLOT(setEnabled(bool)));
        QObject::connect(rbCCustom, SIGNAL(toggled(bool)), colorChangeButton, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(histogramDialog);
    } // setupUi

    void retranslateUi(QDialog *histogramDialog)
    {
        histogramDialog->setWindowTitle(QApplication::translate("histogramDialog", "Histogram", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("histogramDialog", "Result", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("histogramDialog", "Result &Collector:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("histogramDialog", "Result &Data Source:", 0, QApplication::UnicodeUTF8));
        cbAuto->setText(QApplication::translate("histogramDialog", "&Automatic Rebuild", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("histogramDialog", "Histogram", 0, QApplication::UnicodeUTF8));
        progressBarMsg->setText(QString());
#ifndef QT_NO_STATUSTIP
        btnClose->setStatusTip(QApplication::translate("histogramDialog", "Close Tool Window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        btnClose->setText(QApplication::translate("histogramDialog", "&Close", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        btnRebuild->setStatusTip(QApplication::translate("histogramDialog", "Update Plot", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        btnRebuild->setText(QApplication::translate("histogramDialog", "&Build", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        btnSave->setStatusTip(QApplication::translate("histogramDialog", "Save Plot to Image File...", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        btnSave->setText(QApplication::translate("histogramDialog", "&Save...", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("histogramDialog", "Options", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("histogramDialog", "Number of Bins:", 0, QApplication::UnicodeUTF8));
        radioButton->setText(QApplication::translate("histogramDialog", "&Custom", 0, QApplication::UnicodeUTF8));
        radioButton_2->setText(QApplication::translate("histogramDialog", "&Auto", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("histogramDialog", "Histogram Colors:", 0, QApplication::UnicodeUTF8));
        colorChangeButton->setText(QApplication::translate("histogramDialog", "...", 0, QApplication::UnicodeUTF8));
        rbCAuto->setText(QApplication::translate("histogramDialog", "&Auto", 0, QApplication::UnicodeUTF8));
        rbCCustom->setText(QApplication::translate("histogramDialog", "&Single Color", 0, QApplication::UnicodeUTF8));
        lblColorValue->setText(QString());
        rbCGray->setText(QApplication::translate("histogramDialog", "&Grayscale", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        btnExport->setStatusTip(QApplication::translate("histogramDialog", "Save Plot to Image File...", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        btnExport->setText(QApplication::translate("histogramDialog", "&Export...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class histogramDialog: public Ui_histogramDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTOGRAMDIALOG_H
