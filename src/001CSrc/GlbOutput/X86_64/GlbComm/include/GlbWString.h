/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbWString.h
  * @brief   �ַ���������ͷ�ļ�
  *
  * �����������CGlbWString���class,
  * ע�⣺wstring��find_last_not_of��find_last_of�޷�ʹ�ã�
  * �̳к�find_last_not_of��find_last_ofҲ����ʹ��
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 10:40
*********************************************************************/
#pragma once
#pragma warning(disable:4251)//����ȡ
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include <string>
using namespace std;

class CGlbString;///< խ�ַ������壬��������
/**
  * @brief CGlbWString���ַ�����
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 10:40
  */
//#if _MSC_VER >= 1600
//const std::wstring::size_type std::wstring::npos = (std::wstring::size_type) -1;
//#endif
class GLBCOMM_API CGlbWString :public std::wstring
{
public:
/**
  * @brief CGlbWString���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbWString(void);
/**
  * @brief CGlbWString�������캯��
  *
  * copy src�ַ��������ַ���
  *
  * @param src ����������Դ�ַ���
  * @return ��
  */
	CGlbWString(const CGlbWString& src);
/**
  * @brief CGlbWString���캯��
  *
  * copy src�ַ��������ַ���
  *
  * @param src ����������Դ�ַ���
  * @return ��
  */
	CGlbWString(const wstring& src);
/**
  * @brief CGlbWString���캯��
  *
  * copy psz�ַ��������ַ���,���ΪNULL��mpr_strΪ���ַ���
  *
  * @param psz ����������Դ�ַ���
  * @return ��
  */
	CGlbWString(const glbWChar* psz);

	CGlbWString(const glbWChar* psz, glbInt32 count);
/**
  * @brief ת��Ϊխ�ַ���
  * @brief խ�ַ������ʽΪ����ϵͳ��ǰĬ�ϴ���ҳ
  *
  * @return ת�����խ�ַ���
  */
	CGlbString ToString();
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
  * @brief ת��ΪUTF8խ�ַ���
  */
	CGlbString ToUTF8String();
/**
  * @brief �ָ�
*/
	vector<CGlbWString> Split(glbWChar split);
/**
  * @brief �����ض���ʽ�����ַ���
  *  
  * ���ʧ�ܣ�����һ�����ַ���
  *
  * @param format ��ʽ���ַ���
  * @param ...    ������̬����
  * @return �ַ�����
  */
	static CGlbWString FormatString(glbWChar* format, ...);
};

