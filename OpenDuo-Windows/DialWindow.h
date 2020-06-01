#ifndef DIALWINDOW_H
#define DIALWINDOW_H

class roomsettings;
#include <Memory>
#include <QMainWindow>
#include "enterroom.h"
#include <QMainWindow>
namespace Ui {
class DialWindow;
}
class CallingWindow;
class DialWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DialWindow(QWidget *parent = 0);
    explicit DialWindow(QMainWindow* pLastWnd,QWidget *parent = 0);
    ~DialWindow();
    void HideCallingWindow();
    void Clear();
protected:
    void initWindow();

private slots:
    void OnClickBack();
    void OnClickMin();

    void OnlineEditEnter();
    void on_lineEdit_textEdited(const QString &arg1);
    void on_lineEdit_2_textEdited(const QString &arg1);
    void on_lineEdit_3_textEdited(const QString &arg1);
    void on_lineEdit_4_textEdited(const QString &arg1);
    void on_btnjoin_clicked();

    void onLineStatusPeer(QString peerId, bool online, int onlineState);
    void onLocalInvitationFailure(QString errorInfo, int errorCode);
    void onLocalInvitationReceivedByPeer(void* invitation);
    void onLocalInvitationRefused(void* invitation);

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
    Ui::DialWindow *ui;
    QMainWindow* m_pLastWnd;
    QString     m_strRoomId;
    std::unique_ptr<CallingWindow> m_callingWindow;
};

#endif // DIALWINDOW_H
