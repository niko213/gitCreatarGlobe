/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    IGlbLogHandler.h
  * @brief   ��־�����ͷ�ļ�
  *
  * �����������IGlbLogHandler����ӿ���,
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"

/**
  * @brief CGlbLog��־�����
  *
  * ��һ���ӿڣ�ʹ����ʵ�ָýӿڣ�����־��Ϣ���
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
  */
class GLBCOMM_API IGlbLogHandler
{
public:
/**
  * @brief ��־��Ϣ���
  *
  * ����־��Ϣ����ն�
  *
  * @param logInfo ��־��Ϣ
  * @return ��
  */
	virtual void Output(const wchar_t* logInfo)=0;
};

