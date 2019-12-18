/********************************************************************************
** Form generated from reading UI file 'AccessAnalysisWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ACCESSANALYSISWIDGET_H
#define UI_ACCESSANALYSISWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AccessAnalysisWidget
{
public:
    QLabel *startPointLabel;
    QLabel *targetPointLayer;
    QLabel *arcLabel;
    QLabel *nodeLabel;
    QLabel *timeLimitLabel;
    QLineEdit *timeLimitTextLine;
    QPushButton *okButton;
    QComboBox *arcBox;
    QPushButton *cancelButton;
    QLabel *unitLabel;
    QComboBox *nodeBox;
    QComboBox *targetPointLayerBox;
    QComboBox *startBox;

    void setupUi(QWidget *AccessAnalysisWidget)
    {
        if (AccessAnalysisWidget->objectName().isEmpty())
            AccessAnalysisWidget->setObjectName(QString::fromUtf8("AccessAnalysisWidget"));
        AccessAnalysisWidget->resize(603, 516);
        startPointLabel = new QLabel(AccessAnalysisWidget);
        startPointLabel->setObjectName(QString::fromUtf8("startPointLabel"));
        startPointLabel->setGeometry(QRect(40, 40, 71, 20));
        targetPointLayer = new QLabel(AccessAnalysisWidget);
        targetPointLayer->setObjectName(QString::fromUtf8("targetPointLayer"));
        targetPointLayer->setGeometry(QRect(40, 120, 81, 20));
        arcLabel = new QLabel(AccessAnalysisWidget);
        arcLabel->setObjectName(QString::fromUtf8("arcLabel"));
        arcLabel->setGeometry(QRect(40, 190, 71, 21));
        nodeLabel = new QLabel(AccessAnalysisWidget);
        nodeLabel->setObjectName(QString::fromUtf8("nodeLabel"));
        nodeLabel->setGeometry(QRect(40, 270, 81, 20));
        timeLimitLabel = new QLabel(AccessAnalysisWidget);
        timeLimitLabel->setObjectName(QString::fromUtf8("timeLimitLabel"));
        timeLimitLabel->setGeometry(QRect(40, 350, 72, 15));
        timeLimitTextLine = new QLineEdit(AccessAnalysisWidget);
        timeLimitTextLine->setObjectName(QString::fromUtf8("timeLimitTextLine"));
        timeLimitTextLine->setGeometry(QRect(40, 370, 511, 31));
        okButton = new QPushButton(AccessAnalysisWidget);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setGeometry(QRect(360, 460, 93, 28));
        arcBox = new QComboBox(AccessAnalysisWidget);
        arcBox->setObjectName(QString::fromUtf8("arcBox"));
        arcBox->setGeometry(QRect(40, 220, 511, 31));
        cancelButton = new QPushButton(AccessAnalysisWidget);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setGeometry(QRect(460, 460, 93, 28));
        unitLabel = new QLabel(AccessAnalysisWidget);
        unitLabel->setObjectName(QString::fromUtf8("unitLabel"));
        unitLabel->setGeometry(QRect(490, 410, 72, 21));
        nodeBox = new QComboBox(AccessAnalysisWidget);
        nodeBox->setObjectName(QString::fromUtf8("nodeBox"));
        nodeBox->setGeometry(QRect(40, 300, 511, 31));
        targetPointLayerBox = new QComboBox(AccessAnalysisWidget);
        targetPointLayerBox->setObjectName(QString::fromUtf8("targetPointLayerBox"));
        targetPointLayerBox->setGeometry(QRect(40, 150, 511, 31));
        startBox = new QComboBox(AccessAnalysisWidget);
        startBox->setObjectName(QString::fromUtf8("startBox"));
        startBox->setGeometry(QRect(40, 70, 511, 31));

        retranslateUi(AccessAnalysisWidget);

        QMetaObject::connectSlotsByName(AccessAnalysisWidget);
    } // setupUi

    void retranslateUi(QWidget *AccessAnalysisWidget)
    {
        AccessAnalysisWidget->setWindowTitle(QCoreApplication::translate("AccessAnalysisWidget", "AccessAnalysisWidget", nullptr));
        startPointLabel->setText(QCoreApplication::translate("AccessAnalysisWidget", "\345\207\272\345\217\221\347\202\271", nullptr));
        targetPointLayer->setText(QCoreApplication::translate("AccessAnalysisWidget", "\347\233\256\346\240\207\347\202\271\345\233\276\345\261\202", nullptr));
        arcLabel->setText(QCoreApplication::translate("AccessAnalysisWidget", "\351\201\223\350\267\257\350\241\250", nullptr));
        nodeLabel->setText(QCoreApplication::translate("AccessAnalysisWidget", "\344\272\244\345\217\211\350\267\257\345\217\243\350\241\250", nullptr));
        timeLimitLabel->setText(QCoreApplication::translate("AccessAnalysisWidget", "\346\227\266\351\227\264\350\214\203\345\233\264", nullptr));
        okButton->setText(QCoreApplication::translate("AccessAnalysisWidget", "\347\241\256\345\256\232", nullptr));
        cancelButton->setText(QCoreApplication::translate("AccessAnalysisWidget", "\345\217\226\346\266\210", nullptr));
        unitLabel->setText(QCoreApplication::translate("AccessAnalysisWidget", "\345\215\225\344\275\215\357\274\232\347\261\263", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AccessAnalysisWidget: public Ui_AccessAnalysisWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACCESSANALYSISWIDGET_H
