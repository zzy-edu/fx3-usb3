/*file
********************************************************************************
<PRE>
ģ����       : 
�ļ���       : 
����ļ�     : 
�ļ�ʵ�ֹ��� : ���幫�õ�һЩ����
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

// ��������ת��Ϊ�ַ���������¼�ַ������Ȱ�����β���ַ�'\0'
// pdata ��������
// pref  Ԥ�ȿ��ٺõĻ������������ַ�,����11���ֽ�λ��
// nlength  �ַ�����ʵ�ʳ��ȣ�����0��β
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

    //�������
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
    // ���ж�������������������
    // 0x��ͷ��Ϊ16����
    if (m_ref[0] == '0' && (m_ref[1] == 'x' || m_ref[1] == 'X'))
    {
        m_ref += 2; // �ƶ������ݴ�
        //�����ַ�����β���˳�ѭ��
        while (m_ref[count])
        {
            //���������ַ�����8�����Ѿ�����long�͵����ֵ������
            if (count >= 8)
            {
                *data = 0;
                return;
            }
            //����������ַ�����0~F���򷵻ش��󣬷�����д���
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
    else // ����Ϊʮ����������
    {
        //�����ַ�����β���˳�ѭ��
        while (m_ref[count])
        {
            //���������ַ�����10�����Ѿ�����long�͵����ֵ������
            if (count >= 10)
            {
                *data = 0;
                return;
            }
            //����������ַ��Ǵ�Сд�ַ� ��ת����ASICֵ������
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
