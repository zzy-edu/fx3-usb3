#ifndef _FX3_COMMON_H_
#define _FX3_COMMON_H_

#include "cyu3types.h"


// ��������ת��Ϊ�ַ���������¼�ַ������Ȱ�����β���ַ�'\0'
// pdata ��������
// pref  Ԥ�ȿ��ٺõĻ������������ַ�,����11���ֽ�λ��
// nlength  �ַ�����ʵ�ʳ��ȣ�����0��β
void Long2Char(uint32_t r_pData, uint8_t *r_pRef, uint8_t *r_nLength);


// �ַ���ת��Ϊlong ������
void Char2Long(uint8_t *m_ref, uint32_t *data);


#endif