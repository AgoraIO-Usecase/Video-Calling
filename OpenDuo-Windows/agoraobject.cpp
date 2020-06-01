#include "agoraobject.h"
#include <qmessagebox.h>
#include <cassert>
#include <qprocess>
#include <QApplication>
#include "stdafx.h"
int videoDeviceWindowId;
class AgoraRtcEngineEvent : public agora::rtc::IRtcEngineEventHandler
{
    CAgoraObject& m_pInstance;
public:
    AgoraRtcEngineEvent(CAgoraObject& engine)
        :m_pInstance(engine)
    {}

    virtual void onVideoStopped() override
    {
        emit m_pInstance.sender_videoStopped();
    }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override
    {
       emit m_pInstance.sender_joinedChannelSuccess(QString(channel),uid,elapsed);
    }
    virtual void onUserJoined(uid_t uid, int elapsed) override
    {
        //qDebug("%s %u",__FUNCTION__,uid);
        emit m_pInstance.sender_userJoined(uid, elapsed);
    }
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override
    {
        emit m_pInstance.sender_userOffline(uid, reason);
    }
    virtual void onFirstLocalVideoFrame(int width, int height, int elapsed) override
    {
        emit m_pInstance.sender_firstLocalVideoFrame(width, height, elapsed);
    }
    virtual void onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) override
    {
        emit m_pInstance.sender_firstRemoteVideoDecoded(uid, width, height, elapsed);
    }
    virtual void onFirstRemoteVideoFrame(uid_t uid, int width, int height, int elapsed) override
    {
       emit m_pInstance.sender_firstRemoteVideoFrameDrawn(uid, width, height, elapsed);
    }
    virtual void onLocalVideoStats(const LocalVideoStats &stats) override
    {
        emit m_pInstance.sender_localVideoStats(stats);
    }
    virtual void onRemoteVideoStats(const RemoteVideoStats &stats) override
    {
        emit m_pInstance.sender_remoteVideoStats(stats);
    }
    virtual void onRtcStats(const RtcStats &stats)
    {
        emit m_pInstance.sender_rtcStats(stats);
    }

    virtual void onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
    {
        QString id = QString::fromUtf8(deviceId);
        emit m_pInstance.update_videoDevices(id, deviceType, deviceState);
    }

    virtual void onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
    {
        QString id = QString::fromUtf8(deviceId);
        emit m_pInstance.update_audioDevices(id, deviceType, deviceState);
    }
};

CAgoraObject* CAgoraObject::getInstance(QObject *parent)
{
    std::lock_guard<std::mutex> autoLock(m_mutex);
    if(nullptr == m_pInstance)
        m_pInstance = new CAgoraObject(parent);

    return m_pInstance;
}

CAgoraObject* CAgoraObject::m_pInstance = nullptr;
std::mutex  CAgoraObject::m_mutex;

CAgoraObject::CAgoraObject(QObject *parent):
    QObject(parent),
    m_rtcEngine(createAgoraRtcEngine()),
    m_eventHandler(new AgoraRtcEngineEvent(*this))
{
    agora::rtc::RtcEngineContext context;
    context.eventHandler = m_eventHandler.get();
    QByteArray temp;
    if(strlen(APP_ID))
        context.appId = APP_ID;
    else {
        QString strAppId = gAgoraConfig.getAppId();
        if(strAppId.length()== 0){
            gAgoraConfig.setAppId(QString(""));
        }
        temp = strAppId.toUtf8();
        context.appId = const_cast<const char*>(temp.data());
    }
    if (*context.appId == '\0')
    {
        QMessageBox::critical(nullptr, ("AgoraOpenDuo"),
                                       ("You must specify APP ID before using the demo"));
        QProcess process;
        process.startDetached("notepad.exe", {"AgoraConfigOpenDuo.ini"}, "");
        ExitProcess(0);
    }

    context.context = (void*)videoDeviceWindowId;
    m_rtcEngine->initialize(context);

    videoDeviceManager  = new AVideoDeviceManager(m_rtcEngine);
    audioDeviceManager  = new AAudioDeviceManager(m_rtcEngine);

    connect(this, SIGNAL(update_audioDevices(QString,  int, int)),
            this, SLOT(UpdateAudioDevices(QString, int, int)));

    connect(this, SIGNAL(update_videoDevices(QString, int, int)),
            this, SLOT(UpdateVideoDevices(QString, int, int)));
}

CAgoraObject::~CAgoraObject()
{
    if(m_rtcEngine)
        m_rtcEngine->release();
}

int CAgoraObject::joinChannel(const QString& key, const QString& channel, uint uid)
{
    if (channel.isEmpty()) {
        QMessageBox::warning(nullptr,("AgoraHighSound"),("channelname is empty"));
        return -1;
    }
    m_rtcEngine->startPreview();
    int r = m_rtcEngine->joinChannel(key.toUtf8().data(), channel.toUtf8().data(), nullptr, uid);

    return r;
}

int CAgoraObject::leaveChannel()
{
    int r = m_rtcEngine->leaveChannel();
    return r;
}

int CAgoraObject::muteLocalAudioStream(bool muted)
{
    RtcEngineParameters rep(*m_rtcEngine);
    return rep.muteLocalAudioStream(muted);
}

BOOL CAgoraObject::LocalVideoPreview(HWND hVideoWnd, BOOL bPreviewOn, RENDER_MODE_TYPE renderType/* = RENDER_MODE_TYPE::RENDER_MODE_FIT*/)
{
    int nRet = 0;

    if (bPreviewOn) {
        VideoCanvas vc;

        vc.uid = 0;
        vc.view = hVideoWnd;
        vc.renderMode = renderType;

        m_rtcEngine->setupLocalVideo(vc);
        nRet = m_rtcEngine->startPreview();
    }
    else
        nRet = m_rtcEngine->stopPreview();

    return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::RemoteVideoRender(uid_t uid, HWND hVideoWnd, RENDER_MODE_TYPE renderType/* = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN*/)
{
    int nRet = 0;

    VideoCanvas vc;

    vc.uid = uid;
    vc.view = hVideoWnd;
    vc.renderMode = renderType;

    m_rtcEngine->setupRemoteVideo(vc);

    return nRet == 0 ? TRUE : FALSE;
}

int CAgoraObject::enableVideo(bool enabled)
{
    return enabled ? m_rtcEngine->enableVideo() : m_rtcEngine->disableVideo();
}

int CAgoraObject::enableAudio(bool enabled)
{
    return enabled ? m_rtcEngine->enableAudio() : m_rtcEngine->disableAudio();
}

BOOL CAgoraObject::setLogPath(const QString &strDir)
{
    int ret = 0;

    RtcEngineParameters rep(*m_rtcEngine);
    ret = rep.setLogFile(strDir.toUtf8().data());

    return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::SetChannelProfile(CHANNEL_PROFILE_TYPE channelType)
{
    int ret = 0;
    ret = m_rtcEngine->setChannelProfile(channelType);

    return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::SetClientRole(CLIENT_ROLE_TYPE roleType)
{
    int ret = 0;

    ret = m_rtcEngine->setClientRole(roleType);

    return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::EnableWebSdkInteroperability(BOOL bEnable)
{
    RtcEngineParameters rep(*m_rtcEngine);

    int	nRet = rep.enableWebSdkInteroperability(static_cast<bool>(bEnable));

    return nRet == 0 ? TRUE : FALSE;
}

qSSMap CAgoraObject::getRecordingDeviceList()
{
    qSSMap devices;

    if (!audioDeviceManager || !audioDeviceManager->get())
        return devices;

    agora::util::AutoPtr<IAudioDeviceCollection> spCollection((*audioDeviceManager)->enumerateRecordingDevices());
    if (!spCollection)
        return devices;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (!spCollection->getDevice(i, name, guid))
            {
                DeviceInfo info;
                info.id   = guid;
                info.name = name;
                devices.push_back(info);
            }
        }
    }
    return devices;
}

qSSMap CAgoraObject::getPlayoutDeviceList()
{
    qSSMap devices;

    if (!audioDeviceManager || !audioDeviceManager->get())
        return devices;

    agora::util::AutoPtr<IAudioDeviceCollection> spCollection((*audioDeviceManager)->enumeratePlaybackDevices());
    if (!spCollection)
        return devices;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (!spCollection->getDevice(i, name, guid))
            {
                DeviceInfo info;
                info.id   = guid;
                info.name = name;
                devices.push_back(info);
            }
        }
    }
    return devices;
}

qSSMap CAgoraObject::getVideoDeviceList()
{
	m_rtcEngine->enableVideo();
    qSSMap devices;

    if (!videoDeviceManager || !videoDeviceManager->get())
        return devices;

    agora::util::AutoPtr<IVideoDeviceCollection> spCollection((*videoDeviceManager)->enumerateVideoDevices());
    if (!spCollection)
        return devices;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (!spCollection->getDevice(i, name, guid))
            {
                DeviceInfo info;
                info.id   = guid;
                info.name = name;
                devices.push_back(info);
            }
        }
    }
    return devices;
}

QString CAgoraObject::getCurrentVideoDevice()
{
     if (!videoDeviceManager || !videoDeviceManager->get())
         return QString("");
     char deviceId[MAX_DEVICE_ID_LENGTH] = {0};
     QString strId("");
     if(0 == (*videoDeviceManager)->getDevice(deviceId)){
         strId = QString::fromUtf8(deviceId);
     }
     return strId;
}

QString CAgoraObject::getCurrentPlaybackDevice()
{
    if (!audioDeviceManager || !audioDeviceManager->get())
        return QString("");
    char deviceId[MAX_DEVICE_ID_LENGTH] = {0};
    QString strId("");
    if(0 == (*videoDeviceManager)->getDevice(deviceId)){
        strId = QString::fromUtf8(deviceId);
    }
    return strId;
}

QString CAgoraObject::getCurrentRecordingDevice()
{
    if (!audioDeviceManager || !audioDeviceManager->get())
        return QString("");
    char deviceId[MAX_DEVICE_ID_LENGTH] = {0};
    QString strId("");
    if(0 == (*audioDeviceManager)->getPlaybackDevice(deviceId)){
        strId = QString::fromUtf8(deviceId);
    }
    return strId;
}

int CAgoraObject::setRecordingDevice(const QString& guid)
{
    if (guid.isEmpty())
        return -1;

    if (!audioDeviceManager || !audioDeviceManager->get())
        return -1;
    return (*audioDeviceManager)->setRecordingDevice(guid.toUtf8().data());
}

int CAgoraObject::setPlayoutDevice(const QString& guid)
{
    if (guid.isEmpty())
        return -1;

    if (!audioDeviceManager || !audioDeviceManager->get())
        return -1;
    return (*audioDeviceManager)->setPlaybackDevice(guid.toUtf8().data());
}

int CAgoraObject::setVideoDevice(const QString& guid)
{
    if (guid.isEmpty())
        return -1;

    if (!videoDeviceManager || !videoDeviceManager->get())
        return -1;
    return (*videoDeviceManager)->setDevice(guid.toUtf8().data());
}

BOOL CAgoraObject::setVideoProfile(int nWidth,int nHeight, FRAME_RATE fps, int bitrate)
{
    int res = 0;
    VideoEncoderConfiguration vec;

    if(gAgoraConfig.isCustomFPS())
        fps = (FRAME_RATE)gAgoraConfig.getFPS();

    if(gAgoraConfig.isCustomBitrate())
        bitrate = gAgoraConfig.getBitrate();

     if(gAgoraConfig.isCustomResolution())
         gAgoraConfig.getVideoResolution(nWidth, nHeight);

    vec = VideoEncoderConfiguration(nWidth,nHeight,fps,bitrate,ORIENTATION_MODE_FIXED_LANDSCAPE);
    res = m_rtcEngine->setVideoEncoderConfiguration(vec);

    return res ==0 ? TRUE : FALSE;
}

BOOL CAgoraObject::setRecordingIndex(int nIndex)
{
    int res = 0;
   if (!audioDeviceManager || !audioDeviceManager->get())
        return FALSE;

    agora::util::AutoPtr<IAudioDeviceCollection> spCollection((*audioDeviceManager)->enumerateRecordingDevices());
    if (!spCollection)
        return FALSE;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    assert(res < count);
    spCollection->getDevice(nIndex,name,guid);
    res = spCollection->setDevice(guid);

    return res ==0 ? TRUE:FALSE;
}

BOOL CAgoraObject::setPlayoutIndex(int nIndex)
{
    int res = 0;
    if (!audioDeviceManager || !audioDeviceManager->get())
        return FALSE;

    agora::util::AutoPtr<IAudioDeviceCollection> spCollection((*audioDeviceManager)->enumeratePlaybackDevices());
    if (!spCollection)
        return FALSE;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    assert(res < count);
    spCollection->getDevice(nIndex,name,guid);
    res = spCollection->setDevice(guid);

    return res ==0 ? TRUE:FALSE;
}

BOOL CAgoraObject::setVideoIndex(int nIndex)
{
    int res = 0;

    if (!videoDeviceManager || !videoDeviceManager->get())
        return FALSE;

    agora::util::AutoPtr<IVideoDeviceCollection> spCollection((*videoDeviceManager)->enumerateVideoDevices());
    if (!spCollection)
        return FALSE;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    assert(nIndex < count);
    spCollection->getDevice(nIndex,name,guid);
    res = spCollection->setDevice(guid);

    return res == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::MuteLocalVideo(BOOL bMute)
{
     int nRet = 0;

    RtcEngineParameters rep(*m_rtcEngine);
    nRet = rep.enableLocalVideo(!bMute);

    return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::MuteLocalAudio(BOOL bMute)
{
    int nRet = 0;

    RtcEngineParameters rep(*m_rtcEngine);
    nRet = m_rtcEngine->enableLocalAudio(!bMute);

    return nRet == 0 ? TRUE : FALSE;
}

bool CAgoraObject::setBeautyEffectOptions(bool enabled, BeautyOptions& options)
{
	int nRet = 0;

	nRet = m_rtcEngine->setBeautyEffectOptions(enabled, options);
	return nRet == 0 ? true : false;
}

bool CAgoraObject::SetCustomVideoProfile()
{
    FRAME_RATE customFPS = FRAME_RATE_FPS_15;
    int customBitrate    = STANDARD_BITRATE;
    int nWidth = 640, nHeight = 480;

    return setVideoProfile(nWidth, nHeight, customFPS, customBitrate);
}

void CAgoraObject::CAgoraObject::SetDefaultParameters()
{
    std::map<std::string, std::string> mapStringParamsters;
    std::map<std::string, bool> mapBoolParameters;
    std::map<std::string, int> mapIntParameters;
    std::map<std::string, std::string> mapObjectParamsters;
    if(m_agoraJson.GetParameters(mapStringParamsters,
                                 mapBoolParameters,
                                 mapIntParameters,
                                 mapObjectParamsters)){
        AParameter apm(m_rtcEngine);
        for (auto iter = mapBoolParameters.begin();
            iter != mapBoolParameters.end(); ++iter) {
            apm->setBool(iter->first.c_str(), iter->second);
        }
        for (auto iter = mapStringParamsters.begin();
            iter != mapStringParamsters.end(); ++iter) {
            apm->setString(iter->first.c_str(), iter->second.c_str());
        }
        for (auto iter = mapIntParameters.begin();
            iter != mapIntParameters.end(); ++iter) {
            apm->setInt(iter->first.c_str(), iter->second);
        }

        for (auto iter = mapObjectParamsters.begin();
            iter != mapObjectParamsters.end(); ++iter) {
            apm->setObject(iter->first.c_str(), iter->second.c_str());
        }
    }
}

void CAgoraObject::UpdateVideoDevices( QString deviceId, int deviceType, int deviceState)
{
    if(deviceType == VIDEO_CAPTURE_DEVICE && videoDeviceManager){
        videoDeviceManager->release();
        videoDeviceManager = new AVideoDeviceManager(m_rtcEngine);
        QString cameraid = getCurrentVideoDevice();
        if(cameraid == deviceId && (deviceState == MEDIA_DEVICE_STATE_UNPLUGGED || deviceState == MEDIA_DEVICE_STATE_DISABLED)){
            qSSMap videoDeviceList = getVideoDeviceList();
            if(videoDeviceList.size() > 0){
                (*videoDeviceManager)->setDevice(deviceId.toUtf8());
            }
        }
    }
}
void CAgoraObject::UpdateAudioDevices( QString deviceId, int deviceType, int deviceState)
{
    if(audioDeviceManager){
        audioDeviceManager->release();
        audioDeviceManager = new AAudioDeviceManager(m_rtcEngine);

        QString audioId;


        if(deviceState == MEDIA_DEVICE_STATE_UNPLUGGED || deviceState == MEDIA_DEVICE_STATE_DISABLED){
            qSSMap audioDeviceList;
            if(deviceType == AUDIO_RECORDING_DEVICE){
                audioId = getCurrentRecordingDevice();
                audioDeviceList = getRecordingDeviceList();

                if(audioDeviceList.size() > 0 && audioId ==  deviceId){
                    (*audioDeviceManager)->setRecordingDevice(deviceId.toUtf8());
                }
            }
            else if(deviceType == AUDIO_PLAYOUT_DEVICE ){
                audioId = getCurrentPlaybackDevice();
                audioDeviceList = getPlayoutDeviceList();

                if(audioDeviceList.size() > 0 && audioId ==  deviceId){
                    (*audioDeviceManager)->setPlaybackDevice(deviceId.toUtf8());
                }
            }


        }
    }
}

QString CAgoraObject::GetAppToken()
{
    QString strAppToken = APP_TOKEN;
    if(!strAppToken.isEmpty())
        return strAppToken;
    return gAgoraConfig.getAppToken();
}

void CAgoraObject::PlayCalling()
{
    QString path = QApplication::applicationDirPath();
    path += "/ring_send.mp3";
    if(audioDeviceManager){
        (*audioDeviceManager)->startPlaybackDeviceTest(path.toUtf8());
    }
}

void CAgoraObject::PlayCalled()
{
    QString path = QApplication::applicationDirPath();
    path += "/ring_recv.mp3";
    if(audioDeviceManager){
        (*audioDeviceManager)->startPlaybackDeviceTest(path.toUtf8());
    }
}

void CAgoraObject::Stop()
{
    if(audioDeviceManager){
        (*audioDeviceManager)->stopPlaybackDeviceTest();
    }
}


///////////////////////////////
/// RTM Instance
///
///////////////////////////////

class CAgoraRtmServiceEventHandler : public IRtmServiceEventHandler
{
    CAgoraRtmInstance& m_rtmServiceInst;
public:
    CAgoraRtmServiceEventHandler( CAgoraRtmInstance& rtmServiceInst)
        : m_rtmServiceInst(rtmServiceInst)
    {}
    ~CAgoraRtmServiceEventHandler(){}

    virtual void onLoginSuccess()
    { emit m_rtmServiceInst.LoginSuccess();}

    virtual void onQueryPeersOnlineStatusResult(long long requestId, const PeerOnlineStatus* peersStatus, int peerCount, QUERY_PEERS_ONLINE_STATUS_ERR errorCode)
    {
        if(QUERY_PEERS_ONLINE_STATUS_ERR_OK  == errorCode && peerCount == 1){
            emit m_rtmServiceInst.onLineStatusPeer(peersStatus[0].peerId, peersStatus[0].isOnline, (int)peersStatus[0].onlineState);
        }
    }

    virtual void onLogout(LOGOUT_ERR_CODE errorCode)
    {
        emit m_rtmServiceInst.onLogout((int)errorCode);
    }
    virtual void onLoginFailure(LOGIN_ERR_CODE errorCode)
    {
        QString errorInfo;
        switch (errorCode) {
        case LOGIN_ERR_UNKNOWN:
            errorInfo = "Unknown";
            break;
        case LOGIN_ERR_REJECTED:
            errorInfo = "rejected";
            break;
        case LOGIN_ERR_INVALID_ARGUMENT:
            errorInfo = "Invalid argument";
            break;
        case LOGIN_ERR_INVALID_APP_ID:
            errorInfo = "Invalid appid";
            break;
        case LOGIN_ERR_INVALID_TOKEN:
            errorInfo = "Invalid token";
            break;

        case LOGIN_ERR_TOKEN_EXPIRED:
            errorInfo = "Token expired";
            break;
        case LOGIN_ERR_NOT_AUTHORIZED:
            errorInfo = "not authorized";
            break;
        case LOGIN_ERR_ALREADY_LOGGED_IN:
            errorInfo = "already logined";
            break;
        case LOGIN_ERR_TIMEOUT:
            errorInfo = "timeout";
            break;
        case LOGIN_ERR_TOO_OFTEN:
            errorInfo = "too often";
            break;
        case LOGIN_ERR_NOT_INITIALIZED:
            errorInfo = "not initialized";
            break;
        }

        emit m_rtmServiceInst.LoginFailed(errorInfo);
    }
};
class CRtmCallEventHandler : public IRtmCallEventHandler
{
    CAgoraRtmInstance& m_rtmServiceInst;
public:
    CRtmCallEventHandler( CAgoraRtmInstance& rtmServiceInst)
        : m_rtmServiceInst(rtmServiceInst)
    {}
    ~CRtmCallEventHandler(){}

     virtual void onLocalInvitationReceivedByPeer(ILocalCallInvitation *localInvitation)
     {
         const char* response = localInvitation->getResponse();
         emit m_rtmServiceInst.onLocalInvitationReceivedByPeer((void*)localInvitation);
     }

     virtual void onLocalInvitationCanceled(ILocalCallInvitation *localInvitation)
     {
          emit m_rtmServiceInst.onLocalInvitationCanceled(localInvitation);
     }

     virtual void onLocalInvitationFailure(ILocalCallInvitation *localInvitation, LOCAL_INVITATION_ERR_CODE errorCode)
     {
         QString errorInfo;
         switch(errorCode){
         case LOCAL_INVITATION_ERR_PEER_OFFLINE:
             errorInfo = QString("%1 is offline.").arg(localInvitation->getCalleeId());
             break;
         case LOCAL_INVITATION_ERR_PEER_NO_RESPONSE:
             errorInfo = QString("%1 is online but not response.").arg(localInvitation->getCalleeId());
             break;
         case LOCAL_INVITATION_ERR_INVITATION_EXPIRE:
             errorInfo = QString("%1 doesn't accept.").arg(localInvitation->getCalleeId());
             break;
         case LOCAL_INVITATION_ERR_NOT_LOGGEDIN:
             errorInfo = QString("You hanve not logged in.");
             break;
         default:
             break;
         }
         emit m_rtmServiceInst.onLocalInvitationFailure(errorInfo, (int)errorCode);
     }

     virtual void onLocalInvitationAccepted(ILocalCallInvitation *localInvitation, const char *response)
     {
         emit m_rtmServiceInst.onLocalInvitationAccepted(localInvitation->getCalleeId(), QString::fromUtf8(response));
     }

     virtual void onLocalInvitationRefused(ILocalCallInvitation *localInvitation, const char *response)
     {
         emit m_rtmServiceInst.onLocalInvitationRefused(localInvitation);
     }

     virtual void onRemoteInvitationRefused(IRemoteCallInvitation *remoteInvitation)
     {
         emit m_rtmServiceInst.onRemoteInvitationRefused((void*)remoteInvitation);
     }

     virtual void onRemoteInvitationAccepted(IRemoteCallInvitation *remoteInvitation)
     {
         emit m_rtmServiceInst.onRemoteInvitationAccepted(remoteInvitation->getCallerId());
     }

     virtual void onRemoteInvitationReceived(IRemoteCallInvitation *remoteInvitation)
     {
         emit m_rtmServiceInst.onRemoteInvitationReceived((void*)remoteInvitation);
     }

     virtual void onRemoteInvitationFailure(IRemoteCallInvitation *remoteInvitation, REMOTE_INVITATION_ERR_CODE errorCode)
     {
         emit m_rtmServiceInst.onRemoteInvitationFailure(remoteInvitation, (int)errorCode);
     }

     virtual void onRemoteInvitationCanceled(IRemoteCallInvitation *remoteInvitation)
     {
         emit m_rtmServiceInst.onRemoteInvitationCanceled(remoteInvitation->getCallerId());
     }
};

std::mutex  CAgoraRtmInstance::m_mutex;

CAgoraRtmInstance* CAgoraRtmInstance::getInstance()
{
    std::lock_guard<std::mutex> autoLock(m_mutex);
    static CAgoraRtmInstance rmtInstance;
    return &rmtInstance;
}
CAgoraRtmInstance::CAgoraRtmInstance()
     : m_serviceEventHandler(new CAgoraRtmServiceEventHandler(*this))
     , m_callEventHandler(new CRtmCallEventHandler(*this))
     , m_callService(nullptr)
     , m_callManager(nullptr)
     , m_currentRemoteInvitation(nullptr)
     , m_callInvitation(nullptr)
     , m_busy(false)
     , m_remoteUserId(QString(""))
     , userId(QString(""))
     , m_rtcChannelId(QString(""))
{
    QString strAppId;
    QByteArray temp;
    if(strlen(APP_ID))
        strAppId = APP_ID;
    else {
        strAppId = gAgoraConfig.getAppId();
        if(strAppId.length()== 0){
            gAgoraConfig.setAppId(QString(""));
        }
    }
    if (strAppId.isEmpty())
    {
        QMessageBox::critical(nullptr, ("AgoraOpenDuo"),
                                       ("You must specify APP ID before using the demo"));
        QProcess process;
        process.startDetached("notepad.exe", {"AgoraConfigOpenDuo.ini"}, "");
        ExitProcess(0);
    }

    if(!m_callService)
        m_callService = createRtmService();

    if(m_callService){
        m_callService->initialize(strAppId.toUtf8(), m_serviceEventHandler.get());
    }
}

CAgoraRtmInstance::~CAgoraRtmInstance()
{

}

void CAgoraRtmInstance::Clear()
{
    if(m_callInvitation){
        m_callInvitation->release();
        m_callInvitation = nullptr;
    }

    if(m_currentRemoteInvitation){
        m_currentRemoteInvitation->release();
        m_currentRemoteInvitation = nullptr;
    }

    m_busy = false;
    m_rtcChannelId = "";
    m_remoteUserId = "";
}

bool CAgoraRtmInstance::Login(QString qToken, QString qUserId)
{
    if(!m_callService)
        return false;
    int ret = m_callService->login(qToken.toUtf8(), qUserId.toUtf8());
    userId = qUserId;
    return ret == 0 ? true : false;
}

bool CAgoraRtmInstance::Logout()
{
    if(!m_callService)
        return false;
     int ret = m_callService->logout();
     return ret == 0 ? true : false;
}

bool CAgoraRtmInstance::QueryPeerOnlineStatus(QString calleeId)
{
    if(!m_callService)
        return false;

    std::string peerId = calleeId.toUtf8().data();
    const char* peerIds[1] = {peerId.c_str()};
    long long requestId = 0;

    int ret = m_callService->queryPeersOnlineStatus(peerIds, 1, requestId);

    return ret == 0 ? true : false;
}

bool CAgoraRtmInstance::InitCallManager()
{
    if(!m_callService)
        return false;
    m_callManager =m_callService->getRtmCallManager(m_callEventHandler.get());

    return m_callManager != nullptr;
}

bool CAgoraRtmInstance::CallRemoteUser(QString remoteUserId)
{
    if(!m_callManager)
        return false;
    m_remoteUserId   = remoteUserId;
    m_rtcChannelId   = remoteUserId + userId;
    m_callInvitation = m_callManager->createLocalCallInvitation(remoteUserId.toUtf8());
    if(m_callInvitation){
          m_callInvitation->setContent(m_rtcChannelId.toUtf8());
        int ret = m_callManager->sendLocalInvitation(m_callInvitation);
        return ret == 0 ? true : false;
    }
    return false;
}

bool CAgoraRtmInstance::CanelLocalInvitation()
{
    if(!m_callManager || !m_callInvitation)
        return false;
    int ret = m_callManager->cancelLocalInvitation(m_callInvitation);
    return ret == 0 ? true : false;
}

bool CAgoraRtmInstance::RefuseCurrentRemoteInvitation()
{
    if(!m_callManager || !m_currentRemoteInvitation)
        return false;
    int ret = m_callManager->refuseRemoteInvitation(m_currentRemoteInvitation);
    return ret == 0 ? true : false;
}

bool CAgoraRtmInstance::RefuseRemoteInvitation(IRemoteCallInvitation* invitation)
{
    if(!m_callManager || !m_currentRemoteInvitation)
        return false;
    int ret = m_callManager->refuseRemoteInvitation(invitation);
    return ret == 0 ? true : false;
}

bool CAgoraRtmInstance::AcceptCurrentRemoteInvitation()
{
    if(!m_callManager || !m_currentRemoteInvitation)
        return false;
    int ret = m_callManager->acceptRemoteInvitation(m_currentRemoteInvitation);
    return ret == 0 ? true : false;
}

bool CAgoraRtmInstance::isRemoteCalledUser(QString peerId)
{
    if(m_currentRemoteInvitation
       && peerId.compare(m_currentRemoteInvitation->getCallerId()) == 0 )
        return true;
    return false;
}

bool CAgoraRtmInstance::IsRemoteCalledBylocal(QString peerId)
{
    return m_remoteUserId.compare(peerId) == 0;
}

void CAgoraRtmInstance::ReleaseLocalInvitation()
{
    if(m_callInvitation){
        m_callInvitation->release();
    }
}

void CAgoraRtmInstance::ReleaseRemoteInvitation()
{
    if(m_currentRemoteInvitation){
        m_currentRemoteInvitation->release();
    }
}

unsigned int CAgoraRtmInstance::getRtcUid()
{
    return strtoul(userId.toUtf8().data(), NULL, 0);
}
