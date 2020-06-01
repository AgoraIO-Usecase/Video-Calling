#include "stdafx.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "agoraobject.h"
#include "enterroom.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_bMousePressed(false)
    , m_upRs(new roomsettings(static_cast<QMainWindow*>(this),NULL))
    , dialWindow(new DialWindow(static_cast<QMainWindow*>(this), 0))
    , m_calledWindow(new CalledWindow(static_cast<QMainWindow*>(this), 0))
{
    ui->setupUi(this);

    connect(ui->btnset,&QPushButton::clicked,this,&MainWindow::OnClickSettings);
    connect(ui->btnmin,&QPushButton::clicked,this,&MainWindow::OnClickMin);
    connect(ui->btnclose,&QPushButton::clicked,this,&MainWindow::OnClickClose);
    connect(ui->btnjoin,&QPushButton::clicked,this,&MainWindow::OnClickJoin);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::LoginSuccess,this, &MainWindow::loginRtmSuccess);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::LoginFailed,this, &MainWindow::loginRtmFailed);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onLogout,this,&MainWindow::onLogout);

    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onRemoteInvitationRefused,this,&MainWindow::onRemoteInvitationRefused);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onRemoteInvitationFailure,this,&MainWindow::onRemoteInvitationFailure);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onRemoteInvitationReceived,this,&MainWindow::onRemoteInvitationReceived);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onRemoteInvitationAccepted,this,&MainWindow::onRemoteInvitationAccepted);
    connect(CAgoraRtmInstance::getInstance(), &CAgoraRtmInstance::onLocalInvitationAccepted,this,&MainWindow::onLocalInvitationAccepted);
    initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWindow()
{
    ui->btnjoin->setEnabled(false);
    QString callingId = gAgoraConfig.getCallingId();
    ui->labelId->setText(callingId);
    CAgoraRtmInstance::getInstance()->Login(APP_TOKEN, callingId);
    m_strRoomId.clear();
    QString strDir = QCoreApplication::applicationDirPath();
    strDir.append("\\AgoraSDK.log");
    videoDeviceWindowId = winId();
    CAgoraObject::getInstance(this)->setLogPath(strDir);
    CAgoraObject::getInstance()->enableAudio(true);
    CAgoraObject::getInstance()->enableVideo(true);
    CAgoraObject::getInstance()->EnableWebSdkInteroperability(true);
    CAgoraObject::getInstance()->SetChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
    CAgoraObject::getInstance()->SetClientRole(CLIENT_ROLE_BROADCASTER);
    m_originalSize = this->size();
}

void MainWindow::OnClickSettings()
{
    this->hide();
    m_upRs->initWindow(m_strRoomId);
    m_upRs->show();
}

void MainWindow::OnClickMin()
{
    this->showMinimized();
}

void MainWindow::OnClickClose()
{
    CAgoraRtmInstance::getInstance()->Logout();

}

void MainWindow::OnClickJoin()
{
    if(m_upRs)
        m_upRs->SetCustomVideoProfile();
    else
        CAgoraObject::getInstance()->SetCustomVideoProfile();
    dialWindow->show();
    this->hide();
}

void MainWindow::OnlineEditEnter()
{
    OnClickJoin();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
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

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(m_bMousePressed == true) {
        QPoint movePos = e->globalPos() - m_mousePosition;
        move(movePos);
        QString info = QString("%1 %2 %3 %4\n").arg(movePos.x()).arg(movePos.y()).arg(this->width()).arg(this->height());
        qDebug("%s %s", __FUNCTION__, info.toUtf8().data());
        e->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
   m_bMousePressed = false;
}

void MainWindow::receive_exitChannel()
{
    //qDebug(__FUNCTION__);
    dialWindow->Clear();
    m_upIr->leavechannel();
    this->show();
}

void MainWindow::loginRtmSuccess()
{
    if(CAgoraRtmInstance::getInstance()->InitCallManager()){
         ui->btnjoin->setEnabled(true);
    }
}

void MainWindow::loginRtmFailed(QString errorInfo)
{
    ui->errorLabel->setText(errorInfo);
}

void MainWindow::onLogout(int errorCode)
{
    this->close();
}

void MainWindow::onRemoteInvitationReceived(void* invitation)
{
    agora::rtm::IRemoteCallInvitation* remoteInvitation = (agora::rtm::IRemoteCallInvitation*)invitation;
    //busy
    if(CAgoraRtmInstance::getInstance()->IsBusy()){
        remoteInvitation->setResponse("busy");
        CAgoraRtmInstance::getInstance()->RefuseRemoteInvitation(remoteInvitation);
        return;
    }
    CAgoraRtmInstance::getInstance()->SetBusy(true);
    CAgoraObject::getInstance()->PlayCalled();

    m_calledWindow->show();
    m_calledWindow->onRemoteInvitationReceived(invitation);
    CAgoraRtmInstance::getInstance()->setCurrentRemoteInvitation(invitation);
    CAgoraRtmInstance::getInstance()->SetChannelId(remoteInvitation->getContent());
    if(this->isVisible()){
        m_calledWindow->setLastWnd(this);
        hide();
    }else{
        m_calledWindow->setLastWnd(dialWindow.get());
        dialWindow->hide();
    }
}

void MainWindow::onRemoteInvitationFailure(void* invitation, int errorCode)
{
    IRemoteCallInvitation* remoteInvitation = (IRemoteCallInvitation*)invitation;
    CAgoraObject::getInstance()->Stop();
    CAgoraRtmInstance::getInstance()->Clear();
    m_calledWindow->BackLastWindow();
}

void MainWindow::onRemoteInvitationRefused(void *invitation)
{
    IRemoteCallInvitation* remoteInvitation = (IRemoteCallInvitation*)invitation;
    if(CAgoraRtmInstance::getInstance()->isRemoteCalledUser(remoteInvitation->getCallerId())){
        CAgoraObject::getInstance()->Stop();
        CAgoraRtmInstance::getInstance()->Clear();
        m_calledWindow->BackLastWindow();
    }
}

void MainWindow::onRemoteInvitationAccepted(QString peerId)
{
    CAgoraObject::getInstance()->Stop();
    m_calledWindow->hide();
    m_upIr.reset(new InRoom);
    InRoom* receive1 = m_upIr.get();
    m_strRoomId = CAgoraRtmInstance::getInstance()->GetChannelId();
    connect(this,SIGNAL(joinchannel(QMainWindow*,QString,uint)),receive1,SLOT(joinchannel(QMainWindow*,QString,uint)));
    emit joinchannel(this,m_strRoomId,0);
}

void MainWindow::onLocalInvitationAccepted(QString calleeId, QString response)
{
    CAgoraObject::getInstance()->Stop();
    m_upIr.reset(new InRoom);
    InRoom* receive1 = m_upIr.get();
    connect(this,SIGNAL(joinchannel(QMainWindow*,QString,uint)),receive1,SLOT(joinchannel(QMainWindow*,QString,uint)));
    m_strRoomId = CAgoraRtmInstance::getInstance()->GetChannelId();

    unsigned int uid = CAgoraRtmInstance::getInstance()->getRtcUid();
    emit joinchannel(this,m_strRoomId,uid);
    dialWindow->HideCallingWindow();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QSize sz = event->size();
    if(sz.width() != m_originalSize.width()
       ||sz.height() != m_originalSize.height()){

        resize(m_originalSize);
    }

    QMainWindow::resizeEvent(event);
}

