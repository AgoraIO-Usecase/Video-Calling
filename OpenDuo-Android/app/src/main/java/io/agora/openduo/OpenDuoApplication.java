package io.agora.openduo;

import android.app.Application;
import android.text.TextUtils;
import android.util.Log;

import io.agora.openduo.agora.EngineEventListener;
import io.agora.openduo.agora.Config;
import io.agora.openduo.agora.Global;
import io.agora.openduo.agora.IEventListener;
import io.agora.openduo.utils.FileUtil;
import io.agora.rtc.Constants;
import io.agora.rtc.RtcEngine;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.ResultCallback;
import io.agora.rtm.RtmCallManager;
import io.agora.rtm.RtmClient;

public class OpenDuoApplication extends Application {
    private static final String TAG = OpenDuoApplication.class.getSimpleName();

    private RtcEngine mRtcEngine;
    private RtmClient mRtmClient;
    private RtmCallManager rtmCallManager;
    private EngineEventListener mEventListener;
    private Config mConfig;
    private Global mGlobal;

    @Override
    public void onCreate() {
        super.onCreate();
        init();
    }

    private void init() {
        initConfig();
        initEngine();
    }

    private void initConfig() {
        mConfig = new Config(getApplicationContext());
        mGlobal = new Global();
    }

    private void initEngine() {
        String appId = getString(R.string.private_app_id);
        if (TextUtils.isEmpty(appId)) {
            throw new RuntimeException("NEED TO use your App ID, get your own ID at https://dashboard.agora.io/");
        }

        mEventListener = new EngineEventListener();
        try {
            mRtcEngine = RtcEngine.create(getApplicationContext(), appId, mEventListener);
            mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
            mRtcEngine.enableDualStreamMode(true);
            mRtcEngine.enableVideo();
            mRtcEngine.setLogFile(FileUtil.rtmLogFile(getApplicationContext()));

            mRtmClient = RtmClient.createInstance(getApplicationContext(), appId, mEventListener);
            mRtmClient.setLogFile(FileUtil.rtmLogFile(getApplicationContext()));

            if (Config.DEBUG) {
                mRtcEngine.setParameters("{\"rtc.log_filter\":65535}");
                mRtmClient.setParameters("{\"rtm.log_filter\":65535}");
            }

            rtmCallManager = mRtmClient.getRtmCallManager();
            rtmCallManager.setEventListener(mEventListener);

            // By default do not use rtm token
            mRtmClient.login(null, String.valueOf(mConfig.getUserId()), new ResultCallback<Void>() {
                @Override
                public void onSuccess(Void aVoid) {
                    Log.i(TAG, "rtm client login success");
                }

                @Override
                public void onFailure(ErrorInfo errorInfo) {
                    Log.i(TAG, "rtm client login failed:" + errorInfo.getErrorDescription());
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public RtcEngine rtcEngine() {
        return mRtcEngine;
    }

    public RtmClient rtmClient() {
        return mRtmClient;
    }

    public void registerEventListener(IEventListener listener) {
        mEventListener.registerEventListener(listener);
    }

    public void removeEventListener(IEventListener listener) {
        mEventListener.removeEventListener(listener);
    }

    public RtmCallManager rtmCallManager() {
        return rtmCallManager;
    }

    public Config config() {
        return mConfig;
    }

    public Global global() {
        return mGlobal;
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        destroyEngine();
    }

    private void destroyEngine() {
        RtcEngine.destroy();

        mRtmClient.logout(new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {
                Log.i(TAG, "rtm client logout success");
            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
                Log.i(TAG, "rtm client logout failed:" + errorInfo.getErrorDescription());
            }
        });
    }
}
