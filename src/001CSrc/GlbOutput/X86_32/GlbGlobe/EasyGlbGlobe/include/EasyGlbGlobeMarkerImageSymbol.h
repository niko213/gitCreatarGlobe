#pragma once
#include "EasyGlbGlobeSymbol.h"
#include "osg/Geometry"
#include "EasyGlbGlobeAutoTransform.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeMarkerImageSymbol : public CEasyGlbGlobeSymbol
	{
	public:
		CEasyGlbGlobeMarkerImageSymbol(void);
		~CEasyGlbGlobeMarkerImageSymbol(void);
		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Geometry *CreateOutlineQuad(osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec);
	};
}

