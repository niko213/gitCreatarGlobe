/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbCompress.h
  * @brief   ѹ����ѹͷ�ļ�
  *
  * �����������CGlbCompress�����
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-12 16:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"

typedef struct _GlbBmpDataBuffer
{
	int      width;
	int      height;
	glbByte* data;
} GlbBmpDataBuffer;
/**
  * @brief CGlbCompressѹ����ѹ��
  *
  * ����ѹ�����ͣ�ѡ���Ӧ��ѹ����ѹ�㷨����ѹ����ѹ
  * ����ȫ����̬ʵ��
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 14:40
  */
class GLBCOMM_API CGlbCompress
{
public:
/**
  * @brief ѹ������
  *
  * ��̬����������ѹ�����ͣ�ѹ������
  *
  * @param type   ѹ������
  * @param indata ��Ҫѹ���Ķ���������
  * @param inlen  ��Ҫѹ���������ֽڳ���
  * @param oudata ���������ѹ����Ķ���������
  * @param outlen ���������ѹ������ֽڳ���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	static glbBool Compress(GlbCompressTypeEnum type, 
		                    glbByte*            indata, 
							glbUInt32           inlen, 
							glbByte**           oudata, 
							glbUInt32&          outlen);
/**
  * @brief ��ѹ����
  *
  * ��̬����������ѹ�����ͣ���ѹ����
  *
  * @param type   ѹ������,��ǰ֧��zlibѹ��
  * @param indata ��Ҫ��ѹ�Ķ���������
  * @param inlen  ��Ҫ��ѹ�������ֽڳ���
  * @param oudata �����������ѹ��Ķ���������
  * @param outlen �����������ѹ����ֽڳ���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	static glbBool UnCompress(GlbCompressTypeEnum type, 
		                      glbByte* indata, 
							  glbUInt32 inlen, 
							  glbByte** oudata, 
							  glbUInt32& outlen);
private:
/**
  * @brief Zlibѹ������
  *
  * ��̬����������Zlibѹ������
  * ע�⣺���ڽ�ѹʱ����Ҫ��ǰ�Խ�ѹ�����������ڴ棬����ѹ���������
  * ǰ4���ֽڱ���ԭʼ���ݳ���inlen
  *
  * @param indata ��Ҫѹ���Ķ���������
  * @param inlen  ��Ҫѹ���������ֽڳ���
  * @param oudata ���������ѹ����Ķ��������ݣ�ǰ4���ֽڱ���inlen
  * @param outlen ���������ѹ������ֽڳ���+4
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	static glbBool Zlib_Compress(glbByte* indata, glbUInt32 inlen, glbByte** oudata, glbUInt32& outlen);
/**
  * @brief ��ѹ����
  *
  * ��̬����������Zlib��ѹ����
  * ע�⣺��ѹ������ǰ4���ֽڱ����ԭʼ���ݳ��ȣ�Ҳ����outlen������Ĳ�������
  *
  * @param indata ǰ4���ֽ���outlen���������Ҫ��ѹ�Ķ���������
  * @param inlen  ��Ҫ��ѹ�������ֽڳ���+4
  * @param oudata �����������ѹ��Ķ���������
  * @param outlen �����������ѹ����ֽڳ���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	static glbBool Zlib_UnCompress(glbByte* indata, glbUInt32 inlen, glbByte** oudata, glbUInt32& outlen);

	/**
  * @brief Jpegѹ������
  *
  * ��̬����������Jpeg9.libѹ������
  * ע�⣺����ѹ��ʱ����Ҫjpeg�������Ϣ��indataǰ �ֽ������������Ϣ
  * 
  *
  * @param indata ��Ҫѹ���Ķ���������
  * @param inlen  ��Ҫѹ���������ֽڳ���
  * @param oudata ���������ѹ����Ķ���������
  * @param outlen ���������ѹ������ֽڳ���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	static glbBool Jpeg_Compress(glbByte* indata, glbUInt32 inlen, glbByte** oudata, glbUInt32& outlen);
/**
  * @brief ��ѹ����
  *
  * ��̬����������Jpeg��ѹ����
  * ע�⣺����ѹ��ʱ����Ҫjpeg�������Ϣ��indataǰ �ֽ������������Ϣ
  *
  * @param indata ��Ҫ��ѹ�Ķ���������
  * @param inlen  ��Ҫ��ѹ�������ֽڳ���
  * @param oudata �����������ѹ��Ķ���������
  * @param outlen �����������ѹ����ֽڳ���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	static glbBool Jpeg_UnCompress(glbByte* indata, glbUInt32 inlen, glbByte** oudata, glbUInt32& outlen);
};



