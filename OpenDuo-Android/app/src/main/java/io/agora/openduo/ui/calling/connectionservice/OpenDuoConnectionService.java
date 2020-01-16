package io.agora.openduo.ui.calling.connectionservice;

import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.telecom.Connection;
import android.telecom.ConnectionRequest;
import android.telecom.ConnectionService;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.util.Log;

import androidx.annotation.RequiresApi;

import io.agora.openduo.Constants;
import io.agora.openduo.OpenDuoApplication;

@RequiresApi(api = Build.VERSION_CODES.M)
public class OpenDuoConnectionService extends ConnectionService {
    private static final String TAG = OpenDuoConnectionService.class.getSimpleName();

    public static final String SCHEME_AG = "customized_call";

    public OpenDuoApplication application() {
        return (OpenDuoApplication) getApplication();
    }

    @Override
    public Connection onCreateIncomingConnection(PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        Log.i(TAG, "onCreateIncomingConnection: called. " + phoneAccount + " " + request);

        Bundle extras = request.getExtras();
        String uid = extras.getString(Constants.CS_KEY_UID);
        String channel = extras.getString(Constants.CS_KEY_CHANNEL);
        String subscriber = extras.getString(Constants.CS_KEY_SUBSCRIBER);
        int role = extras.getInt(Constants.CS_KEY_ROLE);
        int videoState = extras.getInt(TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE);

        OpenDuoConnection connection = new OpenDuoConnection(getApplicationContext(), uid, channel, subscriber, role);
        connection.setVideoState(videoState);
        connection.setAddress(Uri.fromParts(SCHEME_AG, subscriber, null), TelecomManager.PRESENTATION_ALLOWED);
        connection.setCallerDisplayName(subscriber, TelecomManager.PRESENTATION_ALLOWED);
        connection.setRinging();
        application().config().setConnection(connection);

        return connection;
    }

    @Override
    public Connection onCreateOutgoingConnection(PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        Log.i(TAG, "onCreateOutgoingConnection: called. " + phoneAccount + " " + request);

        Bundle extras = request.getExtras();
        String uid = extras.getString(Constants.CS_KEY_UID);
        String channel = extras.getString(Constants.CS_KEY_CHANNEL);
        String subscriber = extras.getString(Constants.CS_KEY_SUBSCRIBER);
        int role = extras.getInt(Constants.CS_KEY_ROLE);
        int videoState = extras.getInt(TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE);

        OpenDuoConnection connection = new OpenDuoConnection(getApplicationContext(), uid, channel, subscriber, role);
        connection.setVideoState(videoState);
        connection.setAddress(Uri.fromParts(SCHEME_AG, subscriber, null), TelecomManager.PRESENTATION_ALLOWED);
        connection.setCallerDisplayName(subscriber, TelecomManager.PRESENTATION_ALLOWED);
        connection.setRinging();
        application().config().setConnection(connection);

        return connection;
    }

    @Override
    public void onCreateIncomingConnectionFailed(PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        Log.e(TAG, "onCreateIncomingConnectionFailed: called. " + phoneAccount + " " + request);
    }

    @Override
    public void onCreateOutgoingConnectionFailed(PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        Log.e(TAG, "onCreateOutgoingConnectionFailed: called. " + phoneAccount + " " + request);
    }
}
