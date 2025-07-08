/*file
********************************************************************************
<PRE>
模块名       : 
文件名       : 
相关文件     : 
文件实现功能 : 定义命令解析的函数
作者         : 

--------------------------------------------------------------------------------
备注         : 

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

#include "app_cmd_analysis.h"
#include "fx3_common.h"
#include <cyu3system.h>

static tagCmdFormatterContent cmdRecv __attribute__((aligned(32)));
static tagCmdFormatterContent cmdSend __attribute__((aligned(32)));
// 是否带校验,在char接收的模式下,如果为1那么char模式下最后一个数据参数为前面所有的字节和，包括命令和内容
char bCharModeIsCheck = 0;

/*function
********************************************************************************
<PRE>
函数名   : 
功能     :把缓冲区内的数据解析成命令格式
参数     : pContent 解析出来的数据内容
		pbuffer 要准备被解析的缓冲区
		nlen 被解析的数据长度
返回值   : 
抛出异常 : 
--------------------------------------------------------------------------------
备注     : 
典型用法 : 
--------------------------------------------------------------------------------
作者     : 
</PRE>
*******************************************************************************/
static CyBool_t inline CmdFrameRecvFomat(uint8_t *pBuffer, uint32_t nLen)
{
	int i;
	int nPos = 0;
	int nAscStart = 0;
	int nAscEnd = 0;
	char pTempBuffer[12];
    CyU3PMemSet(&cmdRecv, 0, sizeof(cmdRecv));
    CyU3PMemSet(&cmdSend, 0, sizeof(cmdSend));
    // asc mode下大写转换为小写
    for (i = 0; i < nLen; i++)
    {
        if (pBuffer[i] >= 'A' && pBuffer[i] <= 'Z')
        {
            pBuffer[i] += ('a' - 'A');
        }
    }

	//--------------开始解析asc码的格式------------------
	// -----------------第1步先把缓冲区的数据中分隔符清理为0
    for (nPos = 0; nPos < nLen; nPos++)
    {
		if(pBuffer[nPos] == ' ')
		{
			pBuffer[nPos] = 0;
		}
	}
	// -----------------第2 步提取命令头，锁定start和end位置后提取
  	nPos = 0;
	nAscStart = nPos;
    while (nPos < nLen) // 消除命令前面的0
    {
        if (pBuffer[nPos] == 0)
        {
            nPos++;
        }
        else
		{
			nAscStart = nPos;
            break;
        }
    }
    nAscEnd = nAscStart;
    while (nPos < nLen) // 开始命令码接收
    {
        if ((pBuffer[nPos] == '\r') || (pBuffer[nPos] == '\n'))
        {
            break;
        }
        else if (pBuffer[nPos] == 0)
        {
            break;
        }
        nPos++;
        nAscEnd = nPos;
    }
    if ((nAscEnd - nAscStart > 0) && (nAscEnd - nAscStart <= 4))
    {
        CyU3PMemCopy(cmdRecv.Command_char, &pBuffer[nAscStart], nAscEnd - nAscStart);
    }
    else
	{
        return CyFalse;
    }
    // -----------------第3 步提取数据区，锁定start和end位置后提取
    while (nPos < nLen)
    {
		// 遇到帧尾就结束
        if ((pBuffer[nPos] == '\r') || (pBuffer[nPos] == '\n'))
        {
			bCharModeIsCheck= 0;
			break;	
		}
        else if (pBuffer[nPos] == '\t')
        {
            bCharModeIsCheck = 1;
            break;
        } // '\t'这里表示最后一个参数是求和校验参数

        // 找到字符开始位置
		nAscStart = nPos;
        while (nPos < nLen) // 消除命令前面的0
        {
            if (pBuffer[nPos] == 0)
            {
                nPos++;
            }
            else
			{
				nAscStart = nPos;
				break;
			}
		}
		//找到字符结束位置
		nAscEnd = nAscStart;
        while (nPos < nLen) // 开始码接收
        {
            if (pBuffer[nPos] == 0)
            {
                break;
            }
            if ((pBuffer[nPos] == '\r') || (pBuffer[nPos] == '\n'))
            {
                bCharModeIsCheck = 0;
                break;
            }
            else if (pBuffer[nPos] == '\t')
            {
                bCharModeIsCheck = 1;
                break;
            } // '\t'这里表示最后一个参数是求和校验参数

            nPos++;
            nAscEnd = nPos;
        }
        if ((nAscEnd - nAscStart > 0) && (nAscEnd - nAscStart <= 10))
        {
            CyU3PMemCopy(pTempBuffer, &pBuffer[nAscStart], nAscEnd - nAscStart);
            pTempBuffer[nAscEnd - nAscStart] = 0;
            if ((pTempBuffer[0] >= '0') && (pTempBuffer[0] <= '9'))
            {
                Char2Long((uint8_t *)pTempBuffer, &cmdRecv.Params[cmdRecv.Param_Num]);
                cmdRecv.Param_Num++;
			}
			else
			{
                CyU3PMemCopy(&cmdRecv.Params[cmdRecv.Param_Num], &pBuffer[nAscStart], nAscEnd - nAscStart);
                cmdRecv.Param_Num += ((nAscEnd - nAscStart - 1) >> 2) + 1;
            }
		}
		else 
		{
            CyU3PMemCopy(&cmdRecv.Params[cmdRecv.Param_Num], &pBuffer[nAscStart], nAscEnd - nAscStart);
            cmdRecv.Param_Num += ((nAscEnd - nAscStart - 1) >> 2) + 1;
        }
	}

    if (bCharModeIsCheck == 1) // 判断校验，在没有\t结束符的时候不用管该校验
    {
        if (cmdRecv.Param_Num < 1)
        {
            return CyFalse;
        }

        nPos = 0;
        for (i = 0; i < 4; i++)
        {
            nPos += cmdRecv.Command_char[i];
        }

        for (i = 0; i < 4 * (cmdRecv.Param_Num - 1); i++)
        {
            nPos += ((uint8_t *)cmdRecv.Params)[i];
        }

        if (nPos != cmdRecv.Params[cmdRecv.Param_Num - 1])
        {
            return CyFalse;
        }

        cmdRecv.Param_Num--;
    }

    return CyTrue;
}

/*function
********************************************************************************
<PRE>
函数名   : 
功能     :发送命令的反馈数据进行格式化，随后发送
参数     : Uint8 *pBuffer,格式化之后存放的缓冲区
			int* pBufferLen,格式化之后字符长度
			tagCmdFormatterContent *pContent    要进行传输的内容格式化输入
		
返回值   : 
抛出异常 : 
--------------------------------------------------------------------------------
备注     : 
典型用法 : 
--------------------------------------------------------------------------------
作者     : 
</PRE>
*******************************************************************************/
static CyBool_t inline CmdFrameSendFomat(tagFifoParam *SendFifo)
{
    int i, nLen;
    int nPosIn = 0;
    int nPosOut = 0;
    uint8_t *pChar;
    uint8_t pTempBuffer[12]; // 存放字符转换结果的临时缓冲区
    uint8_t byTemp;

    if (cmdSend.Param_Num >= MAX_COMMAND_PARAM_NUM)
    {
        return CyFalse;
    }
    if ((cmdSend.Command_char[0] == 0) && (cmdSend.Command_char[1] == 0) && (cmdSend.Command_char[2] == 0) && (cmdSend.Command_char[3] == 0))
    {
        return CyTrue;
    }

    nPosOut = 0;
    nPosIn = 0;
    // 存放反馈的字符头
    for (i = 0; i < 4; i++)
    {
        if (cmdSend.Command_char[i] != 0)
        {
            FifoPush(SendFifo, cmdSend.Command_char[i]);
        }
	}
    FifoPush(SendFifo, ' ');

    for (i = 0; i < cmdSend.Param_Num; i++)
    {
        Long2Char(cmdSend.Params[i], pTempBuffer, &byTemp);
        FifoPushNLen(SendFifo, pTempBuffer, byTemp);
        FifoPush(SendFifo, ' ');
    }

    if (bCharModeIsCheck == 1) // 如果是需要校验的模式的话，就再发送校验，并添加'\t'
    {
        bCharModeIsCheck = 0;
        nLen = 0;
        for (i = 0; i < 4; i++)
        {
            nLen += cmdSend.Command_char[i];
        }
        pChar = (uint8_t *)cmdSend.Params;
        for (i = 0; i < cmdSend.Param_Num * 4; i++)
        {
            nLen += pChar[i];
        }
        Long2Char(nLen, pTempBuffer, &byTemp);
        FifoPushNLen(SendFifo, pTempBuffer, byTemp);
        FifoPush(SendFifo, '\t');
    }

    FifoPush(SendFifo, '\r');
    FifoPush(SendFifo, '\n');
    FifoPush(SendFifo, '\t');
    FifoPush(SendFifo, 'O');
    FifoPush(SendFifo, 'K');
    return CyTrue;
}


// 计算校验和
uint8_t CmdSimpleHexGetCheckSum(uint8_t *pBuffer, uint32_t nLen)
{
    uint32_t n = 0, i = 0;
    for (i = 0; i < nLen; i++)
        n += pBuffer[i];
    return (n & 0xFF);
}

// 计算校验和
uint8_t CmdHexGetCheckSum(uint8_t *pBuffer, uint32_t nLen, uint8_t cmd_hex, uint8_t parNum)
{
    uint32_t n = 0, i = 0;
    for (i = 0; i < nLen; i++)
        n += pBuffer[i];
    n += cmd_hex;
    n += parNum;
    return (n & 0xFF);
}

// 计算字符串长度
uint8_t CmdFrameGetRecvLen(uint8_t *pBuffer, uint32_t start, uint32_t *pLen)
{
    uint32_t len = 0;
    for (; start < BUFF_SIZE; start++)
    {
        if ((pBuffer[start] == '\r') && (pBuffer[start + 1] == '\n'))
        {
            len += 2;
            *pLen = len;
            return ACK_OVER;
        }
        else
        {
            len++;
        }
    }
    *pLen = 0;
    return ACK_FRAME_ERR;
}

// 处理接收的命令字符串
void CmdSimpleHexProcess(uint8_t *pBuffer, tagFifoParam *sendFifo)
{
    uint32_t hex_cmd = pBuffer[1];
    uint32_t nLen = pBuffer[2];
    nLen = nLen * 4 + 3;
    // 判断校验是否正确
    if (pBuffer[nLen] != CmdSimpleHexGetCheckSum(pBuffer + 1, nLen - 1))
    {
		goto exit_CmdSimpleHexProcess;
	}
//    CyU3PDebugPrint(4,"\npc CheckSum = %d",pBuffer[nLen]);
//    CyU3PDebugPrint(4,"\nfx3 CheckSum = %d",CmdSimpleHexGetCheckSum(pBuffer + 1, nLen - 1));
//    CyU3PDebugPrint(4,"hex_cmd = %d\n",hex_cmd);
    CyU3PMemSet(&cmdRecv, 0, sizeof(cmdRecv));
    CyU3PMemSet(&cmdSend, 0, sizeof(cmdSend));

    cmdRecv.Param_Num = pBuffer[2];                // 参数个数
    CyU3PMemCopy(cmdRecv.Params, &pBuffer[3], nLen - 3); // copy参数内容

    // 执行命令
    if (CyFalse == CmdHexExecute(&cmdRecv, &cmdSend, hex_cmd))
    {
		goto exit_CmdSimpleHexProcess;
	}

	//根据反馈进行处理
	if(cmdSend.Param_Num == 0)
	{
        FifoPush(sendFifo, 0xAA);
	}	
	else // 正确的读取命令反馈结果
	{
        FifoPush(sendFifo, 0xAA);
        FifoPush(sendFifo, hex_cmd);
        FifoPush(sendFifo, cmdSend.Param_Num);
        FifoPushNLen(sendFifo, cmdSend.Params, cmdSend.Param_Num * 4);
        FifoPush(sendFifo, CmdHexGetCheckSum(cmdSend.Params, cmdSend.Param_Num * 4, hex_cmd, cmdSend.Param_Num));
    }
    return;

exit_CmdSimpleHexProcess:
    FifoPush(sendFifo, 0x55);
    return;
}


// 兼容二者命令模式的函数
void CmdAscAndHexRecv(uint8_t *buffer, tagFifoParam *sendFifo)
{
    uint32_t i, cmd_Len;
    CyBool_t found = CyFalse;
    uint8_t nCmdStatus;
    for (i = 0; i < BUFF_SIZE && !found; i++)
    {
        // HEX帧头
        if(buffer[i] == 0xAA)
        {
            CmdSimpleHexProcess(buffer + i, sendFifo);
            found = CyTrue;
        }
        else if ((buffer[i] >= 'a') && (buffer[i] <= 'z'))
        {
            found = CyTrue;
            nCmdStatus = CmdFrameGetRecvLen(buffer, i, &cmd_Len);
            if (ACK_OVER == nCmdStatus)
            {
                if (CyTrue == CmdFrameRecvFomat(buffer + i, cmd_Len))
                {
                    if (CyFalse == CmdFrameExecute(&cmdRecv, &cmdSend))
                    {
                        FifoPushNLen(sendFifo, "exe error", sizeof("exe error") - 1);
                    }
                    else if (CyFalse == CmdFrameSendFomat(sendFifo))
                    {
                        FifoPushNLen(sendFifo, "format error", sizeof("format error") - 1);
                    }
                }
                FifoPushNLen(sendFifo, "\r\n-->", sizeof("\r\n-->"));
            }
            else if (ACK_FRAME_ERR == nCmdStatus)
            {
                FifoPushNLen(sendFifo, "recv error\r\n-->", sizeof("recv error\r\n-->"));
            }
        }
    }
    return;
}
