#include "StdAfx.h"
#include "GlbGlobeDynamicObject.h"
#include "CGlbGlobe.h"
#include "GlbGlobePoint.h"
#include <fstream>
#include "GlbString.h"

#include "GlbGlobeFeatureLayer.h"

#include "GlbCalculateBoundBoxVisitor.h"
#include "GlbCalculateNodeSizeVisitor.h"

#include "GlbGlobeMarkerModelSymbol.h"
#include "GlbGlobeMarkerImageSymbol.h"
#include "GlbGlobeMarkerLabelSymbol.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbConvert.h"

// MCI库及头文件
#include <MMsystem.h>
#pragma comment(lib, "winmm.lib")
//#include <vfw.h>
//#pragma comment(lib,"vfw32.lib")

//#ifdef _DEBUG	
//#include <iostream>
//#endif

bool	_isMyWindowClosed = false;

using namespace GlbGlobe;

void CGlbGlobeDynamicObject::CGlbGlobeDynamicObjCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{ 
	if (nv->getVisitorType()==osg::NodeVisitor::UPDATE_VISITOR && nv->getFrameStamp())
	{	
		_latestTime = nv->getFrameStamp()->getSimulationTime();	
		glbDouble tim = getAnimationTime();	
		if (!_pause)
		{ 		
			if (_firstTime==DBL_MAX)
			{
				_firstTime = _latestTime;
				_lastFrametime = DBL_MAX;
			}
			//获取controlPoint时间点			
			glbDouble periodtim = _tcpmap.rbegin()->first;	
			if (_isLoop)	//loop
			{
				{// 判断是否需要重新初始化用户事件标志
					static int _preLoopNum = 0;
					int _curLoopNum = (tim <= 0) ? 0 : tim / periodtim;					
					if (_preLoopNum != _curLoopNum)
					{
						_dynamicObject->initMediaMap();
						_preLoopNum = _curLoopNum;
					}
				}

				double modulated_time = tim/periodtim;
				double fraction_part = modulated_time - floor(modulated_time);
				tim = fraction_part * periodtim;
			}else{			//noloop
				if (tim > periodtim)
				{
					tim = periodtim;
					setPause(false);
				}
			}
			//获取controlPoint
			osg::AnimationPath::ControlPoint cp;
			osg::AnimationPath::TimeControlPointMap::const_iterator second = _tcpmap.lower_bound(tim);
			if (second == _tcpmap.begin())
			{
				cp = second->second;
			}
			else if (second != _tcpmap.end())
			{
				osg::AnimationPath::TimeControlPointMap::const_iterator first = second;
				--first;   
				// we have both a lower bound and the next item.
				// delta_time = second.time - first.time
				glbDouble delta_time = second->first - first->first;
				if (delta_time == 0.0)
					cp = first->second;
				else
					(cp).interpolate((tim - first->first)/delta_time,first->second,second->second);      
			}
			else //(second == _timeControlPointMap.end())
			{
				cp = _tcpmap.rbegin()->second;
			}
			//将controlPoint转化为Position
			glbDouble xorlon,yorlat,zoralt,yaw,pitch,roll;
			_dynamicObject->CGlbGlobeDynamicObject::ControlPointToPosition(cp,&xorlon,&yorlat,&zoralt,&yaw,&pitch,&roll);

			//根据动态对象的不同运动模式，ControlPoint的姿态会有不同，下面修正
			switch(_dynamicObject->GetPlayMode())
			{
			case GLB_DYNAMICPLAYMODE_CAR:
			case GLB_DYNAMICPLAYMODE_PERSON:	//人或车辆play模式,无修正
			 	break;
			case GLB_DYNAMICPLAYMODE_AIRPLANE:	//飞机play模式，转弯飞机会有一个倾斜角，即roll会改变
			 	{ 
			 		//近似认为倾斜角变化率，与转弯角度变化率成正比，与转弯速度成正比	(deltroll = f(deltyaw * speed))
					if (_lastFrametime != DBL_MAX)
					{
						glbDouble coefficient = _speed / 1000.0;
						coefficient = coefficient > 2.0 ? 2.0 : coefficient;
					   	roll += (yaw - _lastFrameyaw)/(_latestTime - _lastFrametime) * coefficient; 
					}				
			 	}
			 	break;
			case GLB_DYNAMICPLAYMODE_HELICOPTER://直升飞机play模式，转弯飞机会有一个倾斜角，即roll会改变，并且高度不变飞行时机体会向下倾斜，即pitch会改变
			 	{ 
			 		//近似认为倾斜角，与转弯角度成正比，与转弯速度成正比	(deltroll = f(deltyaw * speed)) (pitch = g(speed))
					if (_lastFrametime != DBL_MAX)
					{
						glbDouble coefficient = _speed / 1000.0;
						coefficient = coefficient > 2.0 ? 2.0 : coefficient;
						roll += (yaw - _lastFrameyaw)/(_latestTime - _lastFrametime) * coefficient;
						pitch -= coefficient * 10.0;
						if (pitch < -90.0)
							pitch = -90.0;
					}
			 	}
			 	break;
			}

			{// 保存信息
				GlbScopedLock<GlbCriticalSection> lock(_critical);
				_second = tim;
				_xorlon = xorlon;
				_yorlat = yorlat;
				_zoralt = zoralt;
				_yaw = yaw;
				_pitch = pitch;
				_roll = roll;
			}

			//设置动态对象相关信息：位置、姿态、外包、调度等等
			//设置位置
			_dynamicObject->mpr_xOrLon = xorlon;
			_dynamicObject->mpr_yOrLat = yorlat;
			_dynamicObject->mpr_zOrAlt = zoralt;
			glbref_ptr<GlbRenderInfo> renderinfo = _dynamicObject->GetRenderInfo();
			glbBool isNeedDirectDraw=false;
			//设置姿态
			switch(_dynamicObject->GetDynamicObjMode())
			{
			case GLB_DYNAMICMODE_MODEL:		//三维模型
				{
					GlbMarkerModelSymbolInfo *oldmarkerModelInfo = static_cast<GlbMarkerModelSymbolInfo *>(renderinfo.get());
					//GlbMarkerModelSymbolInfo *markerModelInfo = new GlbMarkerModelSymbolInfo;
					//markerModelInfo->type = oldmarkerModelInfo->type;
					//markerModelInfo->xScale = oldmarkerModelInfo->xScale;
					//markerModelInfo->yScale = oldmarkerModelInfo->yScale;
					//markerModelInfo->zScale = oldmarkerModelInfo->zScale;
					//markerModelInfo->locate = oldmarkerModelInfo->locate;
					//markerModelInfo->yaw = new GlbRenderDouble;
					//markerModelInfo->yaw->value = yaw;
					//markerModelInfo->pitch = new GlbRenderDouble;
					//markerModelInfo->pitch->value = pitch;
					//markerModelInfo->roll = new GlbRenderDouble;
					//markerModelInfo->roll->value = roll;
					//markerModelInfo->opacity = oldmarkerModelInfo->opacity;
					_dynamicObject->SetRenderInfo(oldmarkerModelInfo,isNeedDirectDraw);
					//_dynamicObject->DirectDraw(0);
				}
				break;
			case GLB_DYNAMICMODE_IMAGE:		//图片标记
				{
					//billboard,无需设置姿态
					GlbMarkerImageSymbolInfo *oldmarkerImageInfo = static_cast<GlbMarkerImageSymbolInfo *>(renderinfo.get());
					GlbMarkerImageSymbolInfo *markerImageInfo = new GlbMarkerImageSymbolInfo;
					markerImageInfo->type = oldmarkerImageInfo->type;
					markerImageInfo->imgInfo = oldmarkerImageInfo->imgInfo;
					markerImageInfo->imageAlign = oldmarkerImageInfo->imageAlign;
					markerImageInfo->imgBillboard = oldmarkerImageInfo->imgBillboard;
					markerImageInfo->geoInfo = markerImageInfo->geoInfo;
					_dynamicObject->SetRenderInfo(markerImageInfo,isNeedDirectDraw);
					//GlbMarkerImageSymbolInfo *imageInfo =	static_cast<GlbMarkerImageSymbolInfo *>(renderinfo);
					//imageInfo->yaw->value = yaw;
					//imageInfo->pitch->value = pitch;
					//imageInfo->pitch->value = roll;
					//_dynamicObject->GetDynamicGlobePoint()->SetRenderInfo(imageInfo);
				}
				break;
			case GLB_DYNAMICMODE_TEXT:		//文字标记
				{
					//billboard,无需设置姿态
					GlbMarkerLabelSymbolInfo *markerLabelInfo = static_cast<GlbMarkerLabelSymbolInfo *>(renderinfo.get());
					_dynamicObject->SetRenderInfo(markerLabelInfo,isNeedDirectDraw);
					//GlbMarkerLabelSymbolInfo *labelInfo =	static_cast<GlbMarkerLabelSymbolInfo *>(renderinfo);
					//labelInfo->yaw->value = yaw;
					//labelInfo->pitch->value = pitch;
					//labelInfo->pitch->value = roll;
					//_dynamicObject->GetDynamicGlobePoint()->SetRenderInfo(labelInfo);
				}
				break;
			case GLB_DYNAMICMODE_VIRTUAL:	//虚拟
				break;
			} 
			if (_lastFrametime != DBL_MAX)
			{
				_speed = (cp.getPosition() - _lastFramePosition).length()/(_latestTime-_lastFrametime);
			}
			_lastFrameyaw = yaw;
			_lastFrametime = _latestTime;
			_lastFramePosition = cp.getPosition();
			//_dynamicObject->writepath(L"d:\\outDyobj.txt",cp);
		}		

		// 处理用户自定义事件
		_dynamicObject->callUserFunc(tim);

		traverse(node,nv);
	}
}

glbDouble CGlbGlobeDynamicObject::CGlbGlobeDynamicObjCallback::getAnimationTime() const
{
	return ((_latestTime-_firstTime)-_timeOffset)*_timeMultiplier;
}

void CGlbGlobeDynamicObject::CGlbGlobeDynamicObjCallback::setPause(glbBool pause)
{ 
	if (_pause == pause)
		return;	
	_pause = pause;
	if (_firstTime==DBL_MAX)
		return;
	if (_pause)
		_pauseTime = _latestTime;
	else
		_firstTime += (_latestTime-_pauseTime);
}

glbBool CGlbGlobeDynamicObject::CGlbGlobeDynamicObjCallback::GetCurrentPosition(glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,
																glbDouble* yaw,glbDouble* pitch,glbDouble* roll,glbDouble* seconds)
{
	GlbScopedLock<GlbCriticalSection> lock(_critical);
	*xOrlon = _xorlon;
	*yOrlat = _yorlat;
	*zOrAlt = _zoralt;
	*yaw = _yaw;
	*pitch = _pitch;
	*roll = _roll;
	*seconds = _second;
	return true;
}

CGlbGlobeDynamicObject::CGlbGlobeDynamicObjectTask::CGlbGlobeDynamicObjectTask(CGlbGlobeDynamicObject *obj,glbInt32 level)
{
	mpr_obj = obj;
	mpr_level = level;
}
CGlbGlobeDynamicObject::CGlbGlobeDynamicObjectTask::~CGlbGlobeDynamicObjectTask()
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeDynamicObject::CGlbGlobeDynamicObjectTask::doRequest()
{
	if(mpr_obj)
	{
		CGlbGlobeDynamicObject* obj = dynamic_cast<CGlbGlobeDynamicObject*>(mpr_obj.get());
		if (obj)
			obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject* CGlbGlobeDynamicObject::CGlbGlobeDynamicObjectTask::getObject()
{
	return mpr_obj.get();
}

CGlbGlobeDynamicObject::CGlbGlobeDynamicObject()
{  
	mpr_readData_critical.SetName(L"dynamic_readdata");
	mpr_addToScene_critical.SetName(L"dynamic_addscene");
	mpr_stop_critical.SetName(L"dynamic_stop");
	mpr_PointsPosition = NULL;
	mpr_PointsColor = NULL;
	mpr_isRunning = false;
	mpr_isshowpoints = false;
	mpr_isshowpath = false;
	mpr_DynamicObjCallback = NULL;
	mpr_isDirty = false;
	//mpr_switch = NULL;
	mpt_isShow = true;
	mpr_distance = 0.0;
	mpr_elevation = DBL_MAX;
	mpr_xOrLon = 0.0;
	mpr_yOrLat = 0.0;
	mpr_zOrAlt = 0.0;
	mpr_outline = NULL;
	mpr_renderInfo = NULL;
	mpr_objSize = 0;
	mpr_isFirstAddPosition = true;
	mpr_isMediaPlaying = false;
	mpr_hWnd = MakeWindow();
	mpr_uDeviceID = -1;
	mpt_isEnableSelect = false;  //动态对象不允许被选中

	mpr_additionalYaw = 0;
	mpr_additionalPitch = 0;
	mpr_additionalRoll = 0;
}

CGlbGlobeDynamicObject::~CGlbGlobeDynamicObject(void)
{
	DestroyWindow(mpr_hWnd);
}

glbBool CGlbGlobeDynamicObject::Load(xmlNodePtr *node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Load(node,prjPath);
	xmlChar *szKey = NULL;
	xmlNodePtr pnode = *node;

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"DymicMode")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&mpr_dymicmode);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"TractMode")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&mpr_tractmode);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"PlayMode")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&mpr_playmode);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"Ditan")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%lf",&mpr_ditan);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"Pitch")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%lf",&mpr_pitch);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"IsRepeat")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&mpr_isrepeat);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"Lon")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%lf",&mpr_xOrLon);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"Lat")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%lf",&mpr_yOrLat);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"Alt")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%lf",&mpr_zOrAlt);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	glbInt32 mapSize = 0;
	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"MapSize")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&mapSize);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	std::string index = "MapIndex";
	glbDouble key,positionX,positionY,positionZ,rotationX,
		rotationY,rotationZ,rotationW,scaleX,scaleY,scaleZ;
	for(glbInt32 i = 0; i < mapSize; i++)
	{
		index = "MapIndex";
		//key
		index += CGlbConvert::Int32ToStr(i);
		index += "key";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&key);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		//position
		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "positionX";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&positionX);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "positionY";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&positionY);	
			xmlFree(szKey);
			pnode = pnode->next;
		}
		
		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "positionZ";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&positionZ);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "positionX";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&positionX);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		//rotation
		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "rotationX";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&rotationX);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "rotationY";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&rotationY);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "rotationZ";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&rotationZ);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "rotationW";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&rotationW);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		//scale
		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "scaleX";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&scaleX);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "scaleY";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&scaleY);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "scaleZ";
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%lf",&scaleZ);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		osg::AnimationPath::ControlPoint cPoint;
		cPoint.setPosition(osg::Vec3d(positionX,positionY,positionZ));
		cPoint.setScale(osg::Vec3d(scaleX,scaleY,scaleZ));
		cPoint.setRotation(osg::Quat(rotationX,rotationY,rotationZ,rotationW));
		mpr_tcp_map[key] = cPoint;
		mpr_Tim.push_back(key);
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"RenderInfo")))
	{
		xmlNodePtr rdchild = pnode->xmlChildrenNode;
		GlbMarkerModelSymbolInfo* modeinfo = new GlbMarkerModelSymbolInfo();
		modeinfo->Load(rdchild,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)modeinfo;
	}
	return true;
}

glbBool CGlbGlobeDynamicObject::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);
	char str[32];

	sprintf_s(str,"%d",mpr_dymicmode);
	xmlNewTextChild(node, NULL, BAD_CAST "DymicMode", BAD_CAST str);

	sprintf_s(str,"%d",mpr_tractmode);
	xmlNewTextChild(node, NULL, BAD_CAST "TractMode", BAD_CAST str);

	sprintf_s(str,"%d",mpr_playmode);
	xmlNewTextChild(node, NULL, BAD_CAST "PlayMode", BAD_CAST str);

	sprintf_s(str,"%8lf",mpr_ditan);
	xmlNewTextChild(node, NULL, BAD_CAST "Ditan", BAD_CAST str);

	sprintf_s(str,"%8lf",mpr_pitch);
	xmlNewTextChild(node, NULL, BAD_CAST "Pitch", BAD_CAST str);

	sprintf_s(str,"%d",mpr_isrepeat);
	xmlNewTextChild(node, NULL, BAD_CAST "IsRepeat", BAD_CAST str);

	sprintf_s(str,"%8lf",mpr_xOrLon);
	xmlNewTextChild(node, NULL, BAD_CAST "Lon", BAD_CAST str);

	sprintf_s(str,"%8lf",mpr_yOrLat);
	xmlNewTextChild(node, NULL, BAD_CAST "Lat", BAD_CAST str);

	sprintf_s(str,"%8lf",mpr_zOrAlt);
	xmlNewTextChild(node, NULL, BAD_CAST "Alt", BAD_CAST str);

	if (!mpr_TKp_map.empty())
		mpr_tcp_map = TkpToTcp(mpr_TKp_map);

	sprintf_s(str,"%d",mpr_tcp_map.size()); //mpr_tcp_map
	xmlNewTextChild(node,NULL,BAD_CAST "MapSize",BAD_CAST str);

	osg::AnimationPath::TimeControlPointMap::iterator it = mpr_tcp_map.begin(); //mpr_TKp_map
	glbInt32 i = 0;
	std::string index = "MapIndex";
	for (it; it != mpr_tcp_map.end();it++)
	{
		index = "MapIndex";
		//key -- time
		index += CGlbConvert::Int32ToStr(i);
		index += "key";
		sprintf_s(str,"%8lf",it->first);
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		//position
		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "positionX";
		sprintf_s(str,"%8lf",it->second.getPosition().x());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "positionY";
		sprintf_s(str,"%8lf",it->second.getPosition().y());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "positionZ";
		sprintf_s(str,"%8lf",it->second.getPosition().z());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		//rotation
		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "rotationX";
		sprintf_s(str,"%8lf",it->second.getRotation().x());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "rotationY";
		sprintf_s(str,"%8lf",it->second.getRotation().y());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "rotationZ";
		sprintf_s(str,"%8lf",it->second.getRotation().z());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "rotationW";
		sprintf_s(str,"%8lf",it->second.getRotation().w());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		//scale
		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "scaleX";
		sprintf_s(str,"%8lf",it->second.getScale().x());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "scaleY";
		sprintf_s(str,"%8lf",it->second.getScale().y());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);

		index = "MapIndex";
		index += CGlbConvert::Int32ToStr(i);
		index += "scaleZ";
		sprintf_s(str,"%8lf",it->second.getScale().z());
		xmlNewTextChild(node, NULL, (unsigned char *)index.c_str(), BAD_CAST str);
		i++;
	}

	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		GlbMarkerModelSymbolInfo *modelInfo = (GlbMarkerModelSymbolInfo *)mpr_renderInfo.get();
		modelInfo->Save(rdnode,prjPath);
	}

	return true;
}

GlbGlobeObjectTypeEnum CGlbGlobeDynamicObject::GetType()
{
	return GLB_OBJECTTYPE_DYNAMIC;
}

glbDouble CGlbGlobeDynamicObject::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if (!isCompute)
		return mpr_distance;

	if (!mpt_globe)
		return DBL_MAX;

	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else
	{//重新计算对象相机距离
		osg::Vec3d centerPos(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			osg::Vec3d cameraPos_world;
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos_world.x(),cameraPos_world.y(),cameraPos_world.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(centerPos.y()),osg::DegreesToRadians(centerPos.x()),centerPos.z(),centerPos.x(),centerPos.y(),centerPos.z());
			mpr_distance = (centerPos-cameraPos_world).length();
		}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			mpr_distance = (centerPos-cameraPos).length();
		}
	}
	return mpr_distance;
}

glbref_ptr<CGlbExtent> CGlbGlobeDynamicObject::GetBound(glbBool isWorld/* = true*/)
{
	if (mpt_globe == NULL && isWorld) return NULL;
	glbref_ptr<CGlbExtent> geoExt = NULL;
	if(    mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN
			&& mpr_outline != NULL)
	{//贴地形绘制模式
		/*
		*   索引、查询【qtree】才能准确. z == 0.
		*/
		geoExt = const_cast<CGlbExtent *>(mpr_outline->GetExtent());
	}else{
		if(mpt_feature)
		{
			geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		}
		else
		{
			geoExt = new CGlbExtent();
			geoExt->SetMin(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
			geoExt->SetMax(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);			
		}
	}
	if(isWorld == false)
	{//地理坐标
		/*
		   对象添加时，对贴地形对象,在场景树中是用的地理坐标!!!!
		   贴地形绘制，GetDistance会用到mpr_elevate.
		   所以：
		        调用UpdateElevate().
		*/
		if( mpr_elevation == DBL_MAX
			&& mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
			   UpdateElevate();
		/*
		*   地理坐标，不反应相对地形绘制的地形海拔.
		*/
		return geoExt;
	}
	else
	{//世界坐标		
		if(geoExt == NULL)return NULL;
		glbref_ptr<CGlbExtent> worldExt = new CGlbExtent();
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();
		if(mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地形绘制
			double x,y,z;
			geoExt->GetMin(&x,&y,&z);
			z = mpt_globe->GetElevationAt(x,y);
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->Merge(x,y,z);
			geoExt->GetMax(&x,&y,&z);
			z = mpt_globe->GetElevationAt(x,y);
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->Merge(x,y,z);
			/*
			   贴地形绘制，GetDistance会用到mpr_elevate.
			   所以：
			        调用UpdateElevate().
			*/
			if( mpr_elevation == DBL_MAX)
				   UpdateElevate();
		}
		else
		{//非贴地形.
			osg::ref_ptr<osg::Node> node = mpt_node;
			if(   node != NULL
				&&node->getBound().valid() )
			{				
				CGlbCalculateBoundBoxVisitor bboxV;
				node->accept(bboxV);
				osg::BoundingBoxd bb =bboxV.getBoundBox();
				worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
				worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
			}
			else
			{//osg 节点还没生成.				
				double x,y,z;
				geoExt->GetMin(&x,&y,&z);
				if( mpr_elevation == DBL_MAX)
				   UpdateElevate();
				if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				{					
					z += mpr_elevation;
				}
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);					
				}
				worldExt->Merge(x,y,z);

				geoExt->GetMax(&x,&y,&z);
				if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				{					
					z += mpr_elevation;
				}
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);
				}
				worldExt->Merge(x,y,z);
			}
		}//非贴地模式
		return worldExt;
	}//世界坐标	
	return NULL;
}

void CGlbGlobeDynamicObject::LoadData(glbInt32 level)
{
	if(mpt_currLevel == level) return;
	if(mpt_preNode  !=  NULL)
	{//旧对象还未压入摘队列
		/*
		*   mpt_node = node1,preNode = NULL    -----> node1_h
		*   不做控制，且LoadData快
		*   {
		*       mpt_node = node2,preNode = node1
		*       mpt_node = node3,preNode = node2   -----> node1_h,node2_r,node2_h
		*       造成node1 永远不会摘除
		*   }
		*/
		return;
	}
	{//与DirectDraw互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return;
		mpt_loadState = true;
	}		
	if(mpt_currLevel != level)
	{
		if(mpt_currLevel < level)
			level = mpt_currLevel + 1;
	}
	glbref_ptr<CGlbGlobeDynamicObjectTask> task = new CGlbGlobeDynamicObjectTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeDynamicObject::ReadData(glbInt32 level,glbBool isDirect)
{
	//{
	//	mpt_loadState = false;
	//	RemoveFromScene(this);
	//	mpt_node = new osg::Node;
	//}
	if (mpr_renderInfo==NULL)	{mpt_loadState=false;return;}
	//osg::ref_ptr<osg::Switch> switchnode = new osg::Switch;
	glbDouble yaw = 0.0 , pitch = 0.0 , roll = 0.0;
	glbDouble xScale = 1 , yScale = 1 , zScale = 1;
	osg::Node* node = NULL;
	if (mpr_xOrLon == 0.0 && mpr_yOrLat == 0.0 && mpr_zOrAlt == 0.0)
	{
		mpr_xOrLon = mpr_TKp_map.begin()->second.xorlon;
		mpr_yOrLat = mpr_TKp_map.begin()->second.yorlat;
		mpr_zOrAlt = mpr_TKp_map.begin()->second.zoralt;
	}
	glbDouble positionx = mpr_xOrLon;
	glbDouble positiony = mpr_yOrLat;
	glbDouble positionz = mpr_zOrAlt;
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(positiony),osg::DegreesToRadians(positionx),positionz,positionx,positiony,positionz);
	glbref_ptr<CGlbPoint> point = new CGlbPoint(positionx,positiony,positionz);

	switch (GetDynamicObjMode())
	{
	case GLB_DYNAMICMODE_MODEL:
		{
			GlbMarkerModelSymbolInfo* markerModelInfo = dynamic_cast<GlbMarkerModelSymbolInfo *>(mpr_renderInfo.get());
			if (markerModelInfo)
			{
				CGlbGlobeMarkerModelSymbol markerModelSymbol;
				node = markerModelSymbol.Draw(this,NULL);

				if(markerModelInfo->xScale)
					xScale = markerModelInfo->xScale->GetValue(mpt_feature.get());
				if(markerModelInfo->yScale)
					yScale = markerModelInfo->yScale->GetValue(mpt_feature.get());
				if(markerModelInfo->zScale)
					zScale = markerModelInfo->zScale->GetValue(mpt_feature.get());
				if(markerModelInfo->yaw)
					yaw = markerModelInfo->yaw->GetValue(mpt_feature.get());
				if(markerModelInfo->pitch)
					pitch = markerModelInfo->pitch->GetValue(mpt_feature.get());
				if(markerModelInfo->roll)
					roll = markerModelInfo->roll->GetValue(mpt_feature.get());
			}		
		}
		break;
	case GLB_DYNAMICMODE_IMAGE:
		{			
			GlbMarkerImageSymbolInfo* markerImageInfo = dynamic_cast<GlbMarkerImageSymbolInfo *>(mpr_renderInfo.get());
			if (markerImageInfo) 
			{			
				CGlbGlobeMarkerImageSymbol markerImageSymbol;
				node = markerImageSymbol.Draw(this,point.get());
			}
		}
		break;
	case GLB_DYNAMICMODE_TEXT:
		{
			GlbMarkerLabelSymbolInfo* markerLabelInfo = dynamic_cast<GlbMarkerLabelSymbolInfo *>(mpr_renderInfo.get());
			if (markerLabelInfo) 
			{
				CGlbGlobeMarkerLabelSymbol markerLabelSymbol;
				node = markerLabelSymbol.Draw(this,point.get());
			}

		}
		break;
	case GLB_DYNAMICMODE_VIRTUAL:
		{
			node = new osg::Node;
		}
		break;
	}
	if (node==NULL)
	{
		mpt_loadState = false;	
		return;
	}
	osg::MatrixTransform* mt = new osg::MatrixTransform;
	osg::Matrixd localToworld,m2,m3;
	localToworld = osg::Matrixd::translate(positionx,positiony,positionz);
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(positionx,positiony,positionz,localToworld);
	m2.makeScale(xScale,yScale,zScale);
	yaw = osg::DegreesToRadians(yaw);
	pitch = osg::DegreesToRadians(pitch);
	roll = osg::DegreesToRadians(roll);
	osg::Vec3d yaw_vec(0.0,0.0,1.0);
	osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
	osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
	m3.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
	mt->setMatrix(m2 * m3 * localToworld);
	mt->addChild(node);
	osg::ref_ptr<osg::Switch> swiNode = new osg::Switch;
	swiNode->addChild(mt);

	//mpr_outline = new CGlbPoint3D(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
	CGlbMultiPoint * multiPts = new CGlbMultiPoint;
	multiPts->AddPoint(mpr_xOrLon,mpr_yOrLat);
	mpr_outline = multiPts;

	if(mpr_needReReadData)
	{
		/*
		*   在检测mpr_needReReadData时,其它线程设置mpr_needReReadData=true还没执行！！！.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}
	
	//增加使用内存
	glbInt32 objsize = this->ComputeNodeSize(swiNode);
	glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);
	if(isOK||isDirect)
	{
		// 预先计算bound，节省时间
		swiNode->getBound();

		mpt_preNode   = mpt_node;
		mpt_node      = swiNode;
		mpt_currLevel = level;		
		mpr_objSize   = objsize;
	}else{
		swiNode = NULL;
	}
	
	mpt_loadState = false;	
}

void CGlbGlobeDynamicObject::AddToScene()
{
	if(mpt_node == NULL)return;
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	bool needUpdate = false;
	if(mpt_preNode == NULL)
	{//没有产生新osg节点
		if(mpt_HangTaskNum == 0
			&& mpt_node->getNumParents() == 0
			&& mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地模式,节点不需要挂.
			//挂mpt_node任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;
			needUpdate = true;
		}			
	}
	else
	{//产生了新osg节点						
		{//旧节点已经挂到场景树
			//摘旧节点mpt_preNode的任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
			task->mpr_size = this->ComputeNodeSize(mpt_preNode);
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			mpt_preNode = NULL;
			CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(task->mpr_size);
		}
		if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//挂新节点mpt_node任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;	
		}
		needUpdate = true;
	}
	if(needUpdate)
	{
		mpt_globe->UpdateObject(this);
		glbBool isnew = mpt_isDispShow && mpt_isShow;
		if (isnew == false)			
			mpt_node->asSwitch()->setValue(0,false);
		//if (mpt_isSelected)
		//	DealSelected();
		//if(mpt_isBlink)
		//	DealBlink();
	}
}

glbInt32 CGlbGlobeDynamicObject::RemoveFromScene(glbBool isClean)
{
	{//与LoadData互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return 0;	
		mpt_loadState = true;
		/*
		*    mpt_node = node1,pre=NULL
		*    读:mpt_node = node2,pre=node1		
		*/
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	////处理贴地面	
	//if(  mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	//{		
	//	DirtyOnTerrainObject();
	//}
	glbInt32 tsize = 0;
	if(isClean == false)
	{//从显存卸载对象，节约显存.
		if ((mpt_node != NULL))// && (mpt_node->getNumParents() != 0))
		{//obj在显存	
			/*
			     mpt_node 已经在挂队列，但是还没挂到场景树上
				 这时判断getNumParents() != 0 是不可以的.
			*/
			CGlbGlobeCallBack::CGlbGlobeCallBackTask *task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			tsize = task->mpr_size;
		}
	}
	else
	{//从内存中卸载对象，节约内存
		//删除 上一次装载的节点		
		if (mpt_preNode != NULL)
		{
			//if(mpt_preNode->getNumParents() > 0 )
			{				
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
				task->mpr_size = ComputeNodeSize(mpt_preNode);
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize += ComputeNodeSize(mpt_preNode);
			mpt_preNode = NULL;
		}
		//删除当前节点
		if (mpt_node != NULL) 
		{
			//if(mpt_node->getNumParents() > 0)
			{		
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
				task->mpr_size = this->GetOsgNodeSize();
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize       += this->GetOsgNodeSize();			
			mpt_node    =  NULL;
			mpr_objSize =  0;
		}		
		if (mpt_feature != NULL && mpt_featureLayer != NULL)
		{
			CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer);
			if (fl)
				fl->NotifyFeatureDelete(mpt_feature->GetOid());
		}
		mpt_currLevel = -1;					
	}
	mpt_loadState = false;
	//减少使用内存
	if(tsize>0 && isClean)
	{
		CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(tsize);		
	}
	return tsize;	
}

void CGlbGlobeDynamicObject::DirectDraw(glbInt32 level)
{
	if (mpt_parent    == NULL) return;		
	{//与LoadData互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			    LoadData->ReadData，选择退出，但是会产生如下问题：
				        LoadData->ReadData 使用的是旧的渲染相关的属性，不能反映新设置的渲染属性.
				还必须直接退出，如果选择等待LoadData->ReadData的话，会堵塞界面线程！！！！！

				所以设置一个标志，ReadData 执行到末尾时，检查该标志,如果标示是true，ReadData在执行一次.
			*/
			mpr_needReReadData = true;
			return;
		}
		mpt_loadState = true;
	}
	if(mpt_preNode != NULL)
	{
	/*
	*    mpr_node = node2,preNode = node1  ----A线程调用AddToScene，
	*                                         还没有preNode Remove还没执行完
	*    下面ReadData 已经做完：
	*    mpr_ndoe = node3,preNode = node2   造成node1永远不会摘除.
	*/
		AddToScene();
	}
	/*
	*    因为在LoadData->ReadData时，ReadData 装载对象后发现超过内存限额
	*        会抛弃新加载的osg节点.
	*    但对DirectDraw而言：如果抛弃新的osg节点，就不能反映最新的变化.
	*    所以：
	*         ReadData方法新增一个参数，标记是DirectDraw调用的,不管是否超过
	*         内存限额,都不抛弃新的osg节点.
	*/
	ReadData(level,true);
	AddToScene();
}

void CGlbGlobeDynamicObject::UpdateElevate()
{
	if (!mpt_globe) 	return;
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
	{
		//贴地形绘制,GetDistance()是会用到mpr_elevate.
		return;
	}

	if (mpt_feature)
	{
		CGlbExtent* ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		if (ext)
		{
			glbDouble xOrLon,yOrLat;
			ext->GetCenter(&xOrLon,&yOrLat);
			mpr_elevation = mpt_globe->GetElevationAt(xOrLon,yOrLat);
		}
	}else
		mpr_elevation = mpt_globe->GetElevationAt(mpr_xOrLon,mpr_yOrLat);	
}

/*
 *	动态对象高程设置包含2种模式：相对地形高度、相对海平面高度
*/
glbBool CGlbGlobeDynamicObject::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	if (mpt_altitudeMode == mode)
		return true;
	isNeedDirectDraw = false;
	if(mpt_globe == NULL)
	{
		mpt_altitudeMode = mode;
		return true;
	}
	/////////////////////////////////////////////////////////////////
	{   		
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			 *  如果正在ReadData，ReadData可能在新的地形模式之前使用了旧的模式.
			 *  所以：
					需要告知调用者，调用DirectDraw.
			*/
			isNeedDirectDraw = true;			
			mpt_altitudeMode = mode;						
			return true;
		}
		mpt_loadState = true;
	}
	mpt_altitudeMode = mode;	
	if (mpt_node != NULL)
	{
		mpt_altitudeMode = mode;
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			UpdateElevate();
			/*
			    UpdateElevate()中有一条件：if(fabs(elevation - mpr_elevation) < 0.0001)
				在模式变换时,很有可能不满足上述条件。
				所以：
				     必须调用DealModePosByChangeAltitudeOrChangePos().
			*/
		}
		DealModePosByChangeAltitudeOrChangePos();			
	}
	mpt_loadState = false;
	return true;
}

glbBool CGlbGlobeDynamicObject::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	//新增,2014/11/14敖建
	//次此处引用计数不增加一，下边给其赋值会析构崩溃
	if (mpr_renderInfo.valid())
		mpr_renderInfo->Ref();
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_POINT)  return false;
	GlbMarkerSymbolInfo *markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(renderInfo);
	if (markerInfo == NULL) return false;		
	{//与LoadData互斥.
		/*
		*   这是因为： ReadData执行在中途时，生成的结果可能不会反应 新设置的属性.
		*/
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			return true;			
		}
		mpt_loadState = true;
	}
	if(mpt_node == NULL)
	{		
		mpr_renderInfo   = renderInfo;
		mpt_loadState    = false;		
		return true;
	}
	
	glbDouble xorlon = mpr_xOrLon;
	glbDouble yorlat = mpr_yOrLat;
	glbDouble zoralt = mpr_zOrAlt;
	glbDouble yaw = 0.0,pitch = 0.0,roll = 0.0,xscale = 1.0,yscale = 1.0,zscale = 1.0;		
	switch(GetDynamicObjMode())
	{
	case GLB_DYNAMICMODE_MODEL:
		{
			GlbMarkerModelSymbolInfo* markerModelInfo = dynamic_cast<GlbMarkerModelSymbolInfo*> (markerInfo);
			GlbMarkerModelSymbolInfo* oldmarkerModelInfo = dynamic_cast<GlbMarkerModelSymbolInfo*> (mpr_renderInfo.get());
			if (oldmarkerModelInfo/* && markerModelInfo*/)
			{
				if (oldmarkerModelInfo->locate && markerModelInfo->locate)
				{
					if (oldmarkerModelInfo->locate->GetValue(mpt_feature.get()) != markerModelInfo->locate->GetValue(mpt_feature.get()))
						isNeedDirectDraw = true;
				}else if (oldmarkerModelInfo->locate && !markerModelInfo->locate)
					isNeedDirectDraw = true;
				else if (!oldmarkerModelInfo->locate && markerModelInfo->locate)
					isNeedDirectDraw = true;
			}else{
				isNeedDirectDraw = true;
			}

			yaw = markerModelInfo->yaw->GetValue(mpt_feature.get());
			pitch = markerModelInfo->pitch->GetValue(mpt_feature.get());
			roll = markerModelInfo->roll->GetValue(mpt_feature.get());
			xscale = markerModelInfo->xScale->GetValue(mpt_feature.get());
			yscale = markerModelInfo->yScale->GetValue(mpt_feature.get());
			zscale = markerModelInfo->zScale->GetValue(mpt_feature.get());
		}
		break;
	case GLB_DYNAMICMODE_IMAGE:
		{
			GlbMarkerImageSymbolInfo* markerImageInfo = static_cast<GlbMarkerImageSymbolInfo*> (markerInfo);
			GlbMarkerImageSymbolInfo* oldmarkerImageInfo = static_cast<GlbMarkerImageSymbolInfo*> (mpr_renderInfo.get());
			if (oldmarkerImageInfo/* && markerModelInfo*/)
			{
				if (oldmarkerImageInfo->imgInfo && markerImageInfo->imgInfo)
				{
					if (oldmarkerImageInfo->imgInfo->dataSource != markerImageInfo->imgInfo->dataSource)
						isNeedDirectDraw = true;
				}else if (oldmarkerImageInfo->imgInfo && !markerImageInfo->imgInfo)
					isNeedDirectDraw = true;
				else if (!oldmarkerImageInfo->imgInfo && markerImageInfo->imgInfo)
					isNeedDirectDraw = true;
				if (oldmarkerImageInfo->geoInfo && markerImageInfo->geoInfo)
				{
					if (oldmarkerImageInfo->geoInfo->ratio != markerImageInfo->geoInfo->ratio)
						isNeedDirectDraw =true;
				}
			}else{
				isNeedDirectDraw = true;
			}			
		}
		break;
	case GLB_DYNAMICMODE_TEXT:
		{
			GlbMarkerLabelSymbolInfo* markerLabelInfo = static_cast<GlbMarkerLabelSymbolInfo*> (markerInfo);
			GlbMarkerLabelSymbolInfo* oldmarkerLabelInfo = static_cast<GlbMarkerLabelSymbolInfo*> (mpr_renderInfo.get());
			if (oldmarkerLabelInfo/* && markerModelInfo*/)
			{
				if (oldmarkerLabelInfo->textInfo && markerLabelInfo->textInfo)
				{	
					if (oldmarkerLabelInfo->textInfo != markerLabelInfo->textInfo)
						isNeedDirectDraw = true;
				}else if (oldmarkerLabelInfo->textInfo && !markerLabelInfo->textInfo)
					isNeedDirectDraw = true;
				else if (!oldmarkerLabelInfo->textInfo && markerLabelInfo->textInfo)
					isNeedDirectDraw = true;
				if (oldmarkerLabelInfo->geoInfo && markerLabelInfo->geoInfo)
				{
					if (oldmarkerLabelInfo->geoInfo->ratio != markerLabelInfo->geoInfo->ratio)
						isNeedDirectDraw =true;
				}
			}else{
				isNeedDirectDraw = true;
			}
		}
		break;
	case GLB_DYNAMICMODE_VIRTUAL:		
		break;
	}

	mpr_renderInfo = renderInfo;

	if (!isNeedDirectDraw)
	{
		osg::Group* group = dynamic_cast<osg::Group*>(GetOsgNode());
		if (group)
		{
			osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(group->getChild(0));

			osg::Matrixd localToworld,mat,mat2;
			localToworld = osg::Matrixd::translate(xorlon,yorlat,zoralt);
			if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(yorlat),osg::DegreesToRadians(xorlon),zoralt,localToworld);
			yaw = osg::DegreesToRadians(yaw);
			pitch = osg::DegreesToRadians(pitch);
			roll = osg::DegreesToRadians(roll);
			osg::Vec3d yaw_vec(0.0,0.0,1.0);
			osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
			osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
			mat2.makeScale(xscale,yscale,zscale);
			mat.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
			mt->setMatrix(mat2 * mat * localToworld);

			mpt_globe->UpdateObject(this);
		}
	}

	mpt_loadState  = false;
	return true;
}

GlbRenderInfo* CGlbGlobeDynamicObject::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeDynamicObject::SetShow(glbBool isShow,glbBool isOnState)
{
	if(mpt_node == NULL && isOnState==true)
		return false;	

	if (mpt_isEdit && isOnState)// 编辑状态不再由调度控制显隐
		return true;
	/*
	* 调度器根据可见范围控制对象显示和隐藏此时isOnState==true,isShow==true或false
	*/
	
	glbBool isold = mpt_isDispShow && mpt_isShow;

	if(isOnState)
	{// 调度器控制显隐
		mpt_isDispShow = isShow;
	}
	else
	{// 用户控制显隐
		mpt_isShow = isShow;
	}

	glbBool isnew = mpt_isDispShow && mpt_isShow;
	
	if(isold == isnew)
		return true;
	{
		if (mpt_node)
		{
			if(isnew)
			{
				mpt_node->asSwitch()->setAllChildrenOn();
			}
			else
				mpt_node->asSwitch()->setAllChildrenOff();
		}
	}
	return true;
}

/*
*	动态对象是不需要实现选中效果吗？？？
*/
glbBool CGlbGlobeDynamicObject::SetSelected(glbBool isSelected)
{
	return false;
}

glbInt32 CGlbGlobeDynamicObject::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if(mpr_objSize == 0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

glbBool CGlbGlobeDynamicObject::SetEdit(glbBool isEdit)
{
	if (mpt_isEdit==isEdit) return true;
	mpt_isEdit = isEdit;	
	//DealEdit();
	return true;
}

glbInt32 CGlbGlobeDynamicObject::ComputeNodeSize(osg::Node *node)
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();		
}

void CGlbGlobeDynamicObject::DealModePosByChangeAltitudeOrChangePos()
{
	//if(mpt_node == NULL)return;
	//unsigned int numChild = mpt_node->asSwitch()->getNumChildren();
	//for (unsigned int i = 0; i < numChild; i++)
	//{
	//	osg::MatrixTransform *mt = 
	//		dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
 //       if(mt == NULL)continue;
	//	if (mpt_feature)
	//	{
	//		glbDouble xOrLon,yOrLat,zOrAlt,M;
	//		zOrAlt = 0.0;
	//		M      = 0.0;
	//		if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
	//			return;
	//		/*
	//		*   m 包含了 trans,scale,rotate三个矩阵.
	//		*   此处只修改trans.
	//		*/
	//		osg::Matrix m = mt->getMatrix();
	//		osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
	//		ComputePosByAltitudeAndGloleType(position);
	//		m.setTrans(position);	
	//		mt->setMatrix(m);
	//	}
	//	else
	//	{			
	//		osg::Matrix m = mt->getMatrix();
	//		osg::Vec3d position(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
	//		ComputePosByAltitudeAndGloleType(position);
	//		m.setTrans(position);
	//		mt->setMatrix(m);			
	//	}
	//}
	////更新对象在场景索引位置.
	//mpt_globe->UpdateObject(this);
}

//////////////////////////////////////////////////////////////////////////
osg::AnimationPath::ControlPoint CGlbGlobeDynamicObject::PositionToControlPoint(glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll)
{
	osg::AnimationPath::ControlPoint cp; 
	yaw = osg::DegreesToRadians(yaw);
	pitch = osg::DegreesToRadians(pitch);
	roll = osg::DegreesToRadians(roll);
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{ 	
		osg::Matrixd localToworld;
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(yOrlat),osg::DegreesToRadians(xOrlon),zOrAlt,localToworld); 
		//setPosition
		cp.setPosition(localToworld.getTrans());
		//setRotation
		//先偏航，再俯仰，然后滚转,Z-X-Y
		osg::Vec3d yaw_vec(0.0,0.0,1.0);
		osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);                                                         
		osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
		osg::Quat quat(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
		quat *= localToworld.getRotate();
		cp.setRotation(quat);
	}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		//setPosition
		cp.setPosition(osg::Vec3d(xOrlon,yOrlat,zOrAlt));
		//setRotation
		//先偏航，再俯仰，然后滚转,Z-X-Y
		/************************************************************************/
		/*旋转矩阵： M = My*Mx*Mz
		/* 		|1		0			0	  |		 |cos(roll)	0	-sin(roll)|		 | cos(yaw)	sin(yaw) 0 | 
		/* Mx = |0	cos(pitch)	sin(pitch)|	My = |	0		1		0	  |	Mz = |-sin(yaw)	cos(yaw) 0 |
		/* 		|0	-sin(pitch)	cos(pitch)|		 |sin(roll)	0	 cos(roll)|		 |		0		0	 1 |
		/*		| cos(roll)cos(yaw)-sin(roll)sin(pitch)sin(yaw)	cos(roll)sin(yaw)+sin(roll)sin(pitch)cos(yaw)	-sin(roll)cos(pitch)|	
		/*	M = | -cos(pitch)sin(yaw)							cos(pitch)cos(yaw)								sin(pitch)			|			
		/*		| sin(roll)cos(yaw)+cos(roll)sin(pitch)sin(yaw)	sin(roll)sin(yaw)-cos(roll)sin(pitch)cos(yaw)	cos(roll)cos(pitch)	|
		/************************************************************************/
		osg::Vec3d yaw_vec(0.0,0.0,1.0);
		osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
		osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
		osg::Quat quat(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
		cp.setRotation(quat);
	}
	return cp;
}

void CGlbGlobeDynamicObject::ControlPointToPosition(osg::AnimationPath::ControlPoint& cp,glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,glbDouble* yaw,glbDouble* pitch,glbDouble* roll)
{ 
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		osg::Vec3d position = cp.getPosition();
		g_ellipsoidModel->convertXYZToLatLongHeight(position.x(),position.y(),position.z(),*yOrlat,*xOrlon,*zOrAlt);
		*xOrlon = osg::RadiansToDegrees(*xOrlon);
		*yOrlat = osg::RadiansToDegrees(*yOrlat);
		osg::Matrixd localToworld;
		g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToworld);
		osg::Matrixd mat;
		cp.getMatrix(mat);
		mat.postMult(osg::Matrixd::inverse(localToworld));
		//mat = mat * osg::Matrixd::inverse(localToworld);
		if (mat(1,2) > 1.0)
			*pitch = 90.0;
		else if (mat(1,2) < -1.0)
			*pitch = -90.0;
		else
			*pitch = osg::RadiansToDegrees(asin(mat(1,2)));
		//*pitch -= -90.0;
		*yaw = osg::RadiansToDegrees(atan2(-mat(1,0),mat(1,1)));
		*roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(2,2)));
		if (abs(*pitch) > 89.99)
		{
			*yaw += *roll;
			*roll = 0.0;
		}
	}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{ 
		osg::Vec3d position = cp.getPosition();
		*xOrlon = position.x();
		*yOrlat = position.y();
		*zOrAlt = position.z();
		osg::Matrixd mat;
		cp.getMatrix(mat);
		if (mat(1,2) > 1.0)
			*pitch = 90.0;
		else if (mat(1,2) < -1.0)
			*pitch = -90.0;
		else
			*pitch = osg::RadiansToDegrees(asin(mat(1,2)));
		//*pitch -= -90.0;
		*yaw = osg::RadiansToDegrees(atan2(-mat(1,0),mat(1,1)));
		*roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(2,2)));
		if (abs(*pitch) > 89.99)
		{
			*yaw += *roll;
			*roll = 0.0;
		}
	}
}

osg::AnimationPath::TimeControlPointMap CGlbGlobeDynamicObject::TkpToTcp(std::map<glbDouble,KeyPoint> tkp_map)
{
	osg::AnimationPath::TimeControlPointMap tcp_map;
	std::map<glbDouble,KeyPoint>::iterator iter = tkp_map.begin();
	for ( ;iter != tkp_map.end();iter++)
	{
		tcp_map[iter->first] = PositionToControlPoint(iter->second.xorlon,iter->second.yorlat,iter->second.zoralt,iter->second.yaw,iter->second.pitch,iter->second.roll);
	}
	return tcp_map;
}

glbBool CGlbGlobeDynamicObject::AddPosition(glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds)
{	
	if(mpr_isFirstAddPosition)
	{
		mpr_xOrLon = xOrlon;
		mpr_yOrLat = yOrlat;
		mpr_zOrAlt = zOrAlt;
		mpr_isFirstAddPosition = false;
	}
	mpr_Tim.push_back(seconds);
	KeyPoint kp;
	kp.xorlon = xOrlon;
	kp.yorlat = yOrlat;
	kp.zoralt = zOrAlt;
	kp.yaw = yaw;
	kp.pitch = pitch;
	kp.roll = roll;
	mpr_TKp_map[seconds] = kp;
	//mpr_tcp_map[seconds] = PositionToControlPoint(xOrlon,yOrlat,zOrAlt,yaw,pitch,roll);
	return true;
}

glbBool CGlbGlobeDynamicObject::InsertPosition(glbInt32 nId,glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds)
{
	if (nId < 0 || nId > GetCount())
		return false;
	KeyPoint kp;
	kp.xorlon = xOrlon;
	kp.yorlat = yOrlat;
	kp.zoralt = zOrAlt;
	kp.yaw = yaw;
	kp.pitch = pitch;
	kp.roll = roll;
	mpr_TKp_map[seconds] = kp;
	//osg::AnimationPath::ControlPoint cp = PositionToControlPoint(xOrlon,yOrlat,zOrAlt,yaw,pitch,roll);
	mpr_Tim.insert(mpr_Tim.begin()+nId,seconds);
	//mpr_tcp_map.insert(std::pair<glbDouble,osg::AnimationPath::ControlPoint>(seconds,cp));
	mpr_TKp_map.insert(std::pair<glbDouble,KeyPoint>(seconds,kp));
	return true;
}

glbBool CGlbGlobeDynamicObject::ReplacePosition(glbInt32 nId,glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds)
{
	RemovePosition(nId);
	InsertPosition(nId,xOrlon,yOrlat,zOrAlt,yaw,pitch,roll,seconds);
	return true;
}

glbBool CGlbGlobeDynamicObject::RemovePosition(glbInt32 nId)
{  
	if (nId < 0 || nId >= GetCount())
		return false;
	
	mpr_TKp_map.erase(mpr_TKp_map.find(mpr_Tim[nId]));
	mpr_Tim.erase(mpr_Tim.begin()+nId);

	return true;
}

glbInt32 CGlbGlobeDynamicObject::GetCount()
{
	return mpr_Tim.size();	
}

glbBool CGlbGlobeDynamicObject::GetPosition(glbInt32 nId,glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,glbDouble* yaw,glbDouble* pitch,glbDouble* roll,glbDouble* seconds)
{ 
	if (nId < 0 || nId >= GetCount())
		return false;
	*seconds =  mpr_Tim[nId];
	if (mpr_TKp_map.empty())
	{
		//将mpr_tcp_map转化为mpr_TKp_map
		for (int i = 0;i<mpr_Tim.size();i++)
		{
			KeyPoint kp;
			ControlPointToPosition(mpr_tcp_map[mpr_Tim[i]],&kp.xorlon,&kp.yorlat,&kp.zoralt,&kp.yaw,&kp.pitch,&kp.roll);
			mpr_TKp_map[mpr_Tim[i]] = kp;
		}
	}
//#ifdef _DEBUG 
//	osg::AnimationPath::TimeControlPointMap::iterator _iter;; 
//	_iter = mpr_tcp_map.find(*seconds);
//	if(_iter == mpr_tcp_map.end())
//	{
//		std::cout<<"we do not find "<<*seconds<<std::endl;
//		return false;
//	}
//#endif
	*xOrlon =  mpr_TKp_map[*seconds].xorlon;
	*yOrlat =  mpr_TKp_map[*seconds].yorlat;
	*zOrAlt =  mpr_TKp_map[*seconds].zoralt;
	*yaw =  mpr_TKp_map[*seconds].yaw;
	*pitch =  mpr_TKp_map[*seconds].pitch;
	*roll =  mpr_TKp_map[*seconds].roll;
	//ControlPointToPosition(mpr_tcp_map[*seconds],xOrlon,yOrlat,zOrAlt,yaw,pitch,roll);
	return true;
}

glbBool CGlbGlobeDynamicObject::GetCurrentPosition(glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,glbDouble* yaw,glbDouble* pitch,glbDouble* roll,glbDouble* seconds)
{
	if (mpr_DynamicObjCallback)
	{
		return mpr_DynamicObjCallback->GetCurrentPosition(xOrlon,yOrlat,zOrAlt,yaw,pitch,roll,seconds);
	}
	return false;
}

glbBool CGlbGlobeDynamicObject::GetControlPoint(osg::AnimationPath::ControlPoint *cp)
{	
	//GlbScopedLock<GlbCriticalSection> lock(mpr_stop_critical);
	if (!mpr_DynamicObjCallback)
	{
		if (mpr_tcp_map.empty())
			return false;
		*cp = mpr_tcp_map.begin()->second;
		return true;
	}
	osg::AnimationPath::TimeControlPointMap _tcp_map = mpr_DynamicObjCallback->getTimeControlPointMap();
	glbDouble time = mpr_DynamicObjCallback->getAnimationTime();
	if (mpr_DynamicObjCallback->getPause())
		time = mpr_DynamicObjCallback->getoppositetimeOffset();
	//mpr_dynamicObj_ap->getInterpolatedControlPoint(time,*cp);
	if (_tcp_map.empty())
		return false;
	glbDouble firsttime = _tcp_map.begin()->first;
	glbDouble periodtim = _tcp_map.rbegin()->first - firsttime;
	if (GetRepeat())	//loop
	{
		double modulated_time = (time - firsttime)/periodtim;
		double fraction_part = modulated_time - floor(modulated_time);
		time = fraction_part * periodtim + firsttime;
	}else				//noloop
	{
		if (time > periodtim + firsttime)
			time = periodtim + firsttime;
		else if (time <  firsttime)
			time = firsttime;
	}
	osg::AnimationPath::TimeControlPointMap::const_iterator second = _tcp_map.lower_bound(time);
	if (second == _tcp_map.begin())
	{
		*cp = second->second;
	}
	else if (second != _tcp_map.end())
	{
		osg::AnimationPath::TimeControlPointMap::const_iterator first = second;
		--first;        

		// we have both a lower bound and the next item.

		// delta_time = second.time - first.time
		glbDouble delta_time = second->first - first->first;

		if (delta_time == 0.0)
			*cp = first->second;
		else
			(*cp).interpolate((time - first->first)/delta_time,first->second,second->second);      
	}
	else // (second==_timeControlPointMap.end())
	{
		*cp = _tcp_map.rbegin()->second;
	}

	if (fabs(mpr_additionalPitch)>0.001 || fabs(mpr_additionalRoll)>0.001 || fabs(mpr_additionalYaw)>0.001)
	{// 如果有旋转角或偏移
		glbDouble xorlon,yorlat,zoralt,yaw,pitch,roll;
		ControlPointToPosition(*cp,&xorlon,&yorlat,&zoralt,&yaw,&pitch,&roll);
		yaw += mpr_additionalYaw;
		pitch += mpr_additionalPitch;
		roll += mpr_additionalRoll;
		*cp = PositionToControlPoint(xorlon,yorlat,zoralt,yaw,pitch,roll);
	}
	return true;
}

glbBool CGlbGlobeDynamicObject::SetPointColor(glbInt32 nId,glbInt32 color)
{  
	if (nId < mpr_PointsColor->size())
	{
		(*mpr_PointsColor)[nId] = getColorFromglbInt32(color);
		return true;
	}
	return false;
}

glbBool CGlbGlobeDynamicObject::AddPoint(glbDouble xorlon,glbDouble yorlat,glbDouble zoralt,glbInt32 color)
{ 
	if (!mpr_PointsPosition.valid())
	{  
		mpr_PointsPosition = new osg::Vec3dArray;
		mpr_PointsColor = new osg::Vec4Array;
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yorlat),osg::DegreesToRadians(xorlon),zoralt,mpr_FirstPointPosition.x(),mpr_FirstPointPosition.y(),mpr_FirstPointPosition.z());
		else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
			mpr_FirstPointPosition = osg::Vec3d(xorlon,yorlat,zoralt);
		mpr_PointsPosition->push_back(osg::Vec3d(0.0,0.0,0.0));
		mpr_PointsColor->push_back(getColorFromglbInt32(color));
	}else{
		osg::Vec3d pt;
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yorlat),osg::DegreesToRadians(xorlon),zoralt,pt.x(),pt.y(),pt.z());
		else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
			pt = osg::Vec3d(xorlon,yorlat,zoralt);
		mpr_PointsPosition->push_back(pt - mpr_FirstPointPosition);
		mpr_PointsColor->push_back(getColorFromglbInt32(color));
	}		   	
	return true;
}

glbBool CGlbGlobeDynamicObject::ReplacePoint(glbInt32 nId,glbDouble xorlon,glbDouble yorlat,glbDouble zoralt,glbInt32 color)
{	
	if (!mpr_PointsPosition.valid())
		return false;
	if (nId < mpr_PointsPosition->size() && nId >= 0)
	{
		osg::Vec3d pt;
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yorlat),osg::DegreesToRadians(xorlon),zoralt,pt.x(),pt.y(),pt.z());
		else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
			pt = osg::Vec3d(xorlon,yorlat,zoralt);
		(*mpr_PointsPosition)[nId] = pt - mpr_FirstPointPosition;
		(*mpr_PointsColor)[nId] = getColorFromglbInt32(color);
		return true;
	}
	return false;
}

glbBool CGlbGlobeDynamicObject::InsertPoint(glbInt32 nId,glbDouble xorlon,glbDouble yorlat,glbDouble zoralt,glbInt32 color)
{ 
	if (!mpr_PointsPosition.valid())
	{  
		mpr_PointsPosition = new osg::Vec3dArray;
		mpr_PointsColor = new osg::Vec4Array;
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yorlat),osg::DegreesToRadians(xorlon),zoralt,mpr_FirstPointPosition.x(),mpr_FirstPointPosition.y(),mpr_FirstPointPosition.z());
		else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
			mpr_FirstPointPosition = osg::Vec3(xorlon,yorlat,zoralt);
		mpr_PointsPosition->insert(mpr_PointsPosition->begin(),osg::Vec3d(0.0,0.0,0.0));
		mpr_PointsColor->insert(mpr_PointsColor->begin(),getColorFromglbInt32(color));
		return true;
	}else
	{ 
		if (nId >= 0 && nId < mpr_PointsPosition->size())
		{	
			osg::Vec3d pt;
			if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yorlat),osg::DegreesToRadians(xorlon),zoralt,pt.x(),pt.y(),pt.z());
			else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
				pt = osg::Vec3(xorlon,yorlat,zoralt);
			mpr_PointsPosition->insert(mpr_PointsPosition->begin()+nId,pt - mpr_FirstPointPosition);
			mpr_PointsColor->insert(mpr_PointsColor->begin()+nId,getColorFromglbInt32(color));
			return true;
		}
	}
	return false;
}

glbBool CGlbGlobeDynamicObject::RemovePoint(glbInt32 nId)
{ 
	if (!mpr_PointsPosition.valid())
		return false;
	if (nId < 0 || nId >= mpr_PointsPosition->size())
		return false;
	mpr_PointsPosition->erase(mpr_PointsPosition->begin()+nId);
	mpr_PointsColor->erase(mpr_PointsColor->begin()+nId);
	return true;
}

glbBool CGlbGlobeDynamicObject::RemoveAllPoints()
{ 
	if (mpr_PointsPosition.valid())
	{  		
		mpr_PointsPosition->swap(std::vector<osg::Vec3d>());
		mpr_PointsColor->swap(std::vector<osg::Vec4>());		
		return true;
	}
	return false;
}

glbBool CGlbGlobeDynamicObject::IsShowPoints()
{
	return mpr_isshowpoints;
}

void CGlbGlobeDynamicObject::SetShowPoints(glbBool isShow)
{
	mpr_isshowpoints = isShow;
}

glbBool CGlbGlobeDynamicObject::IsShowPath()
{
	return mpr_isshowpath;
}

void CGlbGlobeDynamicObject::SetShowPath(glbBool isShow)
{
	mpr_isshowpath = isShow;
}

glbBool CGlbGlobeDynamicObject::IsRunning()
{
	return mpr_isRunning;
}

glbBool CGlbGlobeDynamicObject::Run()
{
	if (mpr_Tim.size() < 2)	//存在路径,控制点多于1个
		return false;
	if (!mpr_TKp_map.empty())
		 mpr_tcp_map = TkpToTcp(mpr_TKp_map);
	osg::AnimationPath::TimeControlPointMap temp_tcpmap	= mpr_tcp_map;
	//if (GetRepeat())
	//{  
	//	osg::Vec3d firstpoint = mpr_tcp_map.begin()->second.getPosition();
	//	osg::Vec3d lastpoint = mpr_tcp_map.rbegin()->second.getPosition();
	//	//判断最后一个控制点与第一个控制点是否重合
	//	if (abs(firstpoint.x() - lastpoint.x()) > 0.1 || abs(firstpoint.y() - lastpoint.y()) > 0.1 || abs(firstpoint.z() - lastpoint.z()) > 0.1)	//不重合，误差范围为(-0.1,0.1)
	//	{  
	//		//循环模式下，最后一个点需要与第一个点连起来,相当于给mpr_tcp_map多加一个控制点
	//		osg::AnimationPath::ControlPoint cp = mpr_tcp_map.begin()->second;
	//		osg::Vec3d pos0 = cp.getPosition();
	//		//计算新加控制点时间戳
	//		//计算最后一个点的运动速度
	//		//最后一个点
	//		osg::AnimationPath::TimeControlPointMap::reverse_iterator iter = mpr_tcp_map.rbegin();
	//		glbDouble tim1 = iter->first;
	//		osg::Vec3d pos1 = iter->second.getPosition();
	//		glbDouble distan = (pos0-pos1).length();
	//		//倒数第2个点
	//		iter++;
	//		glbDouble tim2 = iter->first;
	//		osg::Vec3d pos2 = iter->second.getPosition();
	//		glbDouble delt_tim = tim1 - tim2;
	//		glbDouble distan2 = (pos1-pos2).length();
	//		glbDouble speed = distan2/delt_tim;
	//		glbDouble tim = distan/speed + tim1;
	//		temp_tcpmap[tim] = cp;
	//	} 
	//}
	GlbScopedLock<GlbCriticalSection> lock(mpr_stop_critical);
	if (!mpr_DynamicObjCallback.valid())
	{
		mpr_DynamicObjCallback = new CGlbGlobeDynamicObjCallback(this,temp_tcpmap,GetRepeat());
		GetParentNode()->addUpdateCallback(mpr_DynamicObjCallback.get());
	}
	else
	{
		if (mpr_isDirty)
		{  
			glbDouble offsettime = mpr_DynamicObjCallback->getoppositetimeOffset();
			mpr_DynamicObjCallback.release();
			mpr_DynamicObjCallback = NULL;
			mpr_DynamicObjCallback = new CGlbGlobeDynamicObjCallback(this,temp_tcpmap,GetRepeat());
			mpr_DynamicObjCallback->settimeOffset(-offsettime);
			GetParentNode()->addUpdateCallback(mpr_DynamicObjCallback.get());
		}else
		{
			mpr_DynamicObjCallback->setPause(false);	
			MeidaPause(false);
		}
	}
	mpr_isRunning = true;
	return true;
}

glbBool CGlbGlobeDynamicObject::Stop()
{ 
	GlbScopedLock<GlbCriticalSection> lock(mpr_stop_critical);
	if (mpr_DynamicObjCallback.valid())
	{		
		mpr_DynamicObjCallback->setPause(true);
		GetParentNode()->removeUpdateCallback(mpr_DynamicObjCallback.get());
		mpr_DynamicObjCallback.release();
		mpr_DynamicObjCallback = NULL;

		initMediaMap();
	}
	mpr_isRunning = false;
	return true;
}

glbBool CGlbGlobeDynamicObject::Pause()
{	
	if (mpr_DynamicObjCallback.valid())
	{
		mpr_DynamicObjCallback->setPause(true);

		MeidaPause(true);
	}
	mpr_isRunning = false;
	return false;
}

glbBool CGlbGlobeDynamicObject::read_3dmaxpath(CGlbWString fliepath)
{ 
	//std::string filepathstr = fliepath.ToString();
	std::ifstream file(fliepath.ToString(),ios::_Nocreate);
	if (!file)
		return false;
	////判断格式，通过后缀名不同来判断
	//int pos = filepathstr.rfind('.');
	//std::string extensionstr = filepathstr.substr(pos,filepathstr.length() - pos);
	//读3DMax格式路径文件
	while (!file.eof())
	{ 
		double time;
		osg::Vec3d position;
		osg::Quat rotation;
		osg::Vec3d scale;
		file >> time >> position.x() >> position.y() >> position.z() >> rotation.x() >> rotation.y() >> rotation.z() >> rotation.w() >> scale.x() >> scale.y() >> scale.z();
		//if (!file.eof())
		{ 	
			mpr_Tim.push_back(time);
			//mpr_tcp_map[time] = osg::AnimationPath::ControlPoint(position,rotation,scale); 
			osg::AnimationPath::ControlPoint cp(position,rotation,scale);
			KeyPoint kp;
			ControlPointToPosition(cp,&kp.xorlon,&kp.yorlat,&kp.zoralt,&kp.yaw,&kp.pitch,&kp.roll);
			mpr_TKp_map[time] = kp;
		}
	}
	mpr_xOrLon = mpr_TKp_map.begin()->second.xorlon;
	mpr_yOrLat = mpr_TKp_map.begin()->second.yorlat;
	mpr_zOrAlt = mpr_TKp_map.begin()->second.zoralt;
	if (mpt_globe && mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsoidModel->convertXYZToLatLongHeight(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt,mpr_yOrLat,mpr_xOrLon,mpr_zOrAlt);
		mpr_xOrLon = osg::RadiansToDegrees(mpr_xOrLon);
		mpr_yOrLat = osg::RadiansToDegrees(mpr_yOrLat);
	}
	file.close();
	return true;
}

glbBool CGlbGlobeDynamicObject::read_creatarpath(CGlbWString fliepath)
{ 
	std::ifstream file(fliepath.ToString(),ios::in | ios::_Nocreate);
	if (!file)
		return false;
	glbDouble time;
	osg::Vec3d position;
	glbDouble yaw,pitch,roll;
	glbInt32 action;		
	char filepath[128];
	//读CreatarGlobe格式路径文件(球模式下位置用经纬高描述，平面模式用x,y,z(米);yaw,pitch,roll用°描述)
	while (!file.eof())
	{
		osg::Quat rotation;
		osg::Vec3d scale;
		//file >> time >> position.x() >> position.y() >> position.z() >>  rotation.x() >> rotation.y() >> rotation.z() >> rotation.w() >> scale.x() >> scale.y() >> scale.z() >> action;

		file >> position.x() >> position.y() >> position.z() >> yaw >> pitch >> roll >> time >> action;
		if (action > 1)
			file >> filepath;
		//if (!file.eof())
		{ 		
			mpr_Tim.push_back(time);
			KeyPoint kp;
			kp.xorlon = position.x();
			kp.yorlat = position.y();
			kp.zoralt = position.z();
			kp.yaw = yaw;
			kp.pitch = pitch + 90;//教授计算输出的picth角需要+90度
			kp.roll = roll;
			kp.action = action;
			//osg::AnimationPath::ControlPoint cp(position,rotation,scale);
			//ControlPointToPosition(cp,&kp.xorlon,&kp.yorlat,&kp.zoralt,&kp.yaw,&kp.pitch,&kp.roll);
			mpr_TKp_map[time] = kp;		

			if (kp.action != 1)
			{// 用户自定义播放媒体map
				kp.mediaMarker = false;
				kp.mediafilepath = filepath;
				if (kp.mediafilepath.length()>0)
				{// 有设定文件的加入map
					mpr_KeyMeida_map[time] = kp;
				}				
			}			
		}		
	}
	mpr_xOrLon = mpr_TKp_map.begin()->second.xorlon;
	mpr_yOrLat = mpr_TKp_map.begin()->second.yorlat;
	mpr_zOrAlt = mpr_TKp_map.begin()->second.zoralt;
	file.close();
	return true;
}

void CGlbGlobeDynamicObject::write_3dmaxpath(CGlbWString fliepath)
{ 
	std::ofstream file(fliepath.ToString(),ios::out); 
	if (!file)
		return;
	osg::AnimationPath::TimeControlPointMap::iterator iter = mpr_tcp_map.begin();
	glbDouble scale = 1.0;
	while (iter != mpr_tcp_map.end())
	{
		file << iter->first << ' ';
		file << iter->second.getPosition().x()<< ' ';
		file << iter->second.getPosition().y()<< ' ';
		file << iter->second.getPosition().z()<< ' ';
		file << iter->second.getRotation().x()<< ' ';
		file << iter->second.getRotation().y()<< ' ';	
		file << iter->second.getRotation().z()<< ' ';
		file << iter->second.getRotation().w()<< ' ';
		file << scale<< ' ';
		file << scale<< ' ';
		file << scale << std::endl;
		iter++;
	}
	file.close();
}

void CGlbGlobeDynamicObject::write_creatarpath(CGlbWString fliepath)
{ 
	std::ofstream file(fliepath.ToString(),ios::out); 
	if (!file)
		return;
	if ((!mpr_Tim.empty()) && mpr_TKp_map.empty())
	{
		//将mpr_tcp_map转化为mpr_TKp_map
		for (int i = 0;i<mpr_Tim.size();i++)
		{
			KeyPoint kp;
			ControlPointToPosition(mpr_tcp_map[mpr_Tim[i]],&kp.xorlon,&kp.yorlat,&kp.zoralt,&kp.yaw,&kp.pitch,&kp.roll);
			mpr_TKp_map[mpr_Tim[i]] = kp;
		}
	}
	std::map<glbDouble,KeyPoint>::iterator iter = mpr_TKp_map.begin();
	while (iter != mpr_TKp_map.end())
	{
		file << iter->first << ' ';
		file << iter->second.xorlon << ' ';
		file << iter->second.yorlat << ' ';
		file << iter->second.zoralt << ' ';
		file << iter->second.yaw << ' ';
		file << iter->second.pitch << ' ';
		file << iter->second.roll << std::endl;
		iter++;
	}
}

void CGlbGlobeDynamicObject::callUserFunc(glbDouble time)
{
	if (time < 0 || mpr_KeyMeida_map.size()<=0)
		return;

	// 如果有媒体文件正在播放
	if (mpr_isMediaPlaying)
	{		
		MCI_STATUS_PARMS		mciStatus;//MCI状态参数结构体
		mciStatus.dwItem = MCI_STATUS_MODE;
		mciSendCommand(mpr_uDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciStatus);
		if (mciStatus.dwReturn == MCI_MODE_STOP || _isMyWindowClosed)
		{
			// 停止播放媒体文件
			MediaStop();
			//继续漫游	
			Run();		
			_isMyWindowClosed = false;
		}
	}
	else // 没有的话判断是否需要播放媒体了
	{
		//std::map<glbDouble,KeyPoint>::iterator itr = mpr_KeyMeida_map.lower_bound(time);
		//if (itr != mpr_KeyMeida_map.end())
		//{
		//	if ( itr->second.mediaMarker==false)
		//	{
		//		if (itr->second.action!=3)				
		//			Pause();//暂停漫游
		//		// 设置播放标志
		//		itr->second.mediaMarker = true;
		//		// 播放媒体文件
		//		MediaPlay(itr->second.mediafilepath.c_str(),(itr->second.action!=3) ? true : false);	
		//	}
		//}		

		std::map<glbDouble,KeyPoint>::reverse_iterator rItr = mpr_KeyMeida_map.rbegin();
		while(rItr != mpr_KeyMeida_map.rend())
		{
			if (rItr->first <= time)
			{
				if ( rItr->second.mediaMarker==false)
				{
					if (rItr->second.action!=3)		//3表示不用暂停		
						Pause();//暂停漫游
					// 设置播放标志
					rItr->second.mediaMarker = true;
					// 播放媒体文件
					MediaPlay(rItr->second.mediafilepath.c_str(),(rItr->second.action!=3) ? true : false);	
					if (rItr->second.action==3)
						Run(); // 3的情况下不需要停止，继续漫游
					break;
				}
			}
			++rItr;
		}
	}	
}
#include <Digitalv.h>
#include <atlconv.h>
glbBool CGlbGlobeDynamicObject::MediaPlay(const char* filepath, glbBool isNeedPause)
{
	// 已经播放的直接返回
	if (mpr_isMediaPlaying)
	{
		if(isNeedPause==false)
		{// 不需要暂停的情况，先关闭上一个
			MediaStop();
		}
		return true;
	}

	MCI_OPEN_PARMS			mciOpen;//MCI打开参数结构体
	MCI_PLAY_PARMS			mciPlay;//MCI播放参数结构体
	//MCI_STATUS_PARMS		mciStatus;//MCI状态参数结构体
	UINT				uDeviceID = -1 ;//设备ID	

	//mciOpen.dwCallback;         //设置MCI_NOTIFY标志时的回调窗口句柄
	//初始化mciOpen以及打开avi文件
	ZeroMemory(&mciOpen,sizeof(mciOpen));

	CGlbString temp(filepath);
	CGlbWString szFileDir = temp.ToWString();
	mciOpen.lpstrElementName= szFileDir.c_str();//多媒体文件地址

	DWORD dwRes=-1;
	CGlbWString extentname = CGlbPath::GetExtentName(szFileDir);	
	if (extentname == L"wav" || extentname == L"WAV")
	{// 音频文件
#if 0
		PlaySoundW(szFileDir.c_str(),NULL,SND_FILENAME|SND_ASYNC);//异步播放
		//继续漫游	
		//Run();	
#else
		mciOpen.dwCallback=NULL;
		mciOpen.lpstrAlias=NULL;
		mciOpen.lpstrDeviceType = TEXT("WAVEAudio"); //L"mpegaudio"; //L"waveaudio";	

		dwRes = mciSendCommand(NULL , //指定设备标识,MCI此处为NULL,成功Open后标识放入MCI_OPEN_PARMS的wDeviceID变量中
			MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)&mciOpen);
		if (dwRes != 0)
			return false;
		mpr_uDeviceID = mciOpen.wDeviceID;		

		mciPlay.dwCallback = (DWORD)mpr_hWnd;
		dwRes = mciSendCommand(mpr_uDeviceID,MCI_PLAY,MCI_NOTIFY,(DWORD)&mciPlay);		

		if (dwRes != 0) 
			return false;
		// 设置媒体播放标志为true
		mpr_isMediaPlaying = true;
#endif		
	}
	else if (extentname == L"mp3" || extentname== L"MP3")
	{
		DWORD cdlen;//音频文件长度

		mciOpen.dwCallback=NULL;
		mciOpen.lpstrAlias=NULL;
		mciOpen.lpstrDeviceType= TEXT("MPEGAudio");  //TEXT"waveaudio"//设备类型，大多数文件可以这样设置
		//mciOpen.lpstrElementName=TEXT("D:\\2.mp3"); //文件路径
		mciOpen.wDeviceID=NULL;      //打开设备成功以后保存这个设备号备用
		UINT rs;        //接受函数返回结果
		//发送命令打开设备，成功返回0，否则返回错误号，第三个参数这里必须MCI_OPEN_ELEMENT  
		rs=mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_ELEMENT,(DWORD)&mciOpen);
		//mciSendCommand(NULL,MCI_SET,MCI_SET_DOOR_OPEN,NULL);
		//cdlen=getinfo(mciOpen.wDeviceID,MCI_STATUS_LENGTH);//获取音频文件长度
		if(rs==0)        //设备打开成功就播放文件
		{
			mpr_uDeviceID = mciOpen.wDeviceID;	

			mciSendCommand(mpr_uDeviceID,MCI_PLAY,MCI_NOTIFY,(DWORD)&mciPlay);
			//播放文件，如果第三个参数设为MCI_WAIT则程序窗口会被阻塞，为了避免这种情况可以设为MCI_NOTIFY

			// 设置媒体播放标志为true
			mpr_isMediaPlaying = true;
		}
	}
	else
	{
		mciOpen.lpstrDeviceType = TEXT("mpegvideo");  //使用这种类型，可以播放MP3
		dwRes = mciSendCommand(NULL , //指定设备标识,MCI此处为NULL,成功Open后标识放入MCI_OPEN_PARMS的wDeviceID变量中
			MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_SHAREABLE | MCI_WAIT, (DWORD)&mciOpen);

		if (dwRes!=0)
		{
			//if (extentname == L"wav")
			//{
			//	if (PlaySoundW(szFileDir.c_str(),NULL,isNeedPause ? SND_SYNC : SND_ASYNC))
			//	{
			//		mpr_isMediaPlaying = false;
			//		//继续漫游	
			//		Run();		
			//	}
			//}
			DisplayMCIErrorMsg(dwRes);
			return false;
		}

		mpr_uDeviceID = mciOpen.wDeviceID;

		RECT rc;
		GetWindowRect(GetDesktopWindow(),&rc);
		// 获取视频窗口大小
		MCI_DGV_WHERE_PARMS MCIWhere;   
		dwRes = mciSendCommand (mpr_uDeviceID, MCI_WHERE, MCI_DGV_WHERE_SOURCE,(DWORD)(LPVOID) &MCIWhere);
		if (dwRes != 0)
		{   
			DisplayMCIErrorMsg(dwRes);
		}   
		//if (_isMyWindowClosed == true)
		//	mpr_hWnd = MakeWindow();

		ShowWindow(mpr_hWnd,SW_SHOW);

		// 将视频窗口放到屏幕中央
		int sx = (rc.right-rc.left)/2 - (MCIWhere.rc.right-MCIWhere.rc.left)/2;
		int sy = (rc.bottom-rc.top)/2 - (MCIWhere.rc.bottom-MCIWhere.rc.top)/2;
		MoveWindow(mpr_hWnd,sx,sy,MCIWhere.rc.right-MCIWhere.rc.left,MCIWhere.rc.bottom-MCIWhere.rc.top,FALSE);  

		// 设置视频播放窗口
		MCI_DGV_WINDOW_PARMS window;
		window.dwCallback = NULL;
		window.hWnd = mpr_hWnd;
		window.nCmdShow = SW_SHOW;
		window.lpstrText = (LPWSTR) NULL;
		dwRes = mciSendCommand(mpr_uDeviceID,MCI_WINDOW,MCI_DGV_WINDOW_HWND,(DWORD)&window);
		if (dwRes !=0 )
		{
			DisplayMCIErrorMsg(dwRes);
		}

		//成功打开文件，将对应设备ID保存以供其它命令使用
		//播放文件，如果第三个参数设为MCI_WAIT则程序窗口会被阻塞，为了避免这种情况可以设为MCI_NOTIFY 
		mciPlay.dwCallback = (DWORD)mpr_hWnd;
		dwRes = mciSendCommand(mpr_uDeviceID,MCI_PLAY,MCI_NOTIFY,(DWORD)&mciPlay);		

		if (dwRes != 0) 
		{
			DisplayMCIErrorMsg(dwRes);
			return false;
		}
		// 设置媒体播放标志为true
		mpr_isMediaPlaying = true;
	}	

	return true;	
}

glbBool CGlbGlobeDynamicObject::MeidaPause(glbBool isPause)
{
	if (mpr_uDeviceID==-1)
		return true;
	MCI_PLAY_PARMS PlayParms;
	if (isPause)
	{
		//DWORD dwRes = mciSendString(L"pause sound", NULL, 0, NULL);
		//DWORD dwRes = mciSendString(TEXT("pausemedia"), NULL, 0, 0);  /*pause NOWMUSIC*/
		DWORD dwRes = mciSendCommand(mpr_uDeviceID,MCI_PAUSE,0,(DWORD)(LPVOID)&PlayParms);
		if (dwRes != 0) 
		{
			wchar_t buff[256];
			mciGetErrorString(dwRes,buff,256);
			return false;
		}
	}
	else
	{
		//DWORD dwRes = mciSendString(TEXT("resume NOWMUSIC"), NULL, 0, 0); 
		DWORD dwRes = mciSendCommand(mpr_uDeviceID,MCI_RESUME,0,(DWORD)&PlayParms);
		if (dwRes != 0) 
			return false;
	}
	return true;
}

glbBool CGlbGlobeDynamicObject::MediaStop()
{
	ShowWindow(mpr_hWnd,SW_HIDE);
	DWORD dwRes = mciSendCommand(mpr_uDeviceID,MCI_CLOSE,0,NULL);
	if (dwRes != 0) 
		return false;
	mpr_uDeviceID = -1;
	// 设置媒体播放标志为false
	mpr_isMediaPlaying = false;
	return true;
}													


void CGlbGlobeDynamicObject::initMediaMap()
{
	std::map<glbDouble,KeyPoint>::iterator itr = mpr_KeyMeida_map.begin();
	while(itr != mpr_KeyMeida_map.end())
	{// 从后往前遍历
		itr->second.mediaMarker = false;
		++itr;
	}
}
																		  
//////////////////////////////////////////////////////////////////////////
void CGlbGlobeDynamicObject::testDynamicObject(CGlbWString filepath)
{
	//filepath(L"D:\\in.pth");
	CGlbWString filepath2(L"D:\\out.pth");
	//读取路径文件
	read_3dmaxpath(filepath);
	//read_creatarpath(filepath);
	//绘制路径，线模型，待测式

	//载入模型，点模型
	glbref_ptr<GlbMarkerModelSymbolInfo> modelInfo = new GlbMarkerModelSymbolInfo;
	modelInfo->type = GLB_OBJECTTYPE_POINT;
	modelInfo->symType = GLB_MARKERSYMBOL_MODEL;
	modelInfo->xScale = new GlbRenderDouble;
	modelInfo->xScale->value = 5;
	modelInfo->yScale = new GlbRenderDouble;
	modelInfo->yScale->value = 5;
	modelInfo->zScale = new GlbRenderDouble;
	modelInfo->zScale->value = 5;
	modelInfo->yaw = new GlbRenderDouble;
	modelInfo->yaw->value = 0.0;
	modelInfo->pitch = new GlbRenderDouble;
	modelInfo->pitch->value = 0.0;
	modelInfo->roll = new GlbRenderDouble;
	modelInfo->roll->value = 0.0;
	modelInfo->locate = new GlbRenderString;
	modelInfo->locate->value = L"D:\\cessna.osg";
	modelInfo->opacity = new GlbRenderInt32;
	modelInfo->opacity->value = 100;
	glbBool isNeedDirectDraw;
	SetAltitudeMode(GLB_ALTITUDEMODE_ABSOLUTE,isNeedDirectDraw);	
	//SetPosition(116.3,39.9,100.0);
	SetRenderInfo(modelInfo.get(),isNeedDirectDraw);
	//DirectDraw(0);
	//pointObject->SetSelected(true);
	//pointObject->SetBlink(true);
	SetInViewPort(true);
	mpt_globe->AddObject(this);
	
	//开启动画回调
	SetRepeat(true);
	SetDynamicObjMode(GLB_DYNAMICMODE_MODEL);
	SetPlayMode(GLB_DYNAMICPLAYMODE_CAR);
	Run();
	//写入路径文件
	//write_3dmaxpath(filepath2);
}

void CGlbGlobeDynamicObject::writepath(CGlbWString fliepath,osg::AnimationPath::ControlPoint cp)
{	
	std::ofstream file(fliepath.ToString(),ios::out|ios::app); 
	if (!file)
		return;
	glbDouble x,y,z,yaw,pitch,roll; 
	ControlPointToPosition(cp,&x,&y,&z,&yaw,&pitch,&roll);
	file << yaw << ' ';
	file << pitch << ' ';
	file << roll << std::endl;
	file.close();
}

//定义窗口函数
LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM IParam
	)
{
	switch(uMsg)
	{
	case WM_CLOSE:
		//PostQuitMessage(0);
		{
			//CloseWindow(hwnd);				
			//hwnd = NULL;
			_isMyWindowClosed = true;
			ShowWindow(hwnd,SW_HIDE);
		}		
		break;
	case MCI_NOTIFY:			
		break;
	default:
		return DefWindowProc(hwnd,uMsg,wParam,IParam);
	}
	return 0;
}

HWND CGlbGlobeDynamicObject::MakeWindow()
{
	//HINSTANCE hInst= GetModuleHandle (L"GlbGlobeObject.dll");
	HINSTANCE hInst= GetModuleHandle (0);

	WNDCLASS wndclass;
	wndclass.lpfnWndProc=WindowProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpszClassName=L"我的窗体";
	wndclass.hInstance=hInst;
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hIcon=0;
	wndclass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName=0;
	//注册窗口类
	RegisterClass(&wndclass);	
	//创建窗口实列
	HWND hWnd = CreateWindow(L"我的窗体",L"视频窗口",WS_OVERLAPPEDWINDOW,100,100,500,500,0,0,hInst,0);
	//显示和更新窗口
	ShowWindow(hWnd,SW_HIDE);
	UpdateWindow(hWnd);

	_isMyWindowClosed = false;
	//消息循环
	//MSG msg;
	//while(GetMessage(&msg,0,0,0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	return hWnd;
}

void CGlbGlobeDynamicObject::DisplayMCIErrorMsg(DWORD dwError)
{
	//check if there was an error
	if(dwError)
	{
		//character string that contains error message
		wchar_t szErrorMsg[MAXERRORLENGTH];
		//retrieve string associated error message
		if(!mciGetErrorString(dwError,szErrorMsg,sizeof(szErrorMsg)))
				wcscpy(szErrorMsg,L"Unknown Error");
		
		//display error string in message box
		MessageBox(NULL,szErrorMsg,L"ERROR",MB_OK);
	}
}

void CGlbGlobeDynamicObject::SetStrartTime( glbDouble startTime )
{
	if (mpr_DynamicObjCallback.valid())
	{
		mpr_DynamicObjCallback->settimeOffset(startTime);
	}
}

void CGlbGlobeDynamicObject::SetAdditionalRotate(glbDouble yaw, glbDouble pitch, glbDouble roll)
{
	mpr_additionalYaw = yaw;
	mpr_additionalPitch = pitch;
	mpr_additionalRoll = roll;
}

void CGlbGlobeDynamicObject::GetAdditionalRotate(glbDouble& yaw, glbDouble& pitch,glbDouble& roll)
{
	yaw = mpr_additionalYaw;
	pitch = mpr_additionalPitch;
	roll = mpr_additionalRoll;
}
