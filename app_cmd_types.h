#ifndef _APP_CMD_TYPES_H_
#define _APP_CMD_TYPES_H_
#include "cyu3types.h"

// ��������������
#define MAX_COMMAND_PARAM_NUM 260
// �ָ�������
//#define CMDFORMATTER_SEPERATOR_TYPE_NUM		        2

#define BUFF_SIZE 4096
// ���֡����
#define FRAME_MAX_LEN (MAX_COMMAND_PARAM_NUM * 6)
// ����ṹ��
//
// ParamsΪ����Ĳ�����MAX_COMMAND_PARAM_NUMΪ���������������������CMDFormatterRef.h��
typedef struct
{
	uint8_t Command_char[4];
	uint32_t Param_Num;
	uint32_t Params[MAX_COMMAND_PARAM_NUM];
} tagCmdFormatterContent;


#endif
