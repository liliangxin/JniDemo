package com.gago.jnidemo.test;

/**
 * Created by llx on 2019/7/10
 */
public class JniUtil {

    static {
        System.loadLibrary("native-lib");
    }

//    public static String getWeiXinKey(){
//        return weiXinKeyFromJNI();
//    }
//
//    public static String weiXinSecret(){
//        return weiXinSecretFromJNI();
//    }
    public static String weiXinSecret(){
        return testStringFromJNI();
    }

//    public static native String weiXinKeyFromJNI();
//    public static native String weiXinSecretFromJNI();
    public static native String testStringFromJNI();

}
