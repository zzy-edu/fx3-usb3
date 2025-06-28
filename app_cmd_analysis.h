/*file
********************************************************************************
<PRE>
模块名       : 
文件名       : 
相关文件     : 
文件实现功能 :命令解析的定义函数 宏等
作者         : 

--------------------------------------------------------------------------------
备注         : 
	支持两种模式的命令格式
	1  为asc码格式的，结尾以\r\n为命令结束标志
	2 使用二进制数据，采用帧头、帧尾、转义字符的模式
	支持长度不定的不同帧类型，最大长度限制为FRAME_MAX_LEN
--------------------------------------------------------------------------------
修改记录 : 
日 期:      2012.03.25
版本 :		v1.0
修改人:
修改内容 :

</PRE>
********************************************************************************

* 版权所有(c) , 保留所有权利
*******************************************************************************/
#ifndef _APP_CMD_ANALYSIS_H_
#define _APP_CMD_ANALYSIS_H_

#include "fx3_fifo.h"
#include "app_cmd_types.h"
#include "app_cmd_exe.h"
#include "cyu3types.h"

// 定义解析串行命令相关的内容
#define FRAME_HEAD_BYTE (0XAA) //帧头

// 下面定义反馈的信息，上位机为主，下位机对应的反馈
#define ACK_OVER 0
#define ACK_RECEVING 1
#define ACK_FRAME_ERR 2
#define ACK_PROCESS_ERR 3
#define ACK_IDLE 0xff



// typedef CyBool_t (*CmdExeFun)(tagCmdFormatterContent *pContentRecv, tagCmdFormatterContent *pContentSend);

// typedef struct
// {
// 	uint8_t Command_char[4];
// 	CmdExeFun pFun;
// } tagCmdExeUnit;

// 兼容二者命令模式的函数
void CmdAscAndHexRecv(uint8_t *buffer, tagFifoParam *sendFifo);

#endif //_APP_CMD_ANALYSIS_H_
