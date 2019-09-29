#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#ifdef _WIN32
#include "windows.h"
#else
#include <string.h>
#include <unistd.h>
#endif
#include "getopt.h"
#include <stdio.h> 
#include <iostream> 
#include <time.h> 
#include <stdlib.h>
//#include <vector>
#include <list>

#include "EasyAACEncoderAPI.h"
#include "EasyRTSPClientAPI.h"
#include "EasyRTMPAPI.h"
#include "ini.h"
#include "trace.h"

#ifdef _WIN32
#pragma comment(lib,"libEasyRTSPClient.lib")
#pragma comment(lib,"libeasyrtmp.lib")
#pragma comment(lib,"libEasyAACEncoder.lib")

#endif

#ifdef _WIN32
#define RTMP_KEY "79736C36655969576B5A7341436D74646F7054317065394659584E35556C525455457870646D55755A58686C4B56634D5671442F706634675A57467A65513D3D"
#define RTSP_KEY "6D75724D7A4969576B5A7341436D74646F7054317065394659584E35556C525455457870646D55755A58686C4931634D5671442F706634675A57467A65513D3D"
#else // linux
#define RTMP_KEY "79736C36655A4F576B596F41436D74646F70543170664E6C59584E35636E527A63477870646D5745567778576F502B6C2F69426C59584E35"
#define RTSP_KEY "6D75724D7A4A4F576B596F41436D74646F70543170664E6C59584E35636E527A63477870646D5868567778576F502B6C2F69426C59584E35"
#endif

#define BUFFER_SIZE  1024*1024

//用户可自定义的RTSP转RTMP拉流转推流路数,官方工具版默认1路拉转推，用户可通过代码定制多路RTSP转RTMP
#define MAX_CHANNEL_INDEX 1

#define CONF_FILE_PATH  "easyrtsplive.ini"  

typedef struct _channel_cfg_struct_t
{
	int channelId;
	int option;
	char channelName[64];
	char srcRtspAddr[512];
	char destRtmpAddr[512];
}_channel_cfg;

typedef struct _rtmp_pusher_struct_t
{
	Easy_Handle aacEncHandle;
	Easy_Handle rtmpHandle;
	unsigned int u32AudioCodec;	
	unsigned int u32AudioSamplerate;
	unsigned int u32AudioChannel;
	unsigned char* pAACCacheBuffer;
}_rtmp_pusher;

typedef struct _channel_info_struct_t
{
	_channel_cfg		fCfgInfo;
	_rtmp_pusher		fPusherInfo;
	Easy_Handle	fNVSHandle;
	FILE*				fLogHandle;
	bool				fHavePrintKeyInfo;
	EASY_MEDIA_INFO_T	fMediainfo;
}_channel_info;

static std::list <_channel_info*> gChannelInfoList;

int __EasyRTMP_Callback(int _frameType, char *pBuf, EASY_RTMP_STATE_T _state, void *_userPtr)
{
	_channel_info* pChannel = (_channel_info*)_userPtr;

	switch(_state)
	{
	case EASY_RTMP_STATE_CONNECTING:
		TRACE_LOG(pChannel->fLogHandle, "Connecting...\n");
		break;
	case EASY_RTMP_STATE_CONNECTED:
		TRACE_LOG(pChannel->fLogHandle, "Connected\n");
		break;
	case EASY_RTMP_STATE_CONNECT_FAILED:
		TRACE_LOG(pChannel->fLogHandle, "Connect failed\n");
		break;
	case EASY_RTMP_STATE_CONNECT_ABORT:
		TRACE_LOG(pChannel->fLogHandle, "Connect abort\n");
		break;
	case EASY_RTMP_STATE_DISCONNECTED:
		TRACE_LOG(pChannel->fLogHandle, "Disconnect.\n");
		break;
	default:
		break;
	}
	return 0;
}

int Easy_APICALL __RTSPSourceCallBack( int _chid, void *_chPtr, int _mediatype, char *pbuf, EASY_FRAME_INFO *frameinfo)
{
	if (NULL != frameinfo)
	{
		if (frameinfo->height==1088)		frameinfo->height=1080;
		else if (frameinfo->height==544)	frameinfo->height=540;
	}
	Easy_Bool bRet = 0;
	int iRet = 0;
	
	_channel_info* pChannel = (_channel_info*)_chPtr;

	if (_mediatype == EASY_SDK_VIDEO_FRAME_FLAG)
	{
		if(frameinfo && frameinfo->length)
		{
			if( frameinfo->type == EASY_SDK_VIDEO_FRAME_I)
			{
				if(pChannel->fPusherInfo.rtmpHandle == 0)
				{
					pChannel->fPusherInfo.rtmpHandle = EasyRTMP_Create();
					if (pChannel->fPusherInfo.rtmpHandle == NULL)
					{
						TRACE_LOG(pChannel->fLogHandle, "Fail to rtmp create failed ...\n");
						return -1;
					}
					EasyRTMP_SetCallback(pChannel->fPusherInfo.rtmpHandle, __EasyRTMP_Callback, pChannel);
					bRet = EasyRTMP_Connect(pChannel->fPusherInfo.rtmpHandle, pChannel->fCfgInfo.destRtmpAddr);
					if (!bRet)
					{
						TRACE_LOG(pChannel->fLogHandle, "Fail to rtmp connect failed ...\n");
					}

					EASY_MEDIA_INFO_T mediaInfo;
					memset(&mediaInfo, 0, sizeof(EASY_MEDIA_INFO_T));
					mediaInfo.u32VideoFps = pChannel->fMediainfo.u32VideoFps;
					mediaInfo.u32AudioSamplerate =pChannel->fMediainfo.u32AudioSamplerate ;				/* 音频采样率 */
					mediaInfo.u32AudioChannel = pChannel->fMediainfo.u32AudioChannel;					/* 音频通道数 */
					mediaInfo.u32AudioBitsPerSample = pChannel->fMediainfo.u32AudioBitsPerSample;		/* 音频采样精度 */


					iRet = EasyRTMP_InitMetadata(pChannel->fPusherInfo.rtmpHandle, &mediaInfo, 1024);
					if (iRet < 0)
					{
						TRACE_LOG(pChannel->fLogHandle, "Fail to Init Metadata ...\n");
					}
				}

				EASY_AV_Frame avFrame;
				memset(&avFrame, 0, sizeof(EASY_AV_Frame));
				avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
				avFrame.u32AVFrameLen = frameinfo->length;
				avFrame.pBuffer = (unsigned char*)pbuf;
				avFrame.u32VFrameType = EASY_SDK_VIDEO_FRAME_I;
				//avFrame.u32TimestampSec = frameinfo->timestamp_sec;
				//avFrame.u32TimestampUsec = frameinfo->timestamp_usec;

				iRet = EasyRTMP_SendPacket(pChannel->fPusherInfo.rtmpHandle, &avFrame);
				if (iRet < 0)
				{
					TRACE_LOG(pChannel->fLogHandle, "Fail to Send H264 Packet(I-frame) ...\n");
				}
				else
				{
					if(!pChannel->fHavePrintKeyInfo)
					{
						TRACE_LOG(pChannel->fLogHandle, "I\n");
						pChannel->fHavePrintKeyInfo = true;
					}
				}
			}
			else
			{
				if(pChannel->fPusherInfo.rtmpHandle)
				{
					EASY_AV_Frame avFrame;
					memset(&avFrame, 0, sizeof(EASY_AV_Frame));
					avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
					avFrame.u32AVFrameLen = frameinfo->length-4;
					avFrame.pBuffer = (unsigned char*)pbuf+4;
					avFrame.u32VFrameType = EASY_SDK_VIDEO_FRAME_P;
					//avFrame.u32TimestampSec = frameinfo->timestamp_sec;
					//avFrame.u32TimestampUsec = frameinfo->timestamp_usec;
					iRet = EasyRTMP_SendPacket(pChannel->fPusherInfo.rtmpHandle, &avFrame);
					if (iRet < 0)
					{
						TRACE_LOG(pChannel->fLogHandle, "Fail to Send H264 Packet(P-frame) ...\n");
					}
					else
					{
						if(!pChannel->fHavePrintKeyInfo)
						{
							TRACE_LOG(pChannel->fLogHandle, "P\n");
						}
					}
				}
			}				
		}	
	}
	if (_mediatype == EASY_SDK_AUDIO_FRAME_FLAG)
	{
		/* 音频编码 */
		// #define EASY_SDK_AUDIO_CODEC_AAC	0x15002			/* AAC */
		// #define EASY_SDK_AUDIO_CODEC_G711U	0x10006		/* G711 ulaw*/
		// #define EASY_SDK_AUDIO_CODEC_G711A	0x10007		/* G711 alaw*/
		// #define EASY_SDK_AUDIO_CODEC_G726	0x1100B		/* G726 */
		
		unsigned char* pSendBuffer = NULL;
		int nSendBufferLen = 0;
		if (frameinfo->codec == EASY_SDK_AUDIO_CODEC_AAC)
		{
			pSendBuffer =  (unsigned char*)pbuf;
			nSendBufferLen = frameinfo->length;
		} 
		else
		{
			// 音频转码 [8/17/2019 SwordTwelve]
			int bits_per_sample = frameinfo->bits_per_sample;
			int channels = frameinfo->channels;
			int sampleRate = frameinfo->sample_rate;

			if (EASY_SDK_AUDIO_CODEC_G711U   ==  frameinfo->codec
				|| EASY_SDK_AUDIO_CODEC_G726  ==  frameinfo->codec 
				|| EASY_SDK_AUDIO_CODEC_G711A == frameinfo->codec ) 
			{
				if (pChannel->fPusherInfo.pAACCacheBuffer == NULL)
				{
					int buf_size = BUFFER_SIZE;
					pChannel->fPusherInfo.pAACCacheBuffer  = new unsigned char[buf_size];
					memset(pChannel->fPusherInfo.pAACCacheBuffer , 0x00, buf_size);
				}

				if (pChannel->fPusherInfo.aacEncHandle == NULL)
				{
					InitParam initParam;
					initParam.u32AudioSamplerate=frameinfo->sample_rate;
					initParam.ucAudioChannel=frameinfo->channels;
					initParam.u32PCMBitSize=frameinfo->bits_per_sample;
					if (frameinfo->codec == EASY_SDK_AUDIO_CODEC_G711U)
					{
						initParam.ucAudioCodec = Law_ULaw;
					} 
					else if (frameinfo->codec == EASY_SDK_AUDIO_CODEC_G726)
					{
						initParam.ucAudioCodec = Law_G726;
					}
					else if (frameinfo->codec == EASY_SDK_AUDIO_CODEC_G711A)
					{
						initParam.ucAudioCodec = Law_ALaw;
					}
					pChannel->fPusherInfo.aacEncHandle = Easy_AACEncoder_Init( initParam);
				}
				unsigned int out_len = 0;
				int nRet = Easy_AACEncoder_Encode(pChannel->fPusherInfo.aacEncHandle, 
					(unsigned char*)pbuf, frameinfo->length, (unsigned char*)pChannel->fPusherInfo.pAACCacheBuffer, &out_len) ;
				if (nRet>0&&out_len>0)
				{
					pSendBuffer = (unsigned char*)pChannel->fPusherInfo.pAACCacheBuffer ;
					nSendBufferLen = out_len;
					frameinfo->codec = EASY_SDK_AUDIO_CODEC_AAC;
				} 
			}
		}

		if(pChannel->fPusherInfo.rtmpHandle&&pSendBuffer&&nSendBufferLen>0)
		{
			EASY_AV_Frame avFrame;
			memset(&avFrame, 0, sizeof(EASY_AV_Frame));
			avFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
			avFrame.u32AVFrameLen = nSendBufferLen;
			avFrame.pBuffer = (unsigned char*)pSendBuffer;
			//avFrame.u32TimestampSec = frameinfo->timestamp_sec;
			//avFrame.u32TimestampUsec = frameinfo->timestamp_usec;
			iRet = EasyRTMP_SendPacket(pChannel->fPusherInfo.rtmpHandle, &avFrame);
			if (iRet < 0)
			{
				TRACE_LOG(pChannel->fLogHandle, "Fail to Send AAC Packet ...\n");
			}
			else
			{
				if(!pChannel->fHavePrintKeyInfo)
				{
					TRACE_LOG(pChannel->fLogHandle, "Audio\n");
				}
			}
		}
	}
	else if (_mediatype == EASY_SDK_MEDIA_INFO_FLAG)
	{
		if(pbuf != NULL)
		{
			EASY_MEDIA_INFO_T mediainfo;
			memset(&(pChannel->fMediainfo), 0x00, sizeof(EASY_MEDIA_INFO_T));
			memcpy(&(pChannel->fMediainfo), pbuf, sizeof(EASY_MEDIA_INFO_T));
			TRACE_LOG(pChannel->fLogHandle,"RTSP DESCRIBE Get Media Info: video:%u fps:%u audio:%u channel:%u sampleRate:%u \n", 
				pChannel->fMediainfo.u32VideoCodec, pChannel->fMediainfo.u32VideoFps, pChannel->fMediainfo.u32AudioCodec, pChannel->fMediainfo.u32AudioChannel, pChannel->fMediainfo.u32AudioSamplerate);
		}
	}

	return 0;
}

bool InitCfgInfo(void)
{
	int i = 0;
	gChannelInfoList.clear();
	for(i = 0; i < MAX_CHANNEL_INDEX; i++)
	{
		_channel_info* pChannelInfo = new _channel_info();
		if(pChannelInfo)
		{
			memset(pChannelInfo, 0, sizeof(_channel_info));
			pChannelInfo->fCfgInfo.channelId = i;
			pChannelInfo->fHavePrintKeyInfo = false;
			sprintf(pChannelInfo->fCfgInfo.channelName, "channel%d",i);
			strcpy(pChannelInfo->fCfgInfo.srcRtspAddr, GetIniKeyString(pChannelInfo->fCfgInfo.channelName, "rtsp", CONF_FILE_PATH));
			strcpy(pChannelInfo->fCfgInfo.destRtmpAddr, GetIniKeyString(pChannelInfo->fCfgInfo.channelName, "rtmp", CONF_FILE_PATH));
			pChannelInfo->fCfgInfo.option = GetIniKeyInt(pChannelInfo->fCfgInfo.channelName, "option", CONF_FILE_PATH);
			if(strlen(pChannelInfo->fCfgInfo.srcRtspAddr) > 0 && strlen(pChannelInfo->fCfgInfo.destRtmpAddr) > 0)
			{
				gChannelInfoList.push_back(pChannelInfo);
			}
		}
	}
	return true;
}

void ReleaseSpace(void)
{
	std::list<_channel_info*>::iterator it;
	for(it = gChannelInfoList.begin(); it != gChannelInfoList.end(); it++)
	{
		_channel_info* pChannel = *it;

		if (NULL != pChannel->fNVSHandle) 
		{
			EasyRTSP_CloseStream(pChannel->fNVSHandle);
			EasyRTSP_Deinit(&(pChannel->fNVSHandle));
			pChannel->fNVSHandle = NULL;
		}

		if (NULL != pChannel->fPusherInfo.rtmpHandle)
		{
			EasyRTMP_Release(pChannel->fPusherInfo.rtmpHandle);
			pChannel->fPusherInfo.rtmpHandle = NULL;
		}

		if(pChannel->fLogHandle)
		{
			TRACE_CloseLogFile(pChannel->fLogHandle);
			pChannel->fLogHandle = NULL;
		}
		if (pChannel->fPusherInfo.aacEncHandle )
		{
			Easy_AACEncoder_Release(pChannel->fPusherInfo.aacEncHandle );
			pChannel->fPusherInfo.aacEncHandle  = NULL;
		}
		if (pChannel->fPusherInfo.pAACCacheBuffer )
		{
			delete[] pChannel->fPusherInfo.pAACCacheBuffer;
			pChannel->fPusherInfo.pAACCacheBuffer = NULL;
		}

		delete pChannel;
	}

	gChannelInfoList.clear();
}

int main(int argc, char * argv[])
{
	printf("\n\n");
	printf("****************************************************************\n");
	printf("**************EasyRTSPLive工具版v2.0.19.0826(免费)**************\n");
	printf("******工具版主要用于开发者调试与测试，只支持一路RTSP转RTMP******\n");
	printf("******EasyRTSPLive工具版由open.tsingsee.com青犀开放平台提供*****\n");
	printf("****************************************************************\n");
	
	//splash
#ifdef _WIN32
		Sleep(3000);
#else
		sleep(3);
#endif

	InitCfgInfo();

	int iret = 0;
	iret = EasyRTMP_Activate(RTMP_KEY);
	if (iret <= 0)
	{
		printf("RTMP Activate error. ret=%d!!!\n", iret);
		getchar();
		return -1;
	}

#ifdef _WIN32
	extern char* optarg;
#endif
	int ch;

	atexit(ReleaseSpace);

	iret = 0;
	iret = EasyRTSP_Activate(RTSP_KEY);
	if(iret <= 0)
	{
		printf("RTSP Activate error. ret=%d!!!\n", iret);
		getchar();
		return -2;
	}

	std::list<_channel_info*>::iterator it;
	for(it = gChannelInfoList.begin(); it != gChannelInfoList.end(); it++)
	{
		_channel_info* pChannel = *it;
		pChannel->fLogHandle = TRACE_OpenLogFile(pChannel->fCfgInfo.channelName);

		TRACE_LOG(pChannel->fLogHandle, "channel[%d] rtsp addr : %s\n", pChannel->fCfgInfo.channelId, pChannel->fCfgInfo.srcRtspAddr);
		TRACE_LOG(pChannel->fLogHandle, "channel[%d] rtmp addr : %s\n", pChannel->fCfgInfo.channelId, pChannel->fCfgInfo.destRtmpAddr);

		EasyRTSP_Init(&(pChannel->fNVSHandle));

		if (NULL == pChannel->fNVSHandle)
		{
			TRACE_LOG(pChannel->fLogHandle, "%s rtsp init error. ret=%d!!!\n", pChannel->fCfgInfo.channelName , iret);
			continue;
		}
		unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;
	
		EasyRTSP_SetCallback(pChannel->fNVSHandle, __RTSPSourceCallBack);

		EasyRTSP_OpenStream(pChannel->fNVSHandle, pChannel->fCfgInfo.channelId, pChannel->fCfgInfo.srcRtspAddr, EASY_RTP_OVER_TCP, mediaType, 0, 0, pChannel, 1000, 0, pChannel->fCfgInfo.option, 3);
	}

	while(true)
	{
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}

    return 0;
}