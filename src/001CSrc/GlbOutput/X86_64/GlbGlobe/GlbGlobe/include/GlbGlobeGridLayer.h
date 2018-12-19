/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeGridLayer.h
* @brief   ����ͼ��ͷ�ļ�
*
* �����������CGlbGlobeGridLayer���class
*
* @version 1.0
* @author  ����
* @date    2018-1-9 13:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "glbglobelayer.h"
#include "GlbGlobeRObject.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbGlobeGridTile.h"

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	class GLB_DLLCLASS_EXPORT CGlbGlobeGridLayer :	public CGlbGlobeLayer
	{
	public:
		enum GLBOperationEnum
		{// �����������
			GLB_OPENERAION_ADD = 0,
			GLB_OPENERAION_REMOVE = 1
			//GLB_OPENERAION_MODIFY = 2
		};

		enum GLBOperationObjectEnum
		{// �����������
			GLB_NODE = 0,
			GLB_DRAWABLE = 2
			//GLB_VECARRAY = 3,
			//GLB_TEXTURE = 4
		};
		/** 
		* @brief �������޸���Ϣ�ṹ��
		* @note �����ڲ��ṹ,��������ʹ��
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
			GlbTileOperation(	GLBOperationEnum addordel, 
				GLBOperationObjectEnum operation, 
				osg::Group* p_tile_node,
				double compiletime=0.0);

			GlbTileOperation(	GLBOperationEnum addordel, 
				GLBOperationObjectEnum operation, 
				osg::Geode* p_tile_geode_node,
				osg::Drawable* p_drawable=NULL);
			virtual ~GlbTileOperation();
		public:
			GLBOperationEnum _addordel;				///< 0 - add; 1 - delete	; 2 - modify
			GLBOperationObjectEnum _nodeordrawable;		///< 0 - node ; 1 - drawable	; 2 - vec3array ; 3 - texture
			osg::ref_ptr<osg::Group> _p_tile_node;			// �ڵ�
			osg::ref_ptr<osg::Geode> _p_tile_geode_node;	///< Ҷ�ӽڵ�
			osg::ref_ptr<osg::Drawable> _p_drawable; ///< �ɻ��ƶ���
			//osg::ref_ptr<osg::Vec3Array> _p_vecarray; ///< ��������
			double _node_compiletime;						///< ����_p_tile_node��Ҫ��ʱ��
			//osg::ref_ptr<osg::Texture>	_p_texture;
			//osg::ref_ptr<osg::Image>	_p_image;
		};
	private:
		/** 
		* @brief ����osg�ڵ���»ص���
		* @author ����
		* @date    2018-7-30 11:10
		* @note �����ڲ���,����ժ�ҽڵ�,drawable
		*/
		class CGlbGlobeGridCallBack : public osg::NodeCallback
		{
		public:
			/**
			* @brief ����osg�ڵ�ص��๹�캯��
			* @param [in] pGlobeTerrain ���ζ���	  
			* @return  ��
			*/
			CGlbGlobeGridCallBack(CGlbGlobeGridLayer* pGridLayer); 	
			/**
			* @brief ����osg�ڵ�ص�ʵ��
			* @param [in] node ���ε�osg�ڵ�
			* @param [in] nv osg�ڵ������	  
			* @return  ��
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		public:
			CGlbGlobeGridLayer* m_p_globe_gridlayer;
		};
	public:
		/**
		* @brief gridͼ�㹹�캯��
		* @return ��
		*/
		CGlbGlobeGridLayer(void);
		/**
		* @brief gridͼ����������
		* @return ��
		*/
		~CGlbGlobeGridLayer(void);
		/*����ӿ�*/
		/**
		* @brief ��ȡͼ������
		* @return ͼ������ 
		*/
		const glbWChar* GetName();
		/**
		* @brief ����ͼ������
		* @param [in] name ͼ������
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
		glbBool   Load( xmlNodePtr node, glbWChar* relativepath );
		glbBool   Load2( xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief ͼ�����л������棩
		* @param [in]node xmlд��ڵ�
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
		glbBool Show(glbBool show,glbBool isOnState=false);
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

		/*��ӿ�*/
		/**
		* @brief ������Ⱦ������Ϣ
		* @param [in] rinfo ��Ⱦ������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetRenderInfo(GlbRenderInfo* rinfo);
		/**
		* @brief ��ȡ��Ⱦ������Ϣ
		* @return ��Ⱦ������Ϣ
		*/
		GlbRenderInfo* GetRenderInfo();
		// ��Ƭ�ӿ�
		/**
		* @brief �Ը�����������Ƭ
		* @param [in] geo ����		
		*/
		void ClipMap(IGlbGeometry* geo);
		/**
		* @brief �Ը�������I��J����X,Y������Ƭ
		* @param [in] I,J ��Ƭ����λ��		
		*/
		void ClipMap(glbInt32 I,glbInt32 J);
		/**
		* @brief �Ը�������դ����Ƭ
		* @param [in] INum,JNum --I,J ������Ƭ����		
		*/
		void ClipBarrierMap(glbInt32 INum, glbInt32 JNum);
		/**
		* @brief �ر���Ƭ
		*/
		void Restore();

		// ͼ�����
		void Update();
		/**
		* @brief ��ȡ������Χ
		* @param [out] west ���߽�	  
		* @param [out] east ���߽�
		* @param [out] south �ϱ߽�
		* @param [out] north ���߽�
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool GetExtent(glbDouble &west, glbDouble &east, glbDouble &south , glbDouble &north);
		/**
		* @brief ����  
		* @return  ��
		*/
		void Dispose();
		/**
		* @brief ��ȡ��ǰ������ͼ 
		* @return  �ɹ�������ͼ����ָ��
		-	 ʧ�ܷ���NULL
		*/
		CGlbGlobeView* GetView();
		/**
		* @brief ��ӵ����޸Ĳ���
		* @param [in] opera �����޸Ĳ���	  
		* @return ��
		*/
		void AddOperation(GlbTileOperation opera);
		/**
		* @brief ���������Ƿ�������ཻ      
		*	@param [in] ln_pt1������һ��(�˵�)
		* @param [in] ln_dir���߷���
		* @param [out] InterPos ����ཻ����������	
		* @return �ཻ����true
		*		-	���ཻ����false
		*/
		glbBool IsRayInterGrid(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos);
		/**
		* @brief ����osg�ڵ�ĸ��»ص�
		* @param [in] node osg�ڵ�
		* @param [in] nv osg�ڵ������
		* @return  �ɹ�����S_OK,S_FALSE
		-	 ʧ�ܷ���E_FAIL,E_POINTER
		*/
		HRESULT UpdateOsgCallBack(osg::Node* node,osg::NodeVisitor* nv);

		/**
		* @brief ��ȡtile����level
		* @return  ����level
		*/
		glbInt32 GetMaxLevel();

		/**
		* @brief ��Ԫ��ʰȡ
		* @param [in] mousex	����x����
		*		 [in] mousey	����y����
		* @param [out] tilelevel, tileRow, tileColumn, tileLayer �߿�ĵȼ������в��
		* @param [out] cell_I, cell_J, cell_K ��Ԫ���I,J,Kֵ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		//glbBool Pick(glbInt32 mousex,glbInt32 mousey,
		//			glbInt32 &tilelevel, glbInt32 &tileRow, glbInt32 &tileColumn, glbInt32 &tileLayer,
		//			glbInt32& cell_I, glbInt32& cell_J, glbInt32& cell_K);

		glbBool Pick(glbInt32 mousex,glbInt32 mousey,glbDouble &interX,glbDouble &interY,glbDouble &interZ);

		// �Ƿ���Ҫ��ͣ
		glbBool IsUpdateNeedTerminate();
		
		// ��ȡĳ��Tile��������
		glbBool GetRowColumnCount(glbInt32 level, glbInt32 &startRow,glbInt32 &startColumn,glbInt32 &rowNum, glbInt32 columnNum);
	private:
		// ��ʼ��
		glbBool init();	
		// ƽ��ģʽ�µ�gridͼ�����
		void UpdateFlatGrid();
		// ����ģʽ�µ�gridͼ�����
		void UpdateGlobeGrid();
		// �Ƴ����в��ɼ��߿�
		void RemoveInvisibleTiles();
		// �ж����������grid����������Ľ��㣬�����������ͽ������ڵ���enum���ϱ���,�±��棬ǰ���棬����棬����棬�ұ��棩
		//glbBool IsRayInterGrid(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos, BYTE& faceType);

		// ������׶�봹��(���)���ཻ���� --> �ӿ�Ҫ�Ƶ�globeView��
		//glbBool computeViewFrustumInterVerticalLine(glbDouble x,glbDouble y, glbDouble minz, glbDouble maxz,glbDouble& outminz, glbDouble& outmaxz);
		
		// ������׶�봹(��)����ཻ���� [���ཻ������Ҫ�淶Ϊ�����x,y����������+minz+maxz�ĸ�ʽ���] --> �ӿ�Ҫ�Ƶ�globeView��
		//glbBool computeViewFrustumInterVerticalPlane(			
		//				glbDouble x0,glbDouble y0,glbDouble x1, glbDouble y1, glbDouble minz, glbDouble maxz,
		//				glbDouble& outx0,glbDouble& outy0,glbDouble& outx1, glbDouble& outy1, glbDouble& outminz, glbDouble& outmaxz);

		
	private:
		glbBool mpr_isInitialized;									///<�Ƿ��ʼ��
		glbInt32 mpr_minLevel;										///<��С�ȼ�
		glbInt32 mpr_maxLevel;										///<���ȼ�
		glbDouble mpr_south;										///<�ϱ߽�
		glbDouble mpr_north;										///<���߽�
		glbDouble mpr_west;											///<���߽�
		glbDouble mpr_east;											///<���߽�
		glbref_ptr<CGlbGlobeRObject> mpr_gridObject;				///<��Ӧ��grid����
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;					///<��Ⱦ������Ϣ

		glbInt32 mpr_minlevel_tiles_columns;								///<��С���ֿ�����
		glbInt32 mpr_minlevel_tiles_rows;									///<��С���ֿ�����
		glbInt32 mpr_minlevel_tiles_layers;									///<��С���ֿ����
		osg::ref_ptr<osg::Switch> mpr_node;									///<����ͼ��osg�ڵ�
		std::map<glbInt32,glbref_ptr<CGlbGlobeGridTile>> mpr_topmost_tiles;	///<0�������鼯��mpr_lztiles , 0��������	

		CRITICAL_SECTION mpr_osgcritical;									///<osg�ڵ�ص�������޸�����ȡ�߳� ����
		CRITICAL_SECTION mpr_tilebuffer_exchange_criticalsection;			///<���ݽ����ٽ���
		std::vector<GlbTileOperation> mpr_operations;						///<��������
		std::vector<GlbTileOperation> mpr_operations_buffer;				///<�������ϻ���	

		glbInt32 mpr_addOperationCount;										///<��Ҫ�������ӽڵ��������

		glbBool	mpr_isNeedTerminate;										///<��ͣ



	};
}

