#include <jni.h>
#include <string>
#include <string.h>
#include <malloc.h>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gago_jnidemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gago_jnidemo_test_JniUtil_testStringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++ 测试";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
const char *WEI_XIN_KEY = "wei——xin——key";
const char *PACKAGE_NAME = "com.gago.jnidemo";
const char *WEI_XIN_SECRET = "wei——xin——secret";
const char *RELEASE_SIGN_MD5 = "release_sign_md5";
const char *TIP_STRING = "破解别人应用好玩么？";

/**
  * getApplication
  */
static jobject getApplication(JNIEnv* env){
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(
                activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        }
        env->DeleteLocalRef(activity_thread_clz);
    }
    return application;
}

/**
 * HexToString
 * @param source
 * @param dest
 * @param sourceLen
 */
static void ToHexStr(const char *source, char *dest, int sourceLen) {
    short i;
    char highByte, lowByte;

    for (i = 0; i < sourceLen; i++) {
        highByte = source[i] >> 4;
        lowByte = (char) (source[i] & 0x0f);
        highByte += 0x30;

        if (highByte > 0x39) {
            dest[i * 2] = (char) (highByte + 0x07);
        } else {
            dest[i * 2] = highByte;
        }

        lowByte += 0x30;
        if (lowByte > 0x39) {
            dest[i * 2 + 1] = (char) (lowByte + 0x07);
        } else {
            dest[i * 2 + 1] = lowByte;
        }
    }
}

/**
 *
 * byteArrayToMd5
 * @param env
 * @param source
 * @return j_string
 */
static jstring ToMd5(JNIEnv *env, jbyteArray source) {
    // MessageDigest
    jclass classMessageDigest = env->FindClass("java/security/MessageDigest");
    // MessageDigest.getInstance()
    jmethodID midGetInstance = env->GetStaticMethodID(classMessageDigest, "getInstance",
                                                      "(Ljava/lang/String;)Ljava/security/MessageDigest;");
    // MessageDigest object
    jobject objMessageDigest = env->CallStaticObjectMethod(classMessageDigest, midGetInstance,
                                                           env->NewStringUTF("md5"));

    jmethodID midUpdate = env->GetMethodID(classMessageDigest, "update", "([B)V");
    env->CallVoidMethod(objMessageDigest, midUpdate, source);

    // Digest
    jmethodID midDigest = env->GetMethodID(classMessageDigest, "digest", "()[B");
    jbyteArray objArraySign = (jbyteArray) env->CallObjectMethod(objMessageDigest, midDigest);

    jsize intArrayLength = env->GetArrayLength(objArraySign);
    jbyte *byte_array_elements = env->GetByteArrayElements(objArraySign, NULL);
    size_t length = (size_t) intArrayLength * 2 + 1;
    char *char_result = (char *) malloc(length);
    memset(char_result, 0, length);

    ToHexStr((const char *) byte_array_elements, char_result, intArrayLength);
    // 在末尾补\0
    *(char_result + intArrayLength * 2) = '\0';

    jstring stringResult = env->NewStringUTF(char_result);
    // release
    env->ReleaseByteArrayElements(objArraySign, byte_array_elements, JNI_ABORT);
    // 指针
    free(char_result);

    return stringResult;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gago_jnidemo_MainActivity_weiXinSecretFromJNI(JNIEnv* env,jobject /* this */) {

    jobject context = getApplication(env);
    // 获得Context类
    jclass cls = env->GetObjectClass(context);
    // 得到getPackageManager方法的ID
    jmethodID mid = env->GetMethodID(cls, "getPackageManager","()Landroid/content/pm/PackageManager;");
    // 获得应用包的管理器
    jobject pm = env->CallObjectMethod(context, mid);

    // 得到getPackageName方法的ID
    mid = env->GetMethodID(cls, "getPackageName", "()Ljava/lang/String;");
    // 获得当前应用包名
    jstring packageName = (jstring) env->CallObjectMethod(context, mid);
    const char *c_pack_name = env->GetStringUTFChars(packageName, NULL);

    // 比较包名,若不一致，直接return包名
    if (strcmp(c_pack_name, PACKAGE_NAME) != 0) {
        return (env)->NewStringUTF(c_pack_name);
    }

    // 获得PackageManager类
    cls = env->GetObjectClass(pm);
    // 得到getPackageInfo方法的ID
    mid = env->GetMethodID(cls, "getPackageInfo",
                               "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    // 获得应用包的信息
    jobject packageInfo = env->CallObjectMethod(pm, mid, packageName, 0x40); //GET_SIGNATURES = 64;
    // 获得PackageInfo 类
    cls = env->GetObjectClass(packageInfo);
    // 获得签名数组属性的ID
    jfieldID fid = env->GetFieldID(cls, "signatures", "[Landroid/content/pm/Signature;");
    // 得到签名数组
    jobjectArray signatures = (jobjectArray) env->GetObjectField(packageInfo, fid);
    // 得到签名
    jobject signature = env->GetObjectArrayElement(signatures, 0);

    // 获得Signature类
    cls = env->GetObjectClass(signature);
    mid = env->GetMethodID(cls, "toByteArray", "()[B");
    // 当前应用签名信息
    jbyteArray signatureByteArray = (jbyteArray) env->CallObjectMethod(signature, mid);

    //转成jstring
    jstring str = ToMd5(env, signatureByteArray);
    char *c_msg = (char *) env->GetStringUTFChars(str, 0);

    if (strcmp(c_msg, RELEASE_SIGN_MD5) == 0) {
        return (env)->NewStringUTF(WEI_XIN_SECRET);
    } else {
        return (env)->NewStringUTF(TIP_STRING);
    }
}

extern "C"
JNIEXPORT jstring JNICALL
 Java_com_gago_jnidemo_MainActivity_weiXinKeyFromJNI(JNIEnv* env,jobject /* this */) {

     jobject context = getApplication(env);
     // 获得Context类
     jclass cls = env->GetObjectClass(context);
     // 得到getPackageManager方法的ID
     jmethodID mid = env->GetMethodID(cls, "getPackageManager", "()Landroid/content/pm/PackageManager;");
     // 获得应用包的管理器
     jobject pm = env->CallObjectMethod(context, mid);

     // 得到getPackageName方法的ID
     mid = env->GetMethodID(cls, "getPackageName", "()Ljava/lang/String;");
     // 获得当前应用包名
     jstring packageName = (jstring) env->CallObjectMethod(context, mid);
     const char *c_pack_name = env->GetStringUTFChars(packageName, NULL);

     // 比较包名,若不一致，直接return包名
     if (strcmp(c_pack_name, PACKAGE_NAME) != 0) {
         return (env)->NewStringUTF(c_pack_name);
     }

     // 获得PackageManager类
     cls = env->GetObjectClass(pm);
     // 得到getPackageInfo方法的ID
     mid = env->GetMethodID(cls, "getPackageInfo",
                                    "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
     // 获得应用包的信息
     jobject packageInfo = env->CallObjectMethod(pm, mid, packageName, 0x40); //GET_SIGNATURES = 64;
     // 获得PackageInfo 类
     cls = env->GetObjectClass(packageInfo);
     // 获得签名数组属性的ID
     jfieldID fid = env->GetFieldID(cls, "signatures", "[Landroid/content/pm/Signature;");
     // 得到签名数组
     jobjectArray signatures = (jobjectArray) env->GetObjectField(packageInfo, fid);
     // 得到签名
     jobject signature = env->GetObjectArrayElement(signatures, 0);

     // 获得Signature类
     cls = env->GetObjectClass(signature);
     mid = env->GetMethodID(cls, "toByteArray", "()[B");
     // 当前应用签名信息
     jbyteArray signatureByteArray = (jbyteArray) env->CallObjectMethod(signature, mid);

     //转成jstring
     jstring str = ToMd5(env, signatureByteArray);
     char *c_msg = (char *) env->GetStringUTFChars(str, 0);

     if (strcmp(c_msg, RELEASE_SIGN_MD5) == 0) {
         return (env)->NewStringUTF(WEI_XIN_KEY);
     } else {
         return (env)->NewStringUTF(TIP_STRING);
     }

 }
