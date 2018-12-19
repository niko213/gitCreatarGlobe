/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeGridTile.h
* @brief   ������ ͷ�ļ�
*
* �����������CGlbGlobeGridTile���class
*
* @version 1.0
* @author  ����
* @date    2018-7-17 16:10
*********************************************************************/
#pragma once
#include <osg/BoundingBox>
#include <osg/Polytope>
#include "IGlbDataEngine.h"
#include "GlbReference.h"
#include "GlbGlobeTypes.h"
#include "GlbExtent.h"
#include "glbref_ptr.h"
#include "IGlbGlobeTask.h"
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>

namespace GlbGlobe
{
	class CGlbGlobeGridLayer;
	class CGlbGlobeGridTile : public CGlbReference
	{
	public:
		/**
		* @brief ����鹹�캯��
		* @param [in] grid �������
		* @param [in] level �����ȼ�
		* @param [in] south,north,west,east  ����� �����ϱ� �߽�
		* @param [in] minAlt,maxAlt ��������ͺ���� �߳� 
		* @param [in] parent ����	
		* @return  ��	
		*/
		CGlbGlobeGridTile(CGlbGlobeGridLayer* grid,glbInt32 level,
			glbDouble south,glbDouble north,glbDouble west,glbDouble east,
			glbDouble minAlt=0,glbDouble maxAlt=0,
			CGlbGlobeGridTile* parent=NULL);
		/**
		* @brief �������������
		* @return ��
		*/
		~CGlbGlobeGridTile(void);
		/**
		* @brief ������ʼ��
		* @return  ��
		*/
		void Initialize(glbBool	load_direct=false);
		/**
		* @brief ��ȡ������Ƿ��ʼ��
		* @return  �ѳ�ʼ������true
		-	 û�г�ʼ������false
		*/
		glbBool IsInitialized();
		/**
		* @brief ��������
		* @return  ��
		*/
		void Update();
		/**
		* @brief  ��������ݼ���
		* @note  ������ʼ��ʱ�ļ����̵߳��ô˽ӿ�
		* @return  ��
		*/
		void LoadData();
		/**
		* @brief ���������
		* @param [in] dispos_direct �Ƿ�ֱ������ �ƺ�����Ҫ�˲�����������
		* @param [in] dispose_level_zero_tile ����0�������
		* @return ��
		*/
		void Dispose(glbBool dispos_direct=false ,glbBool dispose_level_zero_tile = false);
		/**
		* @brief ��ȡ�����ȼ�
		* @return �����ȼ�
		*/
		glbInt32 GetLevel();
		/**
		* @brief ��ȡ���������к�
		* @param [out] row �к�
		* @param [out] column �к�
		* @return ��
		*/
		void GetPosition(glbInt32& row, glbInt32& column,glbInt32& layer);
		/**
		* @brief �������������к�
		* @param [in] row �к�
		* @param [in] column �к�
		* @return ��
		*/
		glbBool SetPosition(glbInt32 row, glbInt32 column,glbInt32 layer);
		/**
		* @brief ��ȡ������ڵ�XYZ����ĸ�������
		* @param [out] xCellNum x����ĸ�������
		* @param [out] yCellNum y����ĸ�������
		* @param [out] zCellNum z����ĸ�������
		* @return ��
		*/
		void GetXYZCellNum(glbInt32& xCellNum, glbInt32& yCellNum,glbInt32& zCellNum);
		/**
		* @brief ����������ڵ�XYZ����ĸ�������
		* @param [in] xCellNum x����ĸ�������
		* @param [in] yCellNum y����ĸ�������
		* @param [in] zCellNum z����ĸ�������
		* @return true
		*/
		glbBool SetXYZCellNum(glbInt32 xCellNum, glbInt32 yCellNum,glbInt32 zCellNum);
		/**
		* @brief ��ȡ�����ķ�Χ
		* @param [out] south,north,west,east  ����� �����ϱ� �߽�
		* @param [out] minAlt,maxAlt ��������ͺ���� �߳� 
		* @return ��
		*/
		void GetRange(glbDouble& east, glbDouble& west, glbDouble& south, glbDouble& north, glbDouble& minAlt, glbDouble& maxAlt);

		/**
		* @brief ��ȡ�����ķ�Χ
		* @return ����鷶Χ
		*/
		CGlbExtent* GetExtent();
		/**
		* @brief ��ȡ��������һ�����׿�
		* @return ����
		*/
		CGlbGlobeGridTile* GetParentTile();
		/**
		* @brief ��ȡ������Ƿ�ɼ�
		* @return �ɼ�����true
		-	���ɼ�����false
		*/
		glbBool IsVisible(); 
		/**
		* @brief ��ȡ������ڸ����еķ�λ
		* @return ��λ
		- GlB_SOUTHWEST	����
		- GLB_SOUTHEAST ����
		- GLB_NORTHWEST ����
		- GLB_NORTHEAST ����
		*/
		GlbGlobeChildLocationEnum GetTileLocation();
		/**
		* @brief ��ȡ�����Ķ����ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeGridTile* GetNorthEastChild();
		/**
		* @brief ��ȡ�����������ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeGridTile* GetNorthWestChild();
		/**
		* @brief ��ȡ�����Ķ����ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeGridTile* GetSouthEastChild();
		/**
		* @brief ��ȡ�����������ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeGridTile* GetSouthWestChild();
	private:
		// ��������
		void ComputeBoundBox();
		// �жϿ��Ƿ���Ҫ����
		glbBool IsTileSplit();
		// �����ĸ��ӿ�
		void ComputeChildren();
		// ���ظ��������� 
		glbBool LoadTileData();
		// �����������������ڵ�(����geometry)
		osg::ref_ptr<osg::Group> CreateElevatedMesh();
		// ��������ת��Ļ����
		void WorldToScreen(glbDouble &x, glbDouble &y, glbDouble &z);
		// �����ȡ�����������ɽ���Զ�Ŀ�����
		glbBool GetSortedChildTiles(std::map<glbDouble/*���������*/,glbref_ptr<CGlbGlobeGridTile>>& tiles);	
		// ����������surface
		void DestructorTileSurface(GlbGridTileSurface* pTileSurface);
		// �ж��Ƿ����ڲ��� -- ���ڲ��鷵��true
		glbBool IsInsideTile();
		GlbTileLocationEnum ComputeTileLocation();
		// �ж��ڲ�tile�Ƿ����
		glbBool isInsideTileSplit();
		// �жϱ�Եtile�Ƿ����
		glbBool isEdgeTileSplit(GlbTileLocationEnum lc);
		glbBool testEdgeTileWestEdge(glbDouble xmin,glbDouble xmax,glbDouble ymin,glbDouble ymax,glbDouble zmin,glbDouble zmax,osg::Vec3d cameraPos);
		glbBool testEdgeTileEastEdge(glbDouble xmin,glbDouble xmax,glbDouble ymin,glbDouble ymax,glbDouble zmin,glbDouble zmax,osg::Vec3d cameraPos);
		glbBool testEdgeTileSouthEdge(glbDouble xmin,glbDouble xmax,glbDouble ymin,glbDouble ymax,glbDouble zmin,glbDouble zmax,osg::Vec3d cameraPos);
		glbBool testEdgeTileNorthEdge(glbDouble xmin,glbDouble xmax,glbDouble ymin,glbDouble ymax,glbDouble zmin,glbDouble zmax,osg::Vec3d cameraPos);
	private:
		CGlbGlobeGridLayer*	mpr_grid;				//�������
		glbref_ptr<CGlbGlobeGridTile> mpr_nwchild;	//�����ӿ�
		glbref_ptr<CGlbGlobeGridTile> mpr_nechild;	//�����ӿ�
		glbref_ptr<CGlbGlobeGridTile> mpr_swchild;	//�����ӿ�
		glbref_ptr<CGlbGlobeGridTile> mpr_sechild;	//�����ӿ�
		glbref_ptr<CGlbGlobeGridTile> mpr_parent_tile;	//����
		GlbGlobeChildLocationEnum mpr_location_enum;	//�����ڸ����е�λ��
		//osg::ref_ptr<osg::MatrixTransform> mpr_node;	//osg�ڵ�
		osg::ref_ptr<osg::Group> mpr_node;				//osg�ڵ� - switch
		glbDouble mpr_south;							//�ϱ߽�
		glbDouble mpr_north;							//���߽�
		glbDouble mpr_east;								//���߽�
		glbDouble mpr_west;								//���߽�
		glbDouble mpr_minalt;							//��С�߶�
		glbDouble mpr_maxalt;							//���߶�
		glbDouble mpr_center_longitude;					//�����ĵ�ľ���
		glbDouble mpr_center_latitude;					//�����ĵ��γ��
		glbref_ptr<CGlbExtent>	mpr_extent;				//����鷶Χ		
		osg::BoundingBox mpr_boundbox;					//����鷶Χ
		//std::vector<osg::Vec3> mpr_surrond_pointvec;
		osg::Vec3d		mpr_local_origin;				//������׼��[Ĭ��Ϊ���������ĵ�]
		glbBool mpr_isinitialized;						//��ʼ����־
		glbBool	mpr_isLoadingData;						//���ڼ������ݱ�־
		glbBool mpr_isDataLoaded;						//��־�����Ƿ��Ѿ��������
		glbInt32 mpr_level;								//�ȼ�
		glbInt32 mpr_row;								//�к�
		glbInt32 mpr_column;							//�к�
		glbInt32 mpr_layer;								//���
		GlbGlobeTypeEnum mpr_globe_type;				//globe��������
		glbDouble* mpr_data;							//ԭʼ�����dom����
		glbInt32	mpr_xCellNum;						//�����x����ĸ�������
		glbInt32	mpr_yCellNum;						//�����y����ĸ�������
		glbInt32	mpr_zCellNum;						//�����z����ĸ�������	
		//glbInt32	mpr_vertex_count;					

		glbDouble	mpr_longitude_span;
		glbDouble	mpr_latitude_span;	

		GlbGridTileSurface* mpr_pTileSurface;			//�������������

		glbref_ptr<IGlbGlobeTask>	mpr_gridTask;		//�������ݼ�������

		osg::ref_ptr<osg::Geode> mpr_p_geode;
		osg::ref_ptr<osg::Geometry> mpr_p_northWestgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_northEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southWestgeom;	
	};
}

