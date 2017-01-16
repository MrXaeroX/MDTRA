/********************************************************************************
** Form generated from reading UI file 'multiResultDataSourceDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MULTIRESULTDATASOURCEDIALOG_H
#define UI_MULTIRESULTDATASOURCEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_multiResultDataSourceDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLabel *label;
    QListWidget *dsList;
    QGroupBox *groupBox_2;
    QLabel *label_2;
    QLabel *label_3;
    QDoubleSpinBox *dsScale;
    QDoubleSpinBox *dsBias;
    QCheckBox *cbVis;

    void setupUi(QDialog *multiResultDataSourceDialog)
    {
        if (multiResultDataSourceDialog->objectName().isEmpty())
            multiResultDataSourceDialog->setObjectName(QString::fromUtf8("multiResultDataSourceDialog"));
        multiResultDataSourceDialog->resize(491, 378);
        buttonBox = new QDialogButtonBox(multiResultDataSourceDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 340, 471, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(multiResultDataSourceDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 471, 211));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 111, 21));
        dsList = new QListWidget(groupBox);
        dsList->setObjectName(QString::fromUtf8("dsList"));
        dsList->setGeometry(QRect(130, 30, 321, 161));
        dsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        dsList->setSelectionMode(QAbstractItemView::MultiSelection);
        groupBox_2 = new QGroupBox(multiResultDataSourceDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 230, 471, 71));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(50, 30, 111, 21));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(250, 30, 111, 21));
        dsScale = new QDoubleSpinBox(groupBox_2);
        dsScale->setObjectName(QString::fromUtf8("dsScale"));
        dsScale->setGeometry(QRect(160, 30, 62, 22));
        dsScale->setMinimum(-1000);
        dsScale->setMaximum(1000);
        dsScale->setValue(1);
        dsBias = new QDoubleSpinBox(groupBox_2);
        dsBias->setObjectName(QString::fromUtf8("dsBias"));
        dsBias->setGeometry(QRect(360, 30, 62, 22));
        dsBias->setMinimum(-1000);
        dsBias->setMaximum(1000);
        dsBias->setValue(0);
        cbVis = new QCheckBox(multiResultDataSourceDialog);
        cbVis->setObjectName(QString::fromUtf8("cbVis"));
        cbVis->setGeometry(QRect(30, 310, 451, 17));
        cbVis->setChecked(true);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(dsList);
        label_2->setBuddy(dsScale);
        label_3->setBuddy(dsBias);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(dsList, dsScale);
        QWidget::setTabOrder(dsScale, dsBias);
        QWidget::setTabOrder(dsBias, cbVis);
        QWidget::setTabOrder(cbVis, buttonBox);

        retranslateUi(multiResultDataSourceDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), multiResultDataSourceDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), multiResultDataSourceDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(multiResultDataSourceDialog);
    } // setupUi

    void retranslateUi(QDialog *multiResultDataSourceDialog)
    {
        multiResultDataSourceDialog->setWindowTitle(QString());
        groupBox->setTitle(QApplication::translate("multiResultDataSourceDialog", "Result Data Sources", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("multiResultDataSourceDialog", "&Data Sources:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("multiResultDataSourceDialog", "Data Mapping", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("multiResultDataSourceDialog", "&Scale:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("multiResultDataSourceDialog", "&Bias:", 0, QApplication::UnicodeUTF8));
        cbVis->setText(QApplication::translate("multiResultDataSourceDialog", "Visible on the plot", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class multiResultDataSourceDialog: public Ui_multiResultDataSourceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MULTIRESULTDATASOURCEDIALOG_H
