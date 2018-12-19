/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeWMTSTile.h
* @brief   WMTS ��ͼ��Ƭ ͷ�ļ�
*
* �����������CGlbGlobeWMTSTile���class
*
* @version 1.0
* @author  ����
* @date    2017-6-5 10:10
*********************************************************************/
#pragma once
#include <osg/BoundingBox>
#include <osg/Polytope>
#include "glbref_ptr.h"
#include "GlbReference.h"
#include "GlbGlobeTypes.h"
#include "IGlbGlobeTask.h"
#include "GlbExtent.h"
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Texture2D>

namespace GlbGlobe
{
	class CGlbGlobeWMTSLayer;
	class CGlbGlobeWMTSTile : public CGlbReference
	{
	public:
		/**
		* @brief ��Ƭ�鹹�캯��
		* @param [in] WMTSLayer wmtsͼ�����
		* @param [in] level ���ο�ȼ�
		* @param [in] south,north,west,east  ��Ƭ �����ϱ� �߽�
		* @param [in] minAlt,maxAlt ��Ƭ����ͺ���� �߳� 
		* @param [in] parent ����Ƭ	
		* @return  ��	
		*/
		CGlbGlobeWMTSTile(CGlbGlobeWMTSLayer* player,glbInt32 level,
			glbDouble south,glbDouble north,glbDouble west,glbDouble east,
			glbDouble minAlt=0,glbDouble maxAlt=0,
			CGlbGlobeWMTSTile* parent=NULL);
		/**
		* @brief ��Ƭ��������
		* @return ��
		*/
		~CGlbGlobeWMTSTile(void);

		/**
		* @brief ��Ƭ��ʼ��
		* @return  ��
		*/
		void Initialize(glbBool	load_direct=false);
		/**
		* @brief ��ȡ��Ƭ�Ƿ��ʼ��
		* @return  �ѳ�ʼ������true
		-	 û�г�ʼ������false
		*/
		glbBool IsInitialized();
		/**
		* @brief ���ο����		
		* @return  ��
		*/
		void Update();
		/**
		* @brief ���ο����ݼ���		
		* @return  ��
		*/
		void LoadData();
		/**
		* @brief ���ο�����		
		* @param [in] dispose_level_zero_tile ����0�����ο�
		* @return ��
		*/
		void Dispose(glbBool dispose_level_zero_tile = false);
		/**
		* @brief ���ο�ӱ�
		* @return ��
		*/
		void Merge();
		/**
		* @brief ��ȡ���ο��ΨһID - key
		* @return ���ο�key
		*/
		glbInt32 GetKey();
		/**
		* @brief ��ȡ���ο�ȼ�
		* @return ���ο�ȼ�
		*/
		glbInt32 GetLevel();
		/**
		* @brief ��ȡ���ο�����к�
		* @param [out] row �к�
		* @param [out] column �к�
		* @return ��
		*/
		void GetPosition(glbInt32& row, glbInt32& column);
		/**
		* @brief ���õ��ο�����к�
		* @param [in] row �к�
		* @param [in] column �к�
		* @return ��
		*/
		glbBool SetPosition(glbInt32 row, glbInt32 column);
		/**
		* @brief ��ȡ���ο�ķ�Χ
		* @param [out] south,north,west,east  ���ο� �����ϱ� �߽�
		* @param [out] minAlt,maxAlt ���ο����ͺ���� �߳� 
		* @return ��
		*/
		void GetRange(glbDouble& east, glbDouble& west, glbDouble& south, glbDouble& north, glbDouble& minAlt, glbDouble& maxAlt);
		/**
		* @brief ��ȡ���ο����һ�����׿�
		* @return ����
		*/
		CGlbGlobeWMTSTile* GetParentTile();
		/**
		* @brief ��ȡ���ο��Ƿ�ɼ�
		* @return �ɼ�����true
		-	���ɼ�����false
		*/
		glbBool IsVisible();
		/**
		* @brief ��ȡ���ο��ڸ����еķ�λ
		* @return ��λ
		- GlB_SOUTHWEST	����
		- GLB_SOUTHEAST ����
		- GLB_NORTHWEST ����
		- GLB_NORTHEAST ����
		*/
		GlbGlobeChildLocationEnum GetTileLocation();
		/**
		* @brief ��ȡ���ο�Ķ����ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeWMTSTile* GetNorthEastChild();
		/**
		* @brief ��ȡ���ο�������ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeWMTSTile* GetNorthWestChild();
		/**
		* @brief ��ȡ���ο�Ķ����ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeWMTSTile* GetSouthEastChild();
		/**
		* @brief ��ȡ���ο�������ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeWMTSTile* GetSouthWestChild();

		/**
		* @brief ��ȡָ��λ�õĸ߳�
		* @param [in] xOrLon, yOrLat λ��				 
				 [out] h �߳�
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool	GetPosHeight(glbDouble xOrLon,glbDouble yOrLat, glbDouble& h);
		/**
		* @brief ��ȡָ��λ�õĸ߳�
		* @param [in] xOrLon, yOrLat λ��				 
					  zOrAlt �߳�
		* @return �ɹ�����true��ʧ�ܷ���false
		* @note �������ã�������
		*/
		glbBool	ModifyPosHeight(glbDouble xOrLon,glbDouble yOrLat, glbDouble zOrAlt);
	private:
		void UpdateGlobe();		
		void UpdateFlat();

		void MergeGlobe();
		void MergeFlat();

		void ComputeChildren();
		// ���ص��ο�dom��dem���� 
		glbBool LoadTileData();
		// �������ο��������ڵ�(����geometry)
		osg::ref_ptr<osg::Group> CreateElevatedMesh();
		// ��ȡ��row,column�����߳�ֵ
		glbFloat GetHeightData(glbFloat* p_heightdatas, glbInt32 column, glbInt32 row, GlbGlobeChildLocationEnum location);
		// ���¼���mpr_boundbox
		void ComputeBoundBox();
		// ��������ת��Ļ����
		void WorldToScreen(glbDouble &x, glbDouble &y, glbDouble &z);
		// �жϿ��Ƿ���Ҫ����
		glbBool IsTileSplit();
		// ��ȡ��������tile����
		CGlbGlobeWMTSTile* GetNorthDirectionTile();		
		// ��ȡ��������tile����
		CGlbGlobeWMTSTile* GetWestDirectionTile();
		// ��ȡ��������tile����
		CGlbGlobeWMTSTile* GetSouthDirectionTile();
		// ��ȡ��������tile����
		CGlbGlobeWMTSTile* GetEastDirectionTile();
		// ��ȡ����������ӽ���Զ˳�����е��ĸ�tile
		glbBool GetSortedChildTiles(std::map<glbDouble,glbref_ptr<CGlbGlobeWMTSTile>>& tiles);
		// �ָ�Ĭ��edge����߳�
		glbBool	RecoverEdgeVertexsAltitude(GlbTileEdgeEnum edge);

		glbBool isCoordValidate(double x, double y, double z);
	private:
		CGlbGlobeWMTSLayer*	mpr_wmtsLayer;				//wmtsͼ��
		glbref_ptr<CGlbGlobeWMTSTile> mpr_nwchild;		//�����ӿ�
		glbref_ptr<CGlbGlobeWMTSTile> mpr_nechild;		//�����ӿ�
		glbref_ptr<CGlbGlobeWMTSTile> mpr_swchild;		//�����ӿ�
		glbref_ptr<CGlbGlobeWMTSTile> mpr_sechild;		//�����ӿ�
		glbref_ptr<CGlbGlobeWMTSTile> mpr_parent_tile;	//����
		GlbGlobeChildLocationEnum	  mpr_location_enum;	//�����ڸ����е�λ��
		osg::ref_ptr<osg::Group>	  mpr_node;				//osg�ڵ� - switch
		osg::ref_ptr<osg::Texture2D> mpr_texture;		//���ο�����
		osg::ref_ptr<osg::Image> mpr_p_osgImg;			//��Ӧ���ɵ�osg::Image����	
		glbDouble mpr_south;							//�ϱ߽�
		glbDouble mpr_north;							//���߽�
		glbDouble mpr_east;								//���߽�
		glbDouble mpr_west;								//���߽�
		glbDouble mpr_minalt;							//��С�߶�
		glbDouble mpr_maxalt;							//���߶�
		glbDouble mpr_center_longitude;					//�����ĵ�ľ���
		glbDouble mpr_center_latitude;					//�����ĵ��γ��		
		glbref_ptr<CGlbExtent>	mpr_extent;				//���ο鷶Χ		
		osg::BoundingBox mpr_boundbox;					//���ο鷶Χ
		osg::Vec3d		mpr_local_origin;				//���ο��׼��[Ĭ��Ϊ���ο�����ĵ�]
		glbBool mpr_isinitialized;						//��ʼ����־
		glbBool	mpr_isLoadingData;						//���ڼ������ݱ�־
		glbBool mpr_isDataLoaded;						//��־�����Ƿ��Ѿ��������
		glbInt32 mpr_level;								//�ȼ�
		glbInt32 mpr_row;								//�к�
		glbInt32 mpr_column;							//�к�
		GlbGlobeTypeEnum mpr_globe_type;				//globe��������
		glbInt32		mpr_demsize;					//dem������
		glbFloat*		mpr_demdata;					//dem����17*17
		glbInt32		mpr_vertex_count;

		glbDouble		mpr_longitude_span;
		glbDouble		mpr_latitude_span;	

		// �����geometry
		osg::ref_ptr<osg::Geode>	mpr_p_geode;
		osg::ref_ptr<osg::Geometry> mpr_p_northWestgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_northEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southWestgeom;


		glbInt32	mpr_north_easthalf_merge_code;
		glbInt32	mpr_north_westhalf_merge_code;
		glbInt32	mpr_south_easthalf_merge_code;
		glbInt32	mpr_south_westhalf_merge_code;
		glbInt32	mpr_east_northhalf_merge_code;
		glbInt32	mpr_east_southhalf_merge_code;
		glbInt32	mpr_west_northhalf_merge_code;
		glbInt32	mpr_west_southhalf_merge_code;

		glbref_ptr<IGlbGlobeTask>	mpr_tileTask;
	};
}

