/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbColorVisitor.h
* @brief   颜色修改类头文件，文档定义CGlbColorVisitor类
* @version 1.0
* @author  马林
* @date    2015-6-30 15:00
*********************************************************************/
#pragma once

#include <osg/Geode>
#include <osg/Geometry>
#include <limits>  

namespace GlbGlobe
{
	class CGlbColorVisitor :	public osg::NodeVisitor
	{
	public:
		CGlbColorVisitor(glbInt32 r, glbInt32 g, glbInt32 b);
		~CGlbColorVisitor(void);

		virtual void apply(osg::Geode &node)
		{
			glbInt32 r = mpr_red;
			glbInt32 g = mpr_green;
			glbInt32 b = mpr_blue;
			changeMaterialColor(node.getStateSet(),mpr_red,mpr_green,mpr_blue);

			osg::Geode::DrawableList dlist = (osg::Geode::DrawableList)node.getDrawableList();		

			osg::Geode::DrawableList::iterator itr = dlist.begin();
			while( itr !=  dlist.end())
			{
				osg::Geometry* geom = (*itr)->asGeometry();
				if (geom)
				{
					changeMaterialColor(geom->getStateSet(),mpr_red,mpr_green,mpr_blue);

					osg::Array* clrArray = geom->getColorArray();
					if (clrArray)
					{
						geom->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
						switch(clrArray->getType())
						{
						case osg::Array::Vec4ArrayType:
							{								
								float _r = r / 255.0f;
								float _g = g / 255.0f;
								float _b = b / 255.0f;
								
								osg::Vec4Array* vecArray = dynamic_cast<osg::Vec4Array*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4 vc = vecArray->at(idx);
										vecArray->at(idx).set(_r,_g,_b,vc.a());
									}
									geom->setColorArray(vecArray);
								}								
							}
							break;
						case osg::Array::Vec4bArrayType:
							{							
								signed char _r = r*(numeric_limits<signed char>::max)()/255;
								signed char _g = g*(numeric_limits<signed char>::max)()/255;
								signed char _b = b*(numeric_limits<signed char>::max)()/255;
								osg::Vec4bArray* vecArray = dynamic_cast<osg::Vec4bArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4b vc = vecArray->at(idx);
										vecArray->at(idx).set(_r,_g,_b,vc.a());
									}				
									geom->setColorArray(vecArray);
								}							
							}
							break;
						case osg::Array::Vec4sArrayType:
							{							
								short _r = r*(numeric_limits<short>::max)()/255;
								short _g = g*(numeric_limits<short>::max)()/255;
								short _b = b*(numeric_limits<short>::max)()/255;
								osg::Vec4sArray* vecArray = dynamic_cast<osg::Vec4sArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4s vc = vecArray->at(idx);
										vecArray->at(idx).set(_r,_g,_b,vc.a());
									}			
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4iArrayType:
							{							
								int _r = r*(numeric_limits<int>::max)()/255;
								int _g = g*(numeric_limits<int>::max)()/255;
								int _b = b*(numeric_limits<int>::max)()/255;
								osg::Vec4iArray* vecArray = dynamic_cast<osg::Vec4iArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4i vc = vecArray->at(idx);
										vecArray->at(idx).set(_r,_g,_b,vc.a());
									}				
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4ubArrayType:
							{
								unsigned char _r = r*(numeric_limits<unsigned char>::max)()/255;
								unsigned char _g = g*(numeric_limits<unsigned char>::max)()/255;
								unsigned char _b = b*(numeric_limits<unsigned char>::max)()/255;
								osg::Vec4ubArray* vecArray = dynamic_cast<osg::Vec4ubArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4ub vc = vecArray->at(idx);
										vecArray->at(idx).set(_r,_g,_b,vc.a());
									}		
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4usArrayType:
							{							
								unsigned short _r = r*(numeric_limits<unsigned short>::max)()/255;
								unsigned short _g = g*(numeric_limits<unsigned short>::max)()/255;
								unsigned short _b = b*(numeric_limits<unsigned short>::max)()/255;
								osg::Vec4usArray* vecArray = dynamic_cast<osg::Vec4usArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4us vc = vecArray->at(idx);
										vecArray->at(idx).set(_r,_g,_b,vc.a());
									}
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4uiArrayType:
							{							
								unsigned int _r = r*(numeric_limits<unsigned int>::max)()/255;
								unsigned int _g = g*(numeric_limits<unsigned int>::max)()/255;
								unsigned int _b = b*(numeric_limits<unsigned int>::max)()/255;
								osg::Vec4uiArray* vecArray = dynamic_cast<osg::Vec4uiArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4ui vc = vecArray->at(idx);
										vecArray->at(idx).set(_r,_g,_b,vc.a());
									}	
									geom->setColorArray(vecArray);
								}
							}
							break;						
						case osg::Array::Vec4dArrayType:
							{							
								double _r = r*(numeric_limits<double>::max)()/255.0;
								double _g = g*(numeric_limits<double>::max)()/255.0;
								double _b = b*(numeric_limits<double>::max)()/255.0;
								osg::Vec4dArray* vecArray = dynamic_cast<osg::Vec4dArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4d vc = vecArray->at(idx);
										vecArray->at(idx).set(_r,_g,_b,vc.a());
									}	
									geom->setColorArray(vecArray);
								}
							}
							break;
						default:
							;
						}						
					}		
					else
					{
						CGlbWString errorStr(L"osg::Geometry have no colorarray!");
					}
				}
				itr++;
			}

			traverse(node);
		}

		virtual void apply(osg::Group &node)
		{
			changeMaterialColor(node.getStateSet(),mpr_red,mpr_green,mpr_blue);
			traverse(node);
		}
	private:
		void changeMaterialColor(osg::StateSet* ss,glbInt32 red,glbInt32 green,glbInt32 blue);
	private: 
		glbInt32 mpr_red;
		glbInt32 mpr_green;
		glbInt32 mpr_blue;
	};
}
