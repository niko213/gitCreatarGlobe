/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbOpacityVisitor.h
* @brief   不透明度修改类头文件，文档定义CGlbOpacityVisitor类
* @version 1.0
* @author  马林
* @date    2015-2-3 15:00
*********************************************************************/
#pragma once

#include <osg/Geode>
#include <osg/Geometry>
#include <limits>  

namespace GlbGlobe
{
	class CGlbOpacityVisitor  : public osg::NodeVisitor
	{
	public:
		CGlbOpacityVisitor(glbInt32 opacity);
		~CGlbOpacityVisitor(void);

		virtual void apply(osg::Geode &node)
		{
			changeMaterialAlpha(node.getStateSet(),mpr_opacity);

			osg::Geode::DrawableList dlist = (osg::Geode::DrawableList)node.getDrawableList();		

			osg::Geode::DrawableList::iterator itr = dlist.begin();
			while( itr !=  dlist.end())
			{
				osg::Geometry* geom = (*itr)->asGeometry();
				if (geom)
				{
					changeMaterialAlpha(geom->getStateSet(),mpr_opacity);

					osg::Array* clrArray = geom->getColorArray();
					if (clrArray)
					{
						switch(clrArray->getType())
						{
						case osg::Array::Vec4ArrayType:
							{							
								float opaciy = mpr_opacity/100.0;
								osg::Vec4Array* vecArray = dynamic_cast<osg::Vec4Array*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4 vc = vecArray->at(idx);
										vecArray->at(idx).set(vc.r(),vc.g(),vc.b(),opaciy);
									}
								}
								geom->setColorArray(vecArray);
							}
							break;
						case osg::Array::Vec4bArrayType:
							{							
								signed char opaciy = mpr_opacity*(numeric_limits<signed char>::max)()/100;
								osg::Vec4bArray* vecArray = dynamic_cast<osg::Vec4bArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4b vc = vecArray->at(idx);
										vecArray->at(idx).set(vc.r(),vc.g(),vc.b(),opaciy);
									}
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4sArrayType:
							{							
								short opaciy = mpr_opacity*(numeric_limits<short>::max)()/100;
								osg::Vec4sArray* vecArray = dynamic_cast<osg::Vec4sArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4s vc = vecArray->at(idx);
										vecArray->at(idx).set(vc.r(),vc.g(),vc.b(),opaciy);
									}
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4iArrayType:
							{							
								int opaciy = mpr_opacity*(numeric_limits<int>::max)()/100;
								osg::Vec4iArray* vecArray = dynamic_cast<osg::Vec4iArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4i vc = vecArray->at(idx);
										vecArray->at(idx).set(vc.r(),vc.g(),vc.b(),opaciy);
									}
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4ubArrayType:
							{
								unsigned char opaciy = mpr_opacity*(numeric_limits<unsigned char>::max)()/100;
								osg::Vec4ubArray* vecArray = dynamic_cast<osg::Vec4ubArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4ub vc = vecArray->at(idx);
										vecArray->at(idx).set(vc.r(),vc.g(),vc.b(),opaciy);
									}
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4usArrayType:
							{							
								unsigned short opaciy = mpr_opacity*(numeric_limits<unsigned short>::max)()/100;
								osg::Vec4usArray* vecArray = dynamic_cast<osg::Vec4usArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4us vc = vecArray->at(idx);
										vecArray->at(idx).set(vc.r(),vc.g(),vc.b(),opaciy);
									}
									geom->setColorArray(vecArray);
								}
							}
							break;
						case osg::Array::Vec4uiArrayType:
							{							
								unsigned int opaciy = mpr_opacity*(numeric_limits<unsigned int>::max)()/100;
								osg::Vec4uiArray* vecArray = dynamic_cast<osg::Vec4uiArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4ui vc = vecArray->at(idx);
										vecArray->at(idx).set(vc.r(),vc.g(),vc.b(),opaciy);
									}
									geom->setColorArray(vecArray);
								}
							}
							break;						
						case osg::Array::Vec4dArrayType:
							{							
								double opaciy = mpr_opacity*(numeric_limits<double>::max)()/100.0;
								osg::Vec4dArray* vecArray = dynamic_cast<osg::Vec4dArray*>(clrArray);
								if (vecArray)
								{
									for (size_t idx = 0; idx < vecArray->size() ; idx++)
									{
										osg::Vec4d vc = vecArray->at(idx);
										vecArray->at(idx).set(vc.r(),vc.g(),vc.b(),opaciy);
									}
									geom->setColorArray(vecArray);
								}
							}
							break;
						default:
							;
						}

						osg::StateSet* stateset = geom->getOrCreateStateSet();
						// 透明效果
						if (mpr_opacity<100)
						{// 半透明
							stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
							if (osg::StateSet::TRANSPARENT_BIN != stateset->getRenderingHint())
								stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
						}
						else
						{// 不透明
							stateset->setMode(GL_BLEND, osg::StateAttribute::OFF);
							if (osg::StateSet::OPAQUE_BIN != stateset->getRenderingHint())
								stateset->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
						}
						//stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
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
			changeMaterialAlpha(node.getStateSet(),mpr_opacity);
			traverse(node);
		}
	private:
		void changeMaterialAlpha(osg::StateSet* ss,glbInt32 opacity);
	private: 
		glbInt32 mpr_opacity; ///< 不透明度 [0-100] 0表示全透明 100表示全不透明
	};
}

