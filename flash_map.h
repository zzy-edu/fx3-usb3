#ifndef _FLASH_MAP_H_
#define _FLASH_MAP_H_

/*
 * flash 16M 用户自定义参数存储从15M开始  256个sector
 *
 * 20th sector 系统参数（自定义，存一些自己需要保存的信息）
 * 21th sector 模式1配置参数
 * 22th sector 模式2配置参数
 * 23th sector 模式3配置参数
 * 24th sector 模式4配置参数
 */

#define FLASH_START_SECTOR (3840)
#define FX3_FLASH_LEN 	(0x1000000)  //
#define FX3_FLASH_SECTOR_SIZE (0x1000)//sector大小4k
#define PIC32_FLASH_LARGE_BLOCK_SIZE (0x10000) //boot sect 大小为64K
#define PIC32_FLASH_SMALL_BLOCK_SIZE (0x8000) //boot sect 大小为32K
#define FX3_FLASH_WRITE_MAX_LEN (128) // 一次最多写256字节

//系统参数存储地址
#define SYSTEM_PARAM_DEFAULT_ADDR ((FLASH_START_SECTOR+20)*FX3_FLASH_SECTOR_SIZE)
//采集系统模式
#define GRAB_SYSTEM_MODE_NUMBER (4)
//模式i配置参数存储地址 , i从0开始
#define	GRAB_MODE1_PARAM_ADDR(i) ((FLASH_START_SECTOR+(i)+21)*FX3_FLASH_SECTOR_SIZE)



#endif
