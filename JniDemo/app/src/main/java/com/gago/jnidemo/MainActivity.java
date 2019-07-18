package com.gago.jnidemo;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.gago.jnidemo.test.JniUtil;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        TextView tv2 = (TextView) findViewById(R.id.sample_key);
        TextView tv3 = (TextView) findViewById(R.id.sample_secret);
        tv.setText(JniUtil.testStringFromJNI());
        tv2.setText(weiXinKeyFromJNI());
        tv3.setText(weiXinSecretFromJNI());

    }

    public String getSignMd5Str(Context context) {
        try {
            @SuppressLint("PackageManagerGetSignatures")
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_SIGNATURES);
            Signature[] signs = packageInfo.signatures;
            Signature sign = signs[0];
            return encryptionMD5(sign.toByteArray());
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return "";
    }

    public String encryptionMD5(byte[] byteStr) {
        MessageDigest messageDigest = null;
        StringBuilder md5StrBuff = new StringBuilder();
        try {
            messageDigest = MessageDigest.getInstance("MD5");
            messageDigest.reset();
            messageDigest.update(byteStr);
            byte[] byteArray = messageDigest.digest();
            for (byte aByteArray : byteArray) {
                if (Integer.toHexString(0xFF & aByteArray).length() == 1) {
                    md5StrBuff.append("0").append(Integer.toHexString(0xFF & aByteArray));
                } else {
                    md5StrBuff.append(Integer.toHexString(0xFF & aByteArray));
                }
            }
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return md5StrBuff.toString().toUpperCase();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native String weiXinKeyFromJNI();
    public native String weiXinSecretFromJNI();
}
