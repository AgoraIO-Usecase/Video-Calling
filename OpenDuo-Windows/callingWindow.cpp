#include "stdafx.h"
#include "callingWindow.h"
#include "ui_callingWindow.h"
#include "agoraobject.h"
#include "qgraphicseffect.h"

 CallingWindow::CallingWindow(QWidget *parent)
     : QWidget(parent)
     , ui(new Ui::callingWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
   // this->setAttribute(Qt::WA_TransparentForMouseEvents,false);

    connect(this,SIGNAL(clicked()),this,SLOT(on_mouseclicked()));
}

 CallingWindow::CallingWindow(QWidget* pLastWnd, QWidget *parent)
     : QWidget(parent)
     , ui(new Ui::callingWindow)
     , m_pLastWnd(pLastWnd)
 {
     ui->setupUi(this);

     this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
     this->setAttribute(Qt::WA_TranslucentBackground);
     connect(this,SIGNAL(clicked()),this,SLOT(on_mouseclicked()));
     connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onLocalInvitationCanceled,this,&CallingWindow::onLocalCallInvitationCanceled);
     m_originalSize = this->size();
 }


CallingWindow::~CallingWindow()
{
    delete ui;
}

void CallingWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        m_mousePosition = e->pos();
        if(m_mousePosition.x() < lnTitleWidth &&
           m_mousePosition.y() < lnTitleHeight)
            m_bMousePressed = true;
    }
}

void CallingWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(m_bMousePressed == true) {
        QPoint movePos = e->globalPos() - m_mousePosition;
        move(movePos);
        e->accept();
    }
}

void CallingWindow::mouseReleaseEvent(QMouseEvent *e)
{
   m_bMousePressed = false;
}


void CallingWindow::on_dialCancelButton_clicked()
{
    if(!CAgoraRtmInstance::getInstance()->CanelLocalInvitation())
        return;
}

void CallingWindow::onLocalCallInvitationCanceled(void* invitation)
{
    ILocalCallInvitation* localInvitation =(ILocalCallInvitation*)invitation;

    if(CAgoraRtmInstance::getInstance()->IsRemoteCalledBylocal(localInvitation->getCalleeId())){
        const char* response = localInvitation->getResponse();
        CAgoraRtmInstance::getInstance()->Clear();
        CAgoraObject::getInstance()->Stop();
        this->hide();
        m_pLastWnd->show();
    }
}

void CallingWindow::onLocalInvitationReceivedByPeer(QString id)
{
    ui->labelCallingId->setText(id);
}

void CallingWindow::resizeEvent(QResizeEvent *event)
{
    QSize sz = event->size();
    if(sz.width() != m_originalSize.width()
       ||sz.height() != m_originalSize.height()){

        resize(m_originalSize);
    }

    QWidget::resizeEvent(event);
}
