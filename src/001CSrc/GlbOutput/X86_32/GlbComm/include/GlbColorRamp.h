/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    colorRamp.h
  * @brief   ɫ������ ����/��ֵ/��ν��� ͷ�ļ�
  *
  * �����������CGlbColorRamp�����
  *
  * @version 1.0
  * @author  lfc
  * @date    2018-8-7 14:40
*********************************************************************/
#pragma once
#include "GlbCommTypes.h"
#include "GlbReference.h"
#include <vector>
#include <map>
/**
* @brief ɫ������.
*/
enum GlbRampTypeEnum
{
	GLB_Ramp_Single = 0,			// ��ֵɫ��		S
	GLB_Ramp_Gradient = 1,			// ����ɫ�� ����ʼ��ɫ����ֹ��ɫ�� G
	GLB_Ramp_MultiGradient = 2		// ��ν���ɫ�� MG
};
/**
* @brief ��ɫ
*/
struct GlbColor
{
public:
	GlbColor(){}
	GlbColor(glbDouble r, glbDouble g, glbDouble b, glbDouble a)
	{
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
	}
	glbDouble m_r;
	glbDouble m_g;
	glbDouble m_b;
	glbDouble m_a;

};
/**
* @brief Ԥ����ɫ������(GRamp)
*/
enum GlbGradientModeEnum
{
	GLB_GRamp_Unknown = 0,			//δ֪
	GLB_GRamp_BlueToRed = 1,		//��-��
	GLB_GRamp_RedToBlue = 2,		//��-��
	GLB_GRamp_BrownToYellow = 3,	//��-��
	GLB_GRamp_RedToGreen = 4,		//��-��
	GLB_GRamp_GreenToRed = 5		//��-��
};

/**
* @brief Ԥ����ɫ������(MGRamp)
*/
enum GlbMGradientModeEnum
{
	GLB_MGRamp_Unknown = 0,		//δ֪
	GLB_MGRamp_BlueToRed = 1,	//��-��-��-��
	GLB_MGRamp_Brown = 2,		//��-��-��
	GLB_MGRamp_Rainbow1 = 3,	//��1-Ʒ1-��1-��1-��0.3-��0.7-��1-��0.7-��0.3
	GLB_MGRamp_Rainbow2 = 4,	//��Ȼ���������
	GLB_MGRamp_Rainbow3 = 5,	//�������̻ƳȺ�
	GLB_MGRamp_GreenToRed = 6	//��-��-��	
};
/**
* @brief ɫ���� ����ͼ���е���С��Ԫ��������ʼ��/��ֹ�����ɫ��λ����Ϣ��ɫ���м����ɫ����ֹ�����Բ�ֵ
*/
class GLBCOMM_API CGlbColorSeg : public CGlbReference
{
public:
	CGlbColorSeg();
	CGlbColorSeg(GlbColor fromClr, GlbColor toClr, glbDouble fromVal, glbDouble toVal);
	~CGlbColorSeg();

	/**
	* @brief ���ɫ������ɫ��Ϣ/λ����Ϣ.
	* @param[in]  fromClr,ɫ����ʼ��ɫ
	* @param[in]  toClr,ɫ����ֹ��ɫ
	* @param[in]  fromVal,ɫ����ʼλ��
	* @param[in]  toVal,ɫ����ֹλ��
	* return
	*/
	void SetParameters(GlbColor fromClr, GlbColor toClr, glbDouble fromVal, glbDouble toVal);

	/**
	* @brief ��ȡɫ������ɫ��Ϣ/λ����Ϣ.
	* @param[out]  fromClr,ɫ����ʼ��ɫ
	* @param[out]  toClr,ɫ����ֹ��ɫ
	* @param[out]  fromVal,ɫ����ʼλ��
	* @param[out]  toVal,ɫ����ֹλ��
	* return
	*/
	void GetParameters(GlbColor& fromClr, GlbColor& toClr, glbDouble& fromVal, glbDouble& toVal);

	/**
	* @brief ��λ����Ϣ�����ɫ����
	* @param[in]  val,λ����Ϣ����ΧΪ[0,1]
	* @param[out]  color,���ص���ɫ��Ϣ�����Ϊval����Ӧ��ɫ����ɫ��������βλ������ɫ�����Բ�ֵ���ã�val����ɫ����Χ��ʱ������ɫ���о���val�������ɫ��
	* return  val�뷵�ص���ɫ����Ӧλ�õľ��볤�ȣ�val��ɫ���ڲ�����0���������ж�val��ͬһɫ��������ɫ���е�������ɫֵ��
	*/
	glbDouble GetColor(glbDouble val, GlbColor& color);

private:
	//��ֹ����ɫ��Ϣ
	GlbColor m_fromColor;
	GlbColor m_toColor;
	//��ֹ��λ����Ϣ
	glbDouble  m_fromValue;
	glbDouble  m_toValue;
};
/**
* @brief ɫ��������ɫ����������Ϣ
*/
class GLBCOMM_API CGlbColorRamp : public CGlbReference
{
public:
	CGlbColorRamp(GlbRampTypeEnum rampType = GLB_Ramp_Gradient);
	~CGlbColorRamp();

	/**
	* @brief ����ɫ�������ͣ���ֵ/�ֶ�/����
	* @param[in]  rampType,ɫ������
	* return
	*/
	void SetRampType(GlbRampTypeEnum rampType);

	/**
	* @brief ��ȡɫ�������ͣ���ֵ/�ֶ�/����
	* @param[out]  rampType,ɫ������
	* return
	*/
	void GetRampType(GlbRampTypeEnum& rampType);


#pragma region ��ֵɫ��(SRamp)
	/**
	* @brief ��ӵ�ֵͼ������ɫ��Ϣ.
	* @param[in]  clr ,��ֵͼ����ɫ
	* @param[in]  name,��ɫ��Ӧ������.
	* return �ɹ�����true,ʧ�ܷ���false
	*/
	glbBool AddSRampItem(GlbColor clr,const glbWChar* name);

	/**
	* @brief ��ȡ��ֵͼ������ɫ����.
	* return ��ɫ����
	*/
	glbInt32 GetSRampItemCount();

	/**
	* @brief ��ȡ��ֵͼ���е�����ֵΪidx����ɫ��������Ϣ
	* @param[in]  idx ,����
	* @param[in]  name,��ɫ��Ӧ������.
	* @param[out]  clr,idx��Ӧ����ɫ.
	* return �ɹ�����true,ʧ�ܷ���false��idx���ڷ�Χ�ڣ�
	*/
	glbBool GetSRampItem(glbInt32 idx, glbWChar** name, GlbColor& clr);

	/**
	* @brief ��ȡ��ֵͼ��������Ϊname����ɫ
	* @param[in]  name,��ɫ��Ӧ������.
	* @param[out] clr,��ɫ.
	* return �ɹ�����true,ʧ�ܷ���false��idx���ڷ�Χ�ڣ�
	*/
	glbBool GetSRampItemByName(const glbWChar* name, GlbColor& clr);

	/**
	* @brief ɾ����ֵͼ��������ֵΪidx����ɫ��������Ϣ
	* @param[in]  idx,����.
	* return �ɹ�����true,ʧ�ܷ���false��idx���ڷ�Χ�ڣ�
	*/
	glbBool RemoveSRampItem(glbInt32 idx);

	/**
	* @brief ɾ����ֵͼ��������Ϊname����ɫ��������Ϣ
	* @param[in]  name,����.
	* return �ɹ�����true,ʧ�ܷ���false
	*/
	glbBool RemoveSRampItemByName(const glbWChar* name);

	/**
	* @brief ɾ����ֵͼ����������ɫ��������Ϣ
	* return
	*/
	void RemoveAllSRampItems();
#pragma endregion

#pragma region ����ɫ�� ����ʼ��ɫ����ֹ��ɫ��(GRamp)
	/**
	* @brief ���ý���ͼ���е���ɫ�����ԡ����Ƶ���Ϣ
	* @param[in]  fromClr,��ʼ��ɫ.
	* @param[in]  toClr,��ֹ��ɫ.
	* @param[in]  fromVal,��ʼ����.
	* @param[in]  toVal,��ֹ����.
	* @param[in]  fromName,��ʼ����.
	* @param[in]  toName,��ֹ����.
	* return
	*/
	void SetGRampColor(GlbColor fromClr, GlbColor toClr, glbDouble fromVal, glbDouble toVal, const glbWChar* fromName, const glbWChar* toName);
	
	/**
	* @brief ��Ԥ����ɫ��RampType�����ý���ͼ���е����ԡ����Ƶ���Ϣ
	* @param[in]  RampType,ɫ��.
	* @param[in]  fromVal,��ʼ����.
	* @param[in]  toVal,��ֹ����.
	* @param[in]  fromName,��ʼɫ������.
	* @param[in]  toName,��ֹɫ������.
	* return
	*/
	void SetGRampColorByMode(GlbGradientModeEnum RampType, glbDouble fromVal, glbDouble toVal, const glbWChar* fromName, const glbWChar* toName);
	
	/**
	* @brief ��ȡ����ͼ����Ԥ����ɫ��
	* return ����ͼ����Ԥ����ɫ����ʽ
	*/
	GlbGradientModeEnum GetGrampMode();

	/**
	* @brief ��ȡ����ɫ����ɫ����λ�á�����������Ϣ
	* @param[out]  fromCol,��ʼ��ɫ.
	* @param[out]  toCol,��ֹ��ɫ.
	* @param[out]  fromVal,��ʼ����.
	* @param[out]  toVal,��ֹ����.
	* @param[out]  fromName,��ʼɫ������.
	* @param[out]  toName,��ֹɫ������.
	* return
	*/
	glbBool GetGRampItem(GlbColor& fromClr, GlbColor& toClr, glbDouble& fromVal, glbDouble& toVal, glbWChar** fromName, glbWChar** toName);
	
	/**
	* @brief ��ȡ����ͼ���У�����Ϊval����Ӧ����ɫֵ
	* @param[in]  val,����ֵ.
	* @param[out]  clr,����Ϊval����ɫ.
	* return �ɹ�����true, ʧ�ܷ���false
	*/
	glbBool GetGRampColor(glbDouble val, GlbColor& clr);

#pragma endregion

#pragma region ��ν���ɫ�� (MGRamp)
	/**
	* @brief ��MGRamp�����ɫ����Ϣ
	* @param[in]  fromClr,�����ɫ������ʼ��ɫ��Ϣ.
	* @param[in]  toClr,�����ɫ������ֹ��ɫ��Ϣ.
	* @param[in]  fromPos,�����ɫ������ʼλ����Ϣ.
	* @param[in]  toPos,�����ɫ������ֹ��ɫ��Ϣ.
	* return
	*/
	void AddMGRampColorSeg(GlbColor fromClr, GlbColor toClr,glbDouble fromPos, glbDouble toPos); //fromPos,,toPos [0-1]

	/**
	* @brief ��ȡMGRamp��ɫ������
	* return MGRamp��ɫ������
	*/
	glbInt32 GetMGRampColorSegCount();

	/**
	* @brief ��ȡMGRamp������ֵΪidx��ɫ����Ϣ
	* @param[in]  idx,����.
	* @param[out]  fromClr,ɫ������ʼ��ɫ��Ϣ.
	* @param[out]  toClr,ɫ������ֹ��ɫ��Ϣ.
	* @param[out]  fromPos,ɫ������ʼλ����Ϣ.
	* @param[out]  toPos,ɫ������ֹ��ɫ��Ϣ.
	* return ��ȡ�ɹ�����true�� ��ȡʧ�ܷ���false
	*/
	glbBool GetMGRampColorSeg(glbInt32 idx,GlbColor& fromClr, GlbColor& toClr,glbDouble& fromPos, glbDouble& toPos);

	/**
	* @brief ɾ��MGRamp������ֵΪidx��ɫ����Ϣ
	* @param[in]  idx,����.
	* return ɾ���ɹ�����true�� ɾ��ʧ�ܷ���false
	*/
	glbBool RemoveMGRampColorSeg(glbInt32 idx);

	/**
	* @brief ����MGRamp������ֵΪidx��ɫ����Ϣ
	* @param[in]  idx,����.
	* @param[in]  colSeg,ɫ����Ϣ.
	* return ���óɹ�����true�� ����ʧ�ܷ���false
	*/
	glbBool SetMGRampColorSeg(glbInt32 idx, CGlbColorSeg colSeg);

	/**
	* @brief ��ȡMGRamp������ֵΪidx��ɫ����Ϣ
	* @param[in]  idx,����.
	* @param[out]  colSeg,ɫ����Ϣ.
	* return ��ȡ�ɹ�����true�� ��ȡʧ�ܷ���false
	*/
	glbBool GetMGRampColorSeg(glbInt32 segIndex, CGlbColorSeg& colSeg);

	/**
	* @brief ���MGRamp�е�ɫ����Ϣ
	* return 
	*/
	void RemoveAllMGRampColorSeg();

	/**
	* @brief ����MGRamp��ɫ��Ԥ������Ϣ
	* @param[in]  RampType,Ԥ����ɫ������.
	* return
	*/
	void SetMGRampMode(GlbMGradientModeEnum RampType);

	/**
	* @brief ��ȡMGRamp��ɫ��Ԥ������Ϣ
	* return MGRamp��ɫ��Ԥ��������
	*/
	GlbMGradientModeEnum GetMGRampMode();

	/**
	* @brief ���MGRamp��λ�á�����ֵ������ӳ���ϵ
	* @param[in]  colorPos,λ����Ϣ.
	* @param[in]  mapVal,������Ϣ.
	* @param[in]  mapName,������Ϣ.
	* return ��ӳɹ�����true�� ���ʧ�ܷ���false
	*/
	glbBool AddMGRampMapping(glbDouble colorPos, glbDouble mapVal, const glbWChar* mapName/*����*/);

	/**
	* @brief ��ȡMGRamp��λ�á�����ֵ������ӳ���ϵ������
	* return ӳ������
	*/
	glbInt32 GetMGRampMappingCount();

	/**
	* @brief ��ȡMGRamp������ֵΪidx��λ�á�����ֵ������ӳ���ϵ
	* @param[in]  idx,����.
	* @param[out]  colorPos,λ����Ϣ.
	* @param[out]  mapVal,������Ϣ.
	* @param[out]  name,������Ϣ.
	* return ��ȡ�ɹ�����true�� ��ȡʧ�ܷ���false
	*/
	glbBool GetMGRampMapping(glbInt32 idx, glbDouble& colorPos, glbDouble& mapVal, glbWChar** name /*����*/);

	/**
	* @brief ɾ��MGRamp������ֵΪidx��λ�á�����ֵ������ӳ���ϵ
	* @param[in]  idx,����.
	* return ɾ���ɹ�����true�� ɾ��ʧ�ܷ���false
	*/
	glbBool RemoveMGRampMapping(glbInt32 idx);

	/**
	* @brief ɾ��MGRamp�����е�λ�á�����ֵ������ӳ���ϵ
	* return ɾ���ɹ�����true�� ɾ��ʧ�ܷ���false
	*/
	void RemoveAllMGRampMapping();

	/**
	* @brief ��ȡMGRamp������Ϊval����ɫ��Ϣ
	* @param[in]  val,������Ϣ.
	* @param[out]  clr,��ɫ��Ϣ.
	* return ��ȡ�ɹ�����true�� ��ȡʧ�ܷ���false
	*/
	glbBool GetMGRampColor(glbDouble val, GlbColor& clr);
#pragma endregion

private:
	/********************************
	*func:��ʼ��ɫ�����ֱ�Ϊ��ֵ/�ֶ�/����ɫ�����Ĭ�����ݣ�
	********************************/
	void InitRampData();
	/********************************
	*func:Ԥ��ɫ��
	********************************/
	void PreInstallRampData();

private:
	GlbRampTypeEnum				m_rampType;//ɫ�����ͣ���ֵ/����/��ν���

	GlbGradientModeEnum			m_GradientMode; // ����ɫ��ģʽ
	GlbMGradientModeEnum		m_MGradientMode; // ��ν���ɫ��ģʽ

	std::vector<std::pair<CGlbWString /*����*/, GlbColor/*��ɫ*/>> m_SRamp_rampData;//��ֵͼ������-��ɫ ���ݶ�
	std::pair<CGlbColorSeg/*ɫ��*/, std::pair<CGlbWString /*�������*/, CGlbWString /*�յ�����*/> >  m_GRamp_rampData;//����ͼ��ɫ����������
	
	std::vector<CGlbColorSeg>		m_MGRamp_rampData;//��ν���ͼ��������ɫ��
	std::vector<std::pair<glbDouble /*pos*/, std::pair<CGlbWString /*����*/, glbDouble /*pro*/> > > m_Gradient_valueVec;//��ν���ͼ����λ��/����ӳ���ϵ
	
	std::vector<std::vector<CGlbColorSeg> >	m_MGradient_rampData_preInstall;//Ԥ��ɫ�����ݣ�MGRamp��
	std::vector<CGlbColorSeg>	m_Gradient_rampData_preInstall;//Ԥ��ɫ�����ݣ�GRamp��
};
//}

