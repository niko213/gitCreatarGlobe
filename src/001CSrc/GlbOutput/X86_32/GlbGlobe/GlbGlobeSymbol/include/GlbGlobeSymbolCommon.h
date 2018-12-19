/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobePolygon.h
* @brief   多边形对象头文件，文档定义CGlbGlobePolygon类
* @version 1.0
* @author  龙庭友
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeSymbolExport.h"
#include "GlbExtent.h"

#include <GlbWString.h>
#include <GlbPath.h>
#include <GlbString.h>

#include "osg/Texture2D"
#include "osg/CoordinateSystemNode"
#include "osg/Array"

extern "C" GLB_SYMBOLDLL_CLASSEXPORT bool isFloatEqual(glbFloat leftValue,glbFloat rightValue);

extern "C" GLB_SYMBOLDLL_CLASSEXPORT bool isDoubleEqual(glbDouble leftValue,glbDouble rightValue);

extern GLB_SYMBOLDLL_CLASSEXPORT osg::Image *loadImage(const CGlbWString &name);

extern GLB_SYMBOLDLL_CLASSEXPORT osg::Texture2D *loadTexture(osg::Image *pImg, osg::Texture::WrapMode wrap);

extern GLB_SYMBOLDLL_CLASSEXPORT osg::Texture2D* readTexture(const CGlbWString &name, osg::Texture::WrapMode wrap);

extern osg::Shader* readShaderFile(const std::string& filename);

extern osg::Program* createShaderProgram( const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc, bool loadFromFiles );

extern std::string getDepthVertexShaderSource();

extern std::string getDepthFragmentShaderSource();

//** 反问位置不同的点的数目*/
extern glbUInt32 DelaunayTriangulator_uniqueifyPoints(osg::Vec3Array *points);

extern glbUInt32 DelaunayTriangulator_uniqueifyPoints(osg::Vec3dArray *points);

extern GLB_SYMBOLDLL_CLASSEXPORT osg::Geode *CreateBoundingBox(CGlbExtent extent,glbBool isWorld = false);