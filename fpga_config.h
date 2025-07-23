#ifndef _FPGA_CONFIG_H_
#define _FPGA_CONFIG_H_
#include "cyu3types.h"


#define FPGA_REG_ADDR (0x0001);
/*NOTE: */
#define FPGA_VERSION1_REG_ADDRESS 					(0x0000) //FPGA����汾:�ɼ��豸����
#define FPGA_VERSION2_REG_ADDRESS 					(0x0001) //FPGA����汾��������
#define FPGA_STATUS_REG_ADDRESS						(0x0003) //FPGA״̬

/*NOTE: */
#define CL0_FVAL_CNT_REG_ADDRESS 					(0x0018) //0x0018~0x0019 CL0 FVAL�½��ؼ���
#define CL0_LVAL_CNT_REG_ADDRESS 					(0x001A) //0x001A~0x001B CL0 LVAL�½��ؼ���(LVAL��DVAL����)
#define CL0_CLK_CNT_REG_ADDRESS						(0x001E) //0x001E~0x001F CL0 CLK �½��ؼ���
#define CL1_FVAL_CNT_REG_ADDRESS 					(0x0020) //0x0020~0x0021 CL1 FVAL�½��ؼ���
#define CL1_LVAL_CNT_REG_ADDRESS 					(0x0022) //0x0022~0x0023 CL1 LVAL�½��ؼ���(LVAL��DVAL����)
#define CL1_CLK_CNT_REG_ADDRESS						(0x0026) //0x0026~0x0027 CL1 CLK �½��ؼ���
#define CC1_OUT_NUM_REG_ADDRESS						(0x0028) //0x0028~0x0029 CC1 ����½��ؼ���

/*NOTE: */
#define IMG_CNT_REG_ADDRESS 						(0x0030) //0x0030~0x0033 ֡��ż�������0��ʼһֱ�����������������㡣
#define PIXEL_FORMAT_REG_ADDRESS 					(0x1101) // ͼ��ĸ�ʽ
#define CHANNEL_NUM_REG_ADDRESS 					(0x1102) // ͼ�������ʵ��ͨ������1��2��4��5���仯��4����8��10��
#define DVAL_LVAL_MODE_REG_ADDRESS					(0x1103) //
#define TAP_NUM_REG_ADDRESS 						(0x1105) // �ɼ�ͼ���tap����֡ͷ64�ֽڣ�
#define TAP_MODE_REG_ADDRESS 						(0x1106) //
#define LINE_CLK_NUM_REG_ADDRESS					(0x1107) //0x1107~0x1108 һ�а�����clkʵ�����ݣ���ʵ��ͼ��ĺ���ֱ���/tap��
#define LINE_CNT_REG_ADDRESS 						(0x1109) //0x1109~0x110a ͼ�������
#define X_OFFSET_REG_ADDRESS 						(0x110B) //0x110b~0x110c ˮƽƫ����
#define Y_OFFSET_REG_ADDRESS						(0x110d) //0x110d~0x110e ��ֱƫ����
#define LINE_LEN_REG_ADDRESS						(0x110f) //0x1107~0x1108 ddrһ�еĳ���
#define AOI_WIDTH_0_REG_ADDRESS 					(0x1110) //0x1110~0x1111 ͼ��ʵ�ʿ��
#define AOI_HEIGHT_0_ADDRESS 						(0x1112) //0x1112~0x1113 ͼ��ʵ������
#define LEN_OF_RAW_ADDRESS							(0x1114) //0x1114~0x1115 ��ǰraw���ݲ����ֽڵ���Ŀ������Ч������ŵ��ϴ����ݵĴ�С

/*NOTE: */
#define TEST_S1_S2_REG_ADDRESS 						(0x3000) // S1���Ե� S2���Ե�
#define DDR_OUT_EN_REG_ADDRESS 						(0x3001) // ������������ʱ�����ƴ˶˿�
#define MAIN_FUNCTION_REG_ADDRESS 					(0x3010) // ��Ƭ������
#define TEST_PATTERN_REG_ADDRESS					(0x3011) // ����ͼģʽ
#define CC1_LOW_LEVEL_REG_ADDRESS 					(0x3015) // 0x3015~0x3016 cc1�͵�ƽ���ʱ�䣨100M��
#define CC1_HIGH_LEVEL_REG_ADDRESS 					(0x3017) // 0x3017~0x3018 cc1�ߵ�ƽ���ʱ�䣨100M��
#define CC1_NUM_REG_ADDRESS							(0x3019) // 0x3019~0x301a  �������
#define DEV_INDEX_REG_ADDRESS 						(0x3302) // ͼ��ɼ����豸���

/*NOTE: */
#define FVAL_STTVALUE_CURRENT_REG_ADDRESS_1			(0x0064) // 0x0064~0x0083
#define FVAL_STTVALUE_CURRENT_REG_ADDRESS_2			(0x00A0) // 0x00A0~0x00B3
#define FVAL_STTVALUE_MAX_REG_ADDRESS				(0x00C8) // 0x00C8~0x00E7
#define FVAL_STTVALUE_MIN_REG_ADDRESS				(0x012C) // 0x012C~0x015F
#define FVAL_STTVALUE_FLUCTUATE_REG_ADDRESS			(0x0190) // 0x0190~0x01C3


/*function
********************************************************************************
<PRE>
������   : 
����     :fpga��ͨѶ��ʼ��
����     : 
����ֵ   :  
�׳��쳣 : 
--------------------------------------------------------------------------------
��ע     : 
�����÷� : 
--------------------------------------------------------------------------------
����     : 
</PRE>
*******************************************************************************/
CyBool_t fpga_reg_init(void);
/*function
********************************************************************************
<PRE>
������   : 
����     :��ȡfpga�ڲ��Ĵ�����ֵ
����     : 
		nAddr �Ĵ�����ַ15bit��Ч��ַ,���bit : 1 д0��
		pData 16bit��Ч����ֵָ��
		nLen  ���ݳ��ȣ�16bitΪһ����Ԫ
����ֵ   :  
�׳��쳣 : 
--------------------------------------------------------------------------------
��ע     : 
�����÷� : 
--------------------------------------------------------------------------------
����     : 
</PRE>
*******************************************************************************/
void fpga_reg_read(uint16_t startAddr, uint16_t *pData, uint16_t len);

/*function
********************************************************************************
<PRE>
������   : 
����     :д��fpga�ڲ��Ĵ�����ֵ
����     : 
		nAddr �Ĵ�����ַ15bit��Ч��ַ,���bit : 1 д0��
		pData 16bit��Ч����ֵָ��
		nLen  ���ݳ��ȣ�16bitΪһ����Ԫ
����ֵ   :  
�׳��쳣 : 
--------------------------------------------------------------------------------
��ע     : 
�����÷� : 
--------------------------------------------------------------------------------
����     : 
</PRE>
*******************************************************************************/
void fpga_reg_write(uint16_t startAddr, uint16_t *pData, uint16_t len);

CyBool_t fpga_reg_test(void);

CyBool_t fpga_init(void);
#endif
