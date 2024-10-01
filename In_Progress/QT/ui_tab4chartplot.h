/********************************************************************************
** Form generated from reading UI file 'tab4chartplot.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TAB4CHARTPLOT_H
#define UI_TAB4CHARTPLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Tab4ChartPlot
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pPBClearButton;
    QVBoxLayout *pChartViewLayout;

    void setupUi(QWidget *Tab4ChartPlot)
    {
        if (Tab4ChartPlot->objectName().isEmpty())
            Tab4ChartPlot->setObjectName(QString::fromUtf8("Tab4ChartPlot"));
        Tab4ChartPlot->resize(400, 300);
        verticalLayout = new QVBoxLayout(Tab4ChartPlot);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pPBClearButton = new QPushButton(Tab4ChartPlot);
        pPBClearButton->setObjectName(QString::fromUtf8("pPBClearButton"));

        horizontalLayout->addWidget(pPBClearButton);


        verticalLayout->addLayout(horizontalLayout);

        pChartViewLayout = new QVBoxLayout();
        pChartViewLayout->setObjectName(QString::fromUtf8("pChartViewLayout"));

        verticalLayout->addLayout(pChartViewLayout);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(1, 9);

        retranslateUi(Tab4ChartPlot);

        QMetaObject::connectSlotsByName(Tab4ChartPlot);
    } // setupUi

    void retranslateUi(QWidget *Tab4ChartPlot)
    {
        Tab4ChartPlot->setWindowTitle(QApplication::translate("Tab4ChartPlot", "Form", nullptr));
        pPBClearButton->setText(QApplication::translate("Tab4ChartPlot", "Clear", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Tab4ChartPlot: public Ui_Tab4ChartPlot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TAB4CHARTPLOT_H
