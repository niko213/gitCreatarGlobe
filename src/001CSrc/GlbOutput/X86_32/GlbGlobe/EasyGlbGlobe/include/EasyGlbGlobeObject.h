#pragma once

#include "GlbReference.h"
#include "EasyGlbGlobeTypes.h"
#include "GlbWString.h"
#include "libxml/tree.h"

namespace GlbGlobe
{
	class CEasyGlbGlobe;
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeObject : public CGlbReference
	{
	public:
		CEasyGlbGlobeObject(void);
		~CEasyGlbGlobeObject();
		/**
		* @brief 获取对象名称
		* @return 对象名称
		*/
		const glbWChar *GetName();
		/**
		* @brief 设置对象名称
		* @param[in]  name 对象名称
		*/
		void SetName(const glbWChar *name);
		/**
		* @brief 获取对象类型
		* @return 对象枚举类型
		*/
		virtual GlbGlobeObjectTypeEnum GetType();
		/**
		* @brief 获取对象ID
		* @return 对象ID
		*/
		glbInt32 GetId();
		/**
		* @brief 设置对象ID
		* @param[in]  id 对象ID
		*/
		void SetId(glbInt32 id);
		/**
		* @brief 获取对象所在组的ID
		* @return 对象所在组的ID
		*/
		glbInt32 GetGroupId();
		/**
		* @brief 设置对象所在组的ID
		* @param[in]  id 对象所在组的ID
		*/
		void SetGroupId(glbInt32 id);
		/**
		* @brief 获取场景对象
		* @return 场景对象
		*/
		CEasyGlbGlobe *GetGlobe();
		/**
		* @brief 设置场景对象
		* @param[in]  globe 场景对象
		*/
		virtual void SetGlobe(CEasyGlbGlobe *globe);	
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
		* @brief 获取对象激活方式
		* @return 对象激活方式枚举
		*/
		GlbGlobeActiveActionEnum GetActiveAction();
		/**
		* @brief 设置对象激活方式
		* @param[in]  action 激活方式枚举
		*/
		void SetActiveAction(GlbGlobeActiveActionEnum action);	
	protected:
		glbInt32 mpt_id;///<对象Id
		glbInt32 mpt_grpId;///对象所在组Id
		CGlbWString mpt_name;///<对象名字
		CEasyGlbGlobe *mpt_globe;///<场景对象
		GlbGlobeActiveActionEnum mpt_action;///<激活动作
	};
}


