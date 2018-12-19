/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbPath.h
  * @brief   �ļ�·��������ͷ�ļ�
  *
  * �����������CGlbPath��CGlbFindFile�����
  * ��Ҫ���ļ�·�����в��������ļ��б���
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
  * @brief CGlbPath�ļ�·����
  *
  * ����Ѷ��ļ�·�����в�����������ȡ��ǰִ��·������ǰexeȫ�ļ�����
  * ���·��ת����·��������·��ת���·����
  * �����ļ��С�ɾ���ļ��С��������ȹ��ܡ�
  * ����ʹ�ã�ȫ���Ǿ�̬����
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 10:40
  */
class GLBCOMM_API CGlbPath
{
public:
/**
  * @brief ��ȡ ִ��Ŀ¼
  *
  * @return ʧ�ܷ��س���Ϊ0���ַ���
  */
	static CGlbWString GetExecDir();
/**
  * @brief ��ȡ ģ��Ŀ¼
  *
  * @return ʧ�ܷ��س���Ϊ0���ַ���
  */
	static CGlbWString GetModuleDir(HMODULE hmd);
/**
  * @brief ��ȡ��ǰ����Ŀ¼
  *
  *
  * @return ʧ�ܷ��س���Ϊ0���ַ���
  */
	static CGlbWString GetCurrentDir();
/**
  * @brief ��ȡ��ǰ����Ŀ¼
  *
  */
	static void SetCurrentDir(CGlbWString& dir);
/**
  * @brief ��ȡ�ļ�Ŀ¼����
  *
  * @param fileFullName �ļ�ȫ·��
  * @return ʧ�ܷ��س���Ϊ0���ַ���
  */
	static CGlbWString GetDir(CGlbWString fileFullName);
/**
  * @brief ��ȡ�ļ���(����׺)
  *
  *
  * @param fileFullName �ļ�ȫ·��
  * @return ʧ�ܷ��س���Ϊ0���ַ���
  */
	static CGlbWString GetFileName(CGlbWString fileFullName);
/**
  * @brief ��ȡ�ļ���(������׺)
  *
  * @param fileNameOrFullName ����׺���ļ������ļ�ȫ·��
  * @return ʧ�ܷ��س���Ϊ0���ַ���
  */
	static CGlbWString GetBaseName(CGlbWString fileNameOrFullName);
/**
  * @brief ��ȡ�ļ���չ��
  *
  * @param fileNameOrFullName ����׺���ļ������ļ�ȫ·��
  * @return ʧ�ܷ��س���Ϊ0���ַ���
  */
	static CGlbWString GetExtentName(CGlbWString fileNameOrFullName);
/**
  * @brief �Ƴ��ļ���չ��
  *
  * @param fileNameOrFullName ����׺���ļ������ļ�ȫ·��
  * @return ʧ�ܷ��س���Ϊ0���ַ���
  */
	static CGlbWString RemoveExtName(CGlbWString fileNameOrFullName);
/**
  * @brief ���·��ת����·��
  *
  * ��̬�������������·����ȡ����·��
  * C: + One\Two\Three ==>C:\One\Two\Three
  *
  * @param strRelDir ���·��
  * @param strRefDir �ο�·��
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	static glbWChar* RelativeToAbsolute(const glbWChar *strRelDir, const glbWChar *strRefDir);
/**
  * @brief ����·��ת���·��
  *
  * ��̬���������ݾ���·����ȡ���·��
  * �� c:\a\b\path + c:\a\x\y\file ==>..\..\x\y\file
  *
  * @param strAbsDir ����·��
  * @param strRefDir �ο�·��
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	static glbWChar* AbsoluteToRelative(const glbWChar *strAbsDir, const glbWChar *strRefDir);

/**
  * @brief �޸��ļ���
  *
  * ��̬�������޸��ļ���
  *
  * @param oldFullfilename ���ļ�ȫ·����
  * @param newFullfilename ���ļ�ȫ·����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static glbBool RenameFile(const glbWChar *oldFullfilename, const glbWChar *newFullfilename);
/**
  * @brief �޸��ļ�����
  *
  * ��̬�������޸��ļ�����
  *
  * @param oldDirname ���ļ�����
  * @param newDirname ���ļ�����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static glbBool RenameDir(const glbWChar *oldDirname, const glbWChar *newDirname);
/**
  * @brief �ж��ļ��Ƿ����
  *
  * @param  fileFullName �ļ���ȫ��
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static glbBool FileExist(const glbWChar *fileFullName);
/**
  * @brief �����ļ���
  *
  * ��̬�����������ļ���
  *
  * @param dirname �ļ�����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static glbBool CreateDir(const glbWChar *dirname);
/**
  * @brief ɾ���ļ���
  *
  * ��̬������ɾ���ļ��м���������ļ������ļ���
  *
  * @param dirname �ļ�����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static glbBool DeleteDir(const glbWChar *dirname);
/**
  * @brief �ж�Ŀ¼�Ƿ����
  *
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static glbBool DirExist(const glbWChar* dirname);
/**
  * @brief ɾ���ļ�
  *
  * @param  fileFullName �ļ���ȫ·��
  * @return �ɹ�true,ʧ��false
  */
	static glbBool DeleteFile(const glbWChar* fileFullName);
};

/**
  * @brief CGlbFindFile�ļ�������
  *
  * ����ʵ���˶��ļ������ļ��ı���
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 10:40
  */
class GLBCOMM_API CGlbFindFile
{
public:
/**
  * @brief ���캯��
  *
  * ��ʼ����Ա����
  *
  * @return ��
  */
	CGlbFindFile();
/**
  * @brief ��������
  *
  * �ͷ�mpr_hfind���Ҿ��
  *
  * @return ��
  */
	~CGlbFindFile();

/**
  * @brief ��ȡ��һ���ļ���
  *
  * ��ȡ��һ���ļ�������������"",����·�����Լ����Լ��ϵ�
  *
  * @return �ɹ������ַ�����ʧ�ܻ��������NULL
  */	
	//glbWChar* GetNext();
	CGlbWString GetNext();
/**
  * @brief �����ļ�������
  *
  * ��̬�����������ļ���������ļ�
  *
  * @param dir    �ļ�����
  * @param extname ��������
  * @return �ɹ�����CGlbFindFile��ʧ�ܷ���NULL
  */
	static CGlbFindFile* CreateFind(const glbWChar *dir, const glbWChar *extname);
private:
	WIN32_FIND_DATAW mpr_finddata;	///<�ļ����ҽṹ��
	HANDLE           mpr_hfind;		///<�ļ����Ҿ��
	glbBool          mpr_isEnd;		///<�Ƿ����
};
