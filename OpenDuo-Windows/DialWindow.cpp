#include "stdafx.h"
#include "DialWindow.h"
#include "callingWindow.h"
#include "ui_DialWindow.h"
#include "agoraobject.h"
#include "enterroom.h"
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
DialWindow::DialWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DialWindow)
    , m_bMousePressed(false)
    , m_callingWindow(new CallingWindow())

{
    ui->setupUi(this);
    initWindow();
}

DialWindow::DialWindow(QMainWindow* pLastWnd,QWidget *parent)
    : QWidget(parent)
    , m_pLastWnd(pLastWnd)
    , ui(new Ui::DialWindow)
    , m_bMousePressed(false)
    , m_callingWindow(new CallingWindow(this, 0))
{
    ui->setupUi(this);
    initWindow();
}

DialWindow::~DialWindow()
{
    delete ui;
}

void DialWindow::initWindow()
{
    QRegExp regExp("[0-9]");
    ui->lineEdit->setValidator( new QRegExpValidator(regExp,  ui->lineEdit));
    ui->lineEdit_2->setValidator( new QRegExpValidator(regExp,  ui->lineEdit_2));
    ui->lineEdit_3->setValidator( new QRegExpValidator(regExp,  ui->lineEdit_3));
    ui->lineEdit_4->setValidator( new QRegExpValidator(regExp,  ui->lineEdit_4));
    connect(ui->btnBack,&QPushButton::clicked,this,&DialWindow::OnClickBack);
    connect(ui->btnmin,&QPushButton::clicked,this,&DialWindow::OnClickMin);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onLineStatusPeer,this,&DialWindow::onLineStatusPeer);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onLocalInvitationFailure,this,&DialWindow::onLocalInvitationFailure);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onLocalInvitationReceivedByPeer,this,&DialWindow::onLocalInvitationReceivedByPeer);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onLocalInvitationRefused,this,&DialWindow::onLocalInvitationRefused);
        m_originalSize = this->size();
}

void DialWindow::OnClickBack()
{
   Clear();
    m_pLastWnd->show();
    this->hide();
}

void DialWindow::OnClickMin()
{
    this->showMinimized();
}

void DialWindow::OnlineEditEnter()
{
    on_btnjoin_clicked();
}

void DialWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        m_mousePosition = e->pos();
        if(m_mousePosition.x() < lnTitleWidth &&
           m_mousePosition.x() >= lnGapWidth &&
           m_mousePosition.y() >= lnGapHeight &&
           m_mousePosition.y() < lnTitleHeight)
            m_bMousePressed = true;
    }
}

void DialWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(m_bMousePressed == true) {
        QPoint movePos = e->globalPos() - m_mousePosition;
        move(movePos);
        e->accept();
    }
}

void DialWindow::mouseReleaseEvent(QMouseEvent *e)
{
   m_bMousePressed = false;
}

void DialWindow::on_lineEdit_textEdited(const QString &arg1)
{
    //input
    if(!arg1.isEmpty()){
        ui->lineEdit_2->setFocus();
    }
}

void DialWindow::on_lineEdit_2_textEdited(const QString &arg1)
{
    if(!arg1.isEmpty()){
        ui->lineEdit_3->setFocus();
    }else{
        ui->lineEdit->setFocus();
    }
}

void DialWindow::on_lineEdit_3_textEdited(const QString &arg1)
{
    if(!arg1.isEmpty()){
        ui->lineEdit_4->setFocus();
    }else{
        ui->lineEdit_2->setFocus();
    }
}

void DialWindow::on_lineEdit_4_textEdited(const QString &arg1)
{
    if(!arg1.isEmpty()){
        ui->btnjoin->setFocus();
    } else {
        ui->lineEdit_3->setFocus();
    }
}

void DialWindow::on_btnjoin_clicked()
{
   QString s1 = ui->lineEdit->text();
   QString s2 = ui->lineEdit_2->text();
   QString s3 = ui->lineEdit_3->text();
   QString s4 = ui->lineEdit_4->text();

   if(s1.isEmpty() || s2.isEmpty() || s3.isEmpty() || s4.isEmpty()){
       return;
   }
   ui->btnjoin->setEnabled(false);
   if(!CAgoraRtmInstance::getInstance()->QueryPeerOnlineStatus(s1+s2+s3+s4)){
       ui->btnjoin->setEnabled(true);
   }
}

void DialWindow::HideCallingWindow()
{
    m_callingWindow->hide();
}

void DialWindow::onLineStatusPeer(QString peerId, bool online, int onlineState)
{
    ui->btnjoin->setEnabled(true);
    if(!online){
        QString stateInfo;
        switch (onlineState) {
        case PEER_ONLINE_STATE_UNREACHABLE:
            stateInfo = " online state unreachable";
            break;
        case PEER_ONLINE_STATE_OFFLINE :
            stateInfo = " offline";
            break;
        }
        ui->errorLabel->setText(peerId + stateInfo);
        return;
    }

    CAgoraRtmInstance::getInstance()->CallRemoteUser(peerId);
}

void DialWindow::onLocalInvitationFailure(QString errorInfo, int errorCode)
{
    if(errorCode == LOCAL_INVITATION_ERR_OK){

    }else {
        m_callingWindow->hide();
        show();
    }
    ui->errorLabel->setText(errorInfo);
    ui->btnjoin->setEnabled(true);
    CAgoraRtmInstance::getInstance()->Clear();
    CAgoraObject::getInstance()->Stop();
}

void DialWindow::onLocalInvitationReceivedByPeer(void* invitation)
{
    ILocalCallInvitation* localInvitation = (ILocalCallInvitation*)invitation;

    CAgoraRtmInstance::getInstance()->SetBusy(true);
    this->hide();
    m_callingWindow->show();
    m_callingWindow->onLocalInvitationReceivedByPeer(localInvitation->getCalleeId());
    CAgoraObject::getInstance()->PlayCalling();
}

void DialWindow::onLocalInvitationRefused(void* invitation)
{
    ILocalCallInvitation* localInvitation = (ILocalCallInvitation*)invitation;
    QString response = localInvitation->getResponse();
    QString peerId   = localInvitation->getCalleeId();
    if(response.compare("busy") == 0){
        CAgoraRtmInstance::getInstance()->CanelLocalInvitation();
        CAgoraRtmInstance::getInstance()->Clear();
        CAgoraObject::getInstance()->Stop();
        m_callingWindow->hide();
        this->show();
        ui->errorLabel->setText(QString("%1 is busy").arg(peerId));
        return;
    }

    if(CAgoraRtmInstance::getInstance()->IsRemoteCalledBylocal(peerId) == 0){
        CAgoraRtmInstance::getInstance()->Clear();
        CAgoraObject::getInstance()->Stop();
        m_callingWindow->hide();
        this->show();
        ui->errorLabel->setText(QString("%1 refused").arg(peerId));
    }
}

void DialWindow::Clear()
{
    ui->errorLabel->setText("");
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    ui->lineEdit_4->setText("");
    ui->btnjoin->setEnabled(true);
}


void DialWindow::resizeEvent(QResizeEvent *event)
{
    QSize sz = event->size();
    if(sz.width() != m_originalSize.width()
       ||sz.height() != m_originalSize.height()){

        resize(m_originalSize);
    }

    QWidget::resizeEvent(event);
}


