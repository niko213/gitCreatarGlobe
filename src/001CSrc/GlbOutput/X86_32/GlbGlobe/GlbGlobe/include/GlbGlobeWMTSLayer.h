/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeWMTSLayer.h
* @brief   ������Ƭ��ͼͼ��ͷ�ļ�
*
* �����������CGlbGlobeWMTSLayer���class
*
* @version 1.0
* @author  ����
* @date    2017-6-1 10:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbGlobeLayer.h"
#include "GlbGlobeWMTSTile.h"
#include "libxml/tree.h"

namespace GlbGlobe
{

	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief ������Ƭ��ͼͼ����
	* @note ��������ȾWMTS������Ƭ����
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeWMTSLayer : public CGlbGlobeLayer
	{
		public:
		enum GLBOperationEnum
		{// �����������
			GLB_OPENERAION_ADD = 0,
			GLB_OPENERAION_REMOVE = 1,
			GLB_OPENERAION_MODIFY = 2
		};

		enum GLBOperationObjectEnum
		{// �����������
			GLB_NODE = 0,
			GLB_DRAWABLE = 1,
			GLB_VECARRAY = 2,
			GLB_TEXTURE = 3
		};
		/** 
		* @brief ���ο��޸���Ϣ�ṹ��
		* @note �����ڲ��ṹ,�����ο�ʹ��
		*/
		struct GlbTileOperation
		{
		public:	
			/**
			* @brief �ṹ�幹�캯��
			* @param addordelormodity ��������. 0ɾ��, 1���, 2�޸�
			* @param nodeORdrawableORvecarray ��������. 0�ڵ�, 1�ɻ��ƶ���, 2��������
			* @param p_tile_node �ڵ�
			* @param p_drawable �ɻ��ƶ���
			* @param p_vecarray �������� 
			* @return  ��	
			*/
			GlbTileOperation(	GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation, 
				osg::Group* p_tile_node,
				double compiletime=0.0);

			GlbTileOperation(	GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation, 
				osg::Geode* p_tile_geode_node,
				osg::Drawable* p_drawable=NULL, 
				osg::Vec3Array* p_vecarray=NULL);

			GlbTileOperation ( GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation,
				osg::Texture* p_texture = NULL,
				osg::Image* p_image = NULL);
			virtual ~GlbTileOperation();
		public:
			GLBOperationEnum _addordelormodify;				///< 0 - add; 1 - delete	; 2 - modify
			GLBOperationObjectEnum _nodeordrawableorvecarrayortexture;		///< 0 - node ; 1 - drawable	; 2 - vec3array ; 3 - texture
			osg::ref_ptr<osg::Group> _p_tile_node;			// �ڵ�
			osg::ref_ptr<osg::Geode> _p_tile_geode_node;	///< Ҷ�ӽڵ�
			osg::ref_ptr<osg::Drawable> _p_drawable; ///< �ɻ��ƶ���
			osg::ref_ptr<osg::Vec3Array> _p_vecarray; ///< ��������
			double _node_compiletime;						///< ����_p_tile_node��Ҫ��ʱ��
			osg::ref_ptr<osg::Texture>	_p_texture;
			osg::ref_ptr<osg::Image>	_p_image;
		};
	private:
		/** 
		* @brief ����osg�ڵ���»ص���
		* @author ����
		* @date    2014-5-12 11:10
		* @note �����ڲ���,����ժ�ҽڵ�,drawable
		*/
		class CGlbGlobeWMTSCallBack : public osg::NodeCallback
		{
		public:
			/**
			* @brief ����osg�ڵ�ص��๹�캯��
			* @param [in] pGlobeLayer wmtsͼ�����  
			* @return  ��
			*/
			CGlbGlobeWMTSCallBack(CGlbGlobeWMTSLayer* pGlobeLayer); 	
			/**
			* @brief ����osg�ڵ�ص�ʵ��
			* @param [in] node ���ε�osg�ڵ�
			* @param [in] nv osg�ڵ������	  
			* @return  ��
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		public:
			CGlbGlobeWMTSLayer* p_wmtslayer;
		};
	public:
		/**
		* @brief WMTSͼ�㹹�캯��
		* @return ��
		*/
		CGlbGlobeWMTSLayer(void);
		/**
		* @brief WMTSͼ����������
		* @return ��
		*/
		~CGlbGlobeWMTSLayer(void);

		 /* ����ӿ� */
		/**
		* @brief ��ȡͼ������
		* @return ͼ������ 
		*/
		const glbWChar* GetName();
		/**
		* @brief ����ͼ������
		* @param name ͼ������
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief ��ȡͼ������
		* @return  ͼ������
		*/
		GlbGlobeLayerTypeEnum GetType();
		/**
		* @brief ͼ�����л������أ�
		* @param [in] node xml��ȡ�ڵ�
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
		glbBool   Load2(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief ͼ�����л������棩
		* @param [in] node xmlд��ڵ�
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief ����ͼ����ʾ������
		* @param [in] show ����  ��ʾΪtrue������Ϊfalse
		* @param [in] isOnState true��ʾ�ڿɼ�����  false��ʾ���ڿɼ�����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Show(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief ����ͼ�㲻͸����
		* @param [in] opacity ��͸����(0-100)
		-	0��ʾȫ͸��
		-	100��ʾ��͸��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetOpacity(glbInt32 opacity);
		/**
		* @brief ��ȡͼ�㷶Χ
		* @param [in] isWorld �Ƿ����������꣬������������꣬����Ϊ��λ;��������������꣬��ʹ�õ������꣬�Զ�Ϊ��λ				
		* @return  ͼ�㷶Χ
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

		glbBool GetExtent(glbDouble &west, glbDouble &east, glbDouble &south, glbDouble &north);

		/* WMTSLayer �ӿ� */
		/* 
		* @brief ���ø߳�ģʽ
		* @param [in] mode �߳�ģʽ
		* @return �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode); 
		/**
		* @brief ��ȡ�߳�ģʽ					
		* @return  �߳�ģʽ
		*/
		GlbAltitudeModeEnum GetAltitudeMode();

		/* 
		* @brief ���þ���ģʽ�µĸ߳�Zֵ
		* @param [in] altitude �߳�Zֵ
		* @return �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetAltitudeOrZ(glbDouble altitude);
		/**
		* @brief ��ȡ�߳�Zֵ					
		* @return  �߳�Zֵ
		*/
		glbDouble GetAltitudeOrZ();
		/**
		* @brief ͼ�����		 
		*/
		void Update();

		/**
		* @brief ����osg�ڵ�ĸ��»ص�
		* @param [in] node osg�ڵ�
		* @param [in] nv osg�ڵ������
		* @return  �ɹ�����S_OK,S_FALSE
		-	 ʧ�ܷ���E_FAIL,E_POINTER
		*/
		HRESULT UpdateOsgCallBack(osg::Node* node,osg::NodeVisitor* nv);

		/**
		* @brief ͼ���ʼ��	   
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Initialize();

		glbBool IsInitialized(){return mpr_isInitialized;}
		/**
		* @brief ͼ������  
		* @return  ��
		*/
		void Dispose();
		/**
		* @brief ��ӵ����޸Ĳ���
		* @param [in] opera �����޸Ĳ���	  
		* @return ��
		*/
		void AddOperation(GlbTileOperation opera);
		/**
		* @brief ��ȡ��ǰ������ͼ 
		* @return  �ɹ�������ͼ����ָ��
		-	 ʧ�ܷ���NULL
		*/
		CGlbGlobeView* GetView();
		/**
		* @brief ��ȡGLOBEģʽ��0���ķֿ����[Ĭ��Ϊ36��]
		* @return  0���ֿ����
		*/
		glbInt32 GetLevelZeroTileSizeOfDegrees();
		/**
		* @brief ��ȡ0����Ӧkey��tile[GLOBEģʽ]
		* @return  0�����ο�
		*/
		CGlbGlobeWMTSTile* GetWMTSLevelZeroTile(glbInt32 key);
		/**
		* @brief ��ȡp_tile�����Ķ�����ı������ڿ�
		* @param [in] p_tile ���ο�	 
		* @return  WMTS��Ƭ����ָ��
		*/
		//CGlbGlobeWMTSTile* GetTopmostNorthNeibourTile(CGlbGlobeWMTSTile* p_tile);
		/**
		* @brief ��ȡp_tile�����Ķ�������ϱ����ڿ�
		* @param [in] p_tile ���ο�	 
		* @return  WMTS��Ƭ����ָ��
		*/
		//CGlbGlobeWMTSTile* GetTopmostSouthNeibourTile(CGlbGlobeWMTSTile* p_tile);
		/**
		* @brief ��ȡp_tile�����Ķ�������������ڿ�
		* @param [in] p_tile ���ο�	 
		* @return  WMTS��Ƭ����ָ��
		*/
		//CGlbGlobeWMTSTile* GetTopmostWestNeibourTile(CGlbGlobeWMTSTile* p_tile);
		/**
		* @brief ��ȡp_tile�����Ķ�����Ķ������ڿ�
		* @param [in] p_tile ���ο�	 
		* @return  WMTS��Ƭ����ָ��
		*/
		//CGlbGlobeWMTSTile* GetTopmostEastNeibourTile(CGlbGlobeWMTSTile* p_tile);

		/**
		* @brief ��ȡĳ����Ƭ������
		* @param [in] level ����	 
		* @return  �˼�WMTS��Ƭ����
		*/
		glbInt32 GetTileRowsOfLevel(glbInt32 level);
		/**
		* @brief ��ȡĳ����Ƭ������
		* @param [in] level ����	 
		* @return  �˼�WMTS��Ƭ����
		*/
		glbInt32 GetTileColumnsOfLevel(glbInt32 level);		

		/**
		* @brief ��ȡ��Ƭkeyֵ
		* @param [in]   level ����
				-		row �к�
				-		column �к�
		* @return  keyֵ
		*/
		glbInt32 ComputeTileKey(glbInt32 level, glbInt32 row, glbInt32 column);

		/**
		* @brief ֪ͨ���¼���Ƭ
		* @param [in] pTile ��Ƭ����	 
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/		
		glbBool NotifyTileAdd(CGlbGlobeWMTSTile* pTile);
		
		/**
		* @brief ֪ͨ����Ƭ���Ƴ�
		* @param [in] pTile ��Ƭ����	 
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/	
		glbBool NotifyTileRemove(CGlbGlobeWMTSTile* pTile);

		/**
		* @brief ����level��row��column����tile
		* @param [in] level ��Ƭ�ȼ�
					- row	��Ƭ�к�
					- column��Ƭ�к�
		* @return  WMTS��Ƭ����ָ��
		*/
		CGlbGlobeWMTSTile* FindTile(glbInt32 level, glbInt32 row, glbInt32 column);
		/**
		* @brief ��ȡWMTS�������Ƭ������С����		
		* @return  ��Ƭ��С����
		*/
		glbInt32	GetMinLevel();
		/**
		* @brief ��ȡWMTS�������Ƭ���������		
		* @return  ��Ƭ�����
		*/
		glbInt32	GetMaxLevel();
		/*
		* @brief ����ͼ��ƫ��
		* @param [in] xOffset xƫ��
		*		 [in] yOffset yƫ��
		*		 [in] zOffset zƫ��
		* @return ��
		*/
		void SetLayerOffset(glbDouble xOffset, glbDouble yOffset, glbDouble zOffset);
		/*
		* @brief ��ȡͼ��ƫ��
		* @param [out] xOffset xƫ��
		*		 [out] yOffset yƫ��
		*		 [out] zOffset zƫ��
		* @return ��
		*/
		void GetLayerOffset(glbDouble& xOffset, glbDouble& yOffset, glbDouble& zOffset);
	private:
		void UpdateGlobe();
		void UpdateFlat();
		void RemoveInvisibleTiles();
		glbBool GetSortedTopmostTiles(std::map<glbInt32,glbref_ptr<CGlbGlobeWMTSTile>> tiles, osg::Vec3d camerapos, std::map<glbDouble,glbref_ptr<CGlbGlobeWMTSTile>>& sorted_tiles);
		void UpdateRelativeElevationPos();
	private:
		glbref_ptr<CGlbExtent>	mpr_extent;				///<ͼ�����
		GlbAltitudeModeEnum     mpr_altitudeMode;		///<�߳�ģʽ
		glbDouble				mpr_altitudeOrZ;		///<����ģʽʱ�ĸ߳�
		glbBool					mpr_isInitialized;		///<�Ƿ��ʼ��
		glbDouble				mpr_south;				///<�ϱ߽�
		glbDouble				mpr_north;				///<���߽�
		glbDouble				mpr_west;				///<���߽�
		glbDouble				mpr_east;				///<���߽�
		glbDouble				mpr_lzts;				///<�����ĳߴ�
		CRITICAL_SECTION		mpr_osgcritical;		///<osg�ڵ�ص�������޸�����ȡ�߳� ����
		std::map<glbInt32,glbref_ptr<CGlbGlobeWMTSTile>> mpr_topmost_tiles;	///<0�����ο鼯��mpr_lztiles , 0��������	
		CRITICAL_SECTION mpr_tilebuffer_exchange_criticalsection;			///<���ݽ����ٽ���
		std::vector<GlbTileOperation> mpr_operations;						///<��������
		std::vector<GlbTileOperation> mpr_operations_buffer;				///<�������ϻ���		
		glbBool					mpr_enableUpdate;							///<ͼ�����
		glbInt32				mpr_levelzero_tiles_columns;				///<0���ֿ�����
		glbInt32				mpr_levelzero_tiles_rows;					///<0���ֿ�����

		glbInt32				mpr_minLevel;					///<��С�ȼ�
		glbInt32				mpr_maxLevel;					///<���ȼ�
		
		///<������Ƭ���ϣ�ֻ�Ӳ�ɾ����ƬDispose��������Ƭ���⣩��itr->second->second=NULL,key��ֵΪ��Ƭrow*�˼���Ƭ����+��Ƭcolumn
		std::map<glbInt32/*level*/,std::map<glbInt32/*key=row*cols+col*/, glbref_ptr<CGlbGlobeWMTSTile>>> mpr_all_tiles;

		glbBool					mpr_isDispShow;

		glbDouble				mpr_xOffset;
		glbDouble				mpr_yOffset;
		glbDouble				mpr_zOffset;
	public:
		GlbGlobeTypeEnum		mpr_globe_type;								///<��������
	};
}

