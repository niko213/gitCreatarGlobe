/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    IGlbProgress.h
  * @brief   ��������ͷ�ļ�
  *
  * �����������IGlbProgress����ӿ���,
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-12 9:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"

/**
  * @brief IGlbProgress ���Ƚӿ���
  *
  * ��һ���ӿڣ�ʹ����ʵ�ָýӿڣ����½���
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-12 9:40
  */
class GLBCOMM_API IGlbProgress
{
public:
	IGlbProgress(){}
	virtual void  Update(double percentage,const wchar_t * msg=0) =0;
};
