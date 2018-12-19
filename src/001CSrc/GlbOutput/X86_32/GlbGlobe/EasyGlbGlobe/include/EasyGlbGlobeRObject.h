#pragma once

#include "EasyGlbGlobeObject.h"
#include "GlbWString.h"
#include "GlbExtent.h"
#include "IGlbGeometry.h"
#include "GlbFeature.h"
#include "osg/Node"
#include "EasyGlbGlobeObjectRenderInfos.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeRObject : public CEasyGlbGlobeObject
	{
	public:
		CEasyGlbGlobeRObject(void);
		~CEasyGlbGlobeRObject(void);
		/**
		* @brief 设置元素
		* @param[in]  feature 对象对应的元素
		* @param[in]  geoField 几何字段
		*/
		void SetFeature(CGlbFeature *feature,const glbWChar* geoField=L"");
		/**
		* @brief 获取元素
		* @return 对象对应的元素
		*/
		CGlbFeature *GetFeature();
		/**
		* @brief 获取对象节点包围盒
		* @param[in]  isWorld true表示获取世界坐标包围盒(单位：米)
		*  -                                    false表示获取地理坐标包围盒（单位：度）
		* @return 节点包围盒
		*/
		virtual glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief 设置对象海拔模式
		* @param[in]  mode 海拔模式枚举
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，设置失败返回false
		*/
		virtual glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);

		/**
		* @brief 获取对象的海拔模式
		* @return 海拔模式枚举
		*/
		GlbAltitudeModeEnum GetAltitudeMode();

		/**
		* @brief 设置对象的渲染信息
		* @param[in]  renderInfo 对象的渲染信息
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);
		/**
		* @brief 获取对象的渲染信息
		* @return 对象的渲染信息
		*/
		virtual GlbRenderInfo *GetRenderInfo();

		/**
		* @brief 设置对象显隐
		* @param[in]  isShow 对象显示设置为true，隐藏设置为false
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetShow(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief 获取对象的显隐标识
		* @return 显示返回true，隐藏返回false
		*/
		glbBool IsShow();

		/**
		* @brief 设置对象的选中状态
		* @param[in]  isSelected 被拣选中设置为true，反之设置为false
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetSelected(glbBool isSelected);
		/**
		* @brief 获取对象的选中状态
		* @return 选中返回true，反之返回false
		*/
		glbBool IsSelected();
		/**
		* @brief 对象立即渲染	
		* @param[in]  level 对象渲染等级
		*/
		virtual void DirectDraw(glbInt32 level);
		/**
		* @brief AGG二维绘制到像素
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		virtual void DrawToImage(glbByte *image/*RGBA*/,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief 获取当前LOD级的osg节点
		* @return 当前LOD级的osg节点
		*/
		osg::Node *GetOsgNode();
		/**
		* @brief 反序列化对象，解析xml获取对象属性值
		* @param[in]  node libXML2节点
		* @param[in]  prjPath 工程文件目录
		* @return 反序列化成功返回true、反之返回false
		*/
		virtual glbBool Load(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief 序列化对象，序列化对象属性到xml工程文件
		* @param[in]  os 工程文件输出流
		* @param[in]  prjPath 工程文件目录
		* @return 序列化成功返回true、反之返回false
		*/
		virtual glbBool Save(xmlNodePtr node,const glbWChar* prjPath);

		/**
		* @brief 获取对象父节点
		* @return 父节点指针
		*/
		osg::Node *GetParentNode();
		/**
		* @brief 设置对象父节点
		* @param[in] 父节点指针
		*/
		virtual void SetParentNode(osg::Node *parent);

		/**
		* @brief 设置对象装载优先级
		* @param[in]  order 对象装载优先级
		*/
		virtual void SetLoadOrder(glbInt32 order);
		/**
		* @brief 获取对象装载优先级
		* @return 对象装载优先级
		*/
		glbInt32 GetLoadOrder();
		/**
		* @brief 设置对象渲染优先级
		* @param[in]  order 对象渲染优先级
		*/
		virtual void SetRenderOrder(glbInt32 order);
		/**
		* @brief 获取对象渲染优先级
		* @return 对象渲染优先级
		*/
		glbInt32 GetRenderOrder();
		/**
		* @brief 设置对象最大可见距离
		* @param[in]  distance 对象最大可见距离
		* @return 设置成功返回true，设置失败返回false
		*/
		virtual glbBool SetMaxVisibleDistance(glbDouble distance);
		/**
		* @brief 获取对象最大可见距离
		* @return 对象最大可见距离
		*/
		glbDouble GetMaxVisibleDistance();
		/**
		* @brief 设置对象最小可见距离
		* @param[in]  distance 对象最小可见距离
		* @return 设置成功返回true，设置失败返回false
		*/
		virtual glbBool SetMinVisibleDistance(glbDouble distance);
		/**
		* @brief 获取对象最小可见距离
		* @return 对象最小可见距离
		*/
		glbDouble GetMinVisibleDistance();
		/**
		* @brief 设置对象提示信息
		* @param[in]  tooltip 对象提示信息
		* @return 设置成功返回true，设置失败返回false
		* @note 当鼠标放到对象上时会显示出来
		*/
		virtual glbBool SetToolTip(/*CGlbWString*/ glbWChar* tooltip);
		/**
		* @brief 获取对象提示信息
		* @return 对象提示显示
		* @note 当鼠标放到对象上时会显示出来
		*/		
		const glbWChar* GetTooltip();
		/**
		* @brief 设置对象信息
		* @param[in]  msg 对象信息
		* @return 设置成功返回true，设置失败返回false
		* @note 当点击对象时会显示出来
		*/
		virtual glbBool SetRMessage(/*CGlbWString*/ glbWChar* msg);
		/**
		* @brief 获取对象信息
		* @return 对象信息
		* @note 当点击对象时会显示出来
		*/		
		const glbWChar* GetRMessage();
		/**
		* @brief 获取对象当前LOD级数
		* @return 对象当前LOD级数
		*/
		glbInt32 GetCurrLevel();

		void SetCustomData(osg::Object *data);

		osg::Object *GetCustomData();
	protected:
		glbInt32                mpt_currLevel;///<当前LOD级数
		osg::ref_ptr<osg::Node> mpt_node;///<OSG节点		
		osg::ref_ptr<osg::Node> mpt_parent;///<父节点		
		glbDouble               mpt_maxVisibleDistance;///<最大可见距离
		glbDouble               mpt_minVisibleDistance;///<最小可见距离
		glbInt32                mpt_renderOrder;///<渲染优先级 注:数值小的先绘制，数值大的后绘制 
		glbInt32                mpt_loadOrder;///<装载优先级
		GlbAltitudeModeEnum     mpt_altitudeMode;///<高程模式
		glbBool                 mpt_isSelected;///<是否选中
		glbBool                 mpt_isShow;///<用户控制是否显示		
		glbref_ptr<CGlbFeature> mpt_feature;///<附加元素
		CGlbWString             mpt_geoField;///<几何字段
		CGlbWString             mpt_tooltip;///<提示信息
		CGlbWString             mpt_message;///<信息	
		CEasyGlbGlobeObject *mpt_parentObj;///<父对象[组合对象用]		

		osg::ref_ptr<osg::Object> mpr_customData;
	};
}

