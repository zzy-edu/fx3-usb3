/*file
********************************************************************************
<PRE>
ģ����       : 
�ļ���       : 
����ļ�     : 
�ļ�ʵ�ֹ��� :��������Ķ��庯�� ���
����         : 

--------------------------------------------------------------------------------
��ע         : 
	֧������ģʽ�������ʽ
	1  Ϊasc���ʽ�ģ���β��\r\nΪ���������־
	2 ʹ�ö��������ݣ�����֡ͷ��֡β��ת���ַ���ģʽ
	֧�ֳ��Ȳ����Ĳ�ͬ֡���ͣ���󳤶�����ΪFRAME_MAX_LEN
--------------------------------------------------------------------------------
�޸ļ�¼ : 
�� ��:      2012.03.25
�汾 :		v1.0
�޸���:
�޸����� :

</PRE>
********************************************************************************

* ��Ȩ����(c) , ��������Ȩ��
*******************************************************************************/
#ifndef _APP_CMD_ANALYSIS_H_
#define _APP_CMD_ANALYSIS_H_

#include "fx3_fifo.h"
#include "app_cmd_types.h"
#include "app_cmd_exe.h"
#include "cyu3types.h"

// �����������������ص�����
#define FRAME_HEAD_BYTE (0XAA) //֡ͷ

// ���涨�巴������Ϣ����λ��Ϊ������λ����Ӧ�ķ���
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

// ���ݶ�������ģʽ�ĺ���
void CmdAscAndHexRecv(uint8_t *buffer, tagFifoParam *sendFifo);

#endif //_APP_CMD_ANALYSIS_H_
