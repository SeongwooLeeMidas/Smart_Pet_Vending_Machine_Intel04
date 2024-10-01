#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include "tab2sockeclient.h"
//#include "tab4chartplot.h"
//#include "tab5database.h"
#include "tab3controlpannel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TabWidget; }
QT_END_NAMESPACE

class TabWidget : public QWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent = nullptr);
    ~TabWidget();

private:
    Ui::TabWidget *ui;
    Tab2SockeClient *pTab2SockeClient;
    //Tab4ChartPlot *pTab4ChartPlot;
    //Tab5Database *pTab5Database;
    Tab3ControlPannel *pTab3ControlPannel;

};
#endif // TABWIDGET_H
