package io.agora.openduo.ui.calling.conventional;

import android.content.Context;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Toast;

import androidx.appcompat.widget.AppCompatTextView;

import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import io.agora.openduo.Constants;
import io.agora.openduo.R;
import io.agora.openduo.activities.DialerActivity;
import io.agora.openduo.utils.RtcUtils;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.ResultCallback;

public class DialerLayout extends RelativeLayout implements View.OnClickListener {
    private class CallInputManager {
        private static final int MAX_COUNT = 4;
        private int mCallNumber;
        private int mCount;

        void append(String digit) {
            if (mCount == MAX_COUNT) return;

            if (TextUtils.isDigitsOnly(digit)) {
                mCount++;
                mCallNumber = mCallNumber * 10 + digitToInt(digit);
                mCallNumberSlots[mCount - 1].setText(digit);
            }
        }

        void backspace() {
            if (mCount == 0) return;
            mCount--;
            mCallNumber /= 10;
            mCallNumberSlots[mCount].setText("");
        }

        private int digitToInt(String digit) {
            return Integer.valueOf(digit);
        }

        boolean isValid() {
            return mCount == MAX_COUNT;
        }

        int getCallNumber() {
            return mCallNumber;
        }
    }

    private static int[] CALL_NUMBER_SLOT_RES = {
            R.id.call_number_text1,
            R.id.call_number_text2,
            R.id.call_number_text3,
            R.id.call_number_text4
    };

    private static int[] DIAL_BUTTON_RES = {
            R.id.dial_number_0,
            R.id.dial_number_1,
            R.id.dial_number_2,
            R.id.dial_number_3,
            R.id.dial_number_4,
            R.id.dial_number_5,
            R.id.dial_number_6,
            R.id.dial_number_7,
            R.id.dial_number_8,
            R.id.dial_number_9,
            R.id.dialer_start_call,
            R.id.dialer_backspace
    };

    private DialerActivity mActivity;
    private CallInputManager mCallInputManager;
    private AppCompatTextView[] mCallNumberSlots;

    public DialerLayout(Context context) {
        super(context);
        init();
    }

    public DialerLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public DialerLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        View view = LayoutInflater.from(getContext()).inflate(
                R.layout.dialer_layout, this, false);
        addView(view);

        mCallInputManager = new CallInputManager();
        initDialer();
    }

    public void adjustScreenWidth(int width) {
        LinearLayout callNumberLayout = findViewById(R.id.call_number_layout);
        RelativeLayout.LayoutParams params =
                (RelativeLayout.LayoutParams) callNumberLayout.getLayoutParams();
        int margin = width / 10;
        params.leftMargin = margin;
        params.rightMargin = margin;
        callNumberLayout.setLayoutParams(params);

        LinearLayout dialNumberLayout = findViewById(R.id.dial_number_layout);
        params = (RelativeLayout.LayoutParams) dialNumberLayout.getLayoutParams();
        params.leftMargin = margin;
        params.rightMargin = margin;
        dialNumberLayout.setLayoutParams(params);
    }

    private void initDialer() {
        mCallNumberSlots = new AppCompatTextView[CALL_NUMBER_SLOT_RES.length];
        for (int i = 0; i < mCallNumberSlots.length; i++) {
            mCallNumberSlots[i] = findViewById(CALL_NUMBER_SLOT_RES[i]);
        }

        for (int id : DIAL_BUTTON_RES) {
            findViewById(id).setOnClickListener(this);
        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.dial_number_0:
            case R.id.dial_number_1:
            case R.id.dial_number_2:
            case R.id.dial_number_3:
            case R.id.dial_number_4:
            case R.id.dial_number_5:
            case R.id.dial_number_6:
            case R.id.dial_number_7:
            case R.id.dial_number_8:
            case R.id.dial_number_9:
                String digit = (String) view.getTag();
                mCallInputManager.append(digit);
                break;

            case R.id.dialer_start_call:
                startCall();
                break;
            case R.id.dialer_backspace:
                mCallInputManager.backspace();
                break;
        }
    }

    public void setActivity(DialerActivity activity) {
        mActivity = activity;
    }

    private void startCall() {
        if (!mCallInputManager.isValid()) {
            Toast.makeText(getContext(),
                    R.string.incomplete_dial_number,
                    Toast.LENGTH_SHORT).show();
        }

        if (mActivity != null) {
            int number = mCallInputManager.getCallNumber();
            final String peer = String.valueOf(number);
            Set<String> peerSet = new HashSet<>();
            peerSet.add(peer);

            mActivity.rtmClient().queryPeersOnlineStatus(peerSet,
                    new ResultCallback<Map<String, Boolean>>() {
                @Override
                public void onSuccess(Map<String, Boolean> statusMap) {
                    Boolean bOnline = statusMap.get(peer);
                    if (bOnline != null && bOnline) {
                        String uid = String.valueOf(mActivity.
                                application().config().getUserId());
                        String channel = RtcUtils.channelName(uid, peer);
                        mActivity.gotoCallingInterface(peer, channel, Constants.ROLE_CALLER);
                    } else {
                        mActivity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(mActivity,
                                        R.string.peer_not_online,
                                        Toast.LENGTH_SHORT).show();
                            }
                        });
                    }
                }

                @Override
                public void onFailure(ErrorInfo errorInfo) {

                }
            });
        }
    }
}
