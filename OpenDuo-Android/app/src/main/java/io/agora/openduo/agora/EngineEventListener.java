package io.agora.openduo.agora;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.RtmCallEventListener;
import io.agora.rtm.RtmClientListener;
import io.agora.rtm.RtmMessage;

public class EngineEventListener extends IRtcEngineEventHandler
        implements RtmClientListener, RtmCallEventListener {
    private List<IEventListener> mListeners = new ArrayList<>();

    public void registerEventListener(IEventListener listener) {
        if (!mListeners.contains(listener)) {
            mListeners.add(listener);
        }
    }

    public void removeEventListener(IEventListener listener) {
        mListeners.remove(listener);
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        for (IEventListener listener : mListeners) {
            listener.onJoinChannelSuccess(channel, uid, elapsed);
        }
    }

    @Override
    public void onUserJoined(int uid, int elapsed) {
        for (IEventListener listener : mListeners) {
            listener.onUserJoined(uid, elapsed);
        }
    }

    @Override
    public void onUserOffline(int uid, int reason) {
        for (IEventListener listener : mListeners) {
            listener.onUserOffline(uid, reason);
        }
    }

    @Override
    public void onConnectionStateChanged(int status, int reason) {
        for (IEventListener listener : mListeners) {
            listener.onConnectionStateChanged(status, reason);
        }
    }

    @Override
    public void onMessageReceived(RtmMessage rtmMessage, String peerId) {

    }

    @Override
    public void onTokenExpired() {

    }

    @Override
    public void onPeersOnlineStatusChanged(Map<String, Integer> map) {
        for (IEventListener listener : mListeners) {
            listener.onPeersOnlineStatusChanged(map);
        }
    }

    @Override
    public void onLocalInvitationReceivedByPeer(LocalInvitation localInvitation) {
        for (IEventListener listener : mListeners) {
            listener.onLocalInvitationReceived(localInvitation);
        }
    }

    @Override
    public void onLocalInvitationAccepted(LocalInvitation localInvitation, String response) {
        for (IEventListener listener : mListeners) {
            listener.onLocalInvitationAccepted(localInvitation, response);
        }
    }

    @Override
    public void onLocalInvitationRefused(LocalInvitation localInvitation, String response) {
        for (IEventListener listener : mListeners) {
            listener.onLocalInvitationRefused(localInvitation, response);
        }
    }

    @Override
    public void onLocalInvitationCanceled(LocalInvitation localInvitation) {
        for (IEventListener listener : mListeners) {
            listener.onLocalInvitationCanceled(localInvitation);
        }
    }

    @Override
    public void onLocalInvitationFailure(LocalInvitation localInvitation, int errorCode) {
        for (IEventListener listener : mListeners) {
            listener.onLocalInvitationFailure(localInvitation, errorCode);
        }
    }

    @Override
    public void onRemoteInvitationReceived(RemoteInvitation remoteInvitation) {
        for (IEventListener listener : mListeners) {
            listener.onRemoteInvitationReceived(remoteInvitation);
        }
    }

    @Override
    public void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation) {
        for (IEventListener listener : mListeners) {
            listener.onRemoteInvitationAccepted(remoteInvitation);
        }
    }

    @Override
    public void onRemoteInvitationRefused(RemoteInvitation remoteInvitation) {
        for (IEventListener listener : mListeners) {
            listener.onRemoteInvitationRefused(remoteInvitation);
        }
    }

    @Override
    public void onRemoteInvitationCanceled(RemoteInvitation remoteInvitation) {
        for (IEventListener listener : mListeners) {
            listener.onRemoteInvitationCanceled(remoteInvitation);
        }
    }

    @Override
    public void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int errorCode) {
        for (IEventListener listener : mListeners) {
            listener.onRemoteInvitationFailure(remoteInvitation, errorCode);
        }
    }
}
