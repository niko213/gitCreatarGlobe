/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbString.h
  * @brief   �ַ���������ͷ�ļ�
  *
  * �����������CGlbString���class,
  * GlbChar2WChar��GlbWChar2Char��GlbChar2UTF8��WChar2UTF8�ĸ�ȫ�ֺ���
  *
  * ע�⣺string��find_last_not_of��find_last_of�޷�ʹ�ã�
  * �̳к�find_last_not_of��find_last_ofҲ����ʹ��
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-6 16:40
*********************************************************************/
#pragma once
#pragma warning(disable:4251)//����ȡ
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include <string>
using namespace std;
#ifdef  __cplusplus
extern "C" {
#endif
/**
  * @brief խ�ַ���ת�ɿ��ַ���
  * @brief �ͷŷ��ص��ַ�������free
  *
  * @param srcstr    ����ת����խԴ�ַ���
  * @param codepage  srcstr�ı���ҳ��ȱʡΪ����ϵͳ��ǰ����ҳ
  * @return          ����ת����Ŀ��ַ���
  */
extern GLBCOMM_API glbWChar* GlbChar2WChar(const glbChar* srcstr,const glbUInt32 codepage = CP_ACP);
/**
  * @brief ���ַ���ת��խ�ַ���(windows ���ַ���UTF_16)
  * @brief �ͷŷ��ص��ַ�������free
  *
  * @param   srcwstr ����ת���Ŀ�Դ�ַ���
  * @param   codepage ����ֵ�ı���ҳ
  * @return  ����ת�����խ�ַ���
  */
extern GLBCOMM_API glbChar* GlbWChar2Char(const glbWChar* srcwstr,const glbUInt32 codepage = CP_ACP);
/**
  * @brief ���ַ���ת��խ�ַ���{UTF8����}(windows ���ַ���UTF_16)
  * @brief �ͷŷ��ص��ַ�������free
  *
  * @param   srcwstr ����ת���Ŀ�Դ�ַ���
  * @return  ����ת�����UTF8����խ�ַ���
  */
extern GLBCOMM_API glbChar* GlbWChar2UTF8(const glbWChar* srcwstr);
/**
 * @brief ϵͳȱʡ����խ�ַ���ת��ΪUTF8����խ�ַ���
 * @brief �ͷŷ��ص��ַ�������free
  *
  * @param   srcstr ����ת����ϵͳȱʡ����խ�ַ���
  * @return  ����ת�����UTF8����խ�ַ���
 */
extern GLBCOMM_API glbChar* GlbChar2UTF8(const glbChar* srcstr);

extern GLBCOMM_API glbWChar* UTF82WChar(const glbChar* utf8);

#ifdef  __cplusplus
}
#endif
class CGlbWString; ///< ���ַ������壬��������

/**
  * @brief CGlbStringխ�ַ�����
  *					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-6 16:40
  */
class GLBCOMM_API CGlbString : public std::string
{
public:
	CGlbString(void);
	CGlbString(const CGlbString& src);
	CGlbString(const std::string& src);
	CGlbString(const glbChar* psz);
	CGlbString(const glbChar* psz, glbInt32 count);
/**
  * @brief ת��Ϊ���ַ���
  * @brief CGlbString ���ַ�������������ϵͳ��ǰ����ҳ������ת����������. 
  *
  * @return ת����Ŀ��ַ���
  */
	CGlbWString ToWString();
/**
  * @brief �Ѹ��ַ�������ĸȫ��תΪ��д
  *
  * ���������ַ������������ĸ��ȫ��תΪ��д�������ַ����ıȽ�
  *
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool ToUpper();
/**
  * @brief �Ѹ��ַ�������ĸȫ��תΪСд
  *
  * ���������ַ������������ĸ��ȫ��תΪСд�������ַ����ıȽ�
  *
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool ToLower();
/**
  * @brief ������ַ���ǰ��Ŀո�
  *
  * ��ͷ���������ַ���������ǿո����
  *
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool TrimLeft();
/**
  * @brief ������ַ�������Ŀո�
  *
  * ��ͷ���������ַ���������ǿո񣬼�¼��ǰλ�ã�������涼�ǿո񣬰ѵ�ǰλ���ַ���Ϊ������
  *
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool TrimRight();
/**
  * @brief ������ַ���ǰ�����˵Ŀո�
  *
  * ����TrimLeft��TrimRight���д���
  *
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool Trim();
/**
  * @brief �滻
  *
  * @param rpstr   ��Ҫ�滻���ַ���
  * @param newstr  �����滻���ַ���
  *
  **/
	void ReplaceAll(const CGlbString& rpstr,const CGlbString& newstr);

/**
  * @brief �����ض���ʽ�����ַ���
  *
  * ��̬����������ϵͳ����vsnprintf�����ַ������ַ�����󳤶�GLBSTRBUFFERSIZE
  * ���ʧ�ܣ�����һ�����ַ���
  *
  * @param format ��ʽ���ַ���
  * @param ...    ������̬����
  * @return �ַ�����
  */
	static CGlbString FormatString(glbChar* format, ...);
};

