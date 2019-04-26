package org.easydarwin.video;

import android.annotation.TargetApi;
import android.os.Build;

/**
 * Created by John on 2017/1/5.
 */

public class VideoCodec {

    static {
        System.loadLibrary("VideoCodecer");
    }

    private final int[] paramReuse = new int[7];

    private native int open(int maxWidth, int maxHeight, Object surface);

    private native void close(int handle);

    protected int mHandle;
    private DecodeParam mParam;


    public class DecodeParam {
        public byte[] buffer;
        public int offset;
        public int length;
        public int width, height;
        public byte[] imgRGB;
        public int out_offset, out_length;
    }

    public DecodeParam createReusableParam() {
        if (mParam == null) {
            mParam = new DecodeParam();
        }
        return mParam;
    }

    public int reCreate(int width, int height, Object surface) {
        decoder_close();
        return decoder_create(width, height, surface);
    }

    public int surfaceSizeChanged(Object surface) {
        return reCreateSurface(mHandle, surface);
    }

    public native int reCreateSurface(int handle, Object surface);

    /**
     * @param in     要解码的buffer
     * @param out    解码后的buffer
     * @param params 分别为：
     *               <p/>
     *               in的offset、length;
     *               <p/>
     *               out的offset、length;
     *               <p/>
     *               width、height;
     * @return
     */
    private native int decode(int handle, byte[] in, byte[] out, int[] params);

    public int decoder_create(int width, int height, Object surface) {
        mHandle = open(width, height, surface);
        if (mHandle != 0) {
            return 0;
        }
        return -1;
    }

    public int decoder_decode(Object param) {
        DecodeParam aParam = (DecodeParam) param;
        paramReuse[0] = aParam.offset;
        paramReuse[1] = aParam.length;
        paramReuse[2] = aParam.out_offset;
        paramReuse[3] = aParam.out_length;
        paramReuse[4] = aParam.width;
        paramReuse[5] = aParam.height;
        paramReuse[6] = 0;
        int result = decode(mHandle, aParam.buffer, aParam.imgRGB, paramReuse);
        if (result == 0) {
            aParam.offset = paramReuse[0];
            aParam.length = paramReuse[1];
            aParam.out_offset = paramReuse[2];
            aParam.out_length = paramReuse[3];
            aParam.width = paramReuse[4];
            aParam.height = paramReuse[5];
        }
        return result;
    }

    public void decoder_close() {
        if (mHandle == 0) {
            return;
        }
        close(mHandle);
        mHandle = 0;
    }


    public static class VideoDecoderLite extends VideoCodec {

        private byte[] mImageBufferReuse;
        private int[] mSize;
        private Object surface;

        public void create(int width, int height, Object surface) {
            if (surface == null) {
                throw new NullPointerException("surface is null!");
            }
            this.surface = surface;
            decoder_create(width, height, surface);
            // rgb 565
            mImageBufferReuse = new byte[width * height * 2];
            mSize = new int[2];
        }

        public void close() {
            decoder_close();
        }

        protected int decodeFrame(Client.FrameInfo aFrame, byte[] rgbBuffer,
                                  int[] size) {
            int nRet = 0;
            DecodeParam param = createReusableParam();
            param.buffer = aFrame.buffer;
            param.offset = aFrame.offset;
            param.length = aFrame.length;

            param.imgRGB = rgbBuffer;
            param.out_offset = 0;
            param.out_length = rgbBuffer.length;
            param.width = aFrame.width;
            param.height = aFrame.height;
            // long lastDecodeTm = System.currentTimeMillis();
            nRet = decoder_decode(param);
            if (nRet >= 0) {
                size[0] = param.width;
                size[1] = param.height;

//                Log.d(TAG, String.format("decode size %d:%d", param.width, param.height));
            }
            return nRet;
        }

        @TargetApi(Build.VERSION_CODES.ICE_CREAM_SANDWICH)
        protected void decodeAndSnapAndDisplay(Client.FrameInfo frame) {
            try {
                int result = decodeFrame(frame, mImageBufferReuse, mSize);
            } catch (Exception e) {
                e.fillInStackTrace();
            }
        }
    }
}
