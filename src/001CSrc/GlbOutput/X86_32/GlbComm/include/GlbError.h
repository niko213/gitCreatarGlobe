#pragma once
#include <map>
using namespace std;
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "GlbWString.h"
#include "GlbCriticalSection.h"
/**
  * @brief ������Ϣ��
  *
  * ά�����һ��ִ�з����Ĵ�����Ϣ
  * 
  * 					 
  * @version 1.0
  * @author  LIUJH
  * @date    2014-3-7 14:40 
  */
class GLBCOMM_API CGlbError
{
public:
	CGlbError(void);
	~CGlbError(void){}
	/**
      * @brief ���ô�����Ϣ
      *      
      *
      * @param errinfo ������Ϣ
      * @return ��
    */
	void SetLastError(const glbWChar* errinfo);
	/**
      * @brief ��ȡ������Ϣ
      *      
      *       
      * @return ������Ϣ
	  *         ���û�У�����NULL
	  *
	  *         ���÷�����Ҫ�������ٷ��ص�ָ�롣
    */
	glbWChar* GetLastError();
private:
	map<glbInt32/*�߳�Id*/,CGlbWString/*������Ϣ*/> mpr_lasterrs;
	GlbCriticalSection                              mpr_critical;
};

extern GLBCOMM_API CGlbError g_glbError;
#define GlbSetLastError g_glbError.SetLastError
#define GlbGetLastError g_glbError.GetLastError