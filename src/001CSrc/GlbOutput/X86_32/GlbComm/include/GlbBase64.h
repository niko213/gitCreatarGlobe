/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbBase64.h
  * @brief   Base64�������ͷ�ļ�
  *
  * �����������CGlbBase64�����
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 9:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"

/**
  * @brief CGlbBase64������
  *
  * ����Ѷ��������ݱ�����ַ����������ַ�������ɶ��������ݡ�
  * �������紫��
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 9:40
  */
class GLBCOMM_API CGlbBase64
{
public:
/**
  * @brief Base64����
  *
  * ��̬�������Ѷ��������ݱ�����ַ���
  *
  * @param indata ��Ҫ����Ķ���������
  * @param inlen  ��Ҫ����������ֽڳ���
  * @param oudata ���������������ַ���
  * @param outlen ����������������ֽڳ���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	static glbBool Encode (const glbByte* indata, glbInt32 inlen, glbChar** oudata, glbInt32& outlen);
/**
  * @brief Base64����
  *
  * ��̬���������ַ�������ɶ���������
  *
  * @param indata ��Ҫ������ַ���
  * @param inlen  ��Ҫ������ֽڳ���
  * @param oudata �������������Ķ���������
  * @param outlen ����������������ֽڳ���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	static glbBool Decode (const glbChar* indata, glbInt32 inlen, glbByte** oudata, glbInt32& outlen);
};

