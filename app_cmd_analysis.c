/*file
********************************************************************************
<PRE>
ģ����       : 
�ļ���       : 
����ļ�     : 
�ļ�ʵ�ֹ��� : ������������ĺ���
����         : 

--------------------------------------------------------------------------------
��ע         : 

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

#include "app_cmd_analysis.h"
#include "fx3_common.h"
#include <cyu3system.h>

static tagCmdFormatterContent cmdRecv __attribute__((aligned(32)));
static tagCmdFormatterContent cmdSend __attribute__((aligned(32)));
// �Ƿ��У��,��char���յ�ģʽ��,���Ϊ1��ôcharģʽ�����һ�����ݲ���Ϊǰ�����е��ֽںͣ��������������
char bCharModeIsCheck = 0;

/*function
********************************************************************************
<PRE>
������   : 
����     :�ѻ������ڵ����ݽ����������ʽ
����     : pContent ������������������
		pbuffer Ҫ׼���������Ļ�����
		nlen �����������ݳ���
����ֵ   : 
�׳��쳣 : 
--------------------------------------------------------------------------------
��ע     : 
�����÷� : 
--------------------------------------------------------------------------------
����     : 
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
    // asc mode�´�дת��ΪСд
    for (i = 0; i < nLen; i++)
    {
        if (pBuffer[i] >= 'A' && pBuffer[i] <= 'Z')
        {
            pBuffer[i] += ('a' - 'A');
        }
    }

	//--------------��ʼ����asc��ĸ�ʽ------------------
	// -----------------��1���Ȱѻ������������зָ�������Ϊ0
    for (nPos = 0; nPos < nLen; nPos++)
    {
		if(pBuffer[nPos] == ' ')
		{
			pBuffer[nPos] = 0;
		}
	}
	// -----------------��2 ����ȡ����ͷ������start��endλ�ú���ȡ
  	nPos = 0;
	nAscStart = nPos;
    while (nPos < nLen) // ��������ǰ���0
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
    while (nPos < nLen) // ��ʼ���������
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
    // -----------------��3 ����ȡ������������start��endλ�ú���ȡ
    while (nPos < nLen)
    {
		// ����֡β�ͽ���
        if ((pBuffer[nPos] == '\r') || (pBuffer[nPos] == '\n'))
        {
			bCharModeIsCheck= 0;
			break;	
		}
        else if (pBuffer[nPos] == '\t')
        {
            bCharModeIsCheck = 1;
            break;
        } // '\t'�����ʾ���һ�����������У�����

        // �ҵ��ַ���ʼλ��
		nAscStart = nPos;
        while (nPos < nLen) // ��������ǰ���0
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
		//�ҵ��ַ�����λ��
		nAscEnd = nAscStart;
        while (nPos < nLen) // ��ʼ�����
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
            } // '\t'�����ʾ���һ�����������У�����

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

    if (bCharModeIsCheck == 1) // �ж�У�飬��û��\t��������ʱ���ùܸ�У��
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
������   : 
����     :��������ķ������ݽ��и�ʽ���������
����     : Uint8 *pBuffer,��ʽ��֮���ŵĻ�����
			int* pBufferLen,��ʽ��֮���ַ�����
			tagCmdFormatterContent *pContent    Ҫ���д�������ݸ�ʽ������
		
����ֵ   : 
�׳��쳣 : 
--------------------------------------------------------------------------------
��ע     : 
�����÷� : 
--------------------------------------------------------------------------------
����     : 
</PRE>
*******************************************************************************/
static CyBool_t inline CmdFrameSendFomat(tagFifoParam *SendFifo)
{
    int i, nLen;
    int nPosIn = 0;
    int nPosOut = 0;
    uint8_t *pChar;
    uint8_t pTempBuffer[12]; // ����ַ�ת���������ʱ������
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
    // ��ŷ������ַ�ͷ
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

    if (bCharModeIsCheck == 1) // �������ҪУ���ģʽ�Ļ������ٷ���У�飬�����'\t'
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


// ����У���
uint8_t CmdSimpleHexGetCheckSum(uint8_t *pBuffer, uint32_t nLen)
{
    uint32_t n = 0, i = 0;
    for (i = 0; i < nLen; i++)
        n += pBuffer[i];
    return (n & 0xFF);
}

// ����У���
uint8_t CmdHexGetCheckSum(uint8_t *pBuffer, uint32_t nLen, uint8_t cmd_hex, uint8_t parNum)
{
    uint32_t n = 0, i = 0;
    for (i = 0; i < nLen; i++)
        n += pBuffer[i];
    n += cmd_hex;
    n += parNum;
    return (n & 0xFF);
}

// �����ַ�������
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

// ������յ������ַ���
void CmdSimpleHexProcess(uint8_t *pBuffer, tagFifoParam *sendFifo)
{
    uint32_t hex_cmd = pBuffer[1];
    uint32_t nLen = pBuffer[2];
    nLen = nLen * 4 + 3;
    // �ж�У���Ƿ���ȷ
    if (pBuffer[nLen] != CmdSimpleHexGetCheckSum(pBuffer + 1, nLen - 1))
    {
		goto exit_CmdSimpleHexProcess;
	}
//    CyU3PDebugPrint(4,"\npc CheckSum = %d",pBuffer[nLen]);
//    CyU3PDebugPrint(4,"\nfx3 CheckSum = %d",CmdSimpleHexGetCheckSum(pBuffer + 1, nLen - 1));
//    CyU3PDebugPrint(4,"hex_cmd = %d\n",hex_cmd);
    CyU3PMemSet(&cmdRecv, 0, sizeof(cmdRecv));
    CyU3PMemSet(&cmdSend, 0, sizeof(cmdSend));

    cmdRecv.Param_Num = pBuffer[2];                // ��������
    CyU3PMemCopy(cmdRecv.Params, &pBuffer[3], nLen - 3); // copy��������

    // ִ������
    if (CyFalse == CmdHexExecute(&cmdRecv, &cmdSend, hex_cmd))
    {
		goto exit_CmdSimpleHexProcess;
	}

	//���ݷ������д���
	if(cmdSend.Param_Num == 0)
	{
        FifoPush(sendFifo, 0xAA);
	}	
	else // ��ȷ�Ķ�ȡ��������
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


// ���ݶ�������ģʽ�ĺ���
void CmdAscAndHexRecv(uint8_t *buffer, tagFifoParam *sendFifo)
{
    uint32_t i, cmd_Len;
    CyBool_t found = CyFalse;
    uint8_t nCmdStatus;
    for (i = 0; i < BUFF_SIZE && !found; i++)
    {
        // HEX֡ͷ
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
