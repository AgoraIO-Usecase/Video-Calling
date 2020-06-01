#ifndef CALLEDWINDOW_H
#define CALLEDWINDOW_H

#include <QWidget>

namespace Ui {
class calledWindow;
}

class CalledWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalledWindow(QWidget *parent = 0);
    explicit CalledWindow(QWidget* pLastWnd, QWidget *parent = 0);
    ~CalledWindow();
    void setLastWnd(QWidget* lastWnd){m_pLastWnd = lastWnd;}
    void BackLastWindow();
    void onRemoteInvitationReceived(void* invitation);
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *event);
private slots:
    void on_refuseButton_clicked();
    void on_acceptButton_clicked();
    void onRemoteInvitationCanceled(QString peerId);
private:
    const int lnTitleWidth = 1366;
    const int lnTitleHeight = 30;
    QPoint m_mousePosition;
    bool   m_bMousePressed;
    QSize m_originalSize;
private:
    Ui::calledWindow *ui;
    QWidget* m_pLastWnd;
};

#endif // CALLEDWINDOW_H
