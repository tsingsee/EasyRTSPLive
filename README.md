## 介绍 ##
拉流IPC摄像机或者NVR硬盘录像机RTSP流转成RTMP推送到阿里云CDN/腾讯云CDN/RTMP流媒体服务器，支持多路RTSP流同时拉取并以RTMP协议推送发布。

采用Config.ini配置文件，来配置每路输入的RTSP地址，以及目标RTMP地址。channel必须是channel0到channel1024之间，目标rtmp地址不能重复。

    [channel0]
	rtsp=rtsp://admin:admin@192.168.66.222/11
	rtmp=rtmp://demo.easydss.com:10085/live/test1
	option=1
	[channel1]
	rtsp=rtsp://admin:admin@192.168.66.222/22
	rtmp=rtmp://demo.easydss.com:10085/live/test2

## 编译及运行 ##
Windows上使用Visual Studio 2010开发，当然各位可以改成自己的编译环境。
Linux上编译命令如下：

	清理:	./Buildit clean
	64位编译：./Buildit x64

运行时将Config.ini文件放至于可执行文件相同路径下，然后直接执行可执行程序，不用带参数。

## 下载 ##
可执行程序下载：https://pan.baidu.com/s/1-7lZ3KM4wPl87OLx2tWjTQ


## 获取更多信息 ##

TSINGSEE青犀开放平台：[http://open.tsingsee.com](http://open.tsingsee.com "TSINGSEE青犀开放平台")

Copyright &copy; TSINGSEE.com 2012~2019
