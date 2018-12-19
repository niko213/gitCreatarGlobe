/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbFindNamedNodeVisitor.h
  * @brief   Node节点查找类
  *
  * 这个档案定义CGlbFindNamedNodeVisitor这个class,负责找到node下所有名字为name的节点
  *
  * @version 1.0
  * @author  马林
  * @date    2018-5-28 16:40
*********************************************************************/
#include <osg/NodeVisitor>

#pragma once
namespace GlbGlobe
{
	class CGlbFindNamedNodeVisitor: public osg::NodeVisitor
	{
	public:
		CGlbFindNamedNodeVisitor(const std::string& name):
		  osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
			  _name(name) {}

		  ~CGlbFindNamedNodeVisitor(void);

		  virtual void apply(osg::Node& node)
		  {
			  if (node.getName()==_name)
			  {
				  _foundNodes.push_back(&node);
			  }
			  traverse(node);
		  }		 

		  typedef std::vector< osg::ref_ptr<osg::Node> > NodeList;

		  void getNodeList(NodeList& nodes)
		  {
			  nodes = _foundNodes;
		  }

		  std::string _name;
		  NodeList _foundNodes;
	};

	//使用方法
	//FindNamedNodeVisitor fnnv("Earth");
	//root->accept(fnnv);
	//if (!fnnv._foundNodes.empty())
	//osg::Node* node = fnnv._foundNodes.front().get() 
}

