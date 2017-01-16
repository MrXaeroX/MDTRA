/********************************************************************************
** Form generated from reading UI file 'preferencesDialog.ui'
**
** Created: Mon 16. Jan 19:04:11 2017
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCESDIALOG_H
#define UI_PREFERENCESDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_preferencesDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *inputTab;
    QLabel *label;
    QComboBox *mtCombo;
    QCheckBox *cbSSE;
    QCheckBox *cbLowPriority;
    QCheckBox *cbUseCUDA;
    QWidget *tab;
    QLabel *label_2;
    QComboBox *xsuCombo;
    QGroupBox *cbDataFilter;
    QLabel *label_8;
    QSpinBox *sbDataFilter;
    QCheckBox *cbMultisampleAA;
    QFrame *line_2;
    QCheckBox *cbPlotPolarAngles;
    QComboBox *langCombo;
    QLabel *label_9;
    QWidget *calcTab;
    QGroupBox *groupBox;
    QLabel *label_5;
    QDoubleSpinBox *sasProbeRadius;
    QLabel *label_6;
    QLabel *label_7;
    QComboBox *sasAccuracy;
    QCheckBox *cbSasNoWater;
    QWidget *formatsTab;
    QListWidget *formatList;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *btnFormatAdd;
    QPushButton *btnFormatEdit;
    QPushButton *btnFormatDelete;
    QPushButton *btnFormatDefault;
    QSpacerItem *verticalSpacer;
    QWidget *colorsTab;
    QListWidget *colorList;
    QLabel *lblColorName;
    QFrame *line;
    QToolButton *colorChangeButton;
    QLabel *lblColorValue;
    QLabel *lblText;
    QPushButton *fontButton;
    QWidget *viewerTab;
    QRadioButton *viewer0;
    QRadioButton *viewer1;
    QRadioButton *viewer2;
    QRadioButton *viewer3;
    QLineEdit *strRasMol;
    QLabel *label_3;
    QPushButton *browseRasMol;
    QLabel *label_4;
    QPushButton *browseVMD;
    QLineEdit *strVMD;

    void setupUi(QDialog *preferencesDialog)
    {
        if (preferencesDialog->objectName().isEmpty())
            preferencesDialog->setObjectName(QString::fromUtf8("preferencesDialog"));
        preferencesDialog->resize(540, 307);
        buttonBox = new QDialogButtonBox(preferencesDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 271, 521, 31));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        tabWidget = new QTabWidget(preferencesDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 521, 261));
        inputTab = new QWidget();
        inputTab->setObjectName(QString::fromUtf8("inputTab"));
        label = new QLabel(inputTab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 111, 21));
        mtCombo = new QComboBox(inputTab);
        mtCombo->setObjectName(QString::fromUtf8("mtCombo"));
        mtCombo->setGeometry(QRect(130, 20, 221, 22));
        cbSSE = new QCheckBox(inputTab);
        cbSSE->setObjectName(QString::fromUtf8("cbSSE"));
        cbSSE->setGeometry(QRect(20, 60, 341, 17));
        cbLowPriority = new QCheckBox(inputTab);
        cbLowPriority->setObjectName(QString::fromUtf8("cbLowPriority"));
        cbLowPriority->setGeometry(QRect(20, 90, 341, 17));
        cbUseCUDA = new QCheckBox(inputTab);
        cbUseCUDA->setObjectName(QString::fromUtf8("cbUseCUDA"));
        cbUseCUDA->setGeometry(QRect(20, 120, 341, 17));
        tabWidget->addTab(inputTab, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 190, 111, 21));
        xsuCombo = new QComboBox(tab);
        xsuCombo->setObjectName(QString::fromUtf8("xsuCombo"));
        xsuCombo->setGeometry(QRect(140, 190, 221, 22));
        cbDataFilter = new QGroupBox(tab);
        cbDataFilter->setObjectName(QString::fromUtf8("cbDataFilter"));
        cbDataFilter->setGeometry(QRect(20, 20, 471, 71));
        cbDataFilter->setCheckable(true);
        cbDataFilter->setChecked(false);
        label_8 = new QLabel(cbDataFilter);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 30, 361, 21));
        sbDataFilter = new QSpinBox(cbDataFilter);
        sbDataFilter->setObjectName(QString::fromUtf8("sbDataFilter"));
        sbDataFilter->setGeometry(QRect(380, 30, 71, 22));
        sbDataFilter->setMinimum(1);
        sbDataFilter->setMaximum(999999);
        cbMultisampleAA = new QCheckBox(tab);
        cbMultisampleAA->setObjectName(QString::fromUtf8("cbMultisampleAA"));
        cbMultisampleAA->setGeometry(QRect(20, 110, 341, 17));
        line_2 = new QFrame(tab);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(20, 170, 471, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        cbPlotPolarAngles = new QCheckBox(tab);
        cbPlotPolarAngles->setObjectName(QString::fromUtf8("cbPlotPolarAngles"));
        cbPlotPolarAngles->setGeometry(QRect(20, 140, 341, 17));
        langCombo = new QComboBox(tab);
        langCombo->setObjectName(QString::fromUtf8("langCombo"));
        langCombo->setGeometry(QRect(350, 134, 141, 22));
        label_9 = new QLabel(tab);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(350, 110, 141, 21));
        label_9->setTextFormat(Qt::AutoText);
        label_9->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        tabWidget->addTab(tab, QString());
        calcTab = new QWidget();
        calcTab->setObjectName(QString::fromUtf8("calcTab"));
        groupBox = new QGroupBox(calcTab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 491, 211));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(20, 30, 161, 21));
        sasProbeRadius = new QDoubleSpinBox(groupBox);
        sasProbeRadius->setObjectName(QString::fromUtf8("sasProbeRadius"));
        sasProbeRadius->setGeometry(QRect(180, 30, 81, 22));
        sasProbeRadius->setMinimum(0.1);
        sasProbeRadius->setMaximum(50);
        sasProbeRadius->setSingleStep(0.1);
        sasProbeRadius->setValue(0.1);
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(270, 30, 51, 21));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(20, 60, 161, 21));
        sasAccuracy = new QComboBox(groupBox);
        sasAccuracy->setObjectName(QString::fromUtf8("sasAccuracy"));
        sasAccuracy->setGeometry(QRect(180, 60, 81, 22));
        cbSasNoWater = new QCheckBox(groupBox);
        cbSasNoWater->setObjectName(QString::fromUtf8("cbSasNoWater"));
        cbSasNoWater->setGeometry(QRect(20, 100, 311, 21));
        cbSasNoWater->setChecked(false);
        tabWidget->addTab(calcTab, QString());
        formatsTab = new QWidget();
        formatsTab->setObjectName(QString::fromUtf8("formatsTab"));
        formatList = new QListWidget(formatsTab);
        formatList->setObjectName(QString::fromUtf8("formatList"));
        formatList->setGeometry(QRect(20, 20, 381, 192));
        formatList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        verticalLayoutWidget = new QWidget(formatsTab);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(409, 20, 91, 191));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        btnFormatAdd = new QPushButton(verticalLayoutWidget);
        btnFormatAdd->setObjectName(QString::fromUtf8("btnFormatAdd"));

        verticalLayout->addWidget(btnFormatAdd);

        btnFormatEdit = new QPushButton(verticalLayoutWidget);
        btnFormatEdit->setObjectName(QString::fromUtf8("btnFormatEdit"));

        verticalLayout->addWidget(btnFormatEdit);

        btnFormatDelete = new QPushButton(verticalLayoutWidget);
        btnFormatDelete->setObjectName(QString::fromUtf8("btnFormatDelete"));

        verticalLayout->addWidget(btnFormatDelete);

        btnFormatDefault = new QPushButton(verticalLayoutWidget);
        btnFormatDefault->setObjectName(QString::fromUtf8("btnFormatDefault"));

        verticalLayout->addWidget(btnFormatDefault);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        tabWidget->addTab(formatsTab, QString());
        colorsTab = new QWidget();
        colorsTab->setObjectName(QString::fromUtf8("colorsTab"));
        colorList = new QListWidget(colorsTab);
        colorList->setObjectName(QString::fromUtf8("colorList"));
        colorList->setGeometry(QRect(10, 10, 241, 211));
        lblColorName = new QLabel(colorsTab);
        lblColorName->setObjectName(QString::fromUtf8("lblColorName"));
        lblColorName->setGeometry(QRect(260, 10, 241, 16));
        line = new QFrame(colorsTab);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(260, 30, 241, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        colorChangeButton = new QToolButton(colorsTab);
        colorChangeButton->setObjectName(QString::fromUtf8("colorChangeButton"));
        colorChangeButton->setGeometry(QRect(470, 50, 25, 21));
        lblColorValue = new QLabel(colorsTab);
        lblColorValue->setObjectName(QString::fromUtf8("lblColorValue"));
        lblColorValue->setGeometry(QRect(260, 50, 201, 21));
        lblColorValue->setAutoFillBackground(true);
        lblColorValue->setFrameShape(QFrame::StyledPanel);
        lblText = new QLabel(colorsTab);
        lblText->setObjectName(QString::fromUtf8("lblText"));
        lblText->setGeometry(QRect(260, 80, 231, 41));
        lblText->setFrameShape(QFrame::StyledPanel);
        lblText->setFrameShadow(QFrame::Plain);
        lblText->setLineWidth(1);
        lblText->setAlignment(Qt::AlignCenter);
        fontButton = new QPushButton(colorsTab);
        fontButton->setObjectName(QString::fromUtf8("fontButton"));
        fontButton->setGeometry(QRect(260, 130, 75, 23));
        tabWidget->addTab(colorsTab, QString());
        viewerTab = new QWidget();
        viewerTab->setObjectName(QString::fromUtf8("viewerTab"));
        viewer0 = new QRadioButton(viewerTab);
        viewer0->setObjectName(QString::fromUtf8("viewer0"));
        viewer0->setGeometry(QRect(20, 20, 481, 21));
        viewer1 = new QRadioButton(viewerTab);
        viewer1->setObjectName(QString::fromUtf8("viewer1"));
        viewer1->setGeometry(QRect(20, 50, 481, 21));
        viewer1->setCheckable(true);
        viewer2 = new QRadioButton(viewerTab);
        viewer2->setObjectName(QString::fromUtf8("viewer2"));
        viewer2->setGeometry(QRect(20, 80, 481, 21));
        viewer2->setCheckable(true);
        viewer3 = new QRadioButton(viewerTab);
        viewer3->setObjectName(QString::fromUtf8("viewer3"));
        viewer3->setGeometry(QRect(20, 140, 481, 21));
        viewer3->setCheckable(true);
        strRasMol = new QLineEdit(viewerTab);
        strRasMol->setObjectName(QString::fromUtf8("strRasMol"));
        strRasMol->setEnabled(false);
        strRasMol->setGeometry(QRect(200, 110, 251, 20));
        label_3 = new QLabel(viewerTab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setEnabled(false);
        label_3->setGeometry(QRect(40, 110, 161, 21));
        browseRasMol = new QPushButton(viewerTab);
        browseRasMol->setObjectName(QString::fromUtf8("browseRasMol"));
        browseRasMol->setEnabled(false);
        browseRasMol->setGeometry(QRect(450, 110, 22, 22));
        label_4 = new QLabel(viewerTab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setEnabled(false);
        label_4->setGeometry(QRect(40, 170, 161, 21));
        browseVMD = new QPushButton(viewerTab);
        browseVMD->setObjectName(QString::fromUtf8("browseVMD"));
        browseVMD->setEnabled(false);
        browseVMD->setGeometry(QRect(450, 170, 22, 22));
        strVMD = new QLineEdit(viewerTab);
        strVMD->setObjectName(QString::fromUtf8("strVMD"));
        strVMD->setEnabled(false);
        strVMD->setGeometry(QRect(200, 170, 251, 20));
        tabWidget->addTab(viewerTab, QString());
#ifndef QT_NO_SHORTCUT
        label->setBuddy(mtCombo);
        label_2->setBuddy(xsuCombo);
        label_8->setBuddy(sbDataFilter);
        label_5->setBuddy(sasProbeRadius);
        label_7->setBuddy(sasAccuracy);
        label_3->setBuddy(strRasMol);
        label_4->setBuddy(strVMD);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(tabWidget, mtCombo);
        QWidget::setTabOrder(mtCombo, cbSSE);
        QWidget::setTabOrder(cbSSE, cbLowPriority);
        QWidget::setTabOrder(cbLowPriority, cbUseCUDA);
        QWidget::setTabOrder(cbUseCUDA, cbDataFilter);
        QWidget::setTabOrder(cbDataFilter, sbDataFilter);
        QWidget::setTabOrder(sbDataFilter, cbMultisampleAA);
        QWidget::setTabOrder(cbMultisampleAA, cbPlotPolarAngles);
        QWidget::setTabOrder(cbPlotPolarAngles, langCombo);
        QWidget::setTabOrder(langCombo, xsuCombo);
        QWidget::setTabOrder(xsuCombo, sasProbeRadius);
        QWidget::setTabOrder(sasProbeRadius, sasAccuracy);
        QWidget::setTabOrder(sasAccuracy, cbSasNoWater);
        QWidget::setTabOrder(cbSasNoWater, formatList);
        QWidget::setTabOrder(formatList, btnFormatAdd);
        QWidget::setTabOrder(btnFormatAdd, btnFormatEdit);
        QWidget::setTabOrder(btnFormatEdit, btnFormatDelete);
        QWidget::setTabOrder(btnFormatDelete, btnFormatDefault);
        QWidget::setTabOrder(btnFormatDefault, colorList);
        QWidget::setTabOrder(colorList, colorChangeButton);
        QWidget::setTabOrder(colorChangeButton, fontButton);
        QWidget::setTabOrder(fontButton, viewer0);
        QWidget::setTabOrder(viewer0, viewer1);
        QWidget::setTabOrder(viewer1, viewer2);
        QWidget::setTabOrder(viewer2, strRasMol);
        QWidget::setTabOrder(strRasMol, browseRasMol);
        QWidget::setTabOrder(browseRasMol, viewer3);
        QWidget::setTabOrder(viewer3, strVMD);
        QWidget::setTabOrder(strVMD, browseVMD);
        QWidget::setTabOrder(browseVMD, buttonBox);

        retranslateUi(preferencesDialog);
        QObject::connect(viewer2, SIGNAL(toggled(bool)), label_3, SLOT(setEnabled(bool)));
        QObject::connect(viewer2, SIGNAL(toggled(bool)), strRasMol, SLOT(setEnabled(bool)));
        QObject::connect(viewer2, SIGNAL(toggled(bool)), browseRasMol, SLOT(setEnabled(bool)));
        QObject::connect(viewer3, SIGNAL(toggled(bool)), label_4, SLOT(setEnabled(bool)));
        QObject::connect(viewer3, SIGNAL(toggled(bool)), strVMD, SLOT(setEnabled(bool)));
        QObject::connect(viewer3, SIGNAL(toggled(bool)), browseVMD, SLOT(setEnabled(bool)));
        QObject::connect(buttonBox, SIGNAL(rejected()), preferencesDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(preferencesDialog);
    } // setupUi

    void retranslateUi(QDialog *preferencesDialog)
    {
        preferencesDialog->setWindowTitle(QApplication::translate("preferencesDialog", "Preferences", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("preferencesDialog", "&Multithreading:", 0, QApplication::UnicodeUTF8));
        cbSSE->setText(QApplication::translate("preferencesDialog", "Use &SSE instruction set if available", 0, QApplication::UnicodeUTF8));
        cbLowPriority->setText(QApplication::translate("preferencesDialog", "&Yield resources to other programs", 0, QApplication::UnicodeUTF8));
        cbUseCUDA->setText(QApplication::translate("preferencesDialog", "Use &GPU computing if possible (NVIDIA CUDA)", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(inputTab), QApplication::translate("preferencesDialog", "&General", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("preferencesDialog", "&Time Scale Units:", 0, QApplication::UnicodeUTF8));
        cbDataFilter->setTitle(QApplication::translate("preferencesDialog", "Trajectory Smoothing", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("preferencesDialog", "Central Moving Average sample &width, in snapshots:", 0, QApplication::UnicodeUTF8));
        cbMultisampleAA->setText(QApplication::translate("preferencesDialog", "Plot multi-sample &anti-aliasing", 0, QApplication::UnicodeUTF8));
        cbPlotPolarAngles->setText(QApplication::translate("preferencesDialog", "Present angles in polar coordinates", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("preferencesDialog", "Plot Language:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("preferencesDialog", "&Plot", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("preferencesDialog", "SAS and Occlusion", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("preferencesDialog", "&Probe radius:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("preferencesDialog", "A", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("preferencesDialog", "&Accuracy:", 0, QApplication::UnicodeUTF8));
        sasAccuracy->clear();
        sasAccuracy->insertItems(0, QStringList()
         << QApplication::translate("preferencesDialog", "Low", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("preferencesDialog", "Medium", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("preferencesDialog", "High", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("preferencesDialog", "Very High", 0, QApplication::UnicodeUTF8)
        );
        cbSasNoWater->setText(QApplication::translate("preferencesDialog", "E&xclude water molecules from calculation", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(calcTab), QApplication::translate("preferencesDialog", "&Analysis", 0, QApplication::UnicodeUTF8));
        btnFormatAdd->setText(QApplication::translate("preferencesDialog", "&Add...", 0, QApplication::UnicodeUTF8));
        btnFormatEdit->setText(QApplication::translate("preferencesDialog", "&Edit...", 0, QApplication::UnicodeUTF8));
        btnFormatDelete->setText(QApplication::translate("preferencesDialog", "&Delete", 0, QApplication::UnicodeUTF8));
        btnFormatDefault->setText(QApplication::translate("preferencesDialog", "&Set Default", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(formatsTab), QApplication::translate("preferencesDialog", "&Formats", 0, QApplication::UnicodeUTF8));
        lblColorName->setText(QString());
        colorChangeButton->setText(QApplication::translate("preferencesDialog", "...", 0, QApplication::UnicodeUTF8));
        lblColorValue->setText(QString());
        lblText->setText(QApplication::translate("preferencesDialog", "Example text", 0, QApplication::UnicodeUTF8));
        fontButton->setText(QApplication::translate("preferencesDialog", "Fo&nt...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(colorsTab), QApplication::translate("preferencesDialog", "&Colors", 0, QApplication::UnicodeUTF8));
        viewer0->setText(QApplication::translate("preferencesDialog", "&No PDB Viewer", 0, QApplication::UnicodeUTF8));
        viewer1->setText(QApplication::translate("preferencesDialog", "&Internal PDB Viewer", 0, QApplication::UnicodeUTF8));
        viewer2->setText(QApplication::translate("preferencesDialog", "&RasMol Viewer", 0, QApplication::UnicodeUTF8));
        viewer3->setText(QApplication::translate("preferencesDialog", "&VMD Viewer", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("preferencesDialog", "Path to &RasMol Executable:", 0, QApplication::UnicodeUTF8));
        browseRasMol->setText(QApplication::translate("preferencesDialog", "...", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("preferencesDialog", "Path to &VMD Executable:", 0, QApplication::UnicodeUTF8));
        browseVMD->setText(QApplication::translate("preferencesDialog", "...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(viewerTab), QApplication::translate("preferencesDialog", "&Viewer", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class preferencesDialog: public Ui_preferencesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESDIALOG_H
