#include "tab3controlpannel.h"
#include "ui_tab3controlpannel.h"

Tab3ControlPannel::Tab3ControlPannel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab3ControlPannel),
    totalPrice(0),
    waitingDialog(nullptr)
{
    ui->setupUi(this);

    // 버튼 스타일시트 설정
    QString buttonStyle = "QPushButton {"
                          "    background-color: #4CAF50;"
                          "    color: white;"
                          "    font-size: 22px;"  // 폰트 크기 설정
                          "    border: 2px solid #4CAF50;"
                          "    border-radius: 15px;"  // 둥근 모서리
                          "    padding: 10px;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: #45a049;"
                          "}";

    // 버튼에 스타일 적용
    ui->pushButton->setStyleSheet(buttonStyle);
    ui->pushButton_2->setStyleSheet(buttonStyle);
    ui->pushButton_3->setStyleSheet(buttonStyle);
    ui->pushButton_4->setStyleSheet(buttonStyle);
    ui->pushButton_5->setStyleSheet(buttonStyle);
    ui->pushButton_6->setStyleSheet(buttonStyle);
    ui->pushButton_7->setStyleSheet(buttonStyle);
    ui->pushButton_8->setStyleSheet(buttonStyle);
    ui->pushButton_9->setStyleSheet(buttonStyle);
    ui->pushButton_11->setStyleSheet(buttonStyle);


    // 결제 버튼 스타일 설정
    ui->pbPayment->setStyleSheet("QPushButton {"
                                 "    background-color: #f44336;"  // 빨간색
                                 "    color: white;"
                                 "    border-radius: 15px;"
                                 "    padding: 10px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "    background-color: #e53935;"
                                 "}");
    ui->pbCLR->setStyleSheet("QPushButton {"
                                 "    background-color: #f44336;"  // 빨간색
                                 "    color: white;"
                                 "    border-radius: 15px;"
                                 "    padding: 10px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "    background-color: #e53935;"
                                 "}");

    // 추천 버튼 스타일 적용
    ui->pbRecommend->setStyleSheet(buttonStyle);


    // LCD 디스플레이 스타일 적용 (검정 배경, 녹색 숫자)
    ui->lcdNumber_2->setStyleSheet("QLCDNumber {"
                                   "    background-color: #000000;"
                                   "    color: #00FF00;"
                                   "    border: 2px solid #4CAF50;"
                                   "    border-radius: 10px;"
                                   "}");

    // 선택된 상품 번호의 스타일 설정 (크기를 키움)
    ui->label_3->setStyleSheet("QLabel {"
                               "    font-size: 24px;"  // 폰트 크기 확대
                               "    font-weight: bold;"  // 굵은 글꼴
                               "    color: #333333;"  // 글자색
                               "    padding: 10px;"  // 내부 여백 추가
                               "    border: 2px solid #CCCCCC;"  // 테두리 색상
                               "    border-radius: 10px;"  // 둥근 모서리
                               "    background-color: #F9F9F9;"  // 배경색 설정
                               "    text-align: center;"  // 중앙 정렬
                               "}");

    // 가격 표시의 스타일 설정 (크기를 더 키우고 강조)
    ui->label->setStyleSheet("QLabel {"
                             "    font-size: 30px;"
                             "    font-weight: bold;"
                             "    color: #333333;"
                             "    padding: 10px;"
                             "}");
    ui->label_2->setStyleSheet("QLabel {"
                             "    font-size: 35px;"
                             "    font-weight: bold;"
                             "    color: #333333;"
                             "    padding: 10px;"
                             "}");

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(on_productButton_clicked()));

    connect(ui->pbCLR, SIGNAL(clicked()), this, SLOT(on_pbCLR_clicked()));
    connect(ui->pbPayment, SIGNAL(clicked()), this, SLOT(on_pbPayment_clicked()));

    disconnect(ui->pbRecommend, SIGNAL(clicked()), this, SLOT(on_pbRecommend_clicked()));
    connect(ui->pbRecommend, SIGNAL(clicked()), this, SLOT(on_pbRecommend_clicked()));
    //paletteColorOn.setColor(ui->pPBLamp->backgroundRole(), QColor(255, 0, 0));
    //paletteColorOff.setColor(ui->pPBLamp->backgroundRole(), QColor(0, 0, 255));
    //ui->pPBLamp->setPalette(paletteColorOff);
    //ui->pPBPlug->setPalette(paletteColorOff);
    breed = "";

}

void Tab3ControlPannel::on_productButton_clicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    int productId = -1;

    if(button == ui->pushButton) productId = 1;
    else if(button == ui->pushButton_2) productId = 2;
    else if(button == ui->pushButton_3) productId = 3;
    else if(button == ui->pushButton_4) productId = 4;
    else if(button == ui->pushButton_5) productId = 5;
    else if(button == ui->pushButton_6) productId = 6;
    else if(button == ui->pushButton_7) productId = 7;
    else if(button == ui->pushButton_8) productId = 8;
    else if(button == ui->pushButton_9) productId = 9;

    if(productId != -1){
        // 선택된 상품 번호 배열에 추가
        selectedProducts.append(productId);

        totalPrice += productId * 1000;

        updateLCDDisplays();
    }

}

void Tab3ControlPannel::on_pbCLR_clicked()
{
    resetSelection();
}
void Tab3ControlPannel::on_pbPayment_clicked()
{
    if(!selectedProducts.isEmpty()){
        for (int productId : selectedProducts) {
                // [KJD_STM]MOTOR@'상품 번호' 형식으로 메시지 생성
                QString message = QString("[KJD_STM]MOTOR@%1").arg(productId);
                qDebug() << "Emitting socketSendDataSig with message: " << message;
                // 메시지 전송
                emit socketSendDataSig(message);
                QCoreApplication::processEvents();
                QThread::msleep(1000);
            }
        //선택된 아이템을 0 또는 1로 표시할 배열 생성
        QList<int> itemFlags = {0, 0, 0, 0, 0, 0};
        for (int productID : selectedProducts){
            if (productID >= 1 && productID <= 6){
                itemFlags[productID -1] =1;
            }
        }

        QString sqlMessage;
        // 품종을 사용하여 SQL 메세지 생성
        if (!breed.isEmpty()){
            QString sqlMessage = QString("[KJD_SQL]DB@%1@%2@%3@%4@%5@%6@%7")
                                    .arg(breed)
                                    .arg(itemFlags[0])
                                    .arg(itemFlags[1])
                                    .arg(itemFlags[2])
                                    .arg(itemFlags[3])
                                    .arg(itemFlags[4])
                                    .arg(itemFlags[5]);
            qDebug() << "Emitting socketSendDataSig with SQL message:" << sqlMessage;
            emit socketSendDataSig(sqlMessage);
            QCoreApplication::processEvents();
            QThread::msleep(1000);
         } else{
            // breed 데이터가 없을 때 기본 값으로 전송
            QString sqlMessage = QString("[KJD_SQL]DB@UNKNOWN@%1@%2@%3@%4@%5@%6")
                                     .arg(itemFlags[0])
                                     .arg(itemFlags[1])
                                     .arg(itemFlags[2])
                                     .arg(itemFlags[3])
                                     .arg(itemFlags[4])
                                     .arg(itemFlags[5]);
            qDebug() << "Emitting socketSendDataSig with SQL message (unknown breed):" << sqlMessage;
            emit socketSendDataSig(sqlMessage);
            QCoreApplication::processEvents();
            QThread::msleep(1000);
        }

    }
    breed = "";
    resetSelection();

    // [KJD_SQL]DB@{breed}@{item1}@{item2}@{item3}@{item4}@{item5}@{item6}
    // breed와 어떤 item을 선택하고 '결제' 버튼을 눌렀는지 얻어야 함
    // ex) [KJD_SQL]DB@LAB@0@0@1@0@0@0
}
void Tab3ControlPannel::on_pbRecommend_clicked()
{
    // 추천 버튼 클릭 시 [KJD_JET]RECO 메시지 전송
    QString message = "[KJD_JET]RECO";
    qDebug() << "Emitting socketSendDataSig with message: " << message;
    emit socketSendDataSig(message);

    // 대기 창이 아직 생성되지 않았다면 생성
    if (!waitingDialog) {
        waitingDialog = new QDialog(this);
        waitingDialog->setWindowTitle("분류 중...");
        QVBoxLayout *layout = new QVBoxLayout(waitingDialog);
        QLabel *label = new QLabel("카메라를 통한 품종 분류가 진행 중입니다. 잠시만 기다려 주세요...");
        layout->addWidget(label);
        waitingDialog->setLayout(layout);
    }

    // 대기 창을 모달로 띄움
    waitingDialog->setModal(true);
    waitingDialog->show();

}

void Tab3ControlPannel::updateLCDDisplays()
{
    // 선택된 상품 번호를 콤마로 구분하여 QString으로 변환
    QStringList productList;
    for (int productId : selectedProducts) {
        productList << QString::number(productId);
    }

    // QLabel에 선택된 상품 번호 표시
    ui->label_3->setText(productList.join(", "));

    // 총 가격을 기존처럼 QLCDNumber에 표시
    ui->lcdNumber_2->display(totalPrice);
}
void Tab3ControlPannel::resetSelection()
{
    selectedProducts.clear();
    totalPrice = 0;

    updateLCDDisplays();
}

void Tab3ControlPannel::tab3RecvDataSlot(QString recvData)
{
    QStringList qList = recvData.split("@");
    qDebug() << "Received data: " << recvData;

    if (qList.size() > 2 && qList[2] == "BREED")
    {
        breed = qList[3];
        // [KJD_SQL]로 C 클라이언트에 요청 보내기
        QString sqlMessage = QString("[KJD_SQL]DB_RECO@%1").arg(breed);
        qDebug() << "Emitting socketSendDataSig with message:" << sqlMessage;
        emit socketSendDataSig(sqlMessage);  // SQL 쿼리 요청 전송
        QCoreApplication::processEvents();

        // 품종에 따른 추천 로직
        if (breed == "POM") {
            breedInKorean = "포메라니안";
            recommendation = "관절 건강 중요!:'글루코사민, 콘드로이틴, 오메가-3 지방산'<br>"
                             "피부 건강 중요!:'오메가-3 및 오메가-6 지방산'";
            basicRecommendedProducts << "1" << "2";
        } else if (breed == "LAB") {
            breedInKorean = "리트리버";
            recommendation = "관절 건강 중요!:'글루코사민, 콘드로이틴, 오메가-3 지방산'<br>"
                             "피부 건강 중요!:'오메가-3 및 오메가-6 지방산'";
            basicRecommendedProducts << "1" << "2";
         } else if (breed == "POO") {
            breedInKorean = "푸들";
            recommendation = "푸들의 피부 건강을 위해 오메가-3, 오메가-6 지방산이 좋습니다.<br>"
                             "또한, 치아 건강을 위해 비타민 C, D, K와 칼슘, 오메가-3 지방산을 권장합니다.";
            basicRecommendedProducts << "2" << "3";
        } else if (breed == "BIC") {
            breedInKorean = "비숑 프리제";
            recommendation = "비숑 프리제의 관절 건강을 위해 글루코사민, 콘드로이틴, 오메가-3 지방산을 권장합니다.<br>"
                             "치아 건강을 위해서는 비타민 C, D, K와 칼슘, 오메가-3 지방산이 필요합니다.";
            basicRecommendedProducts << "1" << "3";
        } else if (breed == "CHL") {
            breedInKorean = "치와와";
            recommendation = "치와와의 관절 건강을 위해 글루코사민, 콘드로이틴, 오메가-3 지방산을 섭취하는 것이 좋습니다.<br>"
                             "치아 건강을 위해서는 비타민 C, D, K와 칼슘, 오메가-3 지방산을 권장합니다.";
            basicRecommendedProducts << "1" << "3";
        } else if (breed == "TER") {
            breedInKorean = "테리어";
            recommendation = "테리어의 피부 건강을 위해 오메가-3, 오메가-6 지방산이 좋습니다.<br>"
                             "치아 건강을 위해서는 비타민 C, D, K와 칼슘, 오메가-3 지방산을 섭취하는 것이 좋습니다.";
            basicRecommendedProducts << "2" << "3";
          } else if (breed == "DRI") {
            breedInKorean = "시츄";
            recommendation = "기관지 건강 중요!:'오메가-3 지방산, 비타민 C, E, 글루코사민, 콘드로이틴'<br>"
                             "치아 건강 중요!:'비타민 C, D, K와 칼슘, 오메가-3 지방산'";
            basicRecommendedProducts << "4" << "3";
          } else if (breed == "MAL") {
            breedInKorean = "말티즈";
            recommendation = "말티즈의 저혈당증 예방을 위해 단백질과 비타민 B, 오메가-3 지방산을 섭취하는 것이 좋습니다.<br>"
                                                 "피부 건강을 위해서는 오메가-3, 오메가-6 지방산이 필요합니다.";
            basicRecommendedProducts << "5" << "2";
        } else {
            breedInKorean = "알 수 없는 품종";
            recommendation = "해당 품종의 추천 정보가 없습니다.";
        }
    }
}
void Tab3ControlPannel::tab3RecvDataSlot_db(QString recvData)
{
    QStringList qList = recvData.split("@");

    if (qList.size() >= 3 && qList[2] == "DB_BREED")
    {
        // SQL로부터 받은 추천 상품을 처리
        dbRecommendedProducts.clear();  // 이전 추천 상품 리스트 초기화
        dbRecommendedProducts << qList[3] << qList[4];  // 추천 상품 1과 2를 리스트에 추가

        // 최종 추천 창 표시
        displayRecommendationDialog();
    }
}

// 최종 추천 창을 띄우는 함수
void Tab3ControlPannel::displayRecommendationDialog()
{
    // 대기 창이 열려 있으면 닫기
    if (waitingDialog != nullptr && waitingDialog->isVisible()) {
        qDebug() << "Closing waiting dialog";
        waitingDialog->close();  // 대기 창을 닫음
    }
    // 다이얼로그 생성
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("추천 상품");
    dialog->resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    // 제목 라벨
    QLabel *titleLabel = new QLabel(QString("귀여운 %1(이)군요!").arg(breedInKorean));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #333333;");
    layout->addWidget(titleLabel);

    // 추천 내용 라벨
    QLabel *recommendationLabel = new QLabel();
    recommendationLabel->setWordWrap(true);
    recommendationLabel->setText(QString("<html><body>%1<br>추천 상품 번호: %2<br> %3 견주님들이 선호하는 상품: %4</body></html>")
                                 .arg(recommendation)
                                 .arg(basicRecommendedProducts.join(", "))  // 기본 추천 상품 번호 출력
                                 .arg(breedInKorean)
                                 .arg(dbRecommendedProducts.join(", ")));   // DB 추천 상품 번호 출력

    recommendationLabel->setAlignment(Qt::AlignCenter);
    recommendationLabel->setStyleSheet("font-size: 18px; color: #666666; padding: 15px;");
    layout->addWidget(recommendationLabel);

    // 확인 버튼
    QPushButton *confirmButton = new QPushButton("확인", dialog);
    confirmButton->setStyleSheet("font-size: 18px; padding: 10px 20px; background-color: #4CAF50; color: white; border-radius: 10px;");
    layout->addWidget(confirmButton);
    connect(confirmButton, &QPushButton::clicked, dialog, &QDialog::accept);

    dialog->setLayout(layout);
    dialog->exec();
    basicRecommendedProducts.clear();
    dbRecommendedProducts.clear();
}
Tab3ControlPannel::~Tab3ControlPannel()
{
    delete ui;
    if (waitingDialog) {
            delete waitingDialog;  // 메모리 해제
        }
}



