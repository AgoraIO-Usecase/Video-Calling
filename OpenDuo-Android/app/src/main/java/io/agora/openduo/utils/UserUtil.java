package io.agora.openduo.utils;

import java.util.Random;

public class UserUtil {
    private static final int MAX_ID_NUMBER = 9999;

    public static int randomUserId() {
        return new Random().nextInt(MAX_ID_NUMBER);
    }
}
