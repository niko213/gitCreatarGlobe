/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    ClbRobjectMeshBox.h
* @brief   
* @version 1.0
* @author  xt
* @date    2016-08-31 15:30
*********************************************************************/
#pragma once

#ifndef __GLB_MESH_BOX_H__
#define __GLB_MESH_BOX_H__

#include "GlbExtent.h"
#include "GlbGlobeRObject.h"
#include "IGlbGeometry.h"
#include "GlbTin.h"

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Node>
#include <vector>

namespace GlbGlobe
{
	struct GlbTinSymbolInfo;

	//用来将Tin对象分格子
	struct  GLB_DLLCLASS_EXPORT GridCell
	{
	public:
		GridCell();

		osg::ref_ptr<osg::Node> node; //该格子下的OSG节点
		CGlbExtent extend;  //格子的外扩

		osg::ref_ptr <osg::Vec3Array> verticesArrays;
	};

	struct XYIndex
	{
		XYIndex()
		{
			xindex = yindex = zindex = 0;
		}
		XYIndex(unsigned int x,unsigned int y)
		{
			xindex = x;
			yindex = y;
		}
		XYIndex (unsigned int x,unsigned int y,unsigned int z)
		{
			xindex = x;
			yindex = y;
			zindex = z;
		}
		unsigned int xindex;
		unsigned int yindex;
		unsigned int zindex;
	};


	/************************************************************************/
	/* 
		格子节点的矩阵关系:
		
			C代表坐标系
			mt 矩阵
			Root----> parentM -----> localCenter ----> node
	 平面:	mt0	 C0	  mt1        C1     mt2      C2     
	 球面:
	*/
	/************************************************************************/

//#define USE_Z_AXIS

#ifdef USE_Z_AXIS
	#define CellPointer GridCell*** 
#else
	#define CellPointer GridCell**
#endif

	class GLB_DLLCLASS_EXPORT ClbRobjectMeshBox
	{
	public:

		ClbRobjectMeshBox();
		/*
			构建Box
		*/
		glbBool buildMeshBox(CGlbGlobeRObject * obj,IGlbGeometry *geo);
		
		/************************************************************************/
		/* 更新Grids                                                             
		* @parentMatrix :offset scale矩阵
		* @ glbeMode  :是否是球面模式
		/************************************************************************/
		void	  updateGrids(const osg::Matrix parentMatrix,const GlbTinSymbolInfo * tinSmblInfo,
					glbref_ptr<CGlbFeature> feature,GLboolean globeMode);

		/************************************************************************/
		/* 求交集                                                              */
		/************************************************************************/

		virtual glbBool intersect(const osg::Vec3d start,const osg::Vec3d end,osg::Vec3d&intersectPoint);

		/************************************************************************/
		/* 此时obj对象是否已经被分格子 
		* @ return 已经格子化返回true
		/************************************************************************/
		glbBool valid()						{return !_boxs.empty();}

		/************************************************************************/
		/* 清除格子																 */
		/************************************************************************/

		void		clearGrids();

		/************************************************************************/
		/* 测试格子 加到场景中													*/
		/************************************************************************/

		osg::Node * testShowGridNode();


	private:

		/************************************************************************/
		/* 获取格子父节点矩阵
		/************************************************************************/
		osg::Matrix getParentNodeMatrix(CGlbGlobeRObject * obj, IGlbGeometry *  geo);

		/************************************************************************/
		/* 获取格子局部平移的平移 相对于对象的中心
		/************************************************************************/
		osg::Vec3d  getLocalCenter(CGlbGlobeRObject * obj, IGlbGeometry *  geo);

		/************************************************************************/
		/* 格子划分完毕事件,将格子下的osg组织一个节点 
		*@XcellNum: 局部坐标系X轴下格子个数
		*@YcellNum: 局部坐标系Y轴 格子个数
		*@parentM :平移 缩放矩阵
		*@localCenter:局部对象中心点
		/************************************************************************/

		void  buildOver(const osg::Matrix&parentM,const osg::Matrixd&localCenter,
			unsigned int *axisNum, CellPointer grids);
		


		/************************************************************************/
		/* 初始化格子 
		  返回cellSize:格子大小
		 *@axisNum: 局部坐标系XY轴下格子个数 
		 *@grids : 初始化格子数组
		 *@OBB8Points: 初始化格子后，格子在局部空间的最大点和最小点 两个点
		/************************************************************************/
		CellPointer  initGrids(GLdouble & cellSize,unsigned int * axisNum,
			std::vector<osg::Vec3d>&OBB8Points);

		/************************************************************************/
		/* 		初始化AABB(局部坐标系)	
			获取局部坐标系坐标 已经局部坐标系下的AABB
		*@ OBB8Points 初始化格子后，格子在局部空间的最大点和最小点 两个点
		*@ local: true 球面,false :平面
		*@
		/************************************************************************/
		 void  initLocalAABB(CGlbGlobeRObject * obj, IGlbGeometry *  geo,std::vector<osg::Vec3d>&OBB8Points,
			glbBool local = false);
		
		/************************************************************************/
		/*  从Object对象中获取三角面片 将其放入格子
		*@geo:对象
		*@parentMatrix:平移缩放矩阵
		*@localCenter: 局部平移
		*@cellSize:当前格子大小
		*@axisNum:当前格子X轴分量
		*@grids :grid数组
		/************************************************************************/
		void   getAddTinTriangle(CGlbGlobeRObject * obj,CGlbTin * geo,osg::Vec3d& localCenter,
			GLdouble cellSize,unsigned int* axisNum,
			const osg::Matrixd&parentMatrix,CellPointer grids, 
			std::vector<osg::Vec3d>&OBB8Points);

		
		/************************************************************************/
		/* 	获取三角面片所在格子索引	
		*@ v0 v1 v2 三角形面片三个点
		*@cellSize:当前格子大小
		*@XCellNum:当前格子X轴分量
		*@YCellNum：当前格子Y轴分量 
		*@OBB8Points: 初始化格子后，格子在局部空间的最大点和最小点 两个点

		/************************************************************************/
		void   getGridIndex(const osg::Vec3d* v0,const osg::Vec3d* v1,const osg::Vec3d*v2
			,XYIndex & idx,GLdouble cellSize,unsigned int* axisNum,
			std::vector<osg::Vec3d>&OBB8Points);

		void  addTriangleToBox(const osg::Matrix&parentM,const osg::Matrixd&localCenter,const GridCell&cell, CellPointer grids);

		glbBool buildPlaneTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geo);
		glbBool buildPlaneMultTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geo);

		glbBool buildGlobeTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geo);
		glbBool buildGlobeMultTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geo);

	private:
		osg::Matrixd _worldToAABBSpace; //世界坐标到局部AABB坐标系的矩阵
		osg::Matrixd         _localMatrix;

		std::vector<GridCell> _boxs; //平移缩放的局部坐标系到AABB坐标系的矩阵
		
		GLdouble zCellWidth;
	};
}
#endif
