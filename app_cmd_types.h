#ifndef _APP_CMD_TYPES_H_
#define _APP_CMD_TYPES_H_
#include "cyu3types.h"

// 最大命令参数数量
#define MAX_COMMAND_PARAM_NUM 260
// 分隔符数量
//#define CMDFORMATTER_SEPERATOR_TYPE_NUM		        2

#define BUFF_SIZE 4096
// 最大帧长度
#define FRAME_MAX_LEN (MAX_COMMAND_PARAM_NUM * 6)
// 命令结构体
//
// Params为命令的参数表，MAX_COMMAND_PARAM_NUM为参数的最大数量，定义在CMDFormatterRef.h中
typedef struct
{
	uint8_t Command_char[4];
	uint32_t Param_Num;
	uint32_t Params[MAX_COMMAND_PARAM_NUM];
} tagCmdFormatterContent;


#endif
