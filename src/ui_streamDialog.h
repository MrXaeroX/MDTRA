/********************************************************************************
** Form generated from reading UI file 'streamDialog.ui'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STREAMDIALOG_H
#define UI_STREAMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_streamDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QGroupBox *groupBox_2;
    QListWidget *sList;
    QPushButton *sbAdd;
    QPushButton *sbRemove;
    QPushButton *sbAddMask;
    QLabel *label_4;
    QComboBox *formatCombo;
    QCheckBox *relativeCb;
    QCheckBox *hetatmCb;
    QCheckBox *solventCb;
    QGroupBox *groupBox_3;
    QLabel *label_2;
    QDoubleSpinBox *timestep;
    QLabel *label_3;

    void setupUi(QDialog *streamDialog)
    {
        if (streamDialog->objectName().isEmpty())
            streamDialog->setObjectName(QString::fromUtf8("streamDialog"));
        streamDialog->resize(569, 604);
        buttonBox = new QDialogButtonBox(streamDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 570, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(streamDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 551, 71));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(130, 30, 401, 20));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 111, 21));
        groupBox_2 = new QGroupBox(streamDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 90, 551, 371));
        sList = new QListWidget(groupBox_2);
        sList->setObjectName(QString::fromUtf8("sList"));
        sList->setGeometry(QRect(20, 30, 421, 231));
        sList->setSelectionMode(QAbstractItemView::MultiSelection);
        sbAdd = new QPushButton(groupBox_2);
        sbAdd->setObjectName(QString::fromUtf8("sbAdd"));
        sbAdd->setGeometry(QRect(460, 30, 75, 23));
        sbRemove = new QPushButton(groupBox_2);
        sbRemove->setObjectName(QString::fromUtf8("sbRemove"));
        sbRemove->setGeometry(QRect(460, 90, 75, 23));
        sbAddMask = new QPushButton(groupBox_2);
        sbAddMask->setObjectName(QString::fromUtf8("sbAddMask"));
        sbAddMask->setGeometry(QRect(460, 60, 75, 23));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 270, 141, 21));
        formatCombo = new QComboBox(groupBox_2);
        formatCombo->setObjectName(QString::fromUtf8("formatCombo"));
        formatCombo->setGeometry(QRect(160, 270, 281, 22));
        relativeCb = new QCheckBox(groupBox_2);
        relativeCb->setObjectName(QString::fromUtf8("relativeCb"));
        relativeCb->setGeometry(QRect(20, 300, 421, 21));
        relativeCb->setChecked(true);
        hetatmCb = new QCheckBox(groupBox_2);
        hetatmCb->setObjectName(QString::fromUtf8("hetatmCb"));
        hetatmCb->setGeometry(QRect(20, 320, 421, 21));
        hetatmCb->setChecked(true);
        solventCb = new QCheckBox(groupBox_2);
        solventCb->setObjectName(QString::fromUtf8("solventCb"));
        solventCb->setGeometry(QRect(20, 340, 421, 21));
        solventCb->setChecked(false);
        groupBox_3 = new QGroupBox(streamDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 470, 551, 91));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 30, 191, 21));
        timestep = new QDoubleSpinBox(groupBox_3);
        timestep->setObjectName(QString::fromUtf8("timestep"));
        timestep->setGeometry(QRect(210, 30, 81, 20));
        timestep->setDecimals(3);
        timestep->setMinimum(0.001);
        timestep->setMaximum(99.999);
        timestep->setValue(1);
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(310, 20, 231, 61));
        label_3->setAlignment(Qt::AlignJustify|Qt::AlignTop);
        label_3->setWordWrap(true);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(lineEdit);
        label_4->setBuddy(formatCombo);
        label_2->setBuddy(timestep);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(lineEdit, sList);
        QWidget::setTabOrder(sList, sbAdd);
        QWidget::setTabOrder(sbAdd, sbAddMask);
        QWidget::setTabOrder(sbAddMask, sbRemove);
        QWidget::setTabOrder(sbRemove, formatCombo);
        QWidget::setTabOrder(formatCombo, timestep);
        QWidget::setTabOrder(timestep, buttonBox);

        retranslateUi(streamDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), streamDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(streamDialog);
    } // setupUi

    void retranslateUi(QDialog *streamDialog)
    {
        streamDialog->setWindowTitle(QString());
        groupBox->setTitle(QApplication::translate("streamDialog", "Stream Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("streamDialog", "Stream &Title:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("streamDialog", "Stream Data", 0, QApplication::UnicodeUTF8));
        sbAdd->setText(QApplication::translate("streamDialog", "Add...", 0, QApplication::UnicodeUTF8));
        sbRemove->setText(QApplication::translate("streamDialog", "Remove", 0, QApplication::UnicodeUTF8));
        sbAddMask->setText(QApplication::translate("streamDialog", "Mask...", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("streamDialog", "PDB File &Format:", 0, QApplication::UnicodeUTF8));
        relativeCb->setText(QApplication::translate("streamDialog", "Use &Relative Path Names", 0, QApplication::UnicodeUTF8));
        hetatmCb->setText(QApplication::translate("streamDialog", "Ignore &HETATM Records", 0, QApplication::UnicodeUTF8));
        solventCb->setText(QApplication::translate("streamDialog", "Ignore &Solvent Groups", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("streamDialog", "Trajectory Information", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("streamDialog", "Trajectory &Time Step, ps:", 0, QApplication::UnicodeUTF8));
        timestep->setPrefix(QString());
        timestep->setSuffix(QString());
        label_3->setText(QApplication::translate("streamDialog", "Please specify how much time (in picoseconds) does every single trajectory step take, so it can be rescaled to time on the Plot.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class streamDialog: public Ui_streamDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STREAMDIALOG_H
