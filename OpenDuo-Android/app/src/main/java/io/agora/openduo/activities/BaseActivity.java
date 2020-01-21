package io.agora.openduo.activities;

import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.Window;

import androidx.appcompat.app.AppCompatActivity;

import java.util.Map;

import io.agora.openduo.OpenDuoApplication;
import io.agora.openduo.agora.Config;
import io.agora.openduo.agora.Global;
import io.agora.openduo.agora.IEventListener;
import io.agora.openduo.utils.WindowUtil;
import io.agora.rtc.RtcEngine;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.RtmCallManager;
import io.agora.rtm.RtmClient;

public abstract class BaseActivity extends AppCompatActivity implements IEventListener {
    private static final String TAG = BaseActivity.class.getSimpleName();

    protected int statusBarHeight;
    protected DisplayMetrics displayMetrics = new DisplayMetrics();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        WindowUtil.hideWindowStatusBar(getWindow());
        setGlobalLayoutListener();
        getDisplayMetrics();
        initStatusBarHeight();
    }

    private void setGlobalLayoutListener() {
        final View layout = findViewById(Window.ID_ANDROID_CONTENT);
        ViewTreeObserver observer = layout.getViewTreeObserver();
        observer.addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                layout.getViewTreeObserver().removeOnGlobalLayoutListener(this);
                onGlobalLayoutCompleted();
            }
        });
    }

    /**
     * Give a chance to obtain view layout attributes when the
     * content view layout process is completed.
     * Some layout attributes will be available here but not
     * in onCreate(), like measured width/height.
     * This callback will be called ONLY ONCE before the whole
     * window content is ready to be displayed for first time.
     */
    protected void onGlobalLayoutCompleted() {

    }

    private void getDisplayMetrics() {
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
    }

    private void initStatusBarHeight() {
        statusBarHeight = WindowUtil.getSystemStatusBarHeight(this);
    }

    @Override
    public void onStart() {
        super.onStart();
        registerEventListener(this);
    }

    @Override
    public void onStop() {
        super.onStop();
        removeEventListener(this);
    }

    public OpenDuoApplication application() {
        return (OpenDuoApplication) getApplication();
    }

    protected RtcEngine rtcEngine() {
        return application().rtcEngine();
    }

    protected RtmClient rtmClient() {
        return application().rtmClient();
    }

    protected RtmCallManager rtmCallManager() {
        return application().rtmCallManager();
    }

    protected Config config() {
        return application().config();
    }

    protected Global global() {
        return application().global();
    }

    private void registerEventListener(IEventListener listener) {
        application().registerEventListener(listener);
    }

    private void removeEventListener(IEventListener listener) {
        application().removeEventListener(listener);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        removeEventListener(this);
    }

    @Override
    public void onConnectionStateChanged(int status, int reason) {
        Log.i(TAG, "onConnectionStateChanged status:" + status + " reason:" + reason);
    }

    @Override
    public void onPeersOnlineStatusChanged(Map<String, Integer> map) {

    }

    @Override
    public void onLocalInvitationReceived(LocalInvitation localInvitation) {

    }

    @Override
    public void onLocalInvitationAccepted(LocalInvitation localInvitation, String response) {

    }

    @Override
    public void onLocalInvitationRefused(LocalInvitation localInvitation, String response) {

    }

    @Override
    public void onLocalInvitationCanceled(LocalInvitation localInvitation) {

    }

    @Override
    public void onLocalInvitationFailure(LocalInvitation localInvitation, int errorCode) {

    }

    @Override
    public void onRemoteInvitationReceived(RemoteInvitation remoteInvitation) {

    }

    @Override
    public void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation) {

    }

    @Override
    public void onRemoteInvitationRefused(RemoteInvitation remoteInvitation) {

    }

    @Override
    public void onRemoteInvitationCanceled(RemoteInvitation remoteInvitation) {

    }

    @Override
    public void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int errorCode) {

    }
}
