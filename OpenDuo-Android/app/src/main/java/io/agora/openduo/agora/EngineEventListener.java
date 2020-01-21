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
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onJoinChannelSuccess(channel, uid, elapsed);
        }
    }

    @Override
    public void onUserJoined(int uid, int elapsed) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onUserJoined(uid, elapsed);
        }
    }

    @Override
    public void onUserOffline(int uid, int reason) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onUserOffline(uid, reason);
        }
    }

    @Override
    public void onConnectionStateChanged(int status, int reason) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onConnectionStateChanged(status, reason);
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
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onPeersOnlineStatusChanged(map);
        }
    }

    @Override
    public void onLocalInvitationReceivedByPeer(LocalInvitation localInvitation) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onLocalInvitationReceived(localInvitation);
        }
    }

    @Override
    public void onLocalInvitationAccepted(LocalInvitation localInvitation, String response) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onLocalInvitationAccepted(localInvitation, response);
        }
    }

    @Override
    public void onLocalInvitationRefused(LocalInvitation localInvitation, String response) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onLocalInvitationRefused(localInvitation, response);
        }
    }

    @Override
    public void onLocalInvitationCanceled(LocalInvitation localInvitation) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onLocalInvitationCanceled(localInvitation);
        }
    }

    @Override
    public void onLocalInvitationFailure(LocalInvitation localInvitation, int errorCode) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onLocalInvitationFailure(localInvitation, errorCode);
        }
    }

    @Override
    public void onRemoteInvitationReceived(RemoteInvitation remoteInvitation) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onRemoteInvitationReceived(remoteInvitation);
        }
    }

    @Override
    public void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onRemoteInvitationAccepted(remoteInvitation);
        }
    }

    @Override
    public void onRemoteInvitationRefused(RemoteInvitation remoteInvitation) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onRemoteInvitationRefused(remoteInvitation);
        }
    }

    @Override
    public void onRemoteInvitationCanceled(RemoteInvitation remoteInvitation) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onRemoteInvitationCanceled(remoteInvitation);
        }
    }

    @Override
    public void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int errorCode) {
        int size = mListeners.size();
        if (size > 0) {
            mListeners.get(size - 1).onRemoteInvitationFailure(remoteInvitation, errorCode);
        }
    }
}
