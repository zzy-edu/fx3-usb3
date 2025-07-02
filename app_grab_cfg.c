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

// ϵͳ��״̬
uint32_t grabsysStatus = 0;
uint32_t cl0_fval_cnt = 0;
uint32_t cl0_lval_cnt = 0;
uint32_t cl0_clk_cnt = 0;
uint32_t cl1_fval_cnt = 0;
uint32_t cl1_lval_cnt = 0;
uint32_t cl1_clk_cnt = 0;


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
CyBool_t GrabWriteUserParam(tag_grab_config *pParam, uint8_t nIndex)//�洢��ǰ�������û���
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

CyBool_t GrabReadUserParam(tag_grab_config *pParam, uint8_t nIndex)//��ȡ��ǰ�û�������
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
		return CyFalse;
	}
	else
	{
		if(CyTrue == MCUSpiFlashRead(0,GRAB_MODE1_PARAM_ADDR(nIndex),(uint8_t*)pParam,sizeof(tag_grab_config)))
		{
			if(pParam->tailer != PARAM_VALID_TCODE)
			{
				CyU3PDebugPrint(4,"\ntailer error, return Default");
				return CyFalse;
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
	uint8_t nIndex = 0;
	uint16_t fpga_version = 0;
	tag_grab_config *tmp = NULL;

	fpga_reg_read(FPGA_VERSION1_REG_ADDRESS,&fpga_version,1);
	CyU3PDebugPrint(4,"\nfpga_version = %d", fpga_version);
	switch(fpga_version)
	{
	case 0:
		nIndex = 0;
		if(CyTrue == GrabReadUserParam(tmp,nIndex))
		{
			CyU3PDebugPrint(4,"\nGrabReadUserParam %dnIndex ok",nIndex);
			CyU3PMemCopy((uint8_t*)(&grabconfParam),(uint8_t*)tmp,sizeof(tag_grab_config));
		}
		else
		{
			//��ȡ����ʧ�ܵĻ�����n_device_type���£�������λ��ˢ�µ���ȷ�Ľ��棬������������Ĭ��
			grabconfParam.n_device_type = nIndex;
			return CyFalse;
		}
		break;
	case 1:
		nIndex = 1;
		if(CyTrue == GrabReadUserParam(tmp,nIndex))
		{
			CyU3PDebugPrint(4,"\nGrabReadUserParam %dnIndex ok",nIndex);
			CyU3PMemCopy((uint8_t*)(&grabconfParam),(uint8_t*)tmp,sizeof(tag_grab_config));
		}
		else
		{
			//��ȡ����ʧ�ܵĻ�����n_device_type���£�������λ��ˢ�µ���ȷ�Ľ��棬������������Ĭ��
			grabconfParam.n_device_type = nIndex;
			return CyFalse;
		}
		break;
	case 2:
		nIndex = 2;
		if(CyTrue == GrabReadUserParam(tmp,nIndex))
		{
			CyU3PDebugPrint(4,"\nGrabReadUserParam %dnIndex ok",nIndex);
			CyU3PMemCopy((uint8_t*)(&grabconfParam),(uint8_t*)tmp,sizeof(tag_grab_config));
		}
		else
		{
			//��ȡ����ʧ�ܵĻ�����n_device_type���£�������λ��ˢ�µ���ȷ�Ľ��棬������������Ĭ��
			grabconfParam.n_device_type = nIndex;
			return CyFalse;
		}
		break;
	case 3:
		nIndex = 3;
		if(CyTrue == GrabReadUserParam(tmp,nIndex))
		{
			CyU3PDebugPrint(4,"\nGrabReadUserParam %dnIndex ok",nIndex);
			CyU3PMemCopy((uint8_t*)(&grabconfParam),(uint8_t*)tmp,sizeof(tag_grab_config));
		}
		else
		{
			//��ȡ����ʧ�ܵĻ�����n_device_type���£�������λ��ˢ�µ���ȷ�Ľ��棬������������Ĭ��
			grabconfParam.n_device_type = nIndex;
			return CyFalse;
		}
		break;
	default:
		break;
	}
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
void GrabGetSystemStatus(void)
{
	//TODO
	uint32_t cur_value = 0;
	/* ��cl0_fval_cnt */
	fpga_reg_read(CL0_FVAL_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value != cl0_fval_cnt)
	{
		SET_BIT(grabsysStatus,2);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,2);
	}
	cl0_fval_cnt = cur_value;

	/* ��cl0_lval_cnt */
	fpga_reg_read(CL0_LVAL_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value != cl0_lval_cnt)
	{
		SET_BIT(grabsysStatus,1);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,1);
	}
	cl0_lval_cnt = cur_value;

	/* ��cl0_clk_cnt */
	fpga_reg_read(CL0_CLK_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value > cl0_clk_cnt)
	{
		if(cur_value - cl0_clk_cnt >= CLK_NORM_CNT)
		{
			SET_BIT(grabsysStatus,0);
		}
		else
		{
			CLEAR_BIT(grabsysStatus,0);
		}
		cl0_lval_cnt = cur_value;
	}


	/* ��cl1_fval_cnt */
	fpga_reg_read(CL1_FVAL_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value != cl0_fval_cnt)
	{
		SET_BIT(grabsysStatus,5);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,5);
	}
	cl1_fval_cnt = cur_value;

	/* ��cl1_lval_cnt */
	fpga_reg_read(CL1_LVAL_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value != cl1_lval_cnt)
	{
		SET_BIT(grabsysStatus,4);

	}
	else
	{
		CLEAR_BIT(grabsysStatus,5);
	}
	cl1_lval_cnt = cur_value;

	/* ��cl1_clk_cnt */
	fpga_reg_read(CL1_CLK_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value > cl1_clk_cnt)
	{
		if(cur_value - cl1_clk_cnt >= CLK_NORM_CNT)
		{
			SET_BIT(grabsysStatus,3);
		}
		else
		{
			CLEAR_BIT(grabsysStatus,3);
		}
		cl1_clk_cnt = cur_value;
	}
}


/*function
********************************************************************************
<PRE>
������   :
����     : ����λ�������������ò����ͱ��ص����Ƚ�,���üĴ���
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
CyBool_t GrabParamCompareandSet(tag_grab_config *PcParam)
{
	uint16_t tmp = 0;
	uint8_t *ptmp = NULL;
	if(PcParam->header != PARAM_VALID_HCODE) return CyFalse;
	//Todo ʵ��ˢfpga�Ĵ���

	/* n_dev_index */
	if(grabconfParam.n_dev_index != PcParam->n_dev_index)
	{
		grabconfParam.n_dev_index = PcParam->n_dev_index;
		/* fpga_reg_write */
		CyU3PMemSet((uint8_t*)(&tmp),0,2);
		CyU3PMemCopy((uint8_t*)(&tmp),&grabconfParam.n_dev_index,1);
		fpga_reg_write(DEV_INDEX_REG_ADDRESS,&tmp,1);
	}

	/* n_pixel_format */
	if(grabconfParam.n_pixel_format != PcParam->n_pixel_format)
	{
		grabconfParam.n_pixel_format = PcParam->n_pixel_format;
		/* fpga_reg_write */
		CyU3PMemSet((uint8_t*)(&tmp),0,2);
		CyU3PMemCopy((uint8_t*)(&tmp),&grabconfParam.n_pixel_format,1);
		fpga_reg_write(PIXEL_FORMAT_REG_ADDRESS,&tmp,1);
	}

	/* n_tap_num */
	if(grabconfParam.n_tap_num != PcParam->n_tap_num)
	{
		grabconfParam.n_tap_num = PcParam->n_tap_num;
		/* fpga_reg_write */
		CyU3PMemSet((uint8_t*)(&tmp),0,2);
		CyU3PMemCopy((uint8_t*)(&tmp),&grabconfParam.n_tap_num,1);
		fpga_reg_write(TAP_NUM_REG_ADDRESS,&tmp,1);
	}

	/* n_tap_mode */
	if(grabconfParam.n_tap_mode != PcParam->n_tap_mode)
	{
		grabconfParam.n_tap_mode = PcParam->n_tap_mode;
		/* fpga_reg_write */
		CyU3PMemSet((uint8_t*)(&tmp),0,2);
		CyU3PMemCopy((uint8_t*)(&tmp),&grabconfParam.n_tap_mode,1);
		fpga_reg_write(TAP_MODE_REG_ADDRESS,&tmp,1);
	}

	/* n_width */
	if(grabconfParam.n_width != PcParam->n_width)
	{
		grabconfParam.n_width = PcParam->n_width;
		/* fpga_reg_write */
		fpga_reg_write(AOI_WIDTH_0_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_width),2);
	}

	/* n_height */
	if(grabconfParam.n_height != PcParam->n_height)
	{
		grabconfParam.n_height = PcParam->n_height;
		/*fpga_reg_write*/
		fpga_reg_write(AOI_HEIGHT_0_ADDRESS,(uint16_t *)(&grabconfParam.n_height),2);
	}

	/* n_len_of_raw */
	if(grabconfParam.n_len_of_raw != PcParam->n_len_of_raw)
	{
		grabconfParam.n_len_of_raw = PcParam->n_len_of_raw;
		/*fpga_reg_write*/
		fpga_reg_write(LEN_OF_RAW_ADDRESS,(uint16_t *)(&grabconfParam.n_len_of_raw),2);
	}

	/* n_device_type */
	if(grabconfParam.n_device_type != PcParam->n_device_type)
	{
		grabconfParam.n_device_type = PcParam->n_device_type;
		/*fpga_reg_write*/
		CyU3PMemSet((uint8_t*)(&tmp),0,2);
		CyU3PMemCopy((uint8_t*)(&tmp),&grabconfParam.n_device_type,1);
		fpga_reg_write(FPGA_VERSION1_REG_ADDRESS,&tmp,1);
	}
	/* nBitCount */
	if(grabconfParam.nBitCount != PcParam->nBitCount){grabconfParam.nBitCount = PcParam->nBitCount;}

	/* n_cap_channel_num */
	if(grabconfParam.n_cap_channel_num != PcParam->n_cap_channel_num)
	{
		grabconfParam.n_cap_channel_num = PcParam->n_cap_channel_num;
		/*fpga_reg_write*/
		CyU3PMemSet((uint8_t*)(&tmp),0,2);
		CyU3PMemCopy((uint8_t*)(&tmp),&grabconfParam.n_cap_channel_num,1);
		fpga_reg_write(CHANNEL_NUM_REG_ADDRESS,&tmp,1);
	}

	/* n_dval_lval_mode */
	if(grabconfParam.n_dval_lval_mode != PcParam->n_dval_lval_mode)
	{
		grabconfParam.n_dval_lval_mode = PcParam->n_dval_lval_mode;
		/*fpga_reg_write*/
		CyU3PMemSet((uint8_t*)(&tmp),0,2);
		CyU3PMemCopy((uint8_t*)(&tmp),&grabconfParam.n_dval_lval_mode,1);
		fpga_reg_write(DVAL_LVAL_MODE_REG_ADDRESS,&tmp,1);
	}

	/* n_line_clk_num */
	if(grabconfParam.n_line_clk_num != PcParam->n_line_clk_num)
	{
		grabconfParam.n_line_clk_num = PcParam->n_line_clk_num;
		/*fpga_reg_write*/
		fpga_reg_write(LINE_CLK_NUM_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_line_clk_num),2);
	}

	/* n_line_cnt */
	if(grabconfParam.n_line_cnt != PcParam->n_line_cnt)
	{
		grabconfParam.n_line_cnt = PcParam->n_line_cnt; /*fpga_reg_write*/
		fpga_reg_write(LINE_CNT_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_line_cnt),2);
	}

	/* n_x_offset */
	if(grabconfParam.n_x_offset != PcParam->n_x_offset)
	{
		grabconfParam.n_x_offset = PcParam->n_x_offset;
		/*fpga_reg_write*/
		fpga_reg_write(X_OFFSET_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_x_offset),2);
	}

	/* n_y_offset */
	if(grabconfParam.n_y_offset != PcParam->n_y_offset)
	{
		grabconfParam.n_y_offset = PcParam->n_y_offset;
		/*fpga_reg_write*/
		fpga_reg_write(Y_OFFSET_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_y_offset),2);
	}

	/* n_fval_set_value */
	if(grabconfParam.n_fval_set_value != PcParam->n_fval_set_value){grabconfParam.n_fval_set_value = PcParam->n_fval_set_value;}

	/* n_ddr_line_bytes */
	if(grabconfParam.n_ddr_line_bytes != PcParam->n_ddr_line_bytes)
	{
		grabconfParam.n_ddr_line_bytes = PcParam->n_ddr_line_bytes;
		/*fpga_reg_write*/
		fpga_reg_write(LINE_LEN_REG_ADDRESS,&grabconfParam.n_ddr_line_bytes,1);
	}

	/* n_cc1_pwm_high */
	if(grabconfParam.n_cc1_pwm_high != PcParam->n_cc1_pwm_high)
	{
		grabconfParam.n_cc1_pwm_high = PcParam->n_cc1_pwm_high;
		/*fpga_reg_write*/
		fpga_reg_write(CC1_HIGH_LEVEL_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_cc1_pwm_high),2);
	}

	/* n_cc1_pwm_low */
	if(grabconfParam.n_cc1_pwm_low != PcParam->n_cc1_pwm_low)
	{
		grabconfParam.n_cc1_pwm_low = PcParam->n_cc1_pwm_low;
		/*fpga_reg_write*/
		fpga_reg_write(CC1_LOW_LEVEL_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_cc1_pwm_low),2);
	}

	/* n_cc1_pwm_cnt */
	if(grabconfParam.n_cc1_pwm_cnt != PcParam->n_cc1_pwm_cnt)
	{
		grabconfParam.n_cc1_pwm_cnt = PcParam->n_cc1_pwm_cnt;
		/* fpga_reg_write */
		fpga_reg_write(CC1_NUM_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_cc1_pwm_cnt),2);
	}

	/* n_cc1_pwm_current */
	if(grabconfParam.n_cc1_pwm_current != PcParam->n_cc1_pwm_current)
	{
		grabconfParam.n_cc1_pwm_current = PcParam->n_cc1_pwm_current;
		/*fpga_reg_write*/
		fpga_reg_write(CC1_OUT_NUM_REG_ADDRESS,(uint16_t *)(&grabconfParam.n_cc1_pwm_current),2);
	}

	/* S1_sel  S2_sel */
	if((grabconfParam.S1_sel != PcParam->S1_sel) || (grabconfParam.S2_sel != PcParam->S2_sel))
	{
		grabconfParam.S1_sel = PcParam->S1_sel;
		grabconfParam.S2_sel = PcParam->S2_sel;
		/* fpga_reg_write */
		CyU3PMemSet((uint8_t*)(&tmp),0,2);
		ptmp = (uint8_t *)(&tmp);
		ptmp[0] = grabconfParam.S1_sel;
		ptmp[1] = grabconfParam.S2_sel;
		fpga_reg_write(TEST_S1_S2_REG_ADDRESS,&tmp,1);
	}

	return CyTrue;
}

/*function
********************************************************************************
<PRE>
������   :
����     : ��һ��Ĵ��������±��صĲ���
����     :
	void
����ֵ   :
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
void GrabParamUpdate(void)
{
	uint8_t *ptmp = NULL;
	uint16_t tmp16Bit = 0;
	uint32_t tmp32Bit = 0;
	uint32_t tmp32BitArray[30] = {0};

	/* n_dev_index */
	fpga_reg_read(DEV_INDEX_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_dev_index = *(uint8_t *)(&tmp16Bit);
	/* n_pixel_format */
	fpga_reg_read(PIXEL_FORMAT_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_pixel_format = *(uint8_t *)(&tmp16Bit);

	/* n_tap_num */
	fpga_reg_read(TAP_NUM_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_tap_num = *(uint8_t *)(&tmp16Bit);

	/* n_tap_mode */
	fpga_reg_read(TAP_MODE_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_tap_mode = *(uint8_t *)(&tmp16Bit);

	/* n_width */
	fpga_reg_read(AOI_WIDTH_0_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_width = tmp32Bit;

	/* n_height */
	fpga_reg_read(AOI_HEIGHT_0_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_height = tmp32Bit;

	/* n_len_of_raw */
	fpga_reg_read(LEN_OF_RAW_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_len_of_raw = tmp32Bit;

	/* n_img_cnt */
	fpga_reg_read(IMG_CNT_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_img_cnt = *(uint8_t *)(&tmp16Bit);

	/* n_device_type */
	fpga_reg_read(FPGA_VERSION1_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_device_type = *(uint8_t *)(&tmp16Bit);

	/* nBitCount */

	/* n_cap_channel_num */
	fpga_reg_read(CHANNEL_NUM_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_cap_channel_num = *(uint8_t *)(&tmp16Bit);

	/* n_dval_lval_mode */
	fpga_reg_read(DVAL_LVAL_MODE_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_dval_lval_mode = *(uint8_t *)(&tmp16Bit);

	/* n_line_clk_num */
	fpga_reg_read(LINE_CLK_NUM_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_line_clk_num = tmp32Bit;

	/* n_line_cnt */
	fpga_reg_read(LINE_CNT_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_line_cnt = tmp32Bit;

	/* n_x_offset */
	fpga_reg_read(X_OFFSET_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_x_offset = tmp32Bit;

	/* n_y_offset */
	fpga_reg_read(Y_OFFSET_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_y_offset = tmp32Bit;

	/* n_fval_set_value */

	/* n_ddr_line_bytes */
	fpga_reg_read(LINE_LEN_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_ddr_line_bytes = *(uint8_t *)(&tmp16Bit);

	/* n_cc1_pwm_high */
	fpga_reg_read(CC1_HIGH_LEVEL_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_cc1_pwm_high = tmp32Bit;

	/* n_cc1_pwm_low */
	fpga_reg_read(CC1_LOW_LEVEL_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_cc1_pwm_low = tmp32Bit;

	/* n_cc1_pwm_cnt */
	fpga_reg_read(CC1_NUM_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_cc1_pwm_cnt = tmp32Bit;

	/* n_cc1_pwm_current */
	fpga_reg_read(CC1_OUT_NUM_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_cc1_pwm_current = tmp32Bit;

	/*  S1_sel  S2_sel */
	fpga_reg_read(TEST_S1_S2_REG_ADDRESS,&tmp16Bit,1);
	ptmp = (uint8_t *)(&tmp16Bit);
	grabconfParam.S1_sel = ptmp[0];
	grabconfParam.S2_sel = ptmp[1];

	/* flcͳ��ֵ�� ��ǰֵ */
	fpga_reg_read(FVAL_STTVALUE_CURRENT_REG_ADDRESS_1,(uint16_t *)(&tmp32BitArray[0]),32);
	fpga_reg_read(FVAL_STTVALUE_CURRENT_REG_ADDRESS_2,(uint16_t *)(&tmp32BitArray[16]),10);
	CyU3PMemCopy((uint8_t *)(&grabconfParam.ar_flc_data[0]),(uint8_t *)(&tmp32BitArray[0]),104);

	/* flcͳ��ֵ�� ���ֵ */
	fpga_reg_read(FVAL_STTVALUE_MAX_REG_ADDRESS,(uint16_t *)(&tmp32BitArray[0]),42);
	CyU3PMemCopy((uint8_t *)(&grabconfParam.ar_flc_data[26]),(uint8_t *)(&tmp32BitArray[0]),104);
	/* flcͳ��ֵ�� ��Сֵ */
	fpga_reg_read(FVAL_STTVALUE_MIN_REG_ADDRESS,(uint16_t *)(&tmp32BitArray[0]),42);
	CyU3PMemCopy((uint8_t *)(&grabconfParam.ar_flc_data[52]),(uint8_t *)(&tmp32BitArray[0]),104);
	/* flcͳ��ֵ�� ����ֵ */
	fpga_reg_read(FVAL_STTVALUE_FLUCTUATE_REG_ADDRESS,(uint16_t *)(&tmp32BitArray[0]),42);
	CyU3PMemCopy((uint8_t *)(&grabconfParam.ar_flc_data[78]),(uint8_t *)(&tmp32BitArray[0]),104);
}
/*function
********************************************************************************
<PRE>
������   :
����     : ֹͣ/���� fpga
����     :
	void
����ֵ   :
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
void GrabStopFpgaWork(void)
{
	uint16_t tmp = 0x0;
	fpga_reg_write(DDR_OUT_EN_REG_ADDRESS,&tmp,1);
}
void GrabStartFpgaWork(void)
{
	uint16_t tmp = 0x1;
	fpga_reg_write(DDR_OUT_EN_REG_ADDRESS,&tmp,1);
}

