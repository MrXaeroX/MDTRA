/********************************************************************************
** Form generated from reading UI file 'proteinDataMiningDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROTEINDATAMININGDIALOG_H
#define UI_PROTEINDATAMININGDIALOG_H

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
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_proteinDataMiningDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_2;
    QLabel *sel_label;
    QLineEdit *sel_string;
    QLabel *sel_atCount;
    QLabel *sel_label2;
    QLabel *sel_labelResults;
    QPushButton *sel_parse;
    QGroupBox *groupBox;
    QLabel *label_2;
    QListWidget *sList;
    QGroupBox *groupBox_4;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *saLayout;

    void setupUi(QDialog *proteinDataMiningDialog)
    {
        if (proteinDataMiningDialog->objectName().isEmpty())
            proteinDataMiningDialog->setObjectName(QString::fromUtf8("proteinDataMiningDialog"));
        proteinDataMiningDialog->resize(571, 465);
        buttonBox = new QDialogButtonBox(proteinDataMiningDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 430, 551, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        groupBox_2 = new QGroupBox(proteinDataMiningDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 190, 551, 131));
        sel_label = new QLabel(groupBox_2);
        sel_label->setObjectName(QString::fromUtf8("sel_label"));
        sel_label->setGeometry(QRect(20, 20, 131, 21));
        sel_string = new QLineEdit(groupBox_2);
        sel_string->setObjectName(QString::fromUtf8("sel_string"));
        sel_string->setGeometry(QRect(150, 20, 281, 20));
        sel_atCount = new QLabel(groupBox_2);
        sel_atCount->setObjectName(QString::fromUtf8("sel_atCount"));
        sel_atCount->setGeometry(QRect(470, 20, 61, 21));
        sel_atCount->setAlignment(Qt::AlignCenter);
        sel_label2 = new QLabel(groupBox_2);
        sel_label2->setObjectName(QString::fromUtf8("sel_label2"));
        sel_label2->setGeometry(QRect(20, 60, 111, 20));
        sel_label2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults = new QLabel(groupBox_2);
        sel_labelResults->setObjectName(QString::fromUtf8("sel_labelResults"));
        sel_labelResults->setGeometry(QRect(150, 60, 341, 51));
        sel_labelResults->setTextFormat(Qt::AutoText);
        sel_labelResults->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        sel_labelResults->setWordWrap(true);
        sel_parse = new QPushButton(groupBox_2);
        sel_parse->setObjectName(QString::fromUtf8("sel_parse"));
        sel_parse->setGeometry(QRect(430, 20, 22, 22));
        groupBox = new QGroupBox(proteinDataMiningDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 551, 171));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 20, 111, 21));
        sList = new QListWidget(groupBox);
        sList->setObjectName(QString::fromUtf8("sList"));
        sList->setGeometry(QRect(130, 20, 401, 141));
        sList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        sList->setSelectionMode(QAbstractItemView::MultiSelection);
        groupBox_4 = new QGroupBox(proteinDataMiningDialog);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 330, 551, 91));
        scrollArea = new QScrollArea(groupBox_4);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(10, 20, 531, 61));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setFrameShadow(QFrame::Plain);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 531, 61));
        verticalLayoutWidget = new QWidget(scrollAreaWidgetContents);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 531, 61));
        saLayout = new QVBoxLayout(verticalLayoutWidget);
        saLayout->setObjectName(QString::fromUtf8("saLayout"));
        saLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea->setWidget(scrollAreaWidgetContents);
#ifndef QT_NO_SHORTCUT
        sel_label->setBuddy(sel_string);
        label_2->setBuddy(sList);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(sList, sel_string);
        QWidget::setTabOrder(sel_string, sel_parse);
        QWidget::setTabOrder(sel_parse, scrollArea);
        QWidget::setTabOrder(scrollArea, buttonBox);

        retranslateUi(proteinDataMiningDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), proteinDataMiningDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(proteinDataMiningDialog);
    } // setupUi

    void retranslateUi(QDialog *proteinDataMiningDialog)
    {
        proteinDataMiningDialog->setWindowTitle(QApplication::translate("proteinDataMiningDialog", "Protein Data Mining", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("proteinDataMiningDialog", "Selection", 0, QApplication::UnicodeUTF8));
        sel_label->setText(QApplication::translate("proteinDataMiningDialog", "Selection &Expression:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        sel_string->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        sel_atCount->setText(QApplication::translate("proteinDataMiningDialog", "0", 0, QApplication::UnicodeUTF8));
        sel_label2->setText(QApplication::translate("proteinDataMiningDialog", "Selection Data:", 0, QApplication::UnicodeUTF8));
        sel_labelResults->setText(QString());
        sel_parse->setText(QString());
        groupBox->setTitle(QApplication::translate("proteinDataMiningDialog", "Streams", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("proteinDataMiningDialog", "&Stream Sources:", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("proteinDataMiningDialog", "Protein Data", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class proteinDataMiningDialog: public Ui_proteinDataMiningDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROTEINDATAMININGDIALOG_H
