/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbPath.h
  * @brief   文件路径操作类头文件
  *
  * 这个档案定义CGlbPath、CGlbFindFile这个类
  * 主要对文件路径进行操作，及文件夹遍历
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 10:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "GlbWString.h"

/**
  * @brief CGlbPath文件路径类
  *
  * 该类把对文件路径进行操作，包括获取当前执行路径、当前exe全文件名、
  * 相对路径转绝对路径，绝对路径转相对路径、
  * 创建文件夹、删除文件夹、重命名等功能。
  * 方便使用，全部是静态函数
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 10:40
  */
class GLBCOMM_API CGlbPath
{
public:
/**
  * @brief 获取 执行目录
  *
  * @return 失败返回长度为0的字符串
  */
	static CGlbWString GetExecDir();
/**
  * @brief 获取 模块目录
  *
  * @return 失败返回长度为0的字符串
  */
	static CGlbWString GetModuleDir(HMODULE hmd);
/**
  * @brief 获取当前工作目录
  *
  *
  * @return 失败返回长度为0的字符串
  */
	static CGlbWString GetCurrentDir();
/**
  * @brief 获取当前工作目录
  *
  */
	static void SetCurrentDir(CGlbWString& dir);
/**
  * @brief 获取文件目录部分
  *
  * @param fileFullName 文件全路径
  * @return 失败返回长度为0的字符串
  */
	static CGlbWString GetDir(CGlbWString fileFullName);
/**
  * @brief 获取文件名(含后缀)
  *
  *
  * @param fileFullName 文件全路径
  * @return 失败返回长度为0的字符串
  */
	static CGlbWString GetFileName(CGlbWString fileFullName);
/**
  * @brief 获取文件名(不含后缀)
  *
  * @param fileNameOrFullName 带后缀的文件名或文件全路径
  * @return 失败返回长度为0的字符串
  */
	static CGlbWString GetBaseName(CGlbWString fileNameOrFullName);
/**
  * @brief 获取文件扩展名
  *
  * @param fileNameOrFullName 带后缀的文件名或文件全路径
  * @return 失败返回长度为0的字符串
  */
	static CGlbWString GetExtentName(CGlbWString fileNameOrFullName);
/**
  * @brief 移除文件扩展名
  *
  * @param fileNameOrFullName 带后缀的文件名或文件全路径
  * @return 失败返回长度为0的字符串
  */
	static CGlbWString RemoveExtName(CGlbWString fileNameOrFullName);
/**
  * @brief 相对路径转绝对路径
  *
  * 静态函数，根据相对路径获取绝对路径
  * C: + One\Two\Three ==>C:\One\Two\Three
  *
  * @param strRelDir 相对路径
  * @param strRefDir 参考路径
  * @return 成功返回字符串，失败返回NULL
  */
	static glbWChar* RelativeToAbsolute(const glbWChar *strRelDir, const glbWChar *strRefDir);
/**
  * @brief 绝对路径转相对路径
  *
  * 静态函数，根据绝对路径获取相对路径
  * 如 c:\a\b\path + c:\a\x\y\file ==>..\..\x\y\file
  *
  * @param strAbsDir 绝对路径
  * @param strRefDir 参考路径
  * @return 成功返回字符串，失败返回NULL
  */
	static glbWChar* AbsoluteToRelative(const glbWChar *strAbsDir, const glbWChar *strRefDir);

/**
  * @brief 修改文件名
  *
  * 静态函数，修改文件名
  *
  * @param oldFullfilename 旧文件全路径名
  * @param newFullfilename 新文件全路径名
  * @return 成功返回true，失败返回false
  */
	static glbBool RenameFile(const glbWChar *oldFullfilename, const glbWChar *newFullfilename);
/**
  * @brief 修改文件夹名
  *
  * 静态函数，修改文件夹名
  *
  * @param oldDirname 旧文件夹名
  * @param newDirname 新文件夹名
  * @return 成功返回true，失败返回false
  */
	static glbBool RenameDir(const glbWChar *oldDirname, const glbWChar *newDirname);
/**
  * @brief 判断文件是否存在
  *
  * @param  fileFullName 文件完全名
  * @return 成功返回true，失败返回false
  */
	static glbBool FileExist(const glbWChar *fileFullName);
/**
  * @brief 创建文件夹
  *
  * 静态函数，创建文件夹
  *
  * @param dirname 文件夹名
  * @return 成功返回true，失败返回false
  */
	static glbBool CreateDir(const glbWChar *dirname);
/**
  * @brief 删除文件夹
  *
  * 静态函数，删除文件夹及其下面的文件和子文件夹
  *
  * @param dirname 文件夹名
  * @return 成功返回true，失败返回false
  */
	static glbBool DeleteDir(const glbWChar *dirname);
/**
  * @brief 判断目录是否存在
  *
  * @return 成功返回true，失败返回false
  */
	static glbBool DirExist(const glbWChar* dirname);
/**
  * @brief 删除文件
  *
  * @param  fileFullName 文件完全路径
  * @return 成功true,失败false
  */
	static glbBool DeleteFile(const glbWChar* fileFullName);
};

/**
  * @brief CGlbFindFile文件遍历类
  *
  * 该类实现了对文件夹下文件的遍历
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 10:40
  */
class GLBCOMM_API CGlbFindFile
{
public:
/**
  * @brief 构造函数
  *
  * 初始化成员变量
  *
  * @return 无
  */
	CGlbFindFile();
/**
  * @brief 析构函数
  *
  * 释放mpr_hfind查找句柄
  *
  * @return 无
  */
	~CGlbFindFile();

/**
  * @brief 获取下一个文件名
  *
  * 获取下一个文件名，结束返回"",不带路径，自己可以加上的
  *
  * @return 成功返回字符串，失败或结束返回NULL
  */	
	//glbWChar* GetNext();
	CGlbWString GetNext();
/**
  * @brief 创建文件查找类
  *
  * 静态函数，查找文件夹下面的文件
  *
  * @param dir    文件夹名
  * @param extname 过滤类型
  * @return 成功返回CGlbFindFile，失败返回NULL
  */
	static CGlbFindFile* CreateFind(const glbWChar *dir, const glbWChar *extname);
private:
	WIN32_FIND_DATAW mpr_finddata;	///<文件查找结构体
	HANDLE           mpr_hfind;		///<文件查找句柄
	glbBool          mpr_isEnd;		///<是否结束
};
