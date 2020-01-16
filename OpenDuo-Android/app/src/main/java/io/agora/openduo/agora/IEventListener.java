package io.agora.openduo.agora;

import java.util.Map;

import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;

public interface IEventListener {
    void onJoinChannelSuccess(String channel, int uid, int elapsed);

    void onUserJoined(int uid, int elapsed);

    void onUserOffline(int uid, int reason);

    void onConnectionStateChanged(int status, int reason);

    void onPeersOnlineStatusChanged(Map<String, Integer> map);
    
    void onLocalInvitationReceived(LocalInvitation localInvitation);
    
    void onLocalInvitationAccepted(LocalInvitation localInvitation, String response);
    
    void onLocalInvitationRefused(LocalInvitation localInvitation, String response);
    
    void onLocalInvitationCanceled(LocalInvitation localInvitation);
    
    void onLocalInvitationFailure(LocalInvitation localInvitation, int errorCode);
    
    void onRemoteInvitationReceived(RemoteInvitation remoteInvitation);
    
    void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation);
    
    void onRemoteInvitationRefused(RemoteInvitation remoteInvitation) ;
    
    void onRemoteInvitationCanceled(RemoteInvitation remoteInvitation);
    
    void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int errorCode);
}
