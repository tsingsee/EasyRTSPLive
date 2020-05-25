## EasyRTSPLive介绍

EasyRTSPLive-Android是由[TSINGSEE青犀开放平台](http://open.tsingsee.com "TSINGSEE青犀开放平台")开发和维护的一个完善的行业视频接入网关，拉流IPC摄像机或者NVR硬盘录像机RTSP流转成RTMP推送到阿里云CDN/腾讯云CDN/RTMP流媒体服务器， EasyRTSPLive-Android是一款非常稳定的RTSP协议转RTMP协议的行业视频接入网关，全平台支持（包括Windows/Linux 32&64，ARM各种平台，Android，iOS），是技术研发快速迭代的工具，也是安防运维人员进行现场问题排查的得力帮手！


## 使用方法

EasyRTSPLive采用Config.ini配置文件，来配置每路输入的RTSP地址，以及目标RTMP地址。channel必须是channel0到channel1024之间，目标rtmp地址不能重复。

如示例：

    [channel0]
	rtsp=rtsp://admin:admin@192.168.66.222/11
	rtmp=rtmp://demo.easydss.com:10085/live/test1
	option=1
	[channel1]
	rtsp=rtsp://admin:admin@192.168.66.222/22
	rtmp=rtmp://demo.easydss.com:10085/live/test2


## 编译及运行

### 项目编译依赖

EasyRTSPLive项目依赖3个TSINGSEE青犀开放平台的Git工程：

- Include：https://github.com/tsingsee/Include
- EasyRTSPClient：https://github.com/tsingsee/EasyRTSPClient
- EasyRTMP：https://github.com/tsingsee/EasyRTMP
- EasyAACEncoder：https://github.com/EasyDarwin/EasyAACEncoder

目录结构为：

	/
	/Include/
	/EasyRTSPClient/
	/EasyRTMP/
	/EasyAACEncoder/
	/EasyRTSPLive/

### Windows & Linux（ARM）编译

Windows上使用Visual Studio 2010开发，当然各位可以改成自己的编译环境。

Linux上编译命令如下：

	清理:	./Buildit clean
	64位编译：./Buildit x64

### 程序运行

运行时将Config.ini文件放至于可执行文件相同路径下，然后直接执行可执行程序，不用带参数。


## EasyRTSPLive-Android

 EasyRTSPLive也提供了Android安卓版本，项目地址：[https://github.com/tsingsee/EasyRTSPLive-Android](https://github.com/tsingsee/EasyRTSPLive-Android "EasyRTSPLive-Android")

[http://d.7short.com/EasyRTSPLive](http://d.7short.com/EasyRTSPLive "EasyRTSPLive-Android")

![EasyRTSPLive-Android](https://github.com/tsingsee/images/blob/master/EasyRTSPLive/fir.easyrtsplive.android.png?raw=true)


## 获取更多信息

TSINGSEE青犀开放平台：[http://open.tsingsee.com](http://open.tsingsee.com "TSINGSEE青犀开放平台")

![TSINGSEE青犀开放平台](https://github.com/tsingsee/images/blob/master/TSINGSEE/singsee_qrcode_160.jpg?raw=true)

Copyright &copy; TSINGSEE.com 2012~2020
