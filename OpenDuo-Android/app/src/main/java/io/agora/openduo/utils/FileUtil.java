package io.agora.openduo.utils;

import android.content.Context;
import android.os.Build;
import android.os.Environment;

import java.io.File;

public class FileUtil {
    private static final String LOG_FOLDER_NAME = "log";
    private static final String RTC_LOG_FILE_NAME = "agora-rtc.log";
    private static final String RTM_LOG_FILE_NAME = "agora-rtm.log";

    public static String rtcLogFile(Context context) {
        return logFilePath(context, RTC_LOG_FILE_NAME);
    }

    public static String rtmLogFile(Context context) {
        return logFilePath(context, RTM_LOG_FILE_NAME);
    }

    private static String logFilePath(Context context, String name) {
        File folder;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            folder = new File(context.getExternalFilesDir(Environment.DIRECTORY_DOCUMENTS), LOG_FOLDER_NAME);
        } else {
            String path = Environment.getExternalStorageDirectory()
                    .getAbsolutePath() + File.separator +
                    context.getPackageName() + File.separator +
                    name;
            folder = new File(path);
            if (!folder.exists() && !folder.mkdir()) folder = null;
        }

        if (folder != null && !folder.exists() && !folder.mkdir()) return "";
        else return new File(folder, RTC_LOG_FILE_NAME).getAbsolutePath();
    }
}
