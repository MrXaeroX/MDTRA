/********************************************************************************
** Form generated from reading UI file 'prepWaterShellDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREPWATERSHELLDIALOG_H
#define UI_PREPWATERSHELLDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_prepWaterShellDialog
{
public:
    QGroupBox *groupBox;
    QComboBox *sCombo;
    QLabel *label_2;
    QGroupBox *trajectoryRange;
    QLabel *label_4;
    QSpinBox *eIndex;
    QSpinBox *sIndex;
    QLabel *label_5;
    QGroupBox *groupBox_2;
    QLabel *label;
    QLineEdit *txtOutputPDB;
    QPushButton *btnOutputPDB;
    QGroupBox *groupBox_3;
    QPlainTextEdit *txtOutput;
    QPushButton *btnRun;
    QPushButton *btnClose;

    void setupUi(QDialog *prepWaterShellDialog)
    {
        if (prepWaterShellDialog->objectName().isEmpty())
            prepWaterShellDialog->setObjectName(QString::fromUtf8("prepWaterShellDialog"));
        prepWaterShellDialog->resize(592, 451);
        groupBox = new QGroupBox(prepWaterShellDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 571, 141));
        sCombo = new QComboBox(groupBox);
        sCombo->setObjectName(QString::fromUtf8("sCombo"));
        sCombo->setGeometry(QRect(150, 30, 401, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 30, 111, 21));
        trajectoryRange = new QGroupBox(groupBox);
        trajectoryRange->setObjectName(QString::fromUtf8("trajectoryRange"));
        trajectoryRange->setGeometry(QRect(20, 60, 531, 61));
        trajectoryRange->setFlat(false);
        trajectoryRange->setCheckable(true);
        trajectoryRange->setChecked(false);
        label_4 = new QLabel(trajectoryRange);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(240, 30, 121, 21));
        eIndex = new QSpinBox(trajectoryRange);
        eIndex->setObjectName(QString::fromUtf8("eIndex"));
        eIndex->setGeometry(QRect(360, 30, 61, 22));
        eIndex->setMinimum(1);
        eIndex->setMaximum(999999);
        eIndex->setValue(9999);
        sIndex = new QSpinBox(trajectoryRange);
        sIndex->setObjectName(QString::fromUtf8("sIndex"));
        sIndex->setGeometry(QRect(150, 30, 61, 22));
        sIndex->setMinimum(1);
        sIndex->setMaximum(999999);
        label_5 = new QLabel(trajectoryRange);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(30, 30, 121, 21));
        groupBox_2 = new QGroupBox(prepWaterShellDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 160, 571, 71));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 121, 20));
        txtOutputPDB = new QLineEdit(groupBox_2);
        txtOutputPDB->setObjectName(QString::fromUtf8("txtOutputPDB"));
        txtOutputPDB->setGeometry(QRect(140, 30, 391, 20));
        btnOutputPDB = new QPushButton(groupBox_2);
        btnOutputPDB->setObjectName(QString::fromUtf8("btnOutputPDB"));
        btnOutputPDB->setGeometry(QRect(530, 30, 20, 20));
        groupBox_3 = new QGroupBox(prepWaterShellDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 240, 571, 171));
        txtOutput = new QPlainTextEdit(groupBox_3);
        txtOutput->setObjectName(QString::fromUtf8("txtOutput"));
        txtOutput->setGeometry(QRect(10, 20, 551, 141));
        QPalette palette;
        QBrush brush(QColor(236, 236, 236, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        QBrush brush1(QColor(240, 240, 240, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        txtOutput->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        font.setPointSize(8);
        txtOutput->setFont(font);
        txtOutput->setFrameShape(QFrame::StyledPanel);
        txtOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        txtOutput->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        txtOutput->setUndoRedoEnabled(false);
        txtOutput->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);
        btnRun = new QPushButton(prepWaterShellDialog);
        btnRun->setObjectName(QString::fromUtf8("btnRun"));
        btnRun->setGeometry(QRect(210, 420, 75, 23));
        btnClose = new QPushButton(prepWaterShellDialog);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));
        btnClose->setGeometry(QRect(290, 420, 75, 23));
#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(sCombo);
        label_4->setBuddy(eIndex);
        label_5->setBuddy(sIndex);
        label->setBuddy(txtOutputPDB);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sCombo, trajectoryRange);
        QWidget::setTabOrder(trajectoryRange, sIndex);
        QWidget::setTabOrder(sIndex, eIndex);
        QWidget::setTabOrder(eIndex, txtOutputPDB);
        QWidget::setTabOrder(txtOutputPDB, btnOutputPDB);
        QWidget::setTabOrder(btnOutputPDB, txtOutput);

        retranslateUi(prepWaterShellDialog);

        QMetaObject::connectSlotsByName(prepWaterShellDialog);
    } // setupUi

    void retranslateUi(QDialog *prepWaterShellDialog)
    {
        prepWaterShellDialog->setWindowTitle(QApplication::translate("prepWaterShellDialog", "Water Shell", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("prepWaterShellDialog", "Streams", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("prepWaterShellDialog", "&Stream Source:", 0, QApplication::UnicodeUTF8));
        trajectoryRange->setTitle(QApplication::translate("prepWaterShellDialog", "Trajectory Range", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("prepWaterShellDialog", "&End Index:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("prepWaterShellDialog", "&Start Index:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("prepWaterShellDialog", "Options", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("prepWaterShellDialog", "&Output PDB File:", 0, QApplication::UnicodeUTF8));
        btnOutputPDB->setText(QApplication::translate("prepWaterShellDialog", "...", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("prepWaterShellDialog", "Program Output", 0, QApplication::UnicodeUTF8));
        btnRun->setText(QApplication::translate("prepWaterShellDialog", "&Run", 0, QApplication::UnicodeUTF8));
        btnClose->setText(QApplication::translate("prepWaterShellDialog", "&Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class prepWaterShellDialog: public Ui_prepWaterShellDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREPWATERSHELLDIALOG_H
