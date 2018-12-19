/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/
#include "StdAfx.h"
#include "GlbGlobeOccluderGeometryEx.h"

#include <osg/Notify>
#include <osg/Geode>

#include <osg/CullStack>
#include <osg/CullingSet>
#include <osg/CullSettings>
#include <osg/Polytope>

#include <osg/io_utils>
#include <osg/TriangleFunctor>
#include <osg/TriangleIndexFunctor>
#include <osg/GL>
#include <osg/Timer>

#include <osg/GLBeginEndAdapter>

#include <algorithm>


using namespace GlbGlobe;

OccluderGeometry::OccluderGeometry()
{
}

OccluderGeometry::OccluderGeometry(const OccluderGeometry& oc, const osg::CopyOp& copyop):
    osg::Drawable(oc,copyop)
{

}


class CollectOccludersVisitor : public osg::NodeVisitor
{
public:
    CollectOccludersVisitor(OccluderGeometry* oc, osg::Matrix* matrix, float ratio):
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN),
        _oc(oc),
        _ratio(ratio)
    {
        if (matrix) pushMatrix(*matrix);
    }

    META_NodeVisitor("osgShadow","CollectOccludersVisitor")

    void apply(osg::Node& node)
    {
        if (node.getStateSet()) pushState(node.getStateSet());

        traverse(node);

        if (node.getStateSet()) popState();
    }

    void apply(osg::Transform& transform)
    {
        if (transform.getStateSet()) pushState(transform.getStateSet());

        osg::Matrix matrix;
        if (!_matrixStack.empty()) matrix = _matrixStack.back();

        transform.computeLocalToWorldMatrix(matrix,this);

        pushMatrix(matrix);

        traverse(transform);

        popMatrix();

        if (transform.getStateSet()) popState();
    }

    void apply(osg::Geode& geode)
    {
        if (geode.getStateSet()) pushState(geode.getStateSet());

        for(unsigned int i=0; i<geode.getNumDrawables(); ++i)
        {
            osg::Drawable* drawable = geode.getDrawable(i);

            if (drawable->getStateSet()) pushState(drawable->getStateSet());

            apply(geode.getDrawable(i));

            if (drawable->getStateSet()) popState();
        }

        if (geode.getStateSet()) popState();
    }

    void pushState(osg::StateSet* stateset)
    {
        osg::StateAttribute::GLModeValue prevBlendModeValue = _blendModeStack.empty() ? osg::StateAttribute::GLModeValue(osg::StateAttribute::INHERIT) : _blendModeStack.back();
        osg::StateAttribute::GLModeValue newBlendModeValue = stateset->getMode(GL_BLEND);

        if (!(newBlendModeValue & osg::StateAttribute::PROTECTED) &&
             (prevBlendModeValue & osg::StateAttribute::OVERRIDE) )
        {
            newBlendModeValue = prevBlendModeValue;
        }

        _blendModeStack.push_back(newBlendModeValue);
    }

    void popState()
    {
        _blendModeStack.pop_back();
    }

    void pushMatrix(osg::Matrix& matrix)
    {
        _matrixStack.push_back(matrix);
    }

    void popMatrix()
    {
        _matrixStack.pop_back();
    }

    void apply(osg::Drawable* drawable)
    {
        osg::StateAttribute::GLModeValue blendModeValue = _blendModeStack.empty() ? osg::StateAttribute::GLModeValue(osg::StateAttribute::INHERIT) : _blendModeStack.back();
        if (blendModeValue & osg::StateAttribute::ON)
        {
            // OSG_NOTICE<<"Ignoring transparent drawable."<<std::endl;
            return;
        }

        _oc->processGeometry(drawable, (_matrixStack.empty() ? 0 : &_matrixStack.back()), _ratio);

    }

protected:

    OccluderGeometry* _oc;

    typedef std::vector<osg::Matrix> MatrixStack;
    typedef std::vector<osg::StateAttribute::GLModeValue> ModeStack;

    float           _ratio;
    MatrixStack     _matrixStack;
    ModeStack       _blendModeStack;

};


class CollectOccludersVisitorEx : public osg::NodeVisitor, public osg::CullStack
{
public:
	CollectOccludersVisitorEx(OccluderGeometry* oc, osg::Matrixd* matrix, double ratio):
	  osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN),
		  _oc(oc),
		  _ratio(ratio)
	  {
		  if (matrix) pushMatrix(*matrix);
	  }

	  META_NodeVisitor("GlbGlobe","CollectOccludersVisitorEx")

		  void apply(osg::Node& node)
	  {
		  if (isCulled(node))
			  return;
		  //if (!_boundingPolytope.contains(node.getBound())) return ;

		  if (node.getStateSet()) pushState(node.getStateSet());

		  traverse(node);

		  if (node.getStateSet()) popState();
	  }

	  void apply(osg::Transform& transform)
	  {
		  if (isCulled(transform))
			  return;
		  //if (!_boundingPolytope.contains(transform.getBound())) return ;

		  if (transform.getStateSet()) pushState(transform.getStateSet());

		  osg::Matrixd matrix;
		  if (!_matrixStack.empty()) matrix = _matrixStack.back();

		  transform.computeLocalToWorldMatrix(matrix,this);

		  pushMatrix(matrix);

		  traverse(transform);

		  popMatrix();

		  if (transform.getStateSet()) popState();
	  }

	  void apply(osg::Geode& geode)
	  {
		  if (isCulled(geode))
			  return;
		  //if (!_boundingPolytope.contains(geode.getBound())) return ;

		  if (geode.getStateSet()) pushState(geode.getStateSet());

		  for(unsigned int i=0; i<geode.getNumDrawables(); ++i)
		  {
			  osg::Drawable* drawable = geode.getDrawable(i);

			  if (drawable->getStateSet()) pushState(drawable->getStateSet());

			  apply(geode.getDrawable(i));

			  if (drawable->getStateSet()) popState();
		  }

		  if (geode.getStateSet()) popState();
	  }

	  void pushState(osg::StateSet* stateset)
	  {
		  osg::StateAttribute::GLModeValue prevBlendModeValue = _blendModeStack.empty() ? osg::StateAttribute::GLModeValue(osg::StateAttribute::INHERIT) : _blendModeStack.back();
		  osg::StateAttribute::GLModeValue newBlendModeValue = stateset->getMode(GL_BLEND);

		  if (!(newBlendModeValue & osg::StateAttribute::PROTECTED) &&
			  (prevBlendModeValue & osg::StateAttribute::OVERRIDE) )
		  {
			  newBlendModeValue = prevBlendModeValue;
		  }

		  _blendModeStack.push_back(newBlendModeValue);
	  }

	  void popState()
	  {
		  _blendModeStack.pop_back();
	  }

	  void pushMatrix(osg::Matrixd& matrix)
	  {
		  _matrixStack.push_back(matrix);
	  }

	  void popMatrix()
	  {
		  _matrixStack.pop_back();
	  }

	  void apply(osg::Drawable* drawable)
	  {
		  osg::StateAttribute::GLModeValue blendModeValue = _blendModeStack.empty() ? osg::StateAttribute::GLModeValue(osg::StateAttribute::INHERIT) : _blendModeStack.back();
		  if (blendModeValue & osg::StateAttribute::ON)
		  {
			  // OSG_NOTICE<<"Ignoring transparent drawable."<<std::endl;
			  return;
		  }

		  _oc->processGeometry(drawable, (_matrixStack.empty() ? 0 : &_matrixStack.back()), _ratio);

	  }

	  virtual void reset()
	  {
		  CullStack::reset();
	  }

	  osg::Polytope _boundingPolytope;
protected:

	OccluderGeometry* _oc;



	typedef std::vector<osg::Matrixd> MatrixdStack;
	typedef std::vector<osg::StateAttribute::GLModeValue> ModeStack;

	double           _ratio;
	MatrixdStack     _matrixStack;
	ModeStack       _blendModeStack;

};


void OccluderGeometry::computeOccluderGeometry(osg::Node* subgraph, osg::Matrix* matrix, float sampleRatio)
{
    OSG_NOTICE<<"computeOccluderGeometry(osg::Node* subgraph, float sampleRatio)"<<std::endl;

    osg::Timer_t startTick = osg::Timer::instance()->tick();

    CollectOccludersVisitor cov(this, matrix, sampleRatio);
    subgraph->accept(cov);

    setUpInternalStructures();

    osg::Timer_t endTick = osg::Timer::instance()->tick();

    OSG_NOTICE<<"done in "<<osg::Timer::instance()->delta_m(startTick, endTick)<<" ms"<<std::endl;
}

void OccluderGeometry::computeOccluderGeometryEx(osg::Node* subgraph, osg::Matrixd* matrix, double sampleRatio)
{
	OSG_NOTICE<<"computeOccluderGeometry(osg::Node* subgraph, double sampleRatio)"<<std::endl;

	osg::Timer_t startTick = osg::Timer::instance()->tick();

	CollectOccludersVisitorEx cov(this, matrix, sampleRatio);
	cov.setTraversalMask(~64);
	cov.inheritCullSettings(osg::CullSettings());
	//cullVisitor->setRenderInfo( _renderInfo );
	cov.reset();
	//cov.setTraversalMask(~_viewScene->mpr_viewMask);
	cov.pushViewport(new osg::Viewport(0,0,100,100));
	osg::RefMatrixd *refMat=new osg::RefMatrixd();
	cov.pushProjectionMatrix(refMat);
	cov.pushModelViewMatrix(refMat,osg::Transform::ABSOLUTE_RF);

	osg::CullingSet &cullset= cov.getCurrentCullingSet();
	cullset.setFrustum(_boundingPolytope);
	//cov.getCurrentCullingSet.setFrustum(_boundingPolytope);


	cov._boundingPolytope=this->_boundingPolytope;
	subgraph->accept(cov);

	setUpInternalStructures();

	osg::Timer_t endTick = osg::Timer::instance()->tick();

	OSG_NOTICE<<"done in "<<osg::Timer::instance()->delta_m(startTick, endTick)<<" ms"<<std::endl;
}

void OccluderGeometry::computeOccluderGeometry(osg::Drawable* drawable, osg::Matrix* matrix, float sampleRatio)
{
    processGeometry(drawable, matrix, sampleRatio);

    setUpInternalStructures();
}

struct TriangleCollector
{
    OccluderGeometry::Vec3List* _vertices;
    OccluderGeometry::UIntList* _triangleIndices;
    osg::Matrix*                _matrix;

    typedef std::vector<const osg::Vec3*> VertexPointers;
    VertexPointers _vertexPointers;

    OccluderGeometry::Vec3List _tempoaryTriangleVertices;

    TriangleCollector():_matrix(0) { }

    void set(OccluderGeometry::Vec3List* vertices, OccluderGeometry::UIntList* triangleIndices, osg::Matrix* matrix)
    {
        _vertices = vertices;
        _triangleIndices = triangleIndices;
        _matrix = matrix;
    }


    //   bool intersect(const Vec3& v1,const Vec3& v2,const Vec3& v3,float& r)
    inline void operator () (const osg::Vec3& v1,const osg::Vec3& v2,const osg::Vec3& v3, bool treatVertexDataAsTemporary)
    {
        if (treatVertexDataAsTemporary)
        {
            // OSG_NOTICE<<"Triangle temp ("<<v1<<") ("<<v2<<") ("<<v3<<")"<<std::endl;
            _tempoaryTriangleVertices.push_back(v1);
            _tempoaryTriangleVertices.push_back(v2);
            _tempoaryTriangleVertices.push_back(v3);

        }
        else
        {
            // OSG_NOTICE<<"Triangle ("<<v1<<") ("<<v2<<") ("<<v3<<")"<<std::endl;
            _vertexPointers.push_back(&v1);
            _vertexPointers.push_back(&v2);
            _vertexPointers.push_back(&v3);
        }

    }

    void copyToLocalData()
    {
        if ((_vertexPointers.size()+_tempoaryTriangleVertices.size())<3) return;


        const osg::Vec3* minVertex = _vertexPointers.empty() ? 0 : _vertexPointers.front();
        const osg::Vec3* maxVertex = _vertexPointers.empty() ? 0 : _vertexPointers.front();

        VertexPointers::iterator itr;
        for(itr = _vertexPointers.begin();
            itr != _vertexPointers.end();
            ++itr)
        {
            if (*itr < minVertex) minVertex = *itr;
            if (*itr > maxVertex) maxVertex = *itr;
        }

        unsigned int base = _vertices->size();
        unsigned int numberNewVertices = _vertexPointers.empty() ? 0 : (maxVertex - minVertex) + 1;

        // OSG_NOTICE<<"base = "<<base<<" numberNewVertices="<<numberNewVertices<<std::endl;

        _vertices->resize(base + numberNewVertices + _tempoaryTriangleVertices.size());

        for(itr = _vertexPointers.begin();
            itr != _vertexPointers.end();
            ++itr)
        {
            const osg::Vec3* vec = *itr;
            unsigned int index = base + (vec - minVertex);
            (*_vertices)[index] = *vec;
            _triangleIndices->push_back(index);
        }


        unsigned int pos = base + numberNewVertices;
        for(OccluderGeometry::Vec3List::iterator vitr = _tempoaryTriangleVertices.begin();
             vitr != _tempoaryTriangleVertices.end();
             ++vitr, ++pos)
        {
            (*_vertices)[pos] = *vitr;
            _triangleIndices->push_back(pos);
        }

        if (_matrix)
        {
            for(unsigned int i=base; i<_vertices->size(); ++i)
            {
                (*_vertices)[i] = (*_vertices)[i] * (*_matrix);
            }
        }

    }


};
typedef osg::TriangleFunctor<TriangleCollector> TriangleCollectorFunctor;

void OccluderGeometry::processGeometry(osg::Drawable* drawable, osg::Matrix* matrix, float /*sampleRatio*/)
{
    // OSG_NOTICE<<"computeOccluderGeometry(osg::Node* subgraph, float sampleRatio)"<<std::endl;

    TriangleCollectorFunctor tc;
    tc.set(&_vertices, &_triangleIndices, matrix);

    drawable->accept(tc);

    tc.copyToLocalData();

#if 0
    for(Vec3List::iterator vitr = _vertices.begin();
        vitr != _vertices.end();
        ++vitr)
    {
        OSG_NOTICE<<"v "<<*vitr<<std::endl;
    }

    for(UIntList::iterator titr = _triangleIndices.begin();
        titr != _triangleIndices.end();
        )
    {
        OSG_NOTICE<<"t "<<*titr++<<" "<<*titr++<<" "<<*titr++<<std::endl;
    }
#endif
}

void OccluderGeometry::setUpInternalStructures()
{

    osg::Timer_t t0 = osg::Timer::instance()->tick();

    removeDuplicateVertices();

    osg::Timer_t t1 = osg::Timer::instance()->tick();

    removeNullTriangles();

    osg::Timer_t t2 = osg::Timer::instance()->tick();

    computeNormals();

    osg::Timer_t t3 = osg::Timer::instance()->tick();

    buildEdgeMaps();

    osg::Timer_t t4 = osg::Timer::instance()->tick();


    OSG_NOTICE<<"removeDuplicateVertices "<<osg::Timer::instance()->delta_m(t0,t1)<<" ms"<<std::endl;
    OSG_NOTICE<<"removeNullTriangles "<<osg::Timer::instance()->delta_m(t1,t2)<<" ms"<<std::endl;
    OSG_NOTICE<<"computeNormals "<<osg::Timer::instance()->delta_m(t2,t3)<<" ms"<<std::endl;
    OSG_NOTICE<<"buildEdgeMaps "<<osg::Timer::instance()->delta_m(t3,t4)<<" ms"<<std::endl;
    OSG_NOTICE<<"setUpInternalStructures "<<osg::Timer::instance()->delta_m(t0,t4)<<" ms"<<std::endl;


    dirtyBound();

    dirtyDisplayList();
}

struct IndexVec3PtrPair
{
    IndexVec3PtrPair():
        vec(0),
        index(0) {}

    IndexVec3PtrPair(const osg::Vec3* v, unsigned int i):
        vec(v),
        index(i) {}

    inline bool operator < (const IndexVec3PtrPair& rhs) const
    {
        return *vec < *rhs.vec;
    }

    inline bool operator == (const IndexVec3PtrPair& rhs) const
    {
        return *vec == *rhs.vec;

//        return (*vec - *rhs.vec).length2() < 1e-2;

    }

    const osg::Vec3* vec;
    unsigned int index;
};


void OccluderGeometry::removeDuplicateVertices()
{
    if (_vertices.empty()) return;

    OSG_NOTICE<<"OccluderGeometry::removeDuplicates() before = "<<_vertices.size()<<std::endl;

    typedef std::vector<IndexVec3PtrPair> IndexVec3PtrPairs;
    IndexVec3PtrPairs indexVec3PtrPairs;
    indexVec3PtrPairs.reserve(_vertices.size());

    unsigned int i = 0;
    for(OccluderGeometry::Vec3List::iterator vitr = _vertices.begin();
         vitr != _vertices.end();
         ++vitr, ++i)
    {
        indexVec3PtrPairs.push_back(IndexVec3PtrPair(&(*vitr),i));
    }
    std::sort(indexVec3PtrPairs.begin(),indexVec3PtrPairs.end());;


    // compute size
    IndexVec3PtrPairs::iterator prev = indexVec3PtrPairs.begin();
    IndexVec3PtrPairs::iterator curr = prev;
    ++curr;

    unsigned int numDuplicates = 0;
    unsigned int numUnique = 1;

    for(; curr != indexVec3PtrPairs.end(); ++curr)
    {
        if (*prev==*curr)
        {
            ++numDuplicates;
        }
        else
        {
            prev = curr;
            ++numUnique;
        }
    }

    OSG_NOTICE<<"Num diplicates = "<<numDuplicates<<std::endl;
    OSG_NOTICE<<"Num unique = "<<numUnique<<std::endl;

    if (numDuplicates==0) return;

    // now assign the unique Vec3 to the newVertices arrays
    typedef std::vector<unsigned int> IndexMap;
    IndexMap indexMap(indexVec3PtrPairs.size());

    Vec3List newVertices;
    newVertices.reserve(numUnique);
    unsigned int index = 0;

    prev = indexVec3PtrPairs.begin();
    curr = prev;

    // add first vertex
    indexMap[curr->index] = index;
    newVertices.push_back(*(curr->vec));

    ++curr;

    for(; curr != indexVec3PtrPairs.end(); ++curr)
    {
        if (*prev==*curr)
        {
            indexMap[curr->index] = index;
        }
        else
        {
            ++index;

            // add new unique vertex
            indexMap[curr->index] = index;
            newVertices.push_back(*(curr->vec));

            prev = curr;
        }
    }

    // copy over need arrays and index values
    _vertices.swap(newVertices);

    for(UIntList::iterator titr = _triangleIndices.begin();
        titr != _triangleIndices.end();
        ++titr)
    {
        *titr = indexMap[*titr];
    }

    // OSG_NOTICE<<"OccluderGeometry::removeDuplicates() after = "<<_vertices.size()<<std::endl;
}

void OccluderGeometry::removeNullTriangles()
{
    // OSG_NOTICE<<"OccluderGeometry::removeNullTriangles()"<<std::endl;


    UIntList::iterator lastValidItr = _triangleIndices.begin();
    for(UIntList::iterator titr = _triangleIndices.begin();
        titr != _triangleIndices.end();
        )
    {
        UIntList::iterator currItr = titr;
        GLuint p1 = *titr++;
        GLuint p2 = *titr++;
        GLuint p3 = *titr++;
        if ((p1 != p2) && (p1 != p3) && (p2 != p3))
        {
            if (lastValidItr!=currItr)
            {
                *lastValidItr++ = p1;
                *lastValidItr++ = p2;
                *lastValidItr++ = p3;
            }
            else
            {
                lastValidItr = titr;
            }
        }
        else
        {
            // OSG_NOTICE<<"Null triangle"<<std::endl;
        }
    }
    if (lastValidItr != _triangleIndices.end())
    {
        // OSG_NOTICE<<"Pruning end - before "<<_triangleIndices.size()<<std::endl;
        _triangleIndices.erase(lastValidItr,_triangleIndices.end());
        // OSG_NOTICE<<"Pruning end - after "<<_triangleIndices.size()<<std::endl;
    }
}

void OccluderGeometry::computeNormals()
{
    // OSG_NOTICE<<"OccluderGeometry::computeNormals()"<<std::endl;

    unsigned int numTriangles = _triangleIndices.size() / 3;
    unsigned int redundentIndices = _triangleIndices.size() - numTriangles * 3;
    if (redundentIndices)
    {
        OSG_NOTICE<<"Warning OccluderGeometry::computeNormals() has found redundent trailing indices"<<std::endl;
        _triangleIndices.erase(_triangleIndices.begin() + numTriangles * 3, _triangleIndices.end());
    }

    _triangleNormals.clear();
    _triangleNormals.reserve(numTriangles);

    _normals.resize(_vertices.size());


    for(UIntList::iterator titr = _triangleIndices.begin();
        titr != _triangleIndices.end();
        )
    {
        GLuint p1 = *titr++;
        GLuint p2 = *titr++;
        GLuint p3 = *titr++;
        osg::Vec3 normal = (_vertices[p2] - _vertices[p1]) ^ (_vertices[p3] - _vertices[p2]);
        normal.normalize();

        _triangleNormals.push_back(normal);

        if (!_normals.empty())
        {
            _normals[p1] += normal;
            _normals[p2] += normal;
            _normals[p3] += normal;
        }
    }

    for(Vec3List::iterator nitr = _normals.begin();
        nitr != _normals.end();
        ++nitr)
    {
        nitr->normalize();
    }


}

void OccluderGeometry::buildEdgeMaps()
{
    // OSG_NOTICE<<"OccluderGeometry::buildEdgeMaps()"<<std::endl;

    typedef std::set<Edge> EdgeSet;
    EdgeSet edgeSet;

    unsigned int numTriangleErrors = 0;
    unsigned int triNo=0;
    for(UIntList::iterator titr = _triangleIndices.begin();
        titr != _triangleIndices.end();
        ++triNo)
    {
        GLuint p1 = *titr++;
        GLuint p2 = *titr++;
        GLuint p3 = *titr++;

        {
            Edge edge12(p1,p2);
            EdgeSet::iterator itr = edgeSet.find(edge12);
            if (itr == edgeSet.end())
            {
                if (!edge12.addTriangle(triNo)) ++numTriangleErrors;
                edgeSet.insert(edge12);
            }
            else
            {
                if (!itr->addTriangle(triNo)) ++numTriangleErrors;
            }
        }

        {
            Edge edge23(p2,p3);
            EdgeSet::iterator itr = edgeSet.find(edge23);
            if (itr == edgeSet.end())
            {
                if (!edge23.addTriangle(triNo)) ++numTriangleErrors;
                edgeSet.insert(edge23);
            }
            else
            {
                if (!itr->addTriangle(triNo)) ++numTriangleErrors;
            }
        }

        {
            Edge edge31(p3,p1);
            EdgeSet::iterator itr = edgeSet.find(edge31);
            if (itr == edgeSet.end())
            {
                if (!edge31.addTriangle(triNo)) ++numTriangleErrors;

                edgeSet.insert(edge31);
            }
            else
            {
                if (!itr->addTriangle(triNo)) ++numTriangleErrors;
            }
        }
    }

    _edges.clear();
    _edges.reserve(edgeSet.size());

    unsigned int numEdgesWithNoTriangles = 0;
    unsigned int numEdgesWithOneTriangles = 0;
    unsigned int numEdgesWithTwoTriangles = 0;

    for(EdgeSet::iterator eitr = edgeSet.begin();
        eitr != edgeSet.end();
        ++eitr)
    {
        const Edge& edge = *eitr;
        osg::Vec3 pos(0.0,0.0,0.0);
        osg::Vec3 mid = (_vertices[edge._p1] + _vertices[edge._p2]) * 0.5f;
        unsigned int numTriangles = 0;
        if (edge._t1>=0)
        {
            ++numTriangles;

            GLuint p1 = _triangleIndices[edge._t1*3];
            GLuint p2 = _triangleIndices[edge._t1*3+1];
            GLuint p3 = _triangleIndices[edge._t1*3+2];
            GLuint opposite = p1;
            if (p1 != edge._p1 && p1 != edge._p2) opposite = p1;
            else if (p2 != edge._p1 && p2 != edge._p2) opposite = p2;
            else if (p3 != edge._p1 && p3 != edge._p2) opposite = p3;
            pos = _vertices[opposite];
        }

        if (edge._t2>=0)
        {
            ++numTriangles;

            GLuint p1 = _triangleIndices[edge._t2*3];
            GLuint p2 = _triangleIndices[edge._t2*3+1];
            GLuint p3 = _triangleIndices[edge._t2*3+2];
            GLuint opposite = p1;
            if (p1 != edge._p1 && p1 != edge._p2) opposite = p1;
            else if (p2 != edge._p1 && p2 != edge._p2) opposite = p2;
            else if (p3 != edge._p1 && p3 != edge._p2) opposite = p3;
            pos += _vertices[opposite];
        }

        switch(numTriangles)
        {
            case(0):
                ++numEdgesWithNoTriangles;
                edge._normal.set(0.0,0.0,0.0);
                OSG_NOTICE<<"Warning no triangles on edge."<<std::endl;
                break;
            case(1):
                ++numEdgesWithOneTriangles;
                edge._normal = pos - mid;
                edge._normal.normalize();
                break;
            default:
                ++numEdgesWithTwoTriangles;
                edge._normal = (pos*0.5f) - mid;
                edge._normal.normalize();
                break;
        }

        _edges.push_back(edge);
    }

#if 0
    OSG_NOTICE<<"Num of indices "<<_triangleIndices.size()<<std::endl;
    OSG_NOTICE<<"Num of triangles "<<triNo<<std::endl;
    OSG_NOTICE<<"Num of numTriangleErrors "<<numTriangleErrors<<std::endl;
    OSG_NOTICE<<"Num of edges "<<edgeSet.size()<<std::endl;
    OSG_NOTICE<<"Num of numEdgesWithNoTriangles "<<numEdgesWithNoTriangles<<std::endl;
    OSG_NOTICE<<"Num of numEdgesWithOneTriangles "<<numEdgesWithOneTriangles<<std::endl;
    OSG_NOTICE<<"Num of numEdgesWithTwoTriangles "<<numEdgesWithTwoTriangles<<std::endl;
#endif
}


void OccluderGeometry::computeLightDirectionSilhouetteEdges(const osg::Vec3& lightdirection, UIntList& silhouetteIndices) const
{
    silhouetteIndices.clear();

    for(EdgeList::const_iterator eitr = _edges.begin();
        eitr != _edges.end();
        ++eitr)
    {
        const Edge& edge = *eitr;
        if (isLightDirectionSilhouetteEdge(lightdirection,edge))
        {
            const osg::Vec3& v1 = _vertices[edge._p1];
            const osg::Vec3& v2 = _vertices[edge._p2];
            osg::Vec3 normal = (v2-v1) ^ lightdirection;
            if (normal * edge._normal > 0.0)
            {
                silhouetteIndices.push_back(edge._p1);
                silhouetteIndices.push_back(edge._p2);
            }
            else
            {
                silhouetteIndices.push_back(edge._p2);
                silhouetteIndices.push_back(edge._p1);
            }
        }
    }
}

void OccluderGeometry::computeLightPositionSilhouetteEdges(const osg::Vec3& lightpos, UIntList& silhouetteIndices) const
{
    silhouetteIndices.clear();

    for(EdgeList::const_iterator eitr = _edges.begin();
        eitr != _edges.end();
        ++eitr)
    {
        const Edge& edge = *eitr;
        if (isLightPointSilhouetteEdge(lightpos,edge))
        {
            const osg::Vec3& v1 = _vertices[edge._p1];
            const osg::Vec3& v2 = _vertices[edge._p2];
            osg::Vec3 normal = (v2-v1) ^ (v1-lightpos);
            if (normal * edge._normal > 0.0)
            {
                silhouetteIndices.push_back(edge._p1);
                silhouetteIndices.push_back(edge._p2);
            }
            else
            {
                silhouetteIndices.push_back(edge._p2);
                silhouetteIndices.push_back(edge._p1);
            }

        }
    }
}


void OccluderGeometry::computeShadowVolumeGeometry(const osg::Vec4& lightpos, ShadowVolumeGeometry& svg) const
{
    // osg::Timer_t t0 = osg::Timer::instance()->tick();

    ShadowVolumeGeometry::Vec3List& shadowVertices = svg.getVertices();
    shadowVertices.clear();

    ShadowVolumeGeometry::Vec3List& shadowNormals = svg.getNormals();
    shadowNormals.clear();


    // need to have some kind of handling of case when no planes exist.
    if (_boundingPolytope.getPlaneList().empty())
    {
        OSG_NOTICE<<"Warning: no bounding polytope registered with OccluderGeometry."<<std::endl;
        return;
    }


    if (lightpos.w()==0.0)
    {
        // directional light.
        osg::Vec3 lightdirection( -lightpos.x(), -lightpos.y(), -lightpos.z());

        // OSG_NOTICE<<"Directional light"<<std::endl;

        // choose the base plane
        const osg::Polytope::PlaneList& planes = _boundingPolytope.getPlaneList();
        osg::Polytope::PlaneList::const_iterator pitr = planes.begin();
        osg::Plane basePlane = *pitr;
        ++pitr;

        for(;
            pitr != planes.end();
            ++pitr)
        {
            if (basePlane.dotProductNormal(lightdirection)  > pitr->dotProductNormal(lightdirection))
            {
                basePlane = *pitr;
            }
        }

        // compute the silhouette edge
        UIntList silhouetteIndices;
        computeLightDirectionSilhouetteEdges(lightdirection, silhouetteIndices);

        osg::Vec3 offset( lightdirection*5.0f );

        float directionScale = 1.0f / basePlane.dotProductNormal(lightdirection);

        for(UIntList::iterator itr = silhouetteIndices.begin();
            itr != silhouetteIndices.end();
            )
        {
            const osg::Vec3& v1 = _vertices[*itr++];
            const osg::Vec3& v2 = _vertices[*itr++];

            float r1 = basePlane.distance(v1) * directionScale;
            float r2 = basePlane.distance(v2) * directionScale;

            osg::Vec3 v1_projected = v1 - (lightdirection * r1);
            osg::Vec3 v2_projected = v2 - (lightdirection * r2);

            shadowVertices.push_back( v1);
            shadowVertices.push_back( v1_projected);
            shadowVertices.push_back( v2_projected);
            shadowVertices.push_back( v2);

            osg::Vec3 normal = lightdirection ^ (v2-v1);
            normal.normalize();
            shadowNormals.push_back(normal);
            shadowNormals.push_back(normal);
            shadowNormals.push_back(normal);
            shadowNormals.push_back(normal);
        }

    }
    else
    {
        // positional light
        osg::Vec3 lightposition( lightpos.x(), lightpos.y(), lightpos.z());

        osg::Plane basePlane(0.0, 0.0, 1.0, 0.0);

        // OSG_NOTICE<<"Positional light"<<std::endl;
        UIntList silhouetteIndices;
        computeLightPositionSilhouetteEdges(lightposition, silhouetteIndices);

        // OSG_NOTICE<<"basePlane "<<basePlane[0]<<" "<<basePlane[1]<<" "<<basePlane[2]<<" "<<basePlane[3]<<std::endl;
        // OSG_NOTICE<<"lightpos  = "<<std::endl;
        const osg::Polytope::PlaneList& planes = _boundingPolytope.getPlaneList();

        for(UIntList::iterator itr = silhouetteIndices.begin();itr != silhouetteIndices.end();/*++itr*/)
        {
            const osg::Vec3& v1 = _vertices[*itr++];
            const osg::Vec3& v2 = _vertices[*itr++];

            osg::Vec3 d1 = v1 - lightposition;
            osg::Vec3 d2 = v2 - lightposition;

            osg::Polytope::PlaneList::const_iterator pitr = planes.begin();
            float r1 = - pitr->distance(v1) / (pitr->dotProductNormal(d1));
            float r2 = - pitr->distance(v2) / (pitr->dotProductNormal(d2));
            ++pitr;

            for(;pitr != planes.end();++pitr)
            {
                float lr1 = - pitr->distance(v1) / (pitr->dotProductNormal(d1));
                float lr2 = - pitr->distance(v2) / (pitr->dotProductNormal(d2));

                if (lr1>=0.0f && lr2>=0.0f && (lr1+lr2)<(r1+r2))
                {
                    r1 = lr1;
                    r2 = lr2;
                }
            }

            osg::Vec3 v1_projected = v1 + (d1 * r1);
            osg::Vec3 v2_projected = v2 + (d2 * r2);

            shadowVertices.push_back( v1);
            shadowVertices.push_back( v1_projected);
            shadowVertices.push_back( v2_projected);
            shadowVertices.push_back( v2);

            osg::Vec3 normal = d1 ^ (v2-v1);
            normal.normalize();
            shadowNormals.push_back(normal);
            shadowNormals.push_back(normal);
            shadowNormals.push_back(normal);
            shadowNormals.push_back(normal);
        }

    }

    svg.dirtyDisplayList();
    svg.dirtyBound();

    // osg::Timer_t t1 = osg::Timer::instance()->tick();
    // OSG_NOTICE<<"computeShadowVolumeGeometry "<<osg::Timer::instance()->delta_m(t0,t1)<<" ms"<<std::endl;
}

void OccluderGeometry::drawImplementation(osg::RenderInfo& renderInfo) const
{
    renderInfo.getState()->disableAllVertexArrays();

    renderInfo.getState()->setVertexPointer( 3, GL_FLOAT, 0, &(_vertices.front()) );

    if (!_normals.empty())
    {
        renderInfo.getState()->setNormalPointer( GL_FLOAT, 0, &(_normals.front()) );
    }

    if (!_triangleIndices.empty())
    {
        glDrawElements(GL_TRIANGLES, _triangleIndices.size(), GL_UNSIGNED_INT, &(_triangleIndices.front()) );
    }
}

osg::BoundingBox OccluderGeometry::computeBoundingBox() const
{
    osg::BoundingBox bb;
    for(Vec3List::const_iterator itr =  _vertices.begin();
        itr != _vertices.end();
        ++itr)
    {
        bb.expandBy(*itr);
    }

    return bb;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShadowVolumeGeometry
//
ShadowVolumeGeometry::ShadowVolumeGeometry():
    _drawMode(GEOMETRY)
{
}

ShadowVolumeGeometry::ShadowVolumeGeometry(const ShadowVolumeGeometry& oc, const osg::CopyOp& copyop):
    osg::Drawable(oc,copyop)
{
}


	void ScreenToWorld(double &sx,double &sy,double &sz,osg::RenderInfo& renderInfo)
	{
		osg::Camera *camera = renderInfo.getCurrentCamera();
		osg::Matrixd VPW = camera->getViewMatrix() * 
			camera->getProjectionMatrix() * 
			camera->getViewport()->computeWindowMatrix();
		osg::Matrixd inverseVPW;
		inverseVPW.invert(VPW);
		osg::Vec3d world = osg::Vec3d(sx,sy,sz) * inverseVPW;
		sx = world.x();
		sy = world.y();
		sz = world.z();
	}

	void OthoView()
	{
		glMatrixMode(GL_PROJECTION);							
		glPushMatrix();											
		glLoadIdentity();										
		glOrtho( 0, 1920, 1080, 0, -1, 1 );
		glMatrixMode(GL_MODELVIEW);									
		glPushMatrix();											
		glLoadIdentity();
	}

	void PerpectiveView()
	{
		glMatrixMode( GL_PROJECTION );							
		glPopMatrix();											
		glMatrixMode( GL_MODELVIEW );						
		glPopMatrix();
	}

void ShadowVolumeGeometry::drawImplementation(osg::RenderInfo& renderInfo) const
{
	DrawMode *drawMode=(DrawMode *)&_drawMode;
	//*drawMode=STENCIL_TWO_PASS;
    if (_drawMode==GEOMETRY)
    {
        osg::State* state = renderInfo.getState();

        state->disableAllVertexArrays();

        state->setVertexPointer( 3, GL_FLOAT, 0, &(_vertices.front()) );

        if (!_normals.empty())
        {
            state->setNormalPointer( GL_FLOAT, 0, &(_normals.front()) );
        }
        else
        {
            state->Normal(0.0f, 0.0f, 0.0f);
        }


        state->Color(0.5f, 1.0f, 1.0f, 1.0f);

        glDrawArrays( GL_QUADS, 0, _vertices.size() );
    }
    else if (_drawMode==STENCIL_TWO_PASS)
    {
        osg::State* state = renderInfo.getState();

        state->disableAllVertexArrays();
        state->setVertexPointer( 3, GL_FLOAT, 0, &(_vertices.front()) );

        // draw front faces of shadow volume
        glCullFace(GL_BACK);
        glStencilOp( GL_KEEP, GL_KEEP, GL_INCR);

        glDrawArrays( GL_QUADS, 0, _vertices.size() );

        // draw back faces of shadow volume
        glCullFace(GL_FRONT);
        glStencilOp( GL_KEEP, GL_KEEP, GL_DECR);

        glDrawArrays( GL_QUADS, 0, _vertices.size() );

        state->haveAppliedAttribute(osg::StateAttribute::CULLFACE);
        state->haveAppliedAttribute(osg::StateAttribute::STENCIL);

    }
    else // stencil two sided, note state all set up separately.
    {
        osg::State* state = renderInfo.getState();

        state->disableAllVertexArrays();
        state->setVertexPointer( 3, GL_FLOAT, 0, &(_vertices.front()) );

        glDrawArrays( GL_QUADS, 0, _vertices.size() );
    }

	//return;
	{
		osg::State* state = renderInfo.getState();
		//osg::GLBeginEndAdapter& gl = state->getGLBeginEndAdapter();

		osg::Vec3d leftUp(0,0,0.5);
		osg::Vec3d leftdown(0,1080,0.5);
		osg::Vec3d rightdown(1920,1080,0.5);
		osg::Vec3d rightup(1920,0,0.5);

		/*ScreenToWorld(leftUp.x(),leftUp.y(),leftUp.z(),renderInfo);
		ScreenToWorld(leftdown.x(),leftdown.y(),leftdown.z(),renderInfo);
		ScreenToWorld(rightdown.x(),rightdown.y(),rightdown.z(),renderInfo);
		ScreenToWorld(rightup.x(),rightup.y(),rightup.z(),renderInfo);*/

		glStencilFunc(GL_NOTEQUAL,0,0xFFFFFFFF);
		glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDepthMask((GLboolean)0);
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE ); 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		//glBlendFunc(GL_ONE, GL_ZERO); 

		//gl.Begin(GL_QUADS);
		OthoView();//自定义，和接下来的PerpectiveView()配合
		glColor4f(0.8,0.0,0.0,0.5);
		glRectf(0.0, 0.0, 1920, 1080);
		PerpectiveView;//转回透视视图

		/*gl.Normal3f(0.0f,0.0f,1.0f);

		gl.Color4f(0.5f,0.0f,0.0f,0.5);
		gl.TexCoord2f(0.0f,1.0f);
		gl.Vertex3f(leftUp.x(),leftUp.y(),leftUp.z());
		gl.Color4f(0.5f,0.0f,0.0f,0.5);

		gl.TexCoord2f(0.0f,0.0f);
		gl.Vertex3f(leftdown.x(),leftdown.y(),leftdown.z());
		gl.Color4f(0.5f,0.0f,0.0f,0.5);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(rightdown.x(),rightdown.y(),rightdown.z());
		gl.Color4f(0.5f,0.0f,0.0f,0.5);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(rightup.x(),rightup.y(),rightup.z());*/

		//gl.End();
	}
}

osg::BoundingBox ShadowVolumeGeometry::computeBoundingBox() const
{
    osg::BoundingBox bb;
    for(Vec3List::const_iterator itr =  _vertices.begin();
        itr != _vertices.end();
        ++itr)
    {
        bb.expandBy(*itr);
    }

    return bb;
}
