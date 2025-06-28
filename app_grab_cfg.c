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

// 全局使用的采集参数
tag_grab_config grabconfParam = GRAB_PARAM_DEFAULT_VALUE;
uint32_t grabsysStatus = 0;


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
CyBool_t GrabWriteUserParam(tag_grab_config *pParam, int nIndex)//存储当前参数到用户区
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

CyBool_t GrabReadUserParam(tag_grab_config *pParam, int nIndex)//读取当前用户区参数
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
	return CyTrue;
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
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
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
函数名   :
功能     : 将上位机传下来的配置参数和本地的做比较
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
CyBool_t GrabParamCompareandSet(tag_grab_config PcParam)
{
	if(PcParam.header != PARAM_VALID_HCODE) return CyFalse;
	//Todo 实现刷fpga寄存器
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
