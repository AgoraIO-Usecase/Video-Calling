#ifndef CAGORAOBJECT_H
#define CAGORAOBJECT_H

#include <Memory>
#include <mutex>
#include "agoraqtjson.h"
//Specify your APP ID here
#define APP_ID ""
#define APP_TOKEN ""

#include <QString>
#include <QVariant>
#include <IAgoraRtcEngine.h>
#include <IAgoraMediaEngine.h>
#include <IAgoraRtmService.h>
#include <IAgoraRtmCallManager.h>
#include <vector>
using namespace agora::rtc;
using namespace agora::rtm;
typedef struct{
    QString id;
    QString name;
}DeviceInfo;
typedef std::vector<DeviceInfo> qSSMap;
class CAgoraObject:public QObject
{
    Q_OBJECT
public:

    int joinChannel(const QString& key, const QString& channel, uint uid);
    int leaveChannel();
    int muteLocalAudioStream(bool muted);
    BOOL LocalVideoPreview(HWND hVideoWnd, BOOL bPreviewOn, RENDER_MODE_TYPE renderType = RENDER_MODE_TYPE::RENDER_MODE_FIT);
    BOOL RemoteVideoRender(uid_t uid, HWND hVideoWnd, RENDER_MODE_TYPE renderType = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN);

    int enableVideo(bool enabled);
    int enableAudio(bool enabled);
    BOOL MuteLocalVideo(BOOL bMute);
    BOOL MuteLocalAudio(BOOL bMute);

    BOOL setLogPath(const QString &strDir);
    BOOL SetChannelProfile(CHANNEL_PROFILE_TYPE channelType);
    BOOL SetClientRole(CLIENT_ROLE_TYPE roleType);
    BOOL EnableWebSdkInteroperability(BOOL bEnable);

    qSSMap getRecordingDeviceList();
    qSSMap getPlayoutDeviceList();
    qSSMap getVideoDeviceList();

    int setRecordingDevice(const QString& guid);
    int setPlayoutDevice(const QString& guid);
    int setVideoDevice(const QString& guid);
    QString getCurrentVideoDevice();
    QString getCurrentPlaybackDevice();
    QString getCurrentRecordingDevice();

    BOOL setVideoProfile(int nWidth,int nHeight, FRAME_RATE fps, int bitrate);
    BOOL setRecordingIndex(int nIndex);
    BOOL setPlayoutIndex(int nIndex);
    BOOL setVideoIndex(int nIndex);

    bool setBeautyEffectOptions(bool enabled, BeautyOptions& options);
    void SetDefaultParameters();
    bool SetCustomVideoProfile();

    QString GetAppToken();
    void PlayCalling();
    void PlayCalled();
    void Stop();
public slots:
    void UpdateVideoDevices( QString deviceId, int deviceType, int deviceState);
    void UpdateAudioDevices( QString deviceId, int deviceType, int deviceState);
signals:
    void sender_videoStopped();
    void sender_joinedChannelSuccess(const QString &qsChannel, unsigned int uid, int elapsed);
    void sender_userJoined(unsigned int uid, int elapsed);
    void sender_userOffline(unsigned int uid, USER_OFFLINE_REASON_TYPE reason);
    void sender_firstLocalVideoFrame(int width, int height, int elapsed);
    void sender_firstRemoteVideoDecoded(unsigned int uid, int width, int height, int elapsed);
    void sender_firstRemoteVideoFrameDrawn(unsigned int uid, int width, int height, int elapsed);
    void sender_localVideoStats(const LocalVideoStats &stats);
    void sender_remoteVideoStats(const RemoteVideoStats &stats);
    void sender_rtcStats(const RtcStats &stats);
    void update_videoDevices( QString deviceId, int deviceType, int deviceState);
    void update_audioDevices( QString deviceId, int deviceType, int deviceState);

public:
    static CAgoraObject* getInstance(QObject *parent = 0);
    static CAgoraObject* m_pInstance;
    static std::mutex    m_mutex;

private:
    explicit CAgoraObject(QObject *parent = 0);
    ~CAgoraObject();

    agora::rtc::IRtcEngine* m_rtcEngine;
    std::unique_ptr<agora::rtc::IRtcEngineEventHandler> m_eventHandler;
    AgoraQtJson m_agoraJson;

    AVideoDeviceManager* videoDeviceManager;
    AAudioDeviceManager* audioDeviceManager;
};



class CAgoraRtmInstance :public QObject
{
    Q_OBJECT
public:
    bool Login(QString qToken, QString qUserId);
    bool Logout();
    bool InitCallManager();
    bool CallRemoteUser(QString remoteUserId);
    bool CanelLocalInvitation();
    void ReleaseLocalInvitation();
    bool QueryPeerOnlineStatus(QString calleeId);
    void setCurrentRemoteInvitation(void* invitation){m_currentRemoteInvitation = (IRemoteCallInvitation*)invitation;}
    bool RefuseCurrentRemoteInvitation();
    bool AcceptCurrentRemoteInvitation();
    bool RefuseRemoteInvitation(IRemoteCallInvitation* invitation);
    void ReleaseRemoteInvitation();

    bool isRemoteCalledUser(QString peerId);
    static CAgoraRtmInstance* getInstance();

    QString GetLocalId() { return userId;}
    void SetChannelId(QString channelId){m_rtcChannelId = channelId;}
    QString GetChannelId() {return m_rtcChannelId;}
    void SetBusy(bool busy) {m_busy = busy;}
    bool IsBusy(){return m_busy;}
    bool IsRemoteCalledBylocal(QString peerId);
    // QString GetRemoteUserIdByLocal() { return m_remoteUserId;}
    void Clear();
    unsigned int getRtcUid();
signals:
    void LoginSuccess();
    void LoginFailed(QString errorInfo);
    void onLogout(int errorCode);
    //rtm call manager event
    void onLocalInvitationReceivedByPeer(void* invitation);
    void onLocalInvitationCanceled(void* invitation);
    void onLocalInvitationFailure(QString errorInfo, int errorCode);
    void onLocalInvitationAccepted(QString calleeId, QString response);
    void onLocalInvitationRefused(void* invitation);
    void onRemoteInvitationRefused(void* invitation);
    void onRemoteInvitationAccepted(QString peerId);
    void onRemoteInvitationReceived(void* invitation);
    void onRemoteInvitationFailure(void* invitation, int errorCode);
    void onRemoteInvitationCanceled(QString calleeId);

    //rtm service event
    void onLineStatusPeer(QString peerId, bool online, int onlineState);
protected:
    CAgoraRtmInstance();
    ~CAgoraRtmInstance();
private:
    IRtmService* m_callService;
    IRtmCallManager* m_callManager;
    static std::mutex    m_mutex;
    std::unique_ptr<IRtmServiceEventHandler> m_serviceEventHandler;
    std::unique_ptr<IRtmCallEventHandler> m_callEventHandler;
    ILocalCallInvitation* m_callInvitation;

    IRemoteCallInvitation* m_currentRemoteInvitation;
    QString userId;
    QString m_remoteUserId;
    QString m_rtcChannelId;

    bool m_busy;
};

#endif // CAGORAOBJECT_H
