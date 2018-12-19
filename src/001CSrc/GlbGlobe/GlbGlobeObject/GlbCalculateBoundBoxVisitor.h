/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbCalculateBoundBoxVisitor.h
* @brief   点对象头文件，文档定义CGlbCalculateBoundBoxVisitor类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-14 15:00
*********************************************************************/
#pragma once
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/BoundingBox>
#include <osg/BoundingSphere>
#include <osg/MatrixTransform>

namespace GlbGlobe
{
	/**
	* @brief  计算node节点及其下所有节点的空间范围boundbox类
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-14 15:00
	*/
	class CGlbCalculateBoundBoxVisitor : public osg::NodeVisitor
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbCalculateBoundBoxVisitor(void);
		/**
		* @brief 析构函数
		*/
		~CGlbCalculateBoundBoxVisitor(void);

		/**
		* @brief 访问节点操作
		*/
		virtual void apply(osg::Geode &node)
		{
			osg::BoundingBox bb;
			for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
			{			
				bb.expandBy(node.getDrawable(i)->getBound());
			}

			if (bb.valid())
			{
				// transform corners by current matrix
				osg::BoundingBoxd xbb;
				for (unsigned int i = 0; i < 8; ++i)
				{
					osg::Vec3d xv = bb.corner(i) * mpr_TransformMatrix;
					xbb.expandBy(xv);
				}
				mpr_BoundingBox.expandBy(xbb);
			}

			traverse(node);
		}

		/**
		* @brief 访问节点操作
		*/
		virtual void apply(osg::Transform& node)
		{
			osg::Matrix m;
			node.computeLocalToWorldMatrix(m,NULL);
			osg::Matrix currentMatrix = mpr_TransformMatrix;
			mpr_TransformMatrix.preMult(m);
			traverse(node);
			//恢复矩阵
			mpr_TransformMatrix = currentMatrix;

		}

		/**
		* @brief 获取对象节点包围盒
		* @return 节点包围盒
		*/
		osg::BoundingBoxd &getBoundBox(){  return mpr_BoundingBox; }

	private:
		osg::BoundingBoxd mpr_BoundingBox;	// bound box
		osg::Matrix mpr_TransformMatrix;	// current transform matrix
	};
}