#include "stdafx.h"
#include "mainwindow.h"
#include "enterroom.h"
#include "inroom.h"
#include <QApplication>
#include <IAgoraRtcEngine.h>
#include <IAgoraRtmCallManager.h>
using namespace agora::rtc;
Q_DECLARE_METATYPE(uid_t)
Q_DECLARE_METATYPE(USER_OFFLINE_REASON_TYPE)
Q_DECLARE_METATYPE(LocalVideoStats)
Q_DECLARE_METATYPE(RemoteVideoStats)
Q_DECLARE_METATYPE(RtcStats)
Q_DECLARE_METATYPE(agora::rtm::ILocalCallInvitation*)
Q_DECLARE_METATYPE(agora::rtm::IRemoteCallInvitation*)
CAgoraConfig gAgoraConfig;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("home.ico"));
    qRegisterMetaType<uid_t>();
    qRegisterMetaType<USER_OFFLINE_REASON_TYPE>();
    qRegisterMetaType<LocalVideoStats>();
    qRegisterMetaType<RemoteVideoStats>();
    qRegisterMetaType<RtcStats>();
    qRegisterMetaType<agora::rtm::ILocalCallInvitation*>();
    qRegisterMetaType<agora::rtm::IRemoteCallInvitation*>();
    MainWindow w;
    w.show();

    return a.exec();
}
