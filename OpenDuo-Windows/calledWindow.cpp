#include "stdafx.h"
#include "calledWindow.h"
#include "ui_calledWindow.h"
#include "agoraobject.h"
#include "qgraphicseffect.h"

 CalledWindow::CalledWindow(QWidget *parent)
     : QWidget(parent)
     , ui(new Ui::calledWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
   // this->setAttribute(Qt::WA_TransparentForMouseEvents,false);

    connect(this,SIGNAL(clicked()),this,SLOT(on_mouseclicked()));
}

 CalledWindow::CalledWindow(QWidget* pLastWnd, QWidget *parent)
     : QWidget(parent)
     , ui(new Ui::calledWindow)
     , m_pLastWnd(pLastWnd)
 {
     ui->setupUi(this);

     this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
     this->setAttribute(Qt::WA_TranslucentBackground);
     connect(this,SIGNAL(clicked()),this,SLOT(on_mouseclicked()));
     connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onRemoteInvitationCanceled,this,&CalledWindow::onRemoteInvitationCanceled);
     m_originalSize = this->size();
 }

CalledWindow::~CalledWindow()
{
    delete ui;
}

void CalledWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        m_mousePosition = e->pos();
        if(m_mousePosition.x() < lnTitleWidth &&
           m_mousePosition.y() < lnTitleHeight)
            m_bMousePressed = true;
    }
}

void CalledWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(m_bMousePressed == true) {
        QPoint movePos = e->globalPos() - m_mousePosition;
        move(movePos);
        e->accept();
    }
}

void CalledWindow::mouseReleaseEvent(QMouseEvent *e)
{
   m_bMousePressed = false;
}

void CalledWindow::on_refuseButton_clicked()
{
    CAgoraRtmInstance::getInstance()->RefuseCurrentRemoteInvitation();
    CAgoraRtmInstance::getInstance()->setCurrentRemoteInvitation(nullptr);
}

void CalledWindow::on_acceptButton_clicked()
{
    CAgoraRtmInstance::getInstance()->AcceptCurrentRemoteInvitation();
}

void CalledWindow::BackLastWindow()
{
    hide();
    m_pLastWnd->show();
}

void CalledWindow::onRemoteInvitationReceived(void* invitation)
{
    IRemoteCallInvitation* remoteInvitation = (IRemoteCallInvitation*)invitation;
    ui->labelIncomingId->setText(QString("%1").arg(remoteInvitation->getCallerId()));
}

void CalledWindow::onRemoteInvitationCanceled(QString peerId)
{
    if(CAgoraRtmInstance::getInstance()->isRemoteCalledUser(peerId)){
        CAgoraRtmInstance::getInstance()->Clear();
        CAgoraObject::getInstance()->Stop();
        hide();
        m_pLastWnd->show();
    }
}

void CalledWindow::resizeEvent(QResizeEvent *event)
{
    QSize sz = event->size();
    if(sz.width() != m_originalSize.width()
       ||sz.height() != m_originalSize.height()){

        resize(m_originalSize);
    }

    QWidget::resizeEvent(event);
}



