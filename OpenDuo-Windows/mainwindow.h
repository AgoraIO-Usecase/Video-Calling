#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class roomsettings;
#include <Memory>
#include <QMainWindow>
#include "DialWindow.h"
#include "roomsettings.h"
#include "enterroom.h"
#include "inroom.h"
#include "calledWindow.h"
namespace Ui {
class MainWindow;
}

class DialWindow;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void initWindow();

private slots:
    void OnClickSettings();
    void OnClickMin();
    void OnClickClose();
    //void OnClickLive();
    //void OnClickGuide();
    void OnClickJoin();
   // void OnlineEditChanged();
    void OnlineEditEnter();

    void receive_exitChannel();
    void loginRtmSuccess();
    void loginRtmFailed(QString errorInfo);
    void onLogout(int errorCode);

    void onRemoteInvitationReceived(void* invitation);
    void onRemoteInvitationRefused(void *remoteInvitation);
    void onLocalInvitationAccepted(QString calleeId, QString response);
    void onRemoteInvitationAccepted(QString peerId);
    void onRemoteInvitationFailure(void* invitation, int errorCode);
signals:
    void joinchannel(QMainWindow* pMainWnd,const QString &qsChannel,uint uid);
    void leavechannel();

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *event);
private:
    const int lnGapWidth = 18;
    const int lnGapHeight = 12;
    const int lnTitleWidth = 718;
    const int lnTitleHeight = 30;
    QPoint m_mousePosition;
    bool   m_bMousePressed;
    QSize m_originalSize;
private:
    Ui::MainWindow *ui;
    QString     m_strRoomId;
    std::unique_ptr<roomsettings> m_upRs;
    std::unique_ptr<InRoom> m_upIr;
    std::unique_ptr<DialWindow> dialWindow;
    std::unique_ptr<CalledWindow> m_calledWindow;
    QString callingId;
};

#endif // MAINWINDOW_H
