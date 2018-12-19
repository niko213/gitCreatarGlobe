#pragma once
#include "osg/Node"
#include "EasyGlbGlobeRObject.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeSymbol : public CGlbReference
	{
	public:
		CEasyGlbGlobeSymbol(void);
		~CEasyGlbGlobeSymbol(void);
		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		virtual osg::Node *Draw(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG二维绘制到像素
		* @param[in] obj 渲染对象
		* @param[in] geom 要渲染的几何体
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		virtual void DrawToImage(CEasyGlbGlobeRObject *obj,IGlbGeometry* geom,
			glbByte *image/*RGBA*/,glbInt32 imageW,
			glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief 贴地形绘制对象的二维轮廓线 单位：地理单位
		* @return  对象的2D轮廓
		*/
		virtual IGlbGeometry *GetOutline(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);
	protected:
		glbref_ptr<IGlbGeometry> mpt_outline;///<贴地形绘制对象的二维轮廓线 单位：地理单位
	};
}


