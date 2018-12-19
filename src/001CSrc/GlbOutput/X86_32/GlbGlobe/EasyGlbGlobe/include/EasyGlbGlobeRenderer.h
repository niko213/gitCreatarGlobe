#pragma once
#include "GlbReference.h"
#include "EasyGlbGlobeObject.h"
#include "GlbWString.h"
#include "EasyGlbGlobeObjectRenderInfos.h"
#include <osg\Node>

namespace GlbGlobe
{
	class CEasyGlbGlobe;
	//class CEasyGlbGlobeFeatureLayer;
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeRenderer : public CGlbReference
	{
		//friend class CGlbGlobeFeatureLayer;
	public:
		CEasyGlbGlobeRenderer(void);
		~CEasyGlbGlobeRenderer(void);
		/**
		* @brief 获取渲染器名称
		* @return  渲染器名称
		*/
		const glbWChar* GetName();
		/**
		* @brief 设置渲染器名称
		* @param [in] name 渲染器名称
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief 设置几何字段
		* @param [in] field 几何字段名
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetGeoField(glbWChar* field);
		/**
		* @brief 获取几何字段名称
		* @return 几何字段名称
		*/
		const glbWChar* GetGeoField();
		/**
		* @brief 设置标注字段
		* @param [in] field 标注字段名
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetLabelField(glbWChar* field);
		/**
		* @brief 获取标注字段名称
		* @return 标注字段名称
		*/
		const glbWChar* GetLabelField();
		/**
		* @brief 设置渲染描述信息
		* @param [in] rinfo 渲染描述信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetRenderInfo(GlbRenderInfo* rinfo);
		/**
		* @brief 获取渲染描述信息
		* @return 渲染描述信息
		*/
		GlbRenderInfo* GetRenderInfo();
		/**
		* @brief 获取最小可见距离信息
		* @return 最小可见距离信息
		*/
		GlbRenderDouble* GetMinDistance();
		/**
		* @brief 设置最小可见距离信息
		* @param [in] dis 最小可见距离信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetMinDistance(GlbRenderDouble* dis);
		/**
		* @brief 获取最大可见距离信息
		* @return 最大可见距离信息
		*/	
		GlbRenderDouble* GetMaxDistance();
		/**
		* @brief 设置最大可见距离信息
		* @param [in] dis 最大可见距离信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetMaxDistance(GlbRenderDouble* dis);
		/**
		* @brief 获取渲染顺序
		* @return 渲染顺序
		*/	
		GlbRenderInt32* GetRenderOrder();
		/**
		* @brief 设置渲染顺序
		* @param [in] order 渲染顺序
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetRenderOrder(GlbRenderInt32* order);
		/**
		* @brief 获取加载顺序
		* @return 加载顺序
		*/
		GlbRenderInt32* GetLoadOrder();
		/**
		* @brief 设置加载顺序
		* @param [in] order 加载顺序
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetLoadOrder(GlbRenderInt32* order);
		/**
		* @brief 获取弹出信息
		* @return 弹出信息
		*/
		GlbRenderString* GetRMessage();
		/**
		* @brief 设置弹出信息
		* @param [in] msg 弹出信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetRMessage(GlbRenderString* msg);
		/**
		* @brief 获取提示信息
		* @return 提示信息
		*/
		GlbRenderString* GetToolTip();
		/**
		* @brief 设置提示信息
		* @param [in] tips 提示信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetToolTip(GlbRenderString* tips);
		/**
		* @brief 设置高程模式
		* @param [in] mode 高程模式
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode);
		/**
		* @brief 获取高程模式
		* @return 高程模式
		*/
		GlbAltitudeModeEnum GetAltitudeMode();
		/**
		* @brief 设置渲染器中对象显隐
		* @param [in] isShow true表示显示，false表示隐藏
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetShow(glbBool isShow);
		/**
		* @brief 序列化(加载)
		* @param [in] node xml加载类
		* @param [in] relativepath 相对路径
		* @return  成功返回true
		-	 失败返回false
		*/
		//glbBool Load(xmlNodePtr node,glbWChar* relativepath );
		/**
		* @brief 序列化(保存)
		* @param [in] node xml加载类
		* @param [in] relativepath 相对路径
		* @return  成功返回true
		-	 失败返回false
		*/
		//glbBool Save(xmlNodePtr node, glbWChar* relativepath);
		/**
		* @brief 根据对象id获取对象
		* @param [in] objId 对象id	
		* @return  成功返回对象指针
		-	 失败返回NULL
		*/
		CEasyGlbGlobeObject* GetObject(glbInt32 objId);
		/**
		* @brief 添加对象
		* @param [in] obj 对象指针	
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool AddObject(CEasyGlbGlobeObject* obj);
		/**
		* @brief 根据对象id删除对象
		* @param [in] objId 对象id	
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool RemoveObject(glbInt32 objId);
		/**
		* @brief 删除所有对象		
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool RemoveAllObjects();

		/**
		* @brief 获取渲染器中对象数量	  
		* @return  对象数量
		*/
		glbInt32 GetObjectCount();
		/**
		* @brief 根据要素feature创建对象
		* @param [in] feature 要素	
		* @return  成功返回新建的对象指针
		-	 失败返回NULL
		*/
		//CEasyGlbGlobeObject* CreateObject(CGlbFeature* feature);
		/**
		* @brief 获取渲染器的osg节点 
		* @return osg节点
		*/
		osg::Node* GetNode();
	private:
		// 拷贝Renderer中的相关属性到obj中,如renderorder,loadorder,minDistance.....
		void UpdateAttribute(CEasyGlbGlobeObject* obj);		
	private:
		CGlbWString               mpr_name;							//渲染器名称
		CGlbWString               mpr_geofield;						//几何字段
		CGlbWString               mpr_labelfield;					//注记字段
		glbBool		              mpr_isground;	   				    //是否地面对象
		glbref_ptr<GlbRenderInfo> mpr_renderinfo;					//渲染描述信息
		GlbRenderDouble*          mpr_mindistance;					//最小可见距离
		GlbRenderDouble*          mpr_maxdistance;					//最大可见距离
		GlbRenderInt32*           mpr_loadorder;					//加载顺序
		GlbRenderInt32*           mpr_renderorder;					//渲染顺序
		GlbRenderString*          mpr_message;						//弹窗信息
		GlbRenderString*          mpr_tooltips;						//提示信息
		GlbAltitudeModeEnum       mpr_altitudemode;					//高程模式
		CEasyGlbGlobe*            mpr_globe;
		osg::ref_ptr<osg::Node>   mpr_node;							//osg节点
		std::map<glbInt32,glbref_ptr<CEasyGlbGlobeObject>> mpr_objects;		//对象数组
		GlbCriticalSection	                           mpr_critical;		//修改操作 更新 临界区
		glbBool					  mpr_isShow;						//渲染器对象显隐
	};
}

