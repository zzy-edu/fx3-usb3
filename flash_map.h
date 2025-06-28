#ifndef _FLASH_MAP_H_
#define _FLASH_MAP_H_

/*
 * flash 16M �û��Զ�������洢��15M��ʼ  256��sector
 *
 * 20th sector ϵͳ�������Զ��壬��һЩ�Լ���Ҫ�������Ϣ��
 * 21th sector ģʽ1���ò���
 * 22th sector ģʽ2���ò���
 * 23th sector ģʽ3���ò���
 * 24th sector ģʽ4���ò���
 */

#define FLASH_START_SECTOR (3840)
#define FX3_FLASH_LEN 	(0x1000000)  //
#define FX3_FLASH_SECTOR_SIZE (0x1000)//sector��С4k
#define PIC32_FLASH_LARGE_BLOCK_SIZE (0x10000) //boot sect ��СΪ64K
#define PIC32_FLASH_SMALL_BLOCK_SIZE (0x8000) //boot sect ��СΪ32K
#define FX3_FLASH_WRITE_MAX_LEN (128) // һ�����д256�ֽ�

//ϵͳ�����洢��ַ
#define SYSTEM_PARAM_DEFAULT_ADDR ((FLASH_START_SECTOR+20)*FX3_FLASH_SECTOR_SIZE)
//�ɼ�ϵͳģʽ
#define GRAB_SYSTEM_MODE_NUMBER (4)
//ģʽi���ò����洢��ַ , i��0��ʼ
#define	GRAB_MODE1_PARAM_ADDR(i) ((FLASH_START_SECTOR+(i)+21)*FX3_FLASH_SECTOR_SIZE)



#endif
