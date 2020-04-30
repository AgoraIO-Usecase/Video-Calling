package io.agora.openduo.agora;

import android.content.ComponentName;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.telecom.PhoneAccount;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;

import androidx.annotation.RequiresApi;

import io.agora.openduo.Constants;
import io.agora.openduo.ui.calling.connectionservice.OpenDuoCallSession;
import io.agora.openduo.ui.calling.connectionservice.OpenDuoConnection;
import io.agora.openduo.ui.calling.connectionservice.OpenDuoConnectionService;
import io.agora.openduo.utils.SPUtils;
import io.agora.openduo.utils.UserUtil;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class Config {
    public static final boolean DEBUG = false;

    private static final boolean FLAG_USE_SYSTEM_CALL_UI = false;

    public Config(Context context) {
        initUserId(context);
        checkSystemCallSupport(context);
    }

    private void initUserId(Context context) {
        String currentId = SPUtils.getUserId(context);
        if (currentId == null) {
            currentId = UserUtil.randomUserId();
            SPUtils.saveUserId(context, currentId);
        }
        mUserId = currentId;
    }

    private void checkSystemCallSupport(Context context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            registerPhoneAccount(context);
            mUseSystemCall = true;
        }
    }

    public String getUserId() {
        return mUserId;
    }

    public VideoEncoderConfiguration.VideoDimensions getDimension() {
        return mVideoDimension;
    }

    public VideoEncoderConfiguration.FRAME_RATE getFrameRate() {
        return mFrameRate;
    }

    public VideoEncoderConfiguration.ORIENTATION_MODE getOrientation() {
        return mOrientation;
    }

    public boolean useSystemCallInterface() {
        return mUseSystemCall && FLAG_USE_SYSTEM_CALL_UI;
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    private void registerPhoneAccount(Context context) {
        PhoneAccountHandle accountHandleIn = new PhoneAccountHandle(
                new ComponentName(context, OpenDuoConnectionService.class), Constants.PA_LABEL_CALL_IN);
        PhoneAccountHandle accountHandleOut = new PhoneAccountHandle(
                new ComponentName(context, OpenDuoConnectionService.class), Constants.PA_LABEL_CALL_OUT);

        PhoneAccount.Builder paBuilder;

        paBuilder = PhoneAccount.builder(accountHandleIn, Constants.PA_LABEL_CALL_IN)
                .setCapabilities(PhoneAccount.CAPABILITY_CONNECTION_MANAGER);
        PhoneAccount phoneIn = paBuilder.build();

        paBuilder = PhoneAccount.builder(accountHandleOut, Constants.PA_LABEL_CALL_OUT)
                .setCapabilities(PhoneAccount.CAPABILITY_CONNECTION_MANAGER);

        Bundle extra = new Bundle();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            extra.putBoolean(PhoneAccount.EXTRA_LOG_SELF_MANAGED_CALLS, true);
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            paBuilder.setExtras(extra);
        }

        PhoneAccount phoneOut = paBuilder.build();

        TelecomManager telecomManager = (TelecomManager) context.getSystemService(Context.TELECOM_SERVICE);
        telecomManager.registerPhoneAccount(phoneIn);
        telecomManager.registerPhoneAccount(phoneOut);

        mCallSession = new OpenDuoCallSession();
        mCallSession.setPhoneAccountIn(phoneIn);
        mCallSession.setPhoneAccountOut(phoneOut);
    }

    public PhoneAccount getPhoneAccountIn() {
        return mCallSession == null ? null : mCallSession.getPhoneAccountIn();
    }

    public PhoneAccount getPhoneAccountOut() {
        return mCallSession == null ? null : mCallSession.getPhoneAccountOut();
    }

    public void setConnection(OpenDuoConnection connection) {
        if (mCallSession != null) {
            mCallSession.setConnection(connection);
        }
    }

    private String mUserId;

    private boolean mUseSystemCall;

    private OpenDuoCallSession mCallSession;

    private VideoEncoderConfiguration.VideoDimensions mVideoDimension =
            VideoEncoderConfiguration.VD_640x480;

    private VideoEncoderConfiguration.FRAME_RATE mFrameRate =
            VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;

    private VideoEncoderConfiguration.ORIENTATION_MODE mOrientation =
            VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT;
}
