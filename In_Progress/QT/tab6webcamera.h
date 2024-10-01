#ifndef TAB6WEBCAMERA_H
#define TAB6WEBCAMERA_H

#include <QWidget>
#include <QProcess>
#include <QGraphicsPixmapItem>
#include <QThread>
#include <vlc/vlc.h>  // VLC 라이브러리 추가

namespace Ui {
class Tab6WebCamera;
}

class Tab6WebCamera : public QWidget
{
    Q_OBJECT

public:
    explicit Tab6WebCamera(QWidget *parent = nullptr);
    ~Tab6WebCamera();

private:
    Ui::Tab6WebCamera *ui;
    QProcess *pQProcess;

    // VLC 관련 변수 추가
    libvlc_instance_t *vlcInstance;
    libvlc_media_player_t *mediaPlayer;

private slots:
    void camStartSlot(bool);
};

#endif // TAB6WEBCAMERA_H
