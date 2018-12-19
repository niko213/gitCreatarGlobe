/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbFontPath.h
  * @brief   ����·����Ӧͷ�ļ�
  *
  * �����������CGlbFontPath�����
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-14 9:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "GlbWString.h"

/**
  * @brief CGlbFontPath����·����
  *
  * ��ȡϵͳ�����������Ӧ��·����
  * ��һʵ��
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-14 9:40
  */
class GLBCOMM_API CGlbFontPath
{
private:
/**
  * @brief ���캯��
  *
  * ˽�У���������ʵ��
  * ����ϵͳ�����ļ�Ŀ¼mpr_fontdir��
  * �����������������ļ����ļ�ֵ��mpr_mapfontPaths
  *
  * @return ��
  */
	CGlbFontPath(void);
/**
  * @brief ��������
  *
  * @return ��
  */
	~CGlbFontPath(void){}
/**
  * @brief �����ļ�ȫ·����ȡ������
  *
  * ��ȡ������
  *
  * @param strfilepath �ļ�ȫ·��
  * @return ���������������ؿ��ַ���
  */
	CGlbWString GetFontNameFromFile(CGlbWString strfilepath);
	static CGlbFontPath mpr_instance;///<����·��Ψһ��̬ʵ��������
public:
/**
  * @brief ��ȡһ��ʵ��
  *
  * ��̬���� ��ȡһ��ʵ��
  *
  * @return һ������·��ʵ��
  */
	static CGlbFontPath* GetInstance();
/**
  * @brief ������������ȡ�ļ�ȫ·��
  *
  * ��ȡ�ļ�ȫ·��
  *
  * @param fontName ������
  * @return ���������������ؿ��ַ���
  */
	//const glbWChar* GetFontFileFullName(const glbWChar* fontName);
	CGlbWString GetFontFileFullName(const glbWChar* fontName);
	/**
	  * @brief ��ȡ������Ŀ
	  *	  
	  * @return ������Ŀ
	*/
	glbInt32 GetCount();
	/**
	  * @brief ��ȡ��������
	  *	  
	  * @param idx ����
	  * @return �������ƣ���������ڷ��ؿ��ַ���
	*/
	CGlbWString GetFontName(glbInt32 idx);

private:
	std::map<CGlbWString /*fontname*/, CGlbWString /*fontFileFullName*/> mpr_mapfontPaths;///<���������ļ�ȫ·����ֵ��
	CGlbWString mpr_fontdir;///<ϵͳ����Ŀ¼
};

