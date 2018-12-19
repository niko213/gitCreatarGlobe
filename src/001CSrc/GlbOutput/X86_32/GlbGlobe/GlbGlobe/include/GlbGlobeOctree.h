/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbGlobeOctree.h
  * @brief   Globe八叉树类头文件
  *
  * 这个档案定义CGlbGlobeOctree这个class,管理非贴地形绘制的object对象(添加，删除)
  *
  * @version 1.0
  * @author  马林
  * @date    2014-5-13 10:40
*********************************************************************/

#pragma once
#include "GlbAxisAlignedBox.h"

#include <list>
namespace GlbGlobe
{
	class CGlbGlobeObject;
	class GLB_DLLCLASS_EXPORT CGlbGlobeOctree
	{
	public:
		CGlbGlobeOctree(CGlbGlobeOctree* p);
		~CGlbGlobeOctree(void);

		/** Adds an CGlbGlobeObject to this octree level.
		@remarks
		This is called by the OctreeSceneManager after
		it has determined the correct Octree to insert the node into.
		*/
		void AddObject(CGlbGlobeObject* obj);
		/** Removes an  CGlbGlobeObject to this octree level.
		 */
		void RemoveObject(CGlbGlobeObject* obj);
		/** Returns the number of scene nodes attached to this octree
		*/
		long GetNumObjects()
		{
			return mpr_NumObjects;
		}
		/** Determines if this octree is twice as big as the given box.
		@remarks
		This method is used by the OctreeSceneManager to determine if the given
		box will fit into a child of this octree.
		*/
		bool IsTwiceSize( const CGlbAxisAlignedBox &box ) const;

		/**  Returns the appropriate indexes for the child of this octree into which the box will fit.
		@remarks
		This is used by the OctreeSceneManager to determine which child to traverse next when
		finding the appropriate octree to insert the box.  Since it is a loose octree, only the
		center of the box is checked to determine the octant.
		*/
		void GetChildIndexes( CGlbAxisAlignedBox& , long *x, long *y, long *z ) const;

		/** Creates the AxisAlignedBox used for culling this octree.
		@remarks
		Since it's a loose octree, the culling bounds can be different than the actual bounds of the octree.
		*/
		void GetBoundBox( CGlbAxisAlignedBox * );

	public:
		CGlbAxisAlignedBox mpr_box;
		/** Vector containing the dimensions of this octree / 2
		*/
		osg::Vec3 mpr_halfsize;

		/** 3D array of children of this octree.
		@remarks
		Children are dynamically created as needed when nodes are inserted in the Octree.
		If, later, all the nodes are removed from the child, it is still kept around.
		*/
		CGlbGlobeOctree * mpr_p_children[ 2 ][ 2 ][ 2 ];

		typedef std::list< CGlbGlobeObject* > ObjectList;
		/** Public list of SceneNodes attached to this particular octree
		*/
		ObjectList mpr_objects;

	protected:
		///number of CGlbGlobeObject in this octree and all its children.
		long mpr_NumObjects;

		///parent octree
		CGlbGlobeOctree * mpr_p_parent;

		/** Increments the overall node count of this octree and all its parents
		*/
		inline void _ref()
		{
			mpr_NumObjects++;

			if ( mpr_p_parent != 0 ) mpr_p_parent -> _ref();
		};

		/** Decrements the overall node count of this octree and all its parents
		*/
		inline void _unref()
		{
			mpr_NumObjects--;

			if ( mpr_p_parent != 0 ) mpr_p_parent -> _unref();
		};
		
	};
}
