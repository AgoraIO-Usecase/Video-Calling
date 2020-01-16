package io.agora.openduo.activities;

import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.appcompat.widget.AppCompatImageView;
import androidx.appcompat.widget.AppCompatTextView;

import io.agora.openduo.Constants;
import io.agora.openduo.R;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;

public class CallActivity extends BaseCallActivity implements View.OnClickListener {
    private static final String TAG = CallActivity.class.getSimpleName();

    private static int[] PEER_ID_RES = {
            R.id.peer_id_digit_1,
            R.id.peer_id_digit_2,
            R.id.peer_id_digit_3,
            R.id.peer_id_digit_4
    };

    private int mRole;
    private String mPeer;
    private String mChannel;
    private AppCompatImageView mAcceptBtn;
    private AppCompatImageView mHangupBtn;
    private MediaPlayer mPlayer;

    private boolean mInvitationSending;
    private boolean mInvitationReceiving;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_call);
        initUI();
        if (isCaller()) {
            sendInvitation();
            mInvitationSending = true;
        } else if (isCallee()) {
            mInvitationReceiving = true;
        }
        startRinging();
    }

    private void initUI() {
        ImageView portrait = findViewById(R.id.peer_image);
        portrait.setImageResource(R.drawable.portrait);

        AppCompatTextView[] mPeerUidText = new AppCompatTextView[PEER_ID_RES.length];
        for (int i = 0; i < mPeerUidText.length; i++) {
            mPeerUidText[i] = findViewById(PEER_ID_RES[i]);
        }

        Intent intent = getIntent();
        mChannel = intent.getStringExtra(Constants.KEY_CALLING_CHANNEL);
        mPeer = intent.getStringExtra(Constants.KEY_CALLING_PEER);
        if (mPeer != null) {
            try {
                int peer = Integer.valueOf(mPeer);
                for (int i = mPeerUidText.length - 1; i >= 0; i--) {
                    int digit = peer % 10;
                    peer /= 10;
                    mPeerUidText[i].setText(String.valueOf(digit));
                }
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        mHangupBtn = findViewById(R.id.hang_up_btn);
        mHangupBtn.setVisibility(View.VISIBLE);
        mHangupBtn.setOnClickListener(this);

        TextView roleText = findViewById(R.id.call_role);
        mAcceptBtn = findViewById(R.id.accept_call_btn);

        mRole = intent.getIntExtra(Constants.KEY_CALLING_ROLE, Constants.ROLE_CALLEE);
        if (isCallee()) {
            roleText.setText(R.string.receiving_call);
            mAcceptBtn.setVisibility(View.VISIBLE);
            mAcceptBtn.setOnClickListener(this);
        } else if (isCaller()) {
            roleText.setText(R.string.calling_out);
            mAcceptBtn.setVisibility(View.GONE);
        }
    }

    private boolean isCaller() {
        return mRole == Constants.ROLE_CALLER;
    }

    private boolean isCallee() {
        return mRole == Constants.ROLE_CALLEE;
    }

    @Override
    protected void onGlobalLayoutCompleted() {
        RelativeLayout peerLayout = findViewById(R.id.peer_image_layout);
        ImageView layer1 = findViewById(R.id.anim_layer_1);
        ImageView layer2 = findViewById(R.id.anim_layer_2);
        ImageView layer3 = findViewById(R.id.anim_layer_3);
        ImageView portrait = findViewById(R.id.peer_image);

        int width = displayMetrics.widthPixels;
        RelativeLayout.LayoutParams params =
                (RelativeLayout.LayoutParams) portrait.getLayoutParams();
        int portraitSize = width / 3;
        params.width = portraitSize;
        params.height = portraitSize;
        portrait.setLayoutParams(params);

        if (isCallee()) {
            RelativeLayout actionLayout = findViewById(R.id.action_button_layout);
            params = (RelativeLayout.LayoutParams) actionLayout.getLayoutParams();
            int buttonMargin = displayMetrics.widthPixels / 6;
            params.leftMargin = buttonMargin;
            params.rightMargin = buttonMargin;
            actionLayout.setLayoutParams(params);

            params = (RelativeLayout.LayoutParams) mAcceptBtn.getLayoutParams();
            params.addRule(RelativeLayout.ALIGN_PARENT_END, RelativeLayout.TRUE);
            mAcceptBtn.setLayoutParams(params);

            params = (RelativeLayout.LayoutParams) mHangupBtn.getLayoutParams();
            params.addRule(RelativeLayout.ALIGN_PARENT_START, RelativeLayout.TRUE);
            mHangupBtn.setLayoutParams(params);
        } else if (isCaller()) {
            params = (RelativeLayout.LayoutParams) mHangupBtn.getLayoutParams();
            params.addRule(RelativeLayout.CENTER_HORIZONTAL, RelativeLayout.TRUE);
            mHangupBtn.setLayoutParams(params);
        }

        RelativeLayout content = findViewById(R.id.content_layout);
        params = (RelativeLayout.LayoutParams) content.getLayoutParams();
        int marginTop = (displayMetrics.heightPixels -
                statusBarHeight - content.getHeight()) / 2;
        params.topMargin = marginTop + statusBarHeight;
        content.setLayoutParams(params);
    }

    private void sendInvitation() {
        inviteCall(mPeer, mChannel);
    }

    private void startRinging() {
        if (isCallee()) {
            mPlayer = playCalleeRing();
        } else if (isCaller()) {
            mPlayer = playCallerRing();
        }
    }

    private MediaPlayer playCallerRing() {
        return startRinging(R.raw.basic_ring);
    }

    private MediaPlayer playCalleeRing() {
        return startRinging(R.raw.basic_tones);
    }

    private MediaPlayer startRinging(int resource) {
        MediaPlayer player = MediaPlayer.create(this, resource);
        player.setLooping(true);
        player.start();
        return player;
    }

    private void stopRinging() {
        if (mPlayer != null && mPlayer.isPlaying()) {
            mPlayer.stop();
            mPlayer.release();
            mPlayer = null;
        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.accept_call_btn:
                answerCall(global().getRemoteInvitation());
                break;
            case R.id.hang_up_btn:
                if (isCaller()) {
                    cancelLocalInvitation();
                } else if (isCallee()) {
                    refuseRemoteInvitation(global().getRemoteInvitation());
                }
                break;
        }
    }

    @Override
    public void onLocalInvitationReceived(LocalInvitation localInvitation) {
        super.onLocalInvitationReceived(localInvitation);
    }

    @Override
    public void onLocalInvitationAccepted(LocalInvitation localInvitation, String response) {
        super.onLocalInvitationAccepted(localInvitation, response);
        mInvitationSending = false;
        stopRinging();
    }

    @Override
    public void onLocalInvitationCanceled(LocalInvitation localInvitation) {
        Log.i(TAG, "onLocalInvitationCanceled:" + localInvitation.getContent());
        mInvitationSending = false;
        finish();
    }

    @Override
    public void onLocalInvitationRefused(LocalInvitation localInvitation, String response) {
        Log.i(TAG, "onLocalInvitationRefused:" + response);
        mInvitationSending = false;
        finish();
    }

    @Override
    public void onLocalInvitationFailure(LocalInvitation localInvitation, int errorCode) {
        super.onLocalInvitationFailure(localInvitation, errorCode);
        mInvitationSending = false;
        stopRinging();
    }

    @Override
    public void onRemoteInvitationReceived(RemoteInvitation remoteInvitation) {
        // Do not respond to any other calls
        Log.i(TAG, "Ignore remote invitation from " +
                remoteInvitation.getCallerId() + " while in calling");
    }

    @Override
    public void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation) {
        super.onRemoteInvitationAccepted(remoteInvitation);
        mInvitationReceiving = false;
        stopRinging();
    }

    @Override
    public void onRemoteInvitationRefused(RemoteInvitation remoteInvitation) {
        Log.i(TAG, "onRemoteInvitationRefused:" + remoteInvitation.getContent());
        mInvitationReceiving = false;
        finish();
    }

    @Override
    public void onRemoteInvitationCanceled(RemoteInvitation remoteInvitation) {
        Log.i(TAG, "onRemoteInvitationCanceled:" + remoteInvitation.getContent());
        mInvitationReceiving = false;
        finish();
    }

    @Override
    public void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int errorCode) {
        super.onRemoteInvitationFailure(remoteInvitation, errorCode);
        mInvitationReceiving = false;
        stopRinging();
    }

    @Override
    public void finish() {
        stopRinging();

        if (isCallee() && mInvitationReceiving &&
            global().getRemoteInvitation() != null) {
            refuseRemoteInvitation(global().getRemoteInvitation());
        } else if (isCaller() && mInvitationSending &&
            global().getLocalInvitation() != null) {
            cancelLocalInvitation();
        }

        super.finish();
    }
}
