#include "tab2sockeclient.h"
#include "ui_tab2sockeclient.h"

Tab2SockeClient::Tab2SockeClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab2SockeClient)
{
    ui->setupUi(this);
    pSocketClient = new SocketClient(this);
    pKeyBoard = new Keyboard();
    connect(pSocketClient,SIGNAL(sigSocketRecv(QString)),this,SLOT(socketRecvUpdateSlot(QString)));
    connect(ui->pLERecvId,SIGNAL(selectionChanged()), this, SLOT(keyboardSlot()));
    connect(ui->pLESendData,SIGNAL(selectionChanged()), this, SLOT(keyboardSlot()));
    ui->pPBSendButton->setEnabled(false);
}

Tab2SockeClient::~Tab2SockeClient()
{
    delete ui;
}

void Tab2SockeClient::on_pPBServerConnect_clicked(bool checked)
{
    bool bOk;
    if(checked)
    {
        pSocketClient->slotConnectToServer(bOk);
        if(bOk)
        {
            ui->pPBServerConnect->setText("서버 해제");
            ui->pPBSendButton->setEnabled(true);
        }
    }
    else
    {
        pSocketClient->slotClosedByServer();
        ui->pPBServerConnect->setText("서버 연결");
        ui->pPBSendButton->setEnabled(false);
    }
}

void Tab2SockeClient::socketRecvUpdateSlot(QString strRecvData)
{
    QTime time = QTime::currentTime();
    QString strTime = time.toString();
    strRecvData.chop(1);    //"\n" 제거
    strTime = strTime + " " + strRecvData;

    ui->pTERecvData->append(strTime);
    //[KSH_QT]LED@0xff ==> @KSH_QT@LED@0xff
    strRecvData.replace("[","@");
    strRecvData.replace("]","@");
    QStringList qList = strRecvData.split("@");

    if (qList[2].indexOf("BREED") == 0)  // BREED 메시지 처리 추가
        {
            emit tab3RecvDataSig(strRecvData);  // tab3RecvDataSig로 BREED 메시지 처리
        }
//    if (qList[2].indexOf("DB") == 0)  // DB 메시지 처리 추가
//        {
//            emit tab3RecvDataSig(strRecvData);  // tab3RecvDataSig로 DB 메시지 처리
//        }
    if (qList[2].indexOf("DB_BREED") == 0)  // DB 메시지 처리 추가
        {
            emit tab3RecvDataSig_db(strRecvData);  // tab3RecvDataSig로 DB 메시지 처리
        }

}

void Tab2SockeClient::on_pPBSendButton_clicked()
{
    QString strRecvId = ui->pLERecvId->text();
    QString strSendData = ui->pLESendData->text();
    if(!strSendData.isEmpty())
    {
        if(strRecvId.isEmpty())
           strSendData = "[ALLMSG]"+ strSendData;
        else
           strSendData = "["+strRecvId+"]"+strSendData;

        pSocketClient->slotSocketSendData(strSendData);
        ui->pLESendData->clear();
    }
}

void Tab2SockeClient::keyboardSlot()
{
    QLineEdit *pQLineEdit = (QLineEdit *)sender();
    pKeyBoard->setLineEdit(pQLineEdit);
    pKeyBoard->show();
}
SocketClient *Tab2SockeClient::getpSocketClient()
{
    return pSocketClient;
}
