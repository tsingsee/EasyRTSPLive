package org.easydarwin.sw;

import java.nio.ByteBuffer;

/**
 */
public class JNIUtil {

    static {
        System.loadLibrary("yuv_android");
    }

    /**
     * 旋转1个字节为单位的矩阵
     *
     * @param data   要旋转的矩阵
     * @param offset 偏移量
     * @param width  宽度
     * @param height 高度
     * @param degree 旋转度数
     */
    public static void rotateMatrix(byte[] data, int offset, int width, int height, int degree) {
        callMethod("RotateByteMatrix", null, data, offset, width, height, degree);
    }

    /**
     * 旋转2个字节为单位的矩阵
     *
     * @param data   要旋转的矩阵
     * @param offset 偏移量
     * @param width  宽度
     * @param height 高度
     * @param degree 旋转度数
     */
    public static void rotateShortMatrix(byte[] data, int offset, int width, int height, int degree) {
        callMethod("RotateShortMatrix", null, data, offset, width, height, degree);
    }

    private static native void callMethod(String methodName, Object[] returnValue, Object... params);


    /**
     * 0 NULL,
     * 1 yuv_to_yvu,
     * 2 yuv_to_yuvuv,
     * 3 yuv_to_yvuvu,
     * 4 yuvuv_to_yuv,
     * 5 yuvuv_to_yvu,
     * 6 yuvuv_to_yvuvu,
     *
     * @param data
     * @param width
     * @param height
     * @param mode
     */
    public static native void yuvConvert(byte[] data, int width, int height, int mode);

    public static native void yuvConvert2(ByteBuffer buffer, int width, int height, int mode);


    /**
     * 0 NULL
     * 1 argb2420
     * 2 argb2yv12
     * 3 argb2nv21
     *
     * @param argb
     * @param yuv
     * @param width
     * @param height
     * @param mode
     */
    public static native void argb2yuv(byte[] argb, byte[] yuv, int width, int height, int mode);

}
