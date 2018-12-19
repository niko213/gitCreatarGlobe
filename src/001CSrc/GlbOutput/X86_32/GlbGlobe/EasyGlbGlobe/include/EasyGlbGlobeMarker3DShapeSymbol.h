#pragma once
#include "EasyGlbGlobeSymbol.h"
#include "osg/ShapeDrawable"
namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeMarker3DShapeSymbol : public CEasyGlbGlobeSymbol
	{
	public:
		CEasyGlbGlobeMarker3DShapeSymbol(void);
		~CEasyGlbGlobeMarker3DShapeSymbol(void);
		/**
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		void DealDraw(osg::Shape *shape,
			osg::ShapeDrawable *shapeFillDrawable,
			osg::ShapeDrawable *shapeOutDrawable,
			GlbMarker3DShapeSymbolInfo *marker3DInfo,
			CGlbFeature *feature);
	};
}

