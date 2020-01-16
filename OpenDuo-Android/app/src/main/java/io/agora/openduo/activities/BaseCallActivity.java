package io.agora.openduo.activities;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.telecom.PhoneAccount;
import android.telecom.TelecomManager;
import android.telecom.VideoProfile;
import android.util.Log;

import androidx.annotation.NonNull;

import java.util.List;

import io.agora.openduo.Constants;
import io.agora.openduo.ui.calling.connectionservice.OpenDuoConnectionService;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.ResultCallback;
import io.agora.rtm.RtmCallManager;
import io.agora.rtm.RtmChannelAttribute;
import io.agora.rtm.RtmChannelListener;
import io.agora.rtm.RtmChannelMember;
import io.agora.rtm.RtmMessage;

public abstract class BaseCallActivity extends BaseRtcActivity implements RtmChannelListener, ResultCallback<Void> {
    private static final String TAG = BaseCallActivity.class.getSimpleName();

    protected RtmCallManager mRtmCallManager;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mRtmCallManager = rtmCallManager();
    }

    public void gotoCallingInterface(String peerUid, String channel, int role) {
        if (config().useSystemCallInterface()) {
            // Not supported yet.
            // placeSystemCall(String.valueOf(config().getUserId()), peerUid, channel);
        } else {
            gotoCallingActivity(channel, peerUid, role);
        }
    }

    private void placeSystemCall(String myUid, String peerUid, String channel) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            Bundle extras = new Bundle();
            extras.putInt(TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE, VideoProfile.STATE_BIDIRECTIONAL);

            Bundle extraBundle = new Bundle();
            extraBundle.putString(Constants.CS_KEY_UID, myUid);
            extraBundle.putString(Constants.CS_KEY_SUBSCRIBER, peerUid);
            extraBundle.putString(Constants.CS_KEY_CHANNEL, channel);
            extraBundle.putInt(Constants.CS_KEY_ROLE, Constants.CALL_ID_OUT);
            extras.putBundle(TelecomManager.EXTRA_OUTGOING_CALL_EXTRAS, extraBundle);

            try {
                TelecomManager telecomManager = (TelecomManager)
                        getApplicationContext().getSystemService(Context.TELECOM_SERVICE);
                PhoneAccount pa = telecomManager.getPhoneAccount(
                        config().getPhoneAccountOut().getAccountHandle());
                extras.putParcelable(TelecomManager.EXTRA_PHONE_ACCOUNT_HANDLE, pa.getAccountHandle());
                telecomManager.placeCall(Uri.fromParts(
                        OpenDuoConnectionService.SCHEME_AG, peerUid, null), extras);
            } catch (SecurityException e) {
                e.printStackTrace();
            }
        }
    }

    protected void gotoCallingActivity(String channel, String peer, int role) {
        Intent intent = new Intent(this, CallActivity.class);
        intent.putExtra(Constants.KEY_CALLING_CHANNEL, channel);
        intent.putExtra(Constants.KEY_CALLING_PEER, peer);
        intent.putExtra(Constants.KEY_CALLING_ROLE, role);
        startActivity(intent);
    }

    protected void inviteCall(final String peerUid, final String channel) {
        LocalInvitation invitation = mRtmCallManager.createLocalInvitation(peerUid);
        invitation.setContent(channel);
        mRtmCallManager.sendLocalInvitation(invitation, this);
        global().setLocalInvitation(invitation);
    }

    protected void answerCall(final RemoteInvitation invitation) {
        if (mRtmCallManager != null && invitation != null) {
            mRtmCallManager.acceptRemoteInvitation(invitation, this);
        }
    }

    protected void cancelLocalInvitation() {
        if (mRtmCallManager != null && global().getLocalInvitation() != null) {
            mRtmCallManager.cancelLocalInvitation(global().getLocalInvitation(), this);
        }
    }

    protected void refuseRemoteInvitation(@NonNull RemoteInvitation invitation) {
        if (mRtmCallManager != null) {
            mRtmCallManager.refuseRemoteInvitation(invitation, this);
        }
    }

    @Override
    public void onMemberCountUpdated(int count) {

    }

    @Override
    public void onAttributesUpdated(List<RtmChannelAttribute> list) {

    }

    @Override
    public void onMessageReceived(RtmMessage rtmMessage, RtmChannelMember rtmChannelMember) {

    }

    @Override
    public void onMemberJoined(RtmChannelMember rtmChannelMember) {

    }

    @Override
    public void onMemberLeft(RtmChannelMember rtmChannelMember) {

    }

    @Override
    public void onSuccess(Void aVoid) {

    }

    @Override
    public void onFailure(ErrorInfo errorInfo) {

    }

    @Override
    public void onLocalInvitationReceived(LocalInvitation localInvitation) {
        super.onLocalInvitationReceived(localInvitation);
    }

    @Override
    public void onLocalInvitationAccepted(LocalInvitation localInvitation, String response) {
        if (mActivityVisible) {
            gotoVideoActivity(localInvitation.getContent(), localInvitation.getCalleeId());
        }
    }

    @Override
    public void onLocalInvitationRefused(LocalInvitation localInvitation, String response) {
        super.onLocalInvitationRefused(localInvitation, response);
    }

    @Override
    public void onLocalInvitationCanceled(LocalInvitation localInvitation) {
        super.onLocalInvitationCanceled(localInvitation);
    }

    @Override
    public void onLocalInvitationFailure(LocalInvitation localInvitation, int errorCode) {
        super.onLocalInvitationFailure(localInvitation, errorCode);
        Log.w(TAG, "onLocalInvitationFailure:" + errorCode);
    }

    @Override
    public void onRemoteInvitationReceived(RemoteInvitation remoteInvitation) {
        if (mActivityVisible) {
            global().setRemoteInvitation(remoteInvitation);
            gotoCallingActivity(remoteInvitation.getContent(), remoteInvitation.getCallerId(), Constants.ROLE_CALLEE);
        }
    }

    @Override
    public void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation) {
        if (mActivityVisible) {
            gotoVideoActivity(remoteInvitation.getContent(), remoteInvitation.getCallerId());
        }
    }

    @Override
    public void onRemoteInvitationRefused(RemoteInvitation remoteInvitation) {
        super.onRemoteInvitationRefused(remoteInvitation);
    }

    @Override
    public void onRemoteInvitationCanceled(RemoteInvitation remoteInvitation) {
        super.onRemoteInvitationCanceled(remoteInvitation);
    }

    @Override
    public void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int errorCode) {
        super.onRemoteInvitationFailure(remoteInvitation, errorCode);
        Log.w(TAG, "onRemoteInvitationFailure:" + errorCode);
    }

    public void gotoVideoActivity(String channel, String peer) {
        Intent intent = new Intent(this, VideoActivity.class);
        intent.putExtra(Constants.KEY_CALLING_CHANNEL, channel);
        intent.putExtra(Constants.KEY_CALLING_PEER, peer);
        startActivity(intent);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
