package io.agora.openduo.activities;

import android.os.Bundle;
import android.widget.RelativeLayout;
import android.widget.TextView;

import io.agora.openduo.R;
import io.agora.openduo.ui.calling.conventional.DialerLayout;
import io.agora.rtm.RtmClient;

public class DialerActivity extends BaseCallActivity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dialer);
        initUI();
    }

    private void initUI() {
        TextView identifier = findViewById(R.id.identifier_text);
        identifier.setText(String.format(getResources().
                getString(R.string.identifier_format), config().getUserId()));
    }

    @Override
    protected void onGlobalLayoutCompleted() {
        DialerLayout dialerLayout = findViewById(R.id.dialer_layout);
        dialerLayout.setActivity(this);
        dialerLayout.adjustScreenWidth(displayMetrics.widthPixels);

        int dialerHeight = dialerLayout.getHeight();
        int dialerRemainHeight = displayMetrics.heightPixels - statusBarHeight - dialerHeight;
        RelativeLayout.LayoutParams params =
                (RelativeLayout.LayoutParams) dialerLayout.getLayoutParams();
        params.topMargin = dialerRemainHeight / 4 + statusBarHeight;
        dialerLayout.setLayoutParams(params);
    }

    @Override
    public RtmClient rtmClient() {
        return application().rtmClient();
    }
}
