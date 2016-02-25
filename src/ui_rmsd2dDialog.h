/********************************************************************************
** Form generated from reading UI file 'rmsd2dDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RMSD2DDIALOG_H
#define UI_RMSD2DDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_rmsd2dDialog
{
public:
    QGroupBox *groupBox_2;
    QLabel *sel_label;
    QLineEdit *sel_string;
    QLabel *sel_atCount;
    QLabel *sel_label2;
    QLabel *sel_labelResults;
    QPushButton *sel_parse;
    QGroupBox *groupBox;
    QComboBox *sCombo;
    QLabel *label_2;
    QGroupBox *trajectoryRange;
    QLabel *label_4;
    QSpinBox *eIndex;
    QSpinBox *sIndex;
    QLabel *label_5;
    QGroupBox *groupBox_3;
    QLabel *progressBarMsg;
    QProgressBar *progressBar;
    QPushButton *btnClose;
    QPushButton *btnRebuild;
    QPushButton *btnSave;
    QGroupBox *groupBox_4;
    QGroupBox *groupBox_5;
    QRadioButton *optRGB;
    QLabel *label;
    QRadioButton *optGray;
    QCheckBox *optSmooth;
    QCheckBox *optLegend;
    QLineEdit *plotTitle;
    QLabel *label_3;
    QLabel *label_6;
    QCheckBox *optAutoPlotMin;
    QDoubleSpinBox *spinPlotMin;

    void setupUi(QDialog *rmsd2dDialog)
    {
        if (rmsd2dDialog->objectName().isEmpty())
            rmsd2dDialog->setObjectName(QString::fromUtf8("rmsd2dDialog"));
        rmsd2dDialog->resize(621, 828);
        groupBox_2 = new QGroupBox(rmsd2dDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 130, 601, 101));
        sel_label = new QLabel(groupBox_2);
        sel_label->setObjectName(QString::fromUtf8("sel_label"));
        sel_label->setGeometry(QRect(20, 20, 111, 21));
        sel_string = new QLineEdit(groupBox_2);
        sel_string->setObjectName(QString::fromUtf8("sel_string"));
        sel_string->setGeometry(QRect(150, 20, 331, 20));
        sel_atCount = new QLabel(groupBox_2);
        sel_atCount->setObjectName(QString::fromUtf8("sel_atCount"));
        sel_atCount->setGeometry(QRect(520, 20, 61, 21));
        sel_atCount->setAlignment(Qt::AlignCenter);
        sel_label2 = new QLabel(groupBox_2);
        sel_label2->setObjectName(QString::fromUtf8("sel_label2"));
        sel_label2->setGeometry(QRect(20, 50, 111, 20));
        sel_label2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults = new QLabel(groupBox_2);
        sel_labelResults->setObjectName(QString::fromUtf8("sel_labelResults"));
        sel_labelResults->setGeometry(QRect(150, 50, 431, 41));
        sel_labelResults->setTextFormat(Qt::AutoText);
        sel_labelResults->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults->setWordWrap(true);
        sel_parse = new QPushButton(groupBox_2);
        sel_parse->setObjectName(QString::fromUtf8("sel_parse"));
        sel_parse->setGeometry(QRect(480, 20, 22, 22));
        groupBox = new QGroupBox(rmsd2dDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 601, 111));
        sCombo = new QComboBox(groupBox);
        sCombo->setObjectName(QString::fromUtf8("sCombo"));
        sCombo->setGeometry(QRect(150, 20, 431, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 21, 121, 20));
        trajectoryRange = new QGroupBox(groupBox);
        trajectoryRange->setObjectName(QString::fromUtf8("trajectoryRange"));
        trajectoryRange->setGeometry(QRect(20, 50, 561, 51));
        trajectoryRange->setFlat(false);
        trajectoryRange->setCheckable(true);
        trajectoryRange->setChecked(false);
        label_4 = new QLabel(trajectoryRange);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(310, 20, 101, 21));
        eIndex = new QSpinBox(trajectoryRange);
        eIndex->setObjectName(QString::fromUtf8("eIndex"));
        eIndex->setGeometry(QRect(410, 20, 61, 22));
        eIndex->setMinimum(1);
        eIndex->setMaximum(999999);
        eIndex->setValue(9999);
        sIndex = new QSpinBox(trajectoryRange);
        sIndex->setObjectName(QString::fromUtf8("sIndex"));
        sIndex->setGeometry(QRect(150, 20, 61, 22));
        sIndex->setMinimum(1);
        sIndex->setMaximum(999999);
        label_5 = new QLabel(trajectoryRange);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(50, 20, 101, 21));
        groupBox_3 = new QGroupBox(rmsd2dDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 350, 601, 441));
        progressBarMsg = new QLabel(groupBox_3);
        progressBarMsg->setObjectName(QString::fromUtf8("progressBarMsg"));
        progressBarMsg->setGeometry(QRect(50, 190, 501, 31));
        progressBarMsg->setAlignment(Qt::AlignCenter);
        progressBar = new QProgressBar(groupBox_3);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(170, 230, 271, 16));
        progressBar->setValue(0);
        btnClose = new QPushButton(rmsd2dDialog);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));
        btnClose->setGeometry(QRect(350, 800, 71, 23));
        btnRebuild = new QPushButton(rmsd2dDialog);
        btnRebuild->setObjectName(QString::fromUtf8("btnRebuild"));
        btnRebuild->setGeometry(QRect(190, 800, 71, 23));
        btnSave = new QPushButton(rmsd2dDialog);
        btnSave->setObjectName(QString::fromUtf8("btnSave"));
        btnSave->setEnabled(false);
        btnSave->setGeometry(QRect(270, 800, 71, 23));
        groupBox_4 = new QGroupBox(rmsd2dDialog);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 240, 601, 101));
        groupBox_5 = new QGroupBox(groupBox_4);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(20, 20, 251, 41));
        optRGB = new QRadioButton(groupBox_5);
        optRGB->setObjectName(QString::fromUtf8("optRGB"));
        optRGB->setGeometry(QRect(190, 10, 51, 21));
        optRGB->setChecked(true);
        label = new QLabel(groupBox_5);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 71, 21));
        optGray = new QRadioButton(groupBox_5);
        optGray->setObjectName(QString::fromUtf8("optGray"));
        optGray->setGeometry(QRect(90, 10, 101, 21));
        optSmooth = new QCheckBox(groupBox_4);
        optSmooth->setObjectName(QString::fromUtf8("optSmooth"));
        optSmooth->setGeometry(QRect(20, 70, 121, 21));
        optSmooth->setChecked(true);
        optLegend = new QCheckBox(groupBox_4);
        optLegend->setObjectName(QString::fromUtf8("optLegend"));
        optLegend->setGeometry(QRect(150, 70, 121, 21));
        optLegend->setChecked(true);
        plotTitle = new QLineEdit(groupBox_4);
        plotTitle->setObjectName(QString::fromUtf8("plotTitle"));
        plotTitle->setGeometry(QRect(290, 40, 281, 21));
        label_3 = new QLabel(groupBox_4);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(280, 16, 331, 20));
        label_3->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        label_3->setWordWrap(true);
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(290, 70, 151, 21));
        label_6->setWordWrap(true);
        optAutoPlotMin = new QCheckBox(groupBox_4);
        optAutoPlotMin->setObjectName(QString::fromUtf8("optAutoPlotMin"));
        optAutoPlotMin->setGeometry(QRect(440, 70, 61, 21));
        optAutoPlotMin->setChecked(true);
        spinPlotMin = new QDoubleSpinBox(groupBox_4);
        spinPlotMin->setObjectName(QString::fromUtf8("spinPlotMin"));
        spinPlotMin->setEnabled(false);
        spinPlotMin->setGeometry(QRect(500, 70, 71, 22));
        spinPlotMin->setMinimum(0.1);
        spinPlotMin->setSingleStep(0.1);
        spinPlotMin->setValue(1);
#ifndef QT_NO_SHORTCUT
        sel_label->setBuddy(sel_string);
        label_2->setBuddy(sCombo);
        label_4->setBuddy(eIndex);
        label_5->setBuddy(sIndex);
        label_3->setBuddy(plotTitle);
        label_6->setBuddy(spinPlotMin);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sCombo, trajectoryRange);
        QWidget::setTabOrder(trajectoryRange, sIndex);
        QWidget::setTabOrder(sIndex, eIndex);
        QWidget::setTabOrder(eIndex, sel_string);
        QWidget::setTabOrder(sel_string, sel_parse);
        QWidget::setTabOrder(sel_parse, optGray);
        QWidget::setTabOrder(optGray, optRGB);
        QWidget::setTabOrder(optRGB, optSmooth);
        QWidget::setTabOrder(optSmooth, optLegend);
        QWidget::setTabOrder(optLegend, plotTitle);
        QWidget::setTabOrder(plotTitle, btnRebuild);
        QWidget::setTabOrder(btnRebuild, btnSave);
        QWidget::setTabOrder(btnSave, btnClose);

        retranslateUi(rmsd2dDialog);
        QObject::connect(optAutoPlotMin, SIGNAL(toggled(bool)), spinPlotMin, SLOT(setDisabled(bool)));

        QMetaObject::connectSlotsByName(rmsd2dDialog);
    } // setupUi

    void retranslateUi(QDialog *rmsd2dDialog)
    {
        rmsd2dDialog->setWindowTitle(QApplication::translate("rmsd2dDialog", "2D RMSD", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("rmsd2dDialog", "Selection", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("rmsd2dDialog", "Selection &Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("rmsd2dDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("rmsd2dDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
        groupBox->setTitle(QApplication::translate("rmsd2dDialog", "Stream", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("rmsd2dDialog", "&Stream Source:", 0, QApplication::UnicodeUTF8));
        trajectoryRange->setTitle(QApplication::translate("rmsd2dDialog", "Trajectory Range", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("rmsd2dDialog", "&End Index:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("rmsd2dDialog", "&Start Index:", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("rmsd2dDialog", "2D RMSD Map", 0, QApplication::UnicodeUTF8));
        progressBarMsg->setText(QString());
#ifndef QT_NO_STATUSTIP
        btnClose->setStatusTip(QApplication::translate("rmsd2dDialog", "Close Tool Window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        btnClose->setText(QApplication::translate("rmsd2dDialog", "&Close", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        btnRebuild->setStatusTip(QApplication::translate("rmsd2dDialog", "Update Plot", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        btnRebuild->setText(QApplication::translate("rmsd2dDialog", "&Build", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        btnSave->setStatusTip(QApplication::translate("rmsd2dDialog", "Save Plot to Image File...", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        btnSave->setText(QApplication::translate("rmsd2dDialog", "&Save...", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("rmsd2dDialog", "Options", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QString());
        optRGB->setText(QApplication::translate("rmsd2dDialog", "&RGB", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("rmsd2dDialog", "Map Type:", 0, QApplication::UnicodeUTF8));
        optGray->setText(QApplication::translate("rmsd2dDialog", "&Grayscale", 0, QApplication::UnicodeUTF8));
        optSmooth->setText(QApplication::translate("rmsd2dDialog", "S&mooth Texture", 0, QApplication::UnicodeUTF8));
        optLegend->setText(QApplication::translate("rmsd2dDialog", "Display &Legend", 0, QApplication::UnicodeUTF8));
        plotTitle->setText(QApplication::translate("rmsd2dDialog", "2D RMSD - %s", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("rmsd2dDialog", "Plot &Title Mask (\"%s\" is replaced with Stream Title):", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("rmsd2dDialog", "Minimum Plotted &Value:", 0, QApplication::UnicodeUTF8));
        optAutoPlotMin->setText(QApplication::translate("rmsd2dDialog", "A&uto", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class rmsd2dDialog: public Ui_rmsd2dDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RMSD2DDIALOG_H
