#pragma once
#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <osg/Geometry>

namespace GlbGlobe
{
	/**
	* @brief  用于实现读取tin存出的osg,osgb,ive文件后z轴按比例放缩 类 -- 球模式下使用
	* @version 1.0
	* @author  malin
	* @date    2016-1-11 15:00	
	*/
	class CGlbTinZScaleVisitor :	public osg::NodeVisitor
	{
	public:
		CGlbTinZScaleVisitor(double zScale);
		~CGlbTinZScaleVisitor(void);

		virtual void apply(osg::Node &node)
		{
			osg::Transform* trans = node.asTransform();
			if (trans)
			{				
				osg::MatrixTransform* mt = trans->asMatrixTransform();
				if (mt)
				{
					const osg::Matrix mat = mt->getMatrix();
					osg::Vec3d pos = mat.getTrans();
					mpr_trans.set(pos.x(),pos.y(),pos.z());
				}
				osg::PositionAttitudeTransform* pt = trans->asPositionAttitudeTransform();
				if (pt)
				{
					osg::Vec3d pos = pt->getPosition();
					mpr_trans.set(pos.x(),pos.y(),pos.z());
				}
			}
			traverse(node);
		}
		virtual void apply(osg::Geode& node)
		{			
			for(unsigned int i=0;i<node.getNumDrawables();++i)
			{
				osg::Drawable* drawable = node.getDrawable(i);
				if (drawable==NULL) continue;	
				osg::Geometry* geom = drawable->asGeometry();
				if (geom)
					apply(geom);
			};		
			traverse(node);
		}

		void apply(osg::Geometry* geom)
		{						
			changeVertexCoord(geom);			
		}
	private:
		void changeVertexCoord(osg::Geometry* geom);
	private:
		double mpr_zScale;
		osg::Vec3 mpr_trans;		
	};
}

