#ifndef _FPGA_CONFIG_H_
#define _FPGA_CONFIG_H_
#include "cyu3types.h"


#define FPGA_REG_ADDR (0x0001);
/*NOTE: */
#define FPGA_VERSION1_REG_ADDRESS 					(0x0000) //FPGA程序版本:采集设备类型
#define FPGA_VERSION2_REG_ADDRESS 					(0x0001) //FPGA程序版本：年月日
#define FPGA_STATUS_REG_ADDRESS						(0x0003) //FPGA状态

/*NOTE: */
#define CL0_FVAL_CNT_REG_ADDRESS 					(0x0018) //0x0018~0x0019 CL0 FVAL下降沿计数
#define CL0_LVAL_CNT_REG_ADDRESS 					(0x001A) //0x001A~0x001B CL0 LVAL下降沿计数(LVAL与DVAL整合)
#define CL0_CLK_CNT_REG_ADDRESS						(0x001E) //0x001E~0x001F CL0 CLK 下降沿计数
#define CL1_FVAL_CNT_REG_ADDRESS 					(0x0020) //0x0020~0x0021 CL1 FVAL下降沿计数
#define CL1_LVAL_CNT_REG_ADDRESS 					(0x0022) //0x0022~0x0023 CL1 LVAL下降沿计数(LVAL与DVAL整合)
#define CL1_CLK_CNT_REG_ADDRESS						(0x0026) //0x0026~0x0027 CL1 CLK 下降沿计数
#define CC1_OUT_NUM_REG_ADDRESS						(0x0028) //0x0028~0x0029 CC1 输出下降沿计数

/*NOTE: */
#define IMG_CNT_REG_ADDRESS 						(0x0030) //0x0030~0x0033 帧编号计数，从0开始一直递增，可以命令清零。
#define PIXEL_FORMAT_REG_ADDRESS 					(0x1101) // 图像的格式
#define CHANNEL_NUM_REG_ADDRESS 					(0x1102) // 图像输入的实际通道数（1、2、4、5（变化的4）、8、10）
#define DVAL_LVAL_MODE_REG_ADDRESS					(0x1103) //
#define TAP_NUM_REG_ADDRESS 						(0x1105) // 采集图像的tap数（帧头64字节）
#define TAP_MODE_REG_ADDRESS 						(0x1106) //
#define LINE_CLK_NUM_REG_ADDRESS					(0x1107) //0x1107~0x1108 一行包含的clk实际数据，是实际图像的横向分辨率/tap数
#define LINE_CNT_REG_ADDRESS 						(0x1109) //0x1109~0x110a 图像的行数
#define X_OFFSET_REG_ADDRESS 						(0x110B) //0x110b~0x110c 水平偏移量
#define Y_OFFSET_REG_ADDRESS						(0x110d) //0x110d~0x110e 垂直偏移量
#define LINE_LEN_REG_ADDRESS						(0x110f) //0x1107~0x1108 ddr一行的长度
#define AOI_WIDTH_0_REG_ADDRESS 					(0x1110) //0x1110~0x1111 图像实际宽度
#define AOI_HEIGHT_0_ADDRESS 						(0x1112) //0x1112~0x1113 图像实际行数
#define LEN_OF_RAW_ADDRESS							(0x1114) //0x1114~0x1115 当前raw数据部分字节的数目，即有效后面跟着的上传数据的大小

/*NOTE: */
#define TEST_S1_S2_REG_ADDRESS 						(0x3000) // S1测试点 S2测试点
#define DDR_OUT_EN_REG_ADDRESS 						(0x3001) // 当接收有问题时，控制此端口
#define MAIN_FUNCTION_REG_ADDRESS 					(0x3010) // 单片机控制
#define TEST_PATTERN_REG_ADDRESS					(0x3011) // 测试图模式
#define CC1_LOW_LEVEL_REG_ADDRESS 					(0x3015) // 0x3015~0x3016 cc1低电平输出时间（100M）
#define CC1_HIGH_LEVEL_REG_ADDRESS 					(0x3017) // 0x3017~0x3018 cc1高电平输出时间（100M）
#define CC1_NUM_REG_ADDRESS							(0x3019) // 0x3019~0x301a  输出个数
#define DEV_INDEX_REG_ADDRESS 						(0x3302) // 图像采集的设备编号

/*NOTE: */
#define FVAL_STTVALUE_CURRENT_REG_ADDRESS_1			(0x0064) // 0x0064~0x0083
#define FVAL_STTVALUE_CURRENT_REG_ADDRESS_2			(0x00A0) // 0x00A0~0x00B3
#define FVAL_STTVALUE_MAX_REG_ADDRESS				(0x00C8) // 0x00C8~0x00E7
#define FVAL_STTVALUE_MIN_REG_ADDRESS				(0x012C) // 0x012C~0x015F
#define FVAL_STTVALUE_FLUCTUATE_REG_ADDRESS			(0x0190) // 0x0190~0x01C3


/*function
********************************************************************************
<PRE>
函数名   : 
功能     :fpga的通讯初始化
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
CyBool_t fpga_reg_init(void);
/*function
********************************************************************************
<PRE>
函数名   : 
功能     :读取fpga内部寄存器的值
参数     : 
		nAddr 寄存器地址15bit有效地址,最高bit : 1 写0读
		pData 16bit有效数据值指针
		nLen  数据长度，16bit为一个单元
返回值   :  
抛出异常 : 
--------------------------------------------------------------------------------
备注     : 
典型用法 : 
--------------------------------------------------------------------------------
作者     : 
</PRE>
*******************************************************************************/
void fpga_reg_read(uint16_t startAddr, uint16_t *pData, uint16_t len);

/*function
********************************************************************************
<PRE>
函数名   : 
功能     :写入fpga内部寄存器的值
参数     : 
		nAddr 寄存器地址15bit有效地址,最高bit : 1 写0读
		pData 16bit有效数据值指针
		nLen  数据长度，16bit为一个单元
返回值   :  
抛出异常 : 
--------------------------------------------------------------------------------
备注     : 
典型用法 : 
--------------------------------------------------------------------------------
作者     : 
</PRE>
*******************************************************************************/
void fpga_reg_write(uint16_t startAddr, uint16_t *pData, uint16_t len);

CyBool_t fpga_reg_test(void);

CyBool_t fpga_init(void);
#endif
