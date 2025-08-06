#ifndef VERSION_H
#define VERSION_H
// #define BUILD_YEAR ((((__DATE__[7] - '0') * 16 + (__DATE__[8] - '0')) * 16 + (__DATE__[9] - '0')) * 16 + (__DATE__[10] - '0'))

// #define BUILD_DAY ((__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') * 16 + (__DATE__[5] - '0'))

// #define BUILD_MONTH (__DATE__[2] == 'n'	  ? (__DATE__[1] == 'a' ? 1 : 6) \
// 					 : __DATE__[2] == 'b' ? 2                            \
// 					 : __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) \
// 					 : __DATE__[2] == 'y' ? 5                            \
// 					 : __DATE__[2] == 'l' ? 7                            \
// 					 : __DATE__[2] == 'g' ? 8                            \
// 					 : __DATE__[2] == 'p' ? 9                            \
// 					 : __DATE__[2] == 't' ? 10                           \
// 					 : __DATE__[2] == 'v' ? 11                           \
// 										  : 12)

// #define SOFTWARE_VERSION ((BUILD_YEAR << 16) + (BUILD_MONTH << 8) + BUILD_DAY)

/**
 * 20211101
 * 改造FPGA上电逻辑
 * 
 * 20211118
 * 修改FPGA上电逻辑, 判断一次3寄存器
 * 
 * 20211119
 * 修改FPGA上电逻辑, 重启FPGA
 * 
 * 20211122
 * 延长检测FPGA initial done的时间
 * 
 * 20211123
 * 取消无限重启机制, 重启十次就退出
 * 
 * 20211124
 * DDR成功初始化寄存器改为4
 * 
 * 20211220
 * 修正了读取ID命令码不对的bug
 * 
 * 20211230
 * 增加读取传输包数的指令
 * 增加传输闪灯功能
 * 修改错误状态闪灯频率
 * 
 * 20220113
 * 写入SPI FLASH时挂起FPGA
 * 通过control端点收发数据时上锁
 * 
 * 20220124
 * 将GPIF切换为24bit
 * 将DMA BUFFER切换为3*64
 *
 * 20220830
 * 在通过spi与flash通信时避免产生fer 0的问题
 *
 * 20221109
 * 新增控制管脚功能：初始化管脚、配置管脚状态、获取管脚状态、实现命令控制fpga复位函数
*/



#define SOFT_UPDATE_DATE 0x20250807
#endif
