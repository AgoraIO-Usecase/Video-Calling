package io.agora.openduo.ui.calling.connectionservice;

import android.telecom.PhoneAccount;

public class OpenDuoCallSession {
    private PhoneAccount mPhoneAccountOut;
    private PhoneAccount mPhoneAccountIn;
    private OpenDuoConnection mConnection;

    public void setPhoneAccountIn(PhoneAccount pa) {
        mPhoneAccountIn = pa;
    }

    public void setPhoneAccountOut(PhoneAccount pa) {
        mPhoneAccountOut = pa;
    }

    public PhoneAccount getPhoneAccountIn() {
        return mPhoneAccountIn;
    }

    public PhoneAccount getPhoneAccountOut() {
        return mPhoneAccountOut;
    }

    public void setConnection(OpenDuoConnection connection) {
        mConnection = connection;
    }

    public OpenDuoConnection getConnection() {
        return mConnection;
    }

}
