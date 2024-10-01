#include "tabwidget.h"
#include "ui_tabwidget.h"

TabWidget::TabWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabWidget)
{
    ui->setupUi(this);

    pTab2SockeClient = new Tab2SockeClient(ui->pTab2);
    ui->pTab2->setLayout(pTab2SockeClient->layout());


    pTab3ControlPannel = new Tab3ControlPannel(ui->pTab3);
    //ui->pTab3->setLayout(new QVBoxLayout());
    ui->pTab3->setLayout(pTab3ControlPannel->layout());
    //ui->pTab3->layout()->addWidget(pTab3ControlPannel);


    ui->tabWidget->setCurrentIndex(0);
    //connect(pTab2SockeClient, SIGNAL(tab4RecvDataSig(QString)), pTab4ChartPlot, SLOT(tab4RecvDataSlot(QString)));
    //connect(pTab2SockeClient, SIGNAL(tab5RecvDataSig(QString)), pTab5Database, SLOT(tab5RecvDataSlot(QString)));
    connect(pTab3ControlPannel, SIGNAL(socketSendDataSig(QString)), pTab2SockeClient->getpSocketClient(), SLOT(slotSocketSendData(QString)));
    connect(pTab2SockeClient,SIGNAL(tab3RecvDataSig(QString)), pTab3ControlPannel,SLOT(tab3RecvDataSlot(QString)));
    connect(pTab2SockeClient,SIGNAL(tab3RecvDataSig_db(QString)), pTab3ControlPannel,SLOT(tab3RecvDataSlot_db(QString)));

}

TabWidget::~TabWidget()
{
    delete ui;
}

