package io.agora.openduo.utils;

import java.util.Random;

public class UserUtil {
    private static final int MAX_ID_NUMBER = 9999;

    public static String randomUserId() {
        int number = new Random().nextInt(MAX_ID_NUMBER);
        String id = "";
        if (number < 1000) {
            id += "0";
        }

        if (number < 100) {
            id += "0";
        }

        if (number < 10) {
            id += "0";
        }

        return id + number;
    }
}
