#ifndef CALLINGWINDOW_H
#define CALLINGWINDOW_H

#include <QWidget>

namespace Ui {
class callingWindow;
}

class CallingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CallingWindow(QWidget *parent = 0);
    explicit CallingWindow(QWidget* pLastWnd, QWidget *parent = 0);
    ~CallingWindow();
    void onLocalInvitationReceivedByPeer(QString id);
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *event);
private slots:
    void on_dialCancelButton_clicked();
    void onLocalCallInvitationCanceled(void* invitation);

private:
    const int lnTitleWidth = 1366;
    const int lnTitleHeight = 30;
    QPoint m_mousePosition;
    bool   m_bMousePressed;
    QSize m_originalSize;
private:
    Ui::callingWindow *ui;
    QWidget* m_pLastWnd;
};

#endif // CALLINGWINDOW_H
