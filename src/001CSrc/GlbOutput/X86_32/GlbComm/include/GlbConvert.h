/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbConvert.h
  * @brief   �ַ�����������ֵ����ת�������ļ�
  *
  * �����������CGlbConvert���class,Ϊ����ʹ�ã�ȫ���Ǿ�̬����
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 9:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "GlbString.h"
#include "GlbWString.h"
/**
  * @brief CGlbConvert����ת����
  *
  * ���Ա����ȫ���Ǿ�̬�������������
  * ʵ�����ַ���תint16��int32��int64��float��double
  *��int16��int32��int64��float��doubleת�ַ���
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-6 16:40
  */
class GLBCOMM_API CGlbConvert
{
public:
/**
  * @brief խ�ַ���תint16
  *
  * ��̬����������������ֻ��߳���16λ������Χ������
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����int16����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToInt16 (const glbChar* src, glbInt16& num);
/**
  * @brief խ�ַ���תint32
  *
  * ��̬����������������֣�����
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����int32����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToInt32 (const glbChar* src, glbInt32& num);
/**
  * @brief խ�ַ���תint64
  *
  * ��̬����������������֣�����
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����int64����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToInt64 (const glbChar* src, glbInt64& num);
/**
  * @brief խ�ַ���תfloat
  *
  * ��̬����������������ֻ��߳���float��Χ������
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����float����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToFloat (const glbChar* src, glbFloat& num);
/**
  * @brief խ�ַ���תdouble
  *
  * ��̬����������������֣�����
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����double����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToDouble (const glbChar* src, glbDouble& num);
/**
  * @brief ���ַ���תint16
  *
  * ��̬����������������ֻ��߳���16λ������Χ������
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����int16����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToInt16 (const glbWChar* src, glbInt16& num);
/**
  * @brief ���ַ���תint32
  *
  * ��̬����������������֣�����
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����int32����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToInt32 (const glbWChar* src, glbInt32& num);
/**
  * @brief ���ַ���תint64
  *
  * ��̬����������������֣�����
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����int64����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToInt64 (const glbWChar* src, glbInt64& num);
/**
  * @brief ���ַ���תFloat
  *
  * ��̬����������������ֻ��߳���Float��Χ������
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����Float����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToFloat (const glbWChar* src, glbFloat& num);
/**
  * @brief ���ַ���תDouble
  *
  * ��̬����������������ֻ��߲���Double������������
  *
  * @param src Ҫת�����ַ���
  * @param num ת�����Double����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	static bool ToDouble (const glbWChar* src, glbDouble& num);

/**
  * @brief int16תխ�ַ���
  *
  * ��̬��������int16ת��խ�ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbChar* Int16ToStr (glbInt16 num);
	static CGlbString Int16ToStr (glbInt16 num);
/**
  * @brief int32תխ�ַ���
  *
  * ��̬��������int32ת��խ�ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbChar* Int32ToStr (glbInt32 num);
	static CGlbString Int32ToStr (glbInt32 num);
/**
  * @brief int64תխ�ַ���
  *
  * ��̬��������int64ת��խ�ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbChar* Int64ToStr (glbInt64 num);
	static CGlbString Int64ToStr (glbInt64 num);
/**
  * @brief floatתխ�ַ���
  *
  * ��̬��������floatת��խ�ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbChar* FloatToStr (glbFloat num);
	static CGlbString FloatToStr (glbFloat num);
/**
  * @brief doubleתխ�ַ���
  *
  * ��̬��������doubleת��խ�ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbChar* DoubleToStr (glbDouble num);
	static CGlbString DoubleToStr (glbDouble num);
/**
  * @brief int16ת���ַ���
  *
  * ��̬��������int16ת�ɿ��ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbWChar* Int16ToWStr (glbInt16 num);
	static CGlbWString Int16ToWStr (glbInt16 num);
/**
  * @brief int32ת���ַ���
  *
  * ��̬��������int32ת�ɿ��ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbWChar* Int32ToWStr (glbInt32 num);
	static CGlbWString Int32ToWStr (glbInt32 num);
/**
  * @brief int64ת���ַ���
  *
  * ��̬��������int64ת�ɿ��ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbWChar* Int64ToWStr (glbInt64 num);
	static CGlbWString Int64ToWStr (glbInt64 num);
/**
  * @brief floatת���ַ���
  *
  * ��̬��������floatת�ɿ��ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbWChar* FloatToWStr (glbFloat num);
	static CGlbWString FloatToWStr (glbFloat num);
/**
  * @brief doubleת���ַ���
  *
  * ��̬��������doubleת�ɿ��ַ������
  *
  * @param num ��Ҫת����ֵ
  * @return �ɹ������ַ�����ʧ�ܷ���NULL
  */
	//static glbWChar* DoubleToWStr (glbDouble num);
	static CGlbWString DoubleToWStr (glbDouble num);
};

