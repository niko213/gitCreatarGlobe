/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbTime.h
  * @brief   ʱ�����ͷ�ļ�
  *
  * �����������GlbGetNowTime��GlbTime2Str��GlbTime2WStr��GlbStr2Time��GlbStr2Time
  * 5��ȫ�ֺ���
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"

/**
  * @brief ��ȡ��ǰϵͳʱ��
  *
  * ��̬��������ȡ��ǰϵͳʱ�䣬��64λ������ʾ
  *
  * @return ����64λ������ʾ��ʱ��
  */
extern GLBCOMM_API glbInt64 GlbGetNowTime();
/**
  * @brief ת��ʱ���խ�ַ���
  *
  * ��̬���������ո�ʽ��ת��ʱ���խ�ַ���
  *
  * @param time   Ҫת����ʱ��
  * @param format ��ʽ�ַ��� %d���ڵĸ���Ӧ�ô���0С�ڵ���6
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
extern GLBCOMM_API glbChar* GlbTime2Str(glbInt64 time, const glbChar* format);
/**
  * @brief ת��ʱ��ɿ��ַ���
  *
  * ��̬���������ո�ʽ��ת��ʱ��ɿ��ַ���
  *
  * @param time   Ҫת����ʱ��
  * @param format ��ʽ�ַ��� %d���ڵĸ���Ӧ�ô���0С�ڵ���6
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
extern GLBCOMM_API glbWChar* GlbTime2WStr(glbInt64 time, const glbWChar* format);
/**
  * @brief ��խ�ַ���ת����ʱ��
  *
  * ��̬���������ո�ʽ����ʱ��խ�ַ���ת����ʱ��
  *
  * @param timestr   Ҫת����ʱ���ַ���
  * @param format    ��ʽ�ַ��� %d���ڵĸ���Ӧ�ô���0С�ڵ���6
  * @param time      ���������ת�����ʱ��
  * @return �ɹ�����true��ʧ�ܷ���false
  */
extern GLBCOMM_API glbBool GlbStr2Time(const glbChar* timestr, const glbChar* format, glbInt64* time);
/**
  * @brief �ѿ��ַ���ת����ʱ��
  *
  * ��̬���������ո�ʽ����ʱ����ַ���ת����ʱ��
  *
  * @param timestr   Ҫת����ʱ���ַ���
  * @param format    ��ʽ�ַ��� %d���ڵĸ���Ӧ�ô���0С�ڵ���6
  * @param time      ���������ת�����ʱ��
  * @return �ɹ�����true��ʧ�ܷ���false
  */
extern GLBCOMM_API glbBool GlbWStr2Time(const glbWChar* timestr, const glbWChar* format, glbInt64* time);

