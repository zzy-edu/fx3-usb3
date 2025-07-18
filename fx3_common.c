/*file
********************************************************************************
<PRE>
模块名       : 
文件名       : 
相关文件     : 
文件实现功能 : 定义公用的一些函数
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

#include "fx3_common.h"
#include "fx3_pin_define.h"
#include "cyu3os.h"
#include "cyu3gpio.h"

/* Application Error Handler */
void CyFxAppErrorHandler(
    CyU3PReturnStatus_t apiRetStatus /* API return status */
)
{
    /* Application failed with the error code apiRetStatus */

    /* Add custom debug or recovery actions here */

    /* Loop Indefinitely */
    for (;;)
    {
        /* Thread sleep : 100 ms */
        CyU3PGpioSetValue(FX3_LED_PIN, CyTrue);
        CyU3PThreadSleep(50);
        CyU3PGpioSetValue(FX3_LED_PIN, CyFalse);
        CyU3PThreadSleep(50);
    }
}

void DebugLed(int led_frequency_s)
{
	while(1)
	{
	    CyU3PGpioSetValue(FX3_LED_PIN, CyTrue);
	    CyU3PThreadSleep(led_frequency_s);
	    CyU3PGpioSetValue(FX3_LED_PIN, CyFalse);
	    CyU3PThreadSleep(led_frequency_s);
	}
}

// 整型数据转换为字符串，并记录字符串长度包括结尾的字符'\0'
// pdata 输入数据
// pref  预先开辟好的缓冲区，填入字符,至少11个字节位置
// nlength  字符串的实际长度，包括0结尾
void Long2Char(uint32_t r_pData, uint8_t *r_pRef, uint8_t *r_nLength)
{
    int i;
    uint8_t n;
    //	sprintf(r_pRef,"0x%08X",r_pData);
    if (r_nLength)
    {
        *r_nLength = 10;
    }

    r_pRef[0] = '0';
    r_pRef[1] = 'x';
    //r_pRef[10] = ' ';

    //填充数字
    for (i = 0; i < 8; i++)
    {
        n = (r_pData >> (i * 4)) & 0xF;
        if (n <= 9)
        {
            n = n + '0';
        }
        else
        {
            n = n - 10 + 'A';
        }

        r_pRef[9 - i] = n;
    }
    return;
}

void Char2Long(uint8_t *m_ref, uint32_t *data)
{
    uint8_t count = 0;
    *data = 0;
    uint8_t CharBuffer[9] = {0};
    // 先判断是那种数据类型输入
    // 0x开头的为16进制
    if (m_ref[0] == '0' && (m_ref[1] == 'x' || m_ref[1] == 'X'))
    {
        m_ref += 2; // 移动到数据处
        //遇到字符串结尾则退出循环
        while (m_ref[count])
        {
            //如果处理的字符超过8个，已经超过long型的最大值，报错
            if (count >= 8)
            {
                *data = 0;
                return;
            }
            //若待处理的字符不是0~F，则返回错误，否则进行处理
            if (m_ref[count] >= '0' && m_ref[count] <= '9')
            {
                *data *= 16;
                *data += m_ref[count] - '0';
            }
            else if (m_ref[count] >= 'A' && m_ref[count] <= 'F') // A~F
            {
                *data *= 16;
                *data += m_ref[count] - 'A' + 10;
            }
            else if (m_ref[count] >= 'a' && m_ref[count] <= 'f') // a~f
            {
                *data *= 16;
                *data += m_ref[count] - 'a' + 10;
            }
            else
            {
                *data = 0;
                return;
            }
            count++;
        }
    }
    else // 否则为十进制数处理
    {
        //遇到字符串结尾则退出循环
        while (m_ref[count])
        {
            //如果处理的字符超过10个，已经超过long型的最大值，报错
            if (count >= 10)
            {
                *data = 0;
                return;
            }
            //若待处理的字符是大小写字符 则转化成ASIC值存起来
            if ((m_ref[count] >= 97) && (m_ref[count] <= 122))
            {
                *data = *data + (uint32_t)m_ref[count] << (8 * count);
                CharBuffer[count] = m_ref[count];
                CharBuffer[count] = m_ref[count] - ('a' - 'A');
            }
            else if ((m_ref[count] >= 65) && (m_ref[count] <= 90))
            {
                *data = *data + (uint32_t)m_ref[count] << (8 * count);
                CharBuffer[count] = m_ref[count];
            }
            else if ((m_ref[count] >= 48) && (m_ref[count] <= 57))
            {
                *data *= 10;
                *data += m_ref[count] - 0x30;
                CharBuffer[count] = m_ref[count];
            }
            else
            {
                *data = 0;
                return;
            }
            count++;
        }
    }

    //temp = (float)MAX_DEC - (float)count;
    //*data *= pow(10,temp) ;

    return;
}
