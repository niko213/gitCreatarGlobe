#pragma once
#include "EasyGlbGlobeSymbol.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeMarkerPixelSymbol : public CEasyGlbGlobeSymbol
	{
	public:
		CEasyGlbGlobeMarkerPixelSymbol(void);
		~CEasyGlbGlobeMarkerPixelSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}

