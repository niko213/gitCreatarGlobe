/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    IGlbProgress.h
  * @brief   进度条类头文件
  *
  * 这个档案定义IGlbProgress这个接口类,
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-12 9:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"

/**
  * @brief IGlbProgress 进度接口类
  *
  * 是一个接口，使用者实现该接口，更新进度
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
