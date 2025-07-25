/*file
********************************************************************************
<PRE>
模块名       : 
文件名       : 
相关文件     : 
文件实现功能 : 采集系统的参数存储的配置管理
作者         : 

--------------------------------------------------------------------------------
备注         : 

		外部调用方法
--------------------------------------------------------------------------------
修改记录 : 
日 期:      2025.6.24
版本 :		v0.0
修改人:
修改内容 :

</PRE>
********************************************************************************

* 版权所有(c) , <>, 保留所有权利
*******************************************************************************/

#include "app_grab_cfg.h"
#include "mcu_spi.h"
#include "fpga_config.h"
#include "cyu3system.h"

#include "cyfxslfifosync.h"
#include "cyu3usb.h"
// 全局使用的采集参数
tag_grab_config grabconfParam = GRAB_PARAM_DEFAULT_VALUE;

// 系统灯状态
uint32_t grabsysStatus = 0;
uint32_t cl0_fval_cnt = 0;
uint32_t cl0_lval_cnt = 0;
uint32_t cl0_clk_cnt = 0;
uint32_t cl1_fval_cnt = 0;
uint32_t cl1_lval_cnt = 0;
uint32_t cl1_clk_cnt = 0;

// fpga clk 状态异常计数
uint8_t cl1_nolocked_num = 0;
uint8_t cl2_nolocked_num = 0;
uint8_t cl3_nolocked_num = 0;

uint8_t fpga_led = 0;


/*function
********************************************************************************
<PRE>
函数名   :
功能     : 存储/读取用户参数区内容，设置默认加载用户参数区是哪个
参数     :
	tagCameraParam *pParam 参数存储的结构体定义
		nIndex  用户参数的存储区0~..
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
CyBool_t GrabWriteUserParam(tag_grab_config *pParam, uint8_t nIndex)//存储当前参数到用户区
{
	if(nIndex >= GRAB_SYSTEM_MODE_NUMBER)
	{
		/*
		 * 大于模式的个数的处理逻辑
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

CyBool_t GrabReadUserParam(tag_grab_config *pParam, uint8_t nIndex)//读取当前用户区参数
{
	uint64_t header = 0;
	if(nIndex >= GRAB_SYSTEM_MODE_NUMBER)
	{
		/*
		 * 大于模式的个数的处理逻辑
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
函数名   :
功能     : 获取指定模式下的用户参数配置信息，这里的思路是上电需要先读fpga的程序版本，确定nIndex
参数     : void
返回值   : flash里的参数对且读取成功 	返回CyTrue
		flash里的参数不对或读取失败	返回CyFalse
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
CyBool_t GrabGetDefaultUserParam(void)
{
	//TODO
	uint8_t nIndex = 0;
	uint16_t fpga_version = 0;
	tag_grab_config *tmp = (tag_grab_config *)CyU3PMemAlloc(sizeof(tag_grab_config));
	if(tmp == NULL)
	{
		CyU3PDebugPrint(4,"\nmalloc error");
		return CyFalse;
	}

	fpga_reg_read(FPGA_VERSION1_REG_ADDRESS,&fpga_version,1);
	CyU3PDebugPrint(4,"\nfpga_version = %x", fpga_version);
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
			//获取配置失败的话，将n_device_type更新，方便上位机刷新到正确的界面，其他参数保持默认
			grabconfParam.n_device_type = nIndex;
			CyU3PMemFree(tmp);
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
			//获取配置失败的话，将n_device_type更新，方便上位机刷新到正确的界面，其他参数保持默认
			grabconfParam.n_device_type = nIndex;
			CyU3PMemFree(tmp);
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
			//获取配置失败的话，将n_device_type更新，方便上位机刷新到正确的界面，其他参数保持默认
			grabconfParam.n_device_type = nIndex;
			CyU3PMemFree(tmp);
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
			//获取配置失败的话，将n_device_type更新，方便上位机刷新到正确的界面，其他参数保持默认
			grabconfParam.n_device_type = nIndex;
			CyU3PMemFree(tmp);
			return CyFalse;
		}
		break;
	default:
		break;
	}
	CyU3PMemFree(tmp);
	return CyTrue;
}


/*function
********************************************************************************
<PRE>
函数名   :
功能     : fpga 灯和State 灯控制
参数     :
	   uint32_t status 状态值
返回值   : void
抛出异常 :
--------------------------------------------------------------------------------
备注     : fpga_led_status = 0 常亮
  	  	  	  	  	  	1  4s
  	  	  	  	  	  	5  2s
  	  	  	  	  	  	6  0.2s
  	  	  	  	  	  	21 0.1s
  	  cl0_status	  = 3 1s
  	  	  	  	  	  	4 0.5s
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
void GrabSetFpgaLedStatus(uint32_t low_time, uint32_t high_time)
{
	fpga_reg_write(LED_LOW_ADDRESS,(uint16_t *)&low_time,2);
	fpga_reg_write(LED_HIGHT_ADDRESS,(uint16_t *)&high_time,2);
}

void GrabGetFpgaLedStatus(uint8_t cl0_status, uint8_t fpga_led_status)
{
	if(cl0_status == 3)
	{	// 1s
		GrabSetFpgaLedStatus(100000000,100000000);
	}
	else if(cl0_status == 4)
	{	// 0.5s
		GrabSetFpgaLedStatus(50000000,50000000);
	}
	else if(fpga_led_status == 0)
	{
		//常亮
		GrabSetFpgaLedStatus(0,100000000);
	}
	else if(fpga_led_status == 1)
	{
		//4s
		GrabSetFpgaLedStatus(400000000,400000000);
	}
	else if(fpga_led_status == 5)
	{
		//2s
		GrabSetFpgaLedStatus(200000000,200000000);
	}
	else if(fpga_led_status == 6)
	{
		//0.2s
		GrabSetFpgaLedStatus(20000000,20000000);
	}
	else if(fpga_led_status == 21)
	{
		//0.1s
		GrabSetFpgaLedStatus(10000000,10000000);
	}
	else
	{

	}
}

/*function
********************************************************************************
<PRE>
函数名   :
功能     : 更新系统灯的状态，需要供外部线程调用
参数     :
	   uint32_t *status 状态值指针
返回值   : 成功 CyTrue 失败CyFalse
抛出异常 :
--------------------------------------------------------------------------------
备注     : 		cl0 ck1
		clk  1   4
		fval 2   5
		lval 3   6

典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
void GrabGetSystemStatus(void)
{
	//TODO
	uint32_t cur_value = 0;
	// fpga_led 状态标记，置位对应位时将其状态标记置为对应置
	uint8_t cl0_status = 0; //
	uint8_t fpga_led_status = 0;
	/* 读cl0_fval_cnt */
	fpga_reg_read(CL0_FVAL_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value != cl0_fval_cnt)
	{
		cl0_status += 2;
		SET_BIT(grabsysStatus,2);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,2);
	}
	cl0_fval_cnt = cur_value;

	/* 读cl0_lval_cnt */
	cur_value = 0;
	fpga_reg_read(CL0_LVAL_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value != cl0_lval_cnt)
	{
		cl0_status += 3;
		SET_BIT(grabsysStatus,1);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,1);
	}

	/* 读cl0_clk_cnt */
	cur_value = 0;
	fpga_reg_read(CL0_CLK_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(abs(cur_value - cl0_clk_cnt) >= CLK_NORM_CNT)
	{
		cl0_status += 1;
		cl0_clk_cnt = cur_value;
		SET_BIT(grabsysStatus,0);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,0);
	}

	/* 读cl1_fval_cnt */
	cur_value = 0;
	fpga_reg_read(CL1_FVAL_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value != cl1_fval_cnt)
	{
		fpga_led_status += 5;
		SET_BIT(grabsysStatus,5);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,5);
	}
	cl1_fval_cnt = cur_value;

	/* 读cl1_lval_cnt */
	cur_value = 0;
	fpga_reg_read(CL1_LVAL_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(cur_value != cl1_lval_cnt)
	{
		fpga_led_status += 6;
		SET_BIT(grabsysStatus,4);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,4);
	}
	cl1_lval_cnt = cur_value;

	/* 读cl1_clk_cnt */
	cur_value = 1;
	fpga_reg_read(CL1_CLK_CNT_REG_ADDRESS,(uint16_t*)(&cur_value),2);
	if(abs(cur_value - cl1_clk_cnt) >= CLK_NORM_CNT)
	{
		fpga_led_status += 4;
		cl1_clk_cnt = cur_value;
		SET_BIT(grabsysStatus,3);
	}
	else
	{
		CLEAR_BIT(grabsysStatus,3);
	}

	fpga_led_status += cl0_status;
	if(fpga_led_status != fpga_led)
	{
		fpga_led = fpga_led_status;
		GrabGetFpgaLedStatus(cl0_status,fpga_led);
	}

}

/*function
********************************************************************************
<PRE>
函数名   :
功能     : 监视fpga clk 状态
参数     :
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
void GrabFpgaClkStatusDog(void)
{
	uint16_t statusRegvalue = 0;
	uint16_t tmp16Bit = 0;
	fpga_reg_read(FPGA_STATUS_REG_ADDRESS,&statusRegvalue,1);

	if((statusRegvalue & 0x01) == 0)
	{
		cl1_nolocked_num++;
		if(cl1_nolocked_num >=3)
		{
			cl1_nolocked_num = 0;
			//todo 复位cl1_clk_pll, 复位一下flc
			fpga_reg_read(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
			//将 cl1 和 flc 置1清零
			SET_BIT(tmp16Bit,2);
			SET_BIT(tmp16Bit,5);
			fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
			CyU3PThreadSleep(1);
			//将 cl1 和 flc 置0复位
			CLEAR_BIT(tmp16Bit,2);
			CLEAR_BIT(tmp16Bit,5);
			fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
		}
	}
	else
	{
		cl1_nolocked_num = 0;
	}


	if((statusRegvalue & 0x02) == 0)
	{
		cl2_nolocked_num++;
		if(cl2_nolocked_num >=3)
		{
			cl2_nolocked_num = 0;
			//todo 复位cl2_clk_pll
			fpga_reg_read(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
			SET_BIT(tmp16Bit,3);
			SET_BIT(tmp16Bit,5);
			fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
			CyU3PThreadSleep(1);
			CLEAR_BIT(tmp16Bit,3);
			CLEAR_BIT(tmp16Bit,5);
			fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
		}
	}
	else
	{
		cl2_nolocked_num = 0;
	}

	if((statusRegvalue & 0x04) == 0)
	{
		cl3_nolocked_num++;
		if(cl3_nolocked_num >=3)
		{
			cl3_nolocked_num = 0;
			//todo 复位cl3_clk_pll
			fpga_reg_read(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
			SET_BIT(tmp16Bit,4);
			SET_BIT(tmp16Bit,5);
			fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
			CyU3PThreadSleep(1);
			CLEAR_BIT(tmp16Bit,4);
			CLEAR_BIT(tmp16Bit,5);
			fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&tmp16Bit,1);
		}
	}
	else
	{
		cl3_nolocked_num = 0;
	}
}

/*function
********************************************************************************
<PRE>
函数名   :
功能     : 将上位机传下来的配置参数和本地的做比较,配置寄存器
参数     :
	tagCameraParam PcParam 上位机下传的配置参数
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
CyBool_t GrabParamCompareandSet(tag_grab_config *PcParam)
{
	uint16_t tmp = 0;
	uint8_t *ptmp = NULL;
	if(PcParam->header != PARAM_VALID_HCODE) return CyFalse;
	//Todo 实现刷fpga寄存器

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

	/* n_real_line_bytes_min */
	if(grabconfParam.n_real_line_bytes_min != PcParam->n_real_line_bytes_min){grabconfParam.n_real_line_bytes_min = PcParam->n_real_line_bytes_min;}
	/* n_real_line_bytes_max */
	if(grabconfParam.n_real_line_bytes_max != PcParam->n_real_line_bytes_max){grabconfParam.n_real_line_bytes_max = PcParam->n_real_line_bytes_max;}
	/* n_real_line_num */
	if(grabconfParam.n_real_line_num != PcParam->n_real_line_num){grabconfParam.n_real_line_num = PcParam->n_real_line_num;}

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

	/* n_ddr_line_bytes
	 * 删除了
	 */
//	if(grabconfParam.n_ddr_line_bytes != PcParam->n_ddr_line_bytes)
//	{
//		grabconfParam.n_ddr_line_bytes = PcParam->n_ddr_line_bytes;
//		/*fpga_reg_write*/
//		fpga_reg_write(LINE_LEN_REG_ADDRESS,&grabconfParam.n_ddr_line_bytes,1);
//	}

	/* n_fpga_version */

	/* n_fx3_version */

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
函数名   :
功能     : 读一遍寄存器，更新本地的参数
参数     :
	void
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
void GrabParamUpdate(void)
{
	uint8_t *ptmp = NULL;
	uint16_t tmp16Bit = 0;
	uint32_t tmp32Bit = 0;
	uint64_t tmp64Bit = 0;
	uint16_t tmp16BitArray[60] = {0};

	/* n_dev_index */
	fpga_reg_read(DEV_INDEX_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_dev_index = *(uint8_t *)(&tmp16Bit);
	/* n_pixel_format */
	tmp16Bit = 0;
	fpga_reg_read(PIXEL_FORMAT_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_pixel_format = *(uint8_t *)(&tmp16Bit);

	/* n_tap_num */
	tmp16Bit = 0;
	fpga_reg_read(TAP_NUM_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_tap_num = *(uint8_t *)(&tmp16Bit);

	/* n_tap_mode */
	tmp16Bit = 0;
	fpga_reg_read(TAP_MODE_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_tap_mode = *(uint8_t *)(&tmp16Bit);

	/* n_width */
	fpga_reg_read(AOI_WIDTH_0_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_width = tmp32Bit;

	/* n_height */
	tmp32Bit = 0;
	fpga_reg_read(AOI_HEIGHT_0_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_height = tmp32Bit;

	/* n_len_of_raw */
	tmp32Bit = 0;
	fpga_reg_read(LEN_OF_RAW_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_len_of_raw = tmp32Bit;

	/* n_img_cnt */
	tmp16Bit = 0;
	fpga_reg_read(IMG_CNT_REG_ADDRESS,(uint16_t *)&tmp64Bit,4);
	grabconfParam.n_img_cnt = tmp64Bit;

	/* n_real_line_bytes_min */
	/* n_real_line_bytes_max */
	/* n_real_line_num */

	/* n_device_type */
	tmp16Bit = 0;
	fpga_reg_read(FPGA_VERSION1_REG_ADDRESS,&tmp16Bit,1);
	if(tmp16Bit < 6)
		grabconfParam.n_device_type = *(uint8_t *)(&tmp16Bit);

	/* nBitCount */

	/* n_cap_channel_num */
	tmp16Bit = 0;
	fpga_reg_read(CHANNEL_NUM_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_cap_channel_num = *(uint8_t *)(&tmp16Bit);

	/* n_dval_lval_mode */
	tmp16Bit = 0;
	fpga_reg_read(DVAL_LVAL_MODE_REG_ADDRESS,&tmp16Bit,1);
	grabconfParam.n_dval_lval_mode = *(uint8_t *)(&tmp16Bit);

	/* n_fpga_version*/
	tmp16Bit = 0;
	tmp32Bit = 0;
	fpga_reg_read(FPGA_VERSION2_REG_ADDRESS,&tmp16Bit,1);
	tmp32Bit |= tmp16Bit;
	grabconfParam.n_fpga_version = tmp32Bit;

	/* n_line_clk_num */
	tmp32Bit = 0;
	fpga_reg_read(LINE_CLK_NUM_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_line_clk_num = tmp32Bit;

	/* n_line_cnt */
	tmp32Bit = 0;
	fpga_reg_read(LINE_CNT_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_line_cnt = tmp32Bit;

	/* n_x_offset */
	tmp32Bit = 0;
	fpga_reg_read(X_OFFSET_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_x_offset = tmp32Bit;

	/* n_y_offset */
	tmp32Bit = 0;
	fpga_reg_read(Y_OFFSET_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_y_offset = tmp32Bit;

	/* n_fval_set_value */

	/* n_ddr_line_bytes
	 * 删除了
	 */
//	tmp16Bit = 0;
//	fpga_reg_read(LINE_LEN_REG_ADDRESS,&tmp16Bit,1);
//	grabconfParam.n_ddr_line_bytes = *(uint8_t *)(&tmp16Bit);

	/* n_cc1_pwm_high */
	tmp32Bit = 0;
	fpga_reg_read(CC1_HIGH_LEVEL_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_cc1_pwm_high = tmp32Bit;

	/* n_cc1_pwm_low */
	tmp32Bit = 0;
	fpga_reg_read(CC1_LOW_LEVEL_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_cc1_pwm_low = tmp32Bit;

	/* n_cc1_pwm_cnt */
	tmp32Bit = 0;
	fpga_reg_read(CC1_NUM_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_cc1_pwm_cnt = tmp32Bit;

	/* n_cc1_pwm_current */
	tmp32Bit = 0;
	fpga_reg_read(CC1_OUT_NUM_REG_ADDRESS,(uint16_t *)(&tmp32Bit),2);
	grabconfParam.n_cc1_pwm_current = tmp32Bit;

	/*  S1_sel  S2_sel */
	tmp16Bit = 0;
	fpga_reg_read(TEST_S1_S2_REG_ADDRESS,&tmp16Bit,1);
	ptmp = (uint8_t *)(&tmp16Bit);
	grabconfParam.S1_sel = ptmp[0];
	grabconfParam.S2_sel = ptmp[1];

	/* flc统计值： 当前值  */
	/* 26个uint32_t 是一组， 16位寄存器，所以需要读52个寄存器， 拷贝的时候是以8bit一个字节进行拷贝，需要104字节 */
	CyU3PMemSet((uint8_t*)tmp16BitArray,0,120);
	fpga_reg_read(FVAL_STTVALUE_CURRENT_REG_ADDRESS_1,tmp16BitArray,32);
	fpga_reg_read(FVAL_STTVALUE_CURRENT_REG_ADDRESS_2,&tmp16BitArray[32],20);
	CyU3PMemCopy((uint8_t *)(&grabconfParam.ar_flc_data[0]),(uint8_t *)tmp16BitArray,104);

	/* flc统计值： 最大值 */
	CyU3PMemSet((uint8_t*)tmp16BitArray,0,120);
	fpga_reg_read(FVAL_STTVALUE_MAX_REG_ADDRESS,(uint16_t *)tmp16BitArray,52);
	CyU3PMemCopy((uint8_t *)(&grabconfParam.ar_flc_data[26]),(uint8_t *)tmp16BitArray,104);

	/* flc统计值： 最小值 */
	CyU3PMemSet((uint8_t*)tmp16BitArray,0,120);
	fpga_reg_read(FVAL_STTVALUE_MIN_REG_ADDRESS,(uint16_t *)tmp16BitArray,52);
	CyU3PMemCopy((uint8_t *)(&grabconfParam.ar_flc_data[52]),(uint8_t *)tmp16BitArray,104);

	/* flc统计值： 波动值 */
	CyU3PMemSet((uint8_t*)tmp16BitArray,0,120);
	fpga_reg_read(FVAL_STTVALUE_FLUCTUATE_REG_ADDRESS,(uint16_t *)tmp16BitArray,52);
	CyU3PMemCopy((uint8_t *)(&grabconfParam.ar_flc_data[78]),(uint8_t *)tmp16BitArray,104);


}
/*function
********************************************************************************
<PRE>
函数名   :
功能     : 停止/启动 fpga
参数     :
	void
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
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

/*function
********************************************************************************
<PRE>
函数名   :
功能     : 1s一次flc触发信号，更新当前值
参数     :
	void
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
void GrabTriggerFlcBitAndUpdate(void)
{
	uint16_t mFuncRegValue = 0;
	fpga_reg_read(MAIN_FUNCTION_REG_ADDRESS,&mFuncRegValue,1);
	SET_BIT(mFuncRegValue,6);
	fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&mFuncRegValue,1);
	CyU3PThreadSleep(1);
	CLEAR_BIT(mFuncRegValue,6);
	fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&mFuncRegValue,1);
	GrabParamUpdate();
}

/*Debug funciton */
void Debug_manul_reset(void)
{
	GrabStopFpgaWork();
	CyFxSlFifoApplnStop();
    /* Give a chance for the main thread loop to run. */
    CyU3PThreadSleep (1);
    CyFxSlFifoApplnStart();
    CyU3PUsbStall (CY_FX_EP_CONSUMER, CyFalse, CyTrue);
    CyU3PThreadSleep(20);
    GrabStartFpgaWork();
    CyU3PUsbAckSetup ();
}

