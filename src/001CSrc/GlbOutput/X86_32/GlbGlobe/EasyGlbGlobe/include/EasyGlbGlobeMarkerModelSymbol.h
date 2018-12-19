#pragma once
#include "EasyGlbGlobeSymbol.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeMarkerModelSymbol : public CEasyGlbGlobeSymbol
	{
	public:
		CEasyGlbGlobeMarkerModelSymbol(void);
		~CEasyGlbGlobeMarkerModelSymbol(void);
		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo,glbInt32 level=0);
	};
}


