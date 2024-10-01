#ifndef TAB3CONTROLPANNEL_H
#define TAB3CONTROLPANNEL_H

#include <QWidget>
#include <QPalette>
#include <QDebug>
#include <QLabel>
#include <QThread>
#include <QDialog>
#include <QVBoxLayout>

namespace Ui {
class Tab3ControlPannel;
}

class Tab3ControlPannel : public QWidget
{
    Q_OBJECT

public:
    explicit Tab3ControlPannel(QWidget *parent = nullptr);
    ~Tab3ControlPannel();

signals:
    void socketSendDataSig(QString);
private slots:
    //void on_pPBPlug_clicked(bool checked);
    //void on_pPBLamp_clicked(bool checked);
    void tab3RecvDataSlot(QString);
    void tab3RecvDataSlot_db(QString);

    void on_productButton_clicked();

    void on_pbCLR_clicked();

    void on_pbPayment_clicked();

    void on_pbRecommend_clicked();

private:
    Ui::Tab3ControlPannel *ui;
    //QPalette paletteColorOn;
    //QPalette paletteColorOff;
    QLabel *selectedProductsLabel;  // 선택된 상품 번호를 표시할 QLabel
    QList<int> selectedProducts;  // 선택된 상품 번호를 저장하는 리스트
    int totalPrice;
    void updateLCDDisplays();
    void resetSelection();
    QDialog *waitingDialog;
    QString breed; // 품종 데이터를 저장할 멤버 변수
    QString breedInKorean;
    QString recommendation;
    QList<QString> basicRecommendedProducts;  // 품종에 따른 기본 추천 상품 리스트};
    QList<QString> dbRecommendedProducts;  // SQL 추천 상품 리스트

    void displayRecommendationDialog();  // 최종 추천 다이얼로그를 띄우는 함수
};
#endif // TAB3CONTROLPANNEL_H
