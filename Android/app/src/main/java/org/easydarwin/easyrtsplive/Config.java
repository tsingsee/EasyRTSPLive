/*
	Copyright (c) 2012-2018 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/

package org.easydarwin.easyrtsplive;

/**
 * 类Config的实现描述：
 */
public class Config {
    public static final String RTSP_URL = "RTSPUrl";
    public static final String DEFAULT_RTSP_URL = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
//    public static final String DEFAULT_RTSP_URL = "rtsp://admin:@192.168.43.201/rtsp_live0";

    public static final String SERVER_URL = "serverUrl";
    public static final String DEFAULT_SERVER_URL = "rtmp://demo.easydss.com:10085/live/stream_"+String.valueOf((int) (Math.random() * 1000000 + 100000));
//    public static final String DEFAULT_SERVER_URL = "rtmp://114.251.158.52:10085/live/1111";
}
