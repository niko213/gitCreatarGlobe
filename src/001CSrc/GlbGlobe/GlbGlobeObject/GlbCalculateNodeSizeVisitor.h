/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbCalculateNodeSizeVisitor.h
* @brief   点对象头文件，文档定义CGlbCalculateNodeSizeVisitor类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-22 15:00
*********************************************************************/
#pragma once
#include <osg/NodeVisitor>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/Switch>
#include <osgTerrain/Terrain>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include "GlbCommTypes.h"

namespace GlbGlobe
{
	/**
	* @brief  计算node节点及其下所有节点所占内存大小类
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-22 15:00
	* !!!!!!!! 对象 大小统计准确 非常关键！！！！！！！
	* 绝对不能 少统计！！！
	* 但也不能多统计太多！！！
	*/
	class CGlbCalculateNodeSizeVisitor : public osg::NodeVisitor
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbCalculateNodeSizeVisitor();
		/**
		* @brief 析构函数
		*/
		~CGlbCalculateNodeSizeVisitor();
		/**
		* @brief 访问节点操作
		*/
		virtual void apply(osg::Node& node)
		{
			if (node.getStateSet())
				apply(*(node.getStateSet()));

			if (node.asSwitch())
			{
				mpr_nodeMemSize+=sizeof(osg::Switch);
			}
			else if (node.asTerrain())
			{
				mpr_nodeMemSize+=sizeof(osgTerrain::Terrain);
			}
			else if (node.asTransform())
			{
				osg::MatrixTransform *mt=dynamic_cast<osg::MatrixTransform *>(&node);
				if (mt!=NULL)
					mpr_nodeMemSize+=sizeof(osg::MatrixTransform);
				else
					mpr_nodeMemSize+=sizeof(osg::Transform);
			}
			else if (node.asCamera())
			{
				mpr_nodeMemSize+=sizeof(osg::Camera);
			}
			else if (node.asGroup())
			{
				mpr_nodeMemSize+=sizeof(osg::Group);
			}
			else if (node.asGeode())
			{
				mpr_nodeMemSize+=sizeof(osg::Geode);
			}			
			else
			{
				osg::Node* _node = dynamic_cast<osg::Node*> (&node);
				if (_node)
					mpr_nodeMemSize+=sizeof(osg::Node);
				else
					printf("error! node type is unknown!\n");
			}

			traverse(node);
		}
		/**
		* @brief 访问节点操作
		*/
		virtual void apply(osg::Geode& node)
		{
			if (node.getStateSet())
				apply(*(node.getStateSet()));
			for(unsigned int i=0;i<node.getNumDrawables();++i)
			{
				osg::Drawable* drawable = node.getDrawable(i);
				if (drawable==NULL) continue;
				if (drawable && drawable->getStateSet()) 
					apply(*(drawable->getStateSet()));
				if (drawable->asGeometry())
				{
					osg::Geometry::ArrayList list;
					unsigned int geomSize = drawable->asGeometry()->getGLObjectSizeHint();
					mpr_nodeMemSize+=geomSize;
					//drawable->asGeometry()->getArrayList(list);
					//for (int il = 0;il<(long)list.size();il++)
					//{
					//	osg::Array* array = list[il];						
					//	//mpr_nodeMemSize+=array->getDataSize()*array->getNumElements();// 错误，忘记了类型本身所占的字节数
					//	mpr_nodeMemSize+=array->getTotalDataSize();
					//}
				}
				else
				{// 非osg::geometry的drawable 必定是 osg::DrawPixels类或 osg::ShapeDrawable类

				}
			};
			mpr_nodeMemSize+=sizeof(osg::Geode);
			traverse(node);
		}
		/**
		* @brief 访问节点操作
		*/
		virtual void apply(osg::Billboard& node)
		{
			if (node.getStateSet())
				apply(*(node.getStateSet()));
			for(unsigned int i=0;i<node.getNumDrawables();++i)
			{
				osg::Drawable* drawable = node.getDrawable(i);
				if (drawable && drawable->getStateSet()) 
					apply(*(drawable->getStateSet()));
				if (drawable->asGeometry())
				{
					osg::Geometry::ArrayList list;
					drawable->asGeometry()->getArrayList(list);
					for (int il = 0;il<(long)list.size();il++)
					{
						osg::Array* array = list[il];
						//mpr_nodeMemSize+=array->getDataSize()*array->getNumElements();
						mpr_nodeMemSize+=array->getTotalDataSize();
					}					
				}
			}
			mpr_nodeMemSize+=sizeof(osg::Billboard);
			traverse(node);
		}
		/**
		* @brief 访问节点操作
		*/
		virtual void apply(osg::StateSet& stateset)
		{
			// search for the existence of any texture object attributes
			for(unsigned int i=0;i<stateset.getTextureAttributeList().size();++i)
			{
				osg::Texture* texture = dynamic_cast<osg::Texture*>(stateset.getTextureAttribute(i,osg::StateAttribute::TEXTURE));
				if (texture)
				{
					mpr_textureSet.insert(texture);
				}
			}
		}
		/**
		* @brief 获取节点纹理内存占用量
		*/
		glbInt32 getTextureSize()
		{
			/*
			* 如果几个drawable都共用一个纹理的话，统计的纹理占用内存量会比实际占用量大
			*/
			glbInt32 size = 0;
			for(TextureSet::iterator itr = mpr_textureSet.begin();
				itr != mpr_textureSet.end();
				++itr)
			{
				osg::Texture* texture = const_cast<osg::Texture*>(itr->get());

				osg::Texture2D* texture2D = dynamic_cast<osg::Texture2D*>(texture);
				osg::Texture3D* texture3D = dynamic_cast<osg::Texture3D*>(texture);

				size+=texture2D ? sizeof(osg::Texture2D) : sizeof(osg::Texture3D);

				osg::ref_ptr<osg::Image> image = texture2D ? texture2D->getImage() : (texture3D ? texture3D->getImage() : 0);
				if (image.valid())
				{						
					GLenum format = image->getPixelFormat();
					int _compress = 1;
					switch(format)
					{
					case(GL_COMPRESSED_RGB_S3TC_DXT1_EXT):
						_compress = 4;
						break;
					case(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT):
						_compress = 4;
						break;
					case(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT):
						_compress = 8;
						break;
					case(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT): 
						_compress = 8;
						break;
					default: break;
					}
					//size+=image->getTotalSizeInBytesIncludingMipmaps();
					size+=image->getImageSizeInBytes()+image->getTotalSizeInBytesIncludingMipmaps();
					size+=sizeof(osg::Image);
				}
			}
			return size;
		}
		/**
		* @brief 获取节点内存占用量
		*/
		glbInt32 getNodeMemSize()
		{
			return mpr_nodeMemSize;
		}

	private:
		typedef std::set<osg::ref_ptr<osg::Texture>> TextureSet;
		TextureSet mpr_textureSet;
		glbInt32 mpr_nodeMemSize;
	};
}