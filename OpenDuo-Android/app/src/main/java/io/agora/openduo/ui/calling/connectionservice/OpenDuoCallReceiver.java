package io.agora.openduo.ui.calling.connectionservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.provider.CallLog;
import android.util.Log;

import io.agora.openduo.Constants;
import io.agora.openduo.activities.DialerActivity;
import io.agora.openduo.activities.MainActivity;

public class OpenDuoCallReceiver extends BroadcastReceiver {
    private static final String TAG = OpenDuoCallReceiver.class.getSimpleName();

    @Override
    public void onReceive(Context context, Intent intent) {
        String subscriber = intent.getStringExtra(Intent.EXTRA_PHONE_NUMBER);
        Log.d(TAG, "PROCESS_OUT_GOING_CALL received, Phone number: " + subscriber);


        if (shouldIntercept(context, subscriber)) {
            if (false) { // DialerActivity.running == true
                Intent agIntent = new Intent(context, DialerActivity.class);
                agIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP);
                agIntent.putExtra(Constants.CS_KEY_SUBSCRIBER, subscriber);
                context.startActivity(agIntent);
            } else {
                Intent agIntent = new Intent(context, MainActivity.class);
                agIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP);
                context.startActivity(agIntent);
            }
            setResultData(null);
        }
    }

    private boolean shouldIntercept(Context context, String phoneNumber) {
        try {
            Cursor cursor = context.getContentResolver().query(CallLog.Calls.CONTENT_URI,
                    null, CallLog.Calls.NUMBER + "=?", new String[]{phoneNumber},
                    CallLog.Calls.DATE + " DESC");
            int phoneAccountID = cursor.getColumnIndex(CallLog.Calls.PHONE_ACCOUNT_ID);

            boolean shouldIntercept = false;
            while (cursor.moveToNext()) {
                String phoneAccID = cursor.getString(phoneAccountID);
                Log.d(TAG, "phone number: " + phoneNumber + " phoneAccountID: " + phoneAccID);
                shouldIntercept = phoneAccID.equals(Constants.PA_LABEL_CALL_IN) ||
                        phoneAccID.equals(Constants.PA_LABEL_CALL_OUT);
                if (shouldIntercept) break;
            }
            cursor.close();
            return shouldIntercept;
        } catch (SecurityException e) {
            e.printStackTrace();
            return false;
        }
    }
}
