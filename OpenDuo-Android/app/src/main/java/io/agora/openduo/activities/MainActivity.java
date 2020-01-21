package io.agora.openduo.activities;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatTextView;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import io.agora.openduo.R;

public class MainActivity extends BaseCallActivity {
    private static final String TAG = MainActivity.class.getSimpleName();

    // Permission request when we want to go to next activity
    // when all necessary permissions are granted.
    private static final int PERMISSION_REQ_FORWARD = 1 << 4;

    // Permission request when we want to stay in
    // current activity even if all permissions are granted.
    private static final int PERMISSION_REQ_STAY = 1 << 3;

    private String[] PERMISSIONS = {
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.CALL_PHONE,
            Manifest.permission.READ_CALL_LOG,
            Manifest.permission.WRITE_CALL_LOG
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setIdentifier();
        checkPermissions();
    }

    @Override
    protected void onGlobalLayoutCompleted() {
        // Move whole layout to the relative middle of screen
        RelativeLayout layout = findViewById(R.id.content_layout);
        int visibleHeight = displayMetrics.heightPixels - statusBarHeight;
        int residual = visibleHeight - layout.getMeasuredHeight();
        RelativeLayout.LayoutParams params =
                (RelativeLayout.LayoutParams) layout.getLayoutParams();
        params.topMargin = residual * 2 / 5;
        layout.setLayoutParams(params);

        // Logo is 0.48 times the screen width
        // ImageView logo = findViewById(R.id.main_logo);
        ImageView logo = findViewById(R.id.main_logo);
        params = (RelativeLayout.LayoutParams) logo.getLayoutParams();
        int size = (int) (displayMetrics.widthPixels * 0.48);
        params.width = size;
        params.height = size;
        logo.setLayoutParams(params);

        // set the button 2/3 of the screen width
        AppCompatTextView startCall = findViewById(R.id.start_call_button);
        params = (RelativeLayout.LayoutParams) startCall.getLayoutParams();
        params.width = displayMetrics.widthPixels * 2 / 3;
        startCall.setLayoutParams(params);
    }

    private void setIdentifier() {
        String idFormat = getResources().getString(R.string.identifier_format);
        String identifier = String.format(idFormat, config().getUserId());
        AppCompatTextView idText = findViewById(R.id.main_identifier_code_text);
        idText.setText(identifier);
    }

    private void checkPermissions() {
        if (!permissionArrayGranted(null)) {
            requestPermissions(PERMISSION_REQ_STAY);
        }
    }

    private boolean permissionArrayGranted(@Nullable String[] permissions) {
        String[] permissionArray = permissions;
        if (permissionArray == null) {
            permissionArray = PERMISSIONS;
        }

        boolean granted = true;
        for (String per : permissionArray) {
            if (!permissionGranted(per)) {
                granted = false;
                break;
            }
        }
        return granted;
    }

    private boolean permissionGranted(String permission) {
        return ContextCompat.checkSelfPermission(
                this, permission) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestPermissions(int request) {
        ActivityCompat.requestPermissions(this, PERMISSIONS, request);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSION_REQ_FORWARD ||
                requestCode == PERMISSION_REQ_STAY) {
            boolean granted = permissionArrayGranted(permissions);
            if (granted && requestCode == PERMISSION_REQ_FORWARD) {
                gotoDialerActivity();
            } else if (!granted) {
                toastNeedPermissions();
            }
        }
    }

    public void onStartCall(View view) {
        requestPermissions(PERMISSION_REQ_FORWARD);
    }

    private void toastNeedPermissions() {
        Toast.makeText(this, R.string.need_necessary_permissions, Toast.LENGTH_LONG).show();
    }

    public void gotoDialerActivity() {
        Intent intent = new Intent();
        intent.setClass(this, DialerActivity.class);
        startActivity(intent);
    }
}
