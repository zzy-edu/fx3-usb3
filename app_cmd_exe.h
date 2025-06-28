#ifndef _APP_CMD_EXE_H_
#define _APP_CMD_EXE_H_

#include "cyu3types.h"
#include "app_cmd_types.h"

typedef struct
{
    uint8_t hex_code;
    uint8_t asc_code[4];
} cmd_tag_t;
CyBool_t CmdHexExecute(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend, uint32_t hex_code);
CyBool_t CmdFrameExecute(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend);

#endif
