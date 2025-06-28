#ifndef _FX3_COMMON_H_
#define _FX3_COMMON_H_

#include "cyu3types.h"


// 整型数据转换为字符串，并记录字符串长度包括结尾的字符'\0'
// pdata 输入数据
// pref  预先开辟好的缓冲区，填入字符,至少11个字节位置
// nlength  字符串的实际长度，包括0结尾
void Long2Char(uint32_t r_pData, uint8_t *r_pRef, uint8_t *r_nLength);


// 字符串转换为long 型整数
void Char2Long(uint8_t *m_ref, uint32_t *data);


#endif