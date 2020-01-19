package io.agora.openduo.ui.calling.connectionservice;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.telecom.Connection;
import android.telecom.DisconnectCause;
import android.util.Log;

import androidx.annotation.RequiresApi;

import io.agora.openduo.Constants;
import io.agora.openduo.activities.VideoActivity;

@RequiresApi(api = Build.VERSION_CODES.M)
public class OpenDuoConnection extends Connection {
    private static final String TAG = OpenDuoConnection.class.getSimpleName();

    private Context mContext;
    private String mChannel;
    private String mUid;
    private String mSubscriber;
    private int mRole;

    public OpenDuoConnection(Context context, String uid, String channel, String subscriber, int role) {
        mContext = context;
        mChannel = channel;
        mUid = uid;
        mSubscriber = subscriber;
        mRole = role;
    }

    @Override
    public void onShowIncomingCallUi() {
        Log.d(TAG, "onShowIncomingCallUi called");
        super.onShowIncomingCallUi();
    }

    @Override
    public void onDisconnect() {
        Log.d(TAG, "onDisconnect called");
        super.onDisconnect();
        setDisconnected(new DisconnectCause(DisconnectCause.LOCAL));
        destroy();
    }

    @Override
    public void onAbort() {
        Log.d(TAG, "onAbort called");
        super.onAbort();
        setDisconnected(new DisconnectCause(DisconnectCause.CANCELED));
        destroy();
    }

    @Override
    public void onAnswer() {
        Log.d(TAG, "onAnswer called");
        super.onAnswer();

        setDisconnected(new DisconnectCause(DisconnectCause.LOCAL)); // tricky way to dismiss the system incall ui

        Intent intent = new Intent(mContext, VideoActivity.class);
        intent.putExtra(Constants.CS_KEY_UID, mUid);
        intent.putExtra(Constants.CS_KEY_CHANNEL, mChannel);
        intent.putExtra(Constants.CS_KEY_SUBSCRIBER, mSubscriber);
        intent.putExtra(Constants.CS_KEY_ROLE, mRole);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        mContext.startActivity(intent);
    }

    @Override
    public void onReject() {
        Log.d(TAG, "onReject called");
        super.onReject();

//        WorkerThread worker = ((AGApplication) mContext.getApplicationContext()).getWorkerThread();
//        EngineConfig config = worker.getEngineConfig();
//
//        // "status": 0 // Default
//        // "status": 1 // Busy
//        config.mRemoteInvitation.setResponse("{\"status\":0}");
//        worker.hangupTheCall(config.mRemoteInvitation);

        setDisconnected(new DisconnectCause(DisconnectCause.REJECTED));
        destroy();
    }
}
