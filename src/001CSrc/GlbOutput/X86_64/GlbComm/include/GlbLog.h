/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbLog.h
  * @brief   ��־������ͷ�ļ�
  *
  * �����������CGlbLog����ӿ���
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
*********************************************************************/
#pragma once
#pragma warning(disable:4251)//����ȡ
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "IGlbLogHandler.h"

#define GlbLogOutput g_glblog.Output   ///< ��־��Ϣխ�ַ�������궨��
#define GlbLogWOutput g_glblog.WOutput ///< ��־��Ϣ���ַ�������궨��

/**
  * @brief CGlbLog��־��
  *
  * �������־��Ϣ���ո�ʽ���������־����У�
  * ����ά����־���
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40 
  */
class GLBCOMM_API CGlbLog
{
public:
/**
  * @brief ���캯��
  *
  * ���mpr_logHandlers����
  *
  * @param logInfo ��־��Ϣ
  * @return ��
  */
	CGlbLog(void);
/**
  * @brief ��������
  *
  * ����mpr_logHandlers���飬����о�����ڣ��ͷ�
  *
  * @return ��
  */
	virtual ~CGlbLog(void);

/**
  * @brief խ�ַ���־��Ϣ�������
  *
  * ������־���ͺ������ʽ������־��Ϣ���
  *
  * @param logType ��־����
  * @param format  ��ʽ�ַ���
  * @param ...     ������̬����
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	glbBool Output(GlbLogTypeEnum logType, glbChar* format,...);
/**
  * @brief ���ַ���־��Ϣ�������
  *
  * ������־���ͺ������ʽ������־��Ϣ���
  *
  * @param logType ��־����
  * @param format  ��ʽ�ַ���
  * @param ...     ������̬����
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	glbBool WOutput(GlbLogTypeEnum logType, glbWChar* format,...);
/**
  * @brief ������־���
  *
  * �����ǰ������ڣ�����ʧ��
  *
  * @param logHandler ��־���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	glbBool AddLogHandler(IGlbLogHandler* logHandler);
/**
  * @brief ɾ����־���
  *
  * �����ǰ��������ڣ�ɾ��ʧ��
  *
  * @param logHandler ��־���
  * @return true��ʾ�ɹ���false��ʾʧ��
  */
	glbBool DeleteLogHandler(IGlbLogHandler* logHandler);

private:
	std::vector<IGlbLogHandler*> mpr_logHandlers;///< ��־�������
};

extern GLBCOMM_API CGlbLog g_glblog;///< ȫ�ֱ�������־������
