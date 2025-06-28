/*file
********************************************************************************
<PRE>
ģ����       : 
�ļ���       : 
����ļ�     : 
�ļ�ʵ�ֹ��� : �ɼ�ϵͳ�Ĳ����洢�����ù���
����         : 

--------------------------------------------------------------------------------
��ע         : 

		�ⲿ���÷���
--------------------------------------------------------------------------------
�޸ļ�¼ : 
�� ��:      2025.6.24
�汾 :		v0.0
�޸���:
�޸����� :

</PRE>
********************************************************************************

* ��Ȩ����(c) , <>, ��������Ȩ��
*******************************************************************************/

#include "app_grab_cfg.h"
#include "mcu_spi.h"
#include "fpga_config.h"
#include "cyu3system.h"

// ȫ��ʹ�õĲɼ�����
tag_grab_config grabconfParam = GRAB_PARAM_DEFAULT_VALUE;
uint32_t grabsysStatus = 0;


/*function
********************************************************************************
<PRE>
������   :
����     : �洢/��ȡ�û����������ݣ�����Ĭ�ϼ����û����������ĸ�
����     :
	tagCameraParam *pParam �����洢�Ľṹ�嶨��
		nIndex  �û������Ĵ洢��0~..
����ֵ   :
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
CyBool_t GrabWriteUserParam(tag_grab_config *pParam, int nIndex)//�洢��ǰ�������û���
{
	if(nIndex >= GRAB_SYSTEM_MODE_NUMBER)
	{
		/*
		 * ����ģʽ�ĸ����Ĵ����߼�
		 */
		return CyFalse;
	}

	pParam->header = PARAM_VALID_HCODE;
	pParam->tailer = PARAM_VALID_TCODE;
	if(CyFalse == MCUFlashEraseAndWrite(0,(uint8_t*)pParam,GRAB_MODE1_PARAM_ADDR(nIndex),sizeof(tag_grab_config)))
	{
		return CyFalse;
	}
	return CyTrue;
}

CyBool_t GrabReadUserParam(tag_grab_config *pParam, int nIndex)//��ȡ��ǰ�û�������
{
	uint64_t header = 0;
	if(nIndex >= GRAB_SYSTEM_MODE_NUMBER)
	{
		/*
		 * ����ģʽ�ĸ����Ĵ����߼�
		 */
		return CyFalse;
	}
	if(CyFalse == MCUSpiFlashRead(0,GRAB_MODE1_PARAM_ADDR(nIndex),(uint8_t*)&header,8))
	{
		return CyFalse;
	}

	if(header != PARAM_VALID_HCODE)
	{
		CyU3PDebugPrint(4,"\nheader error, return Default");
		CyU3PMemCopy((uint8_t*)pParam,(uint8_t*)(&grabconfParam),sizeof(tag_grab_config)-8);
		return CyTrue;
	}
	else
	{
		if(CyTrue == MCUSpiFlashRead(0,GRAB_MODE1_PARAM_ADDR(nIndex),(uint8_t*)pParam,sizeof(tag_grab_config)))
		{
			if(pParam->tailer != PARAM_VALID_TCODE)
			{
				CyU3PDebugPrint(4,"\ntailer error, return Default");
				CyU3PMemCopy((uint8_t*)pParam,(uint8_t*)(&grabconfParam),sizeof(tag_grab_config)-8);
				return CyTrue;
			}
		}
		else
		{
			return CyFalse;
		}
	}
	return CyTrue;
}


/*function
********************************************************************************
<PRE>
������   :
����     : ��ȡָ��ģʽ�µ��û�����������Ϣ�������˼·���ϵ���Ҫ�ȶ�fpga�ĳ���汾��ȷ��nIndex
����     : void
����ֵ   : flash��Ĳ������Ҷ�ȡ�ɹ� 	����CyTrue
		flash��Ĳ������Ի��ȡʧ��	����CyFalse
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
CyBool_t GrabGetDefaultUserParam(void)
{
	//TODO
	return CyTrue;
}

/*function
********************************************************************************
<PRE>
������   :
����     : ����ϵͳ�Ƶ�״̬����Ҫ���ⲿ�̵߳���
����     :
	   uint32_t *status ״ֵָ̬��
����ֵ   : �ɹ� CyTrue ʧ��CyFalse
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
CyBool_t GrabGetSystemStatus(uint32_t *status)
{
	//TODO
	return CyTrue;
}


/*function
********************************************************************************
<PRE>
������   :
����     : ����λ�������������ò����ͱ��ص����Ƚ�
����     :
	tagCameraParam PcParam ��λ���´������ò���
����ֵ   :
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
CyBool_t GrabParamCompareandSet(tag_grab_config PcParam)
{
	if(PcParam.header != PARAM_VALID_HCODE) return CyFalse;
	//Todo ʵ��ˢfpga�Ĵ���
	if(grabconfParam.n_dev_index != PcParam.n_dev_index){grabconfParam.n_dev_index = PcParam.n_dev_index;}
	if(grabconfParam.n_pixel_format != PcParam.n_pixel_format){grabconfParam.n_pixel_format = PcParam.n_pixel_format; }
	if(grabconfParam.n_tap_num != PcParam.n_tap_num){grabconfParam.n_tap_num = PcParam.n_tap_num; /*fpga_reg_write*/}
	if(grabconfParam.n_tap_mode != PcParam.n_tap_mode){grabconfParam.n_tap_mode = PcParam.n_tap_mode; /*fpga_reg_write*/}
	if(grabconfParam.n_width != PcParam.n_width){grabconfParam.n_width = PcParam.n_width; /*fpga_reg_write*/}
	if(grabconfParam.n_height != PcParam.n_height){grabconfParam.n_height = PcParam.n_height; /*fpga_reg_write*/}
	if(grabconfParam.n_len_of_raw != PcParam.n_len_of_raw){grabconfParam.n_len_of_raw = PcParam.n_len_of_raw; /*fpga_reg_write*/}
	if(grabconfParam.n_device_type != PcParam.n_device_type){grabconfParam.n_device_type = PcParam.n_device_type; /*fpga_reg_write*/}
	if(grabconfParam.nBitCount != PcParam.nBitCount){grabconfParam.nBitCount = PcParam.nBitCount;}
	if(grabconfParam.n_cap_channel_num != PcParam.n_cap_channel_num){grabconfParam.n_cap_channel_num = PcParam.n_cap_channel_num; /*fpga_reg_write*/}
	if(grabconfParam.n_dval_lval_mode != PcParam.n_dval_lval_mode){grabconfParam.n_dval_lval_mode = PcParam.n_dval_lval_mode; /*fpga_reg_write*/}
	if(grabconfParam.n_line_clk_num != PcParam.n_line_clk_num){grabconfParam.n_line_clk_num = PcParam.n_line_clk_num; /*fpga_reg_write*/}
	if(grabconfParam.n_line_cnt != PcParam.n_line_cnt){grabconfParam.n_line_cnt = PcParam.n_line_cnt; /*fpga_reg_write*/}
	if(grabconfParam.n_x_offset != PcParam.n_x_offset){grabconfParam.n_x_offset = PcParam.n_x_offset; /*fpga_reg_write*/}
	if(grabconfParam.n_y_offset != PcParam.n_y_offset){grabconfParam.n_y_offset = PcParam.n_y_offset; /*fpga_reg_write*/}
	if(grabconfParam.n_fval_set_value != PcParam.n_fval_set_value){grabconfParam.n_fval_set_value = PcParam.n_fval_set_value;}
	if(grabconfParam.n_test_mode != PcParam.n_test_mode){grabconfParam.n_test_mode = PcParam.n_test_mode; /*fpga_reg_write*/}
	if(grabconfParam.n_cc1_pwm_high != PcParam.n_cc1_pwm_high){grabconfParam.n_cc1_pwm_high = PcParam.n_cc1_pwm_high; /*fpga_reg_write*/}
	if(grabconfParam.n_cc1_pwm_low != PcParam.n_cc1_pwm_low){grabconfParam.n_cc1_pwm_low = PcParam.n_cc1_pwm_low; /*fpga_reg_write*/}
	if(grabconfParam.n_cc1_pwm_cnt != PcParam.n_cc1_pwm_cnt){grabconfParam.n_cc1_pwm_cnt = PcParam.n_cc1_pwm_cnt; /*fpga_reg_write*/}
	if(grabconfParam.n_cc1_pwm_current != PcParam.n_cc1_pwm_current){grabconfParam.n_cc1_pwm_current = PcParam.n_cc1_pwm_current; /*fpga_reg_write*/}
	if(grabconfParam.S1_sel != PcParam.S1_sel){grabconfParam.S1_sel = PcParam.S1_sel; /*fpga_reg_write*/}
	if(grabconfParam.S2_sel != PcParam.S2_sel){grabconfParam.S2_sel = PcParam.S2_sel; /*fpga_reg_write*/}

	return CyTrue;
}
