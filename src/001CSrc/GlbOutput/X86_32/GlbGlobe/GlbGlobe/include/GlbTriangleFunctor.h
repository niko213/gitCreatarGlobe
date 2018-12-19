/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbTriangleFunctor.h
* @brief   碰撞检测
*           
* @version 1.0
* @author  xt
* @date    2016-7-28 10:35
*********************************************************************/
#include <osg/TriangleFunctor>

namespace Glb_Collision
{
	/*
	GLb_TriangleFunctor:该类参考OSG TriangleFunctor
	来获取drawable的三角面片上的点
*/
template<class T>
class GLB_DLLCLASS_EXPORT GLb_TriangleFunctor : public osg::PrimitiveFunctor, public T
{
public:

    GLb_TriangleFunctor()
    {
        _vertexArraySize=0;
        _vertexArrayPtr=0;
        _modeCache=0;
        _treatVertexDataAsTemporary=false;
    }

    virtual ~GLb_TriangleFunctor() {}

	void setLocalToWorld(const osg::Matrixd&m) {localToWorld = m;}

    void setTreatVertexDataAsTemporary(bool treatVertexDataAsTemporary)
			{ localToWorld=treatVertexDataAsTemporary; }

    bool getTreatVertexDataAsTemporary() const { return localToWorld; }

    virtual void setVertexArray(unsigned int,const osg::Vec2*)
    {
        osg::notify(osg::WARN)<<"Triangle Functor does not support Vec2* vertex arrays"<<std::endl;
    }

    virtual void setVertexArray(unsigned int count,const osg::Vec3* vertices)
    {
        _vertexArraySize = count;
        _vertexArrayPtr = vertices;
    }

    virtual void setVertexArray(unsigned int,const osg::Vec4* )
    {
        osg::notify(osg::WARN)<<"Triangle Functor does not support Vec4* vertex arrays"<<std::endl;
    }

    virtual void setVertexArray(unsigned int,const osg::Vec2d*)
    {
        osg::notify(osg::WARN)<<"Triangle Functor does not support Vec2d* vertex arrays"<<std::endl;
    }

    virtual void setVertexArray(unsigned int,const osg::Vec3d*)
    {
        osg::notify(osg::WARN)<<"Triangle Functor does not support Vec3d* vertex arrays"<<std::endl;
    }

    virtual void setVertexArray(unsigned int,const osg::Vec4d* )
    {
        osg::notify(osg::WARN)<<"Triangle Functor does not support Vec4d* vertex arrays"<<std::endl;
    }

    virtual void drawArrays(GLenum mode,GLint first,GLsizei count)
    {
        if (_vertexArrayPtr==0 || count==0) return;

        switch(mode)
        {
            case(GL_TRIANGLES):
            {
                const osg::Vec3* vlast = &_vertexArrayPtr[first+count];
                for(const osg::Vec3* vptr=&_vertexArrayPtr[first];vptr<vlast;vptr+=3)
                    this->operator()(vptr,vptr+1,vptr+2,localToWorld);
                break;
            }
            case(GL_TRIANGLE_STRIP):
            {
                const osg::Vec3* vptr = &_vertexArrayPtr[first];
                for(GLsizei i=2;i<count;++i,++vptr)
                {
                    if ((i%2)) this->operator()(vptr,vptr+2,vptr+1,localToWorld);
                    else       this->operator()(vptr,vptr+1,vptr+2,localToWorld);
                }
                break;
            }
            case(GL_QUADS):
            {
                const osg::Vec3* vptr = &_vertexArrayPtr[first];
                for(GLsizei i=3;i<count;i+=4,vptr+=4)
                {
                    this->operator()(vptr,vptr+1,vptr+2,localToWorld);
                    this->operator()(vptr,vptr+2,vptr+3,localToWorld);
                }
                break;
            }
            case(GL_QUAD_STRIP):
            {
                const osg::Vec3* vptr = &_vertexArrayPtr[first];
                for(GLsizei i=3;i<count;i+=2,vptr+=2)
                {
                    this->operator()(vptr,vptr+1,vptr+2,localToWorld);
                    this->operator()(vptr+1,vptr+3,vptr+2,localToWorld);
                }
                break;
            }
            case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
            case(GL_TRIANGLE_FAN):
            {
                const osg::Vec3* vfirst = &_vertexArrayPtr[first];
                const osg::Vec3* vptr = vfirst+1;
                for(GLsizei i=2;i<count;++i,++vptr)
                {
                    this->operator()(vfirst,vptr,vptr+1,localToWorld);
                }
                break;
            }
            case(GL_POINTS):
            case(GL_LINES):
            case(GL_LINE_STRIP):
            case(GL_LINE_LOOP):
            default:
                // can't be converted into to triangles.
                break;
        }
    }

    virtual void drawElements(GLenum mode,GLsizei count,const GLubyte* indices)
    {
#if 1
        if (indices==0 || count==0) return;

        typedef const GLubyte* IndexPointer;

        switch(mode)
        {
            case(GL_TRIANGLES):
            {
                IndexPointer ilast = &indices[count];
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
                for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
				{
					this->operator()(vptr + *iptr/* _vertexArrayPtr[*iptr]*/,(vptr + *(iptr+1))/* _vertexArrayPtr[*(iptr+1)]*/,
						vptr + *(iptr + 2) /*_vertexArrayPtr[*(iptr+2)]*/,localToWorld);
				}
                break;
            }
            case(GL_TRIANGLE_STRIP):
            {
                IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
                for(GLsizei i=2;i<count;++i,++iptr)
                {
                    if ((i%2))
					{
						this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *iptr,
										/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
										/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
									localToWorld);

					}
                    else
					{
						this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *iptr,
							/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
							/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
							localToWorld);
					}
                }
                break;
            }
            case(GL_QUADS):
            {
                IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
                for(GLsizei i=3;i<count;i+=4,iptr+=4)
                {
                    this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);

                    this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						/*_vertexArrayPtr[*(iptr+3)]*/vptr + *(iptr + 3),
						localToWorld);
                }
                break;
            }
            case(GL_QUAD_STRIP):
            {
                IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
                for(GLsizei i=3;i<count;i+=2,iptr+=2)
                {
                    this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);

                    this->operator()(/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+3)]*/vptr + *(iptr + 3),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);
                }
                break;
            }
            case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
            case(GL_TRIANGLE_FAN):
            {
                IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
               /* const Vec3& vfirst = _vertexArrayPtr[*iptr];*/
				const osg::Vec3* vfirst = vptr + *iptr;
                ++iptr;
                for(GLsizei i=2;i<count;++i,++iptr)
                {
                    this->operator()(/*vfirst*/vfirst ,
						/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						localToWorld);
                }
                break;
            }
            case(GL_POINTS):
            case(GL_LINES):
            case(GL_LINE_STRIP):
            case(GL_LINE_LOOP):
            default:
                break;
        }
#endif
    }

    virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indices)
    {
#if 1
		if (indices==0 || count==0) return;

		typedef const GLushort* IndexPointer;

		switch(mode)
		{
		case(GL_TRIANGLES):
			{
				IndexPointer ilast = &indices[count];
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
				{
					this->operator()(vptr + *iptr/* _vertexArrayPtr[*iptr]*/,(vptr + *(iptr+1))/* _vertexArrayPtr[*(iptr+1)]*/,
						vptr + *(iptr + 2) /*_vertexArrayPtr[*(iptr+2)]*/,localToWorld);
				}
				break;
			}
		case(GL_TRIANGLE_STRIP):
			{
				IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				for(GLsizei i=2;i<count;++i,++iptr)
				{
					if ((i%2))
					{
						this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *iptr,
							/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
							/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
							localToWorld);

					}
					else
					{
						this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *iptr,
							/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
							/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
							localToWorld);
					}
				}
				break;
			}
		case(GL_QUADS):
			{
				IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				for(GLsizei i=3;i<count;i+=4,iptr+=4)
				{
					this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);

					this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						/*_vertexArrayPtr[*(iptr+3)]*/vptr + *(iptr + 3),
						localToWorld);
				}
				break;
			}
		case(GL_QUAD_STRIP):
			{
				IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				for(GLsizei i=3;i<count;i+=2,iptr+=2)
				{
					this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);

					this->operator()(/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+3)]*/vptr + *(iptr + 3),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);
				}
				break;
			}
		case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
		case(GL_TRIANGLE_FAN):
			{
				IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				/* const Vec3& vfirst = _vertexArrayPtr[*iptr];*/
				const osg::Vec3* vfirst = vptr + *iptr;
				++iptr;
				for(GLsizei i=2;i<count;++i,++iptr)
				{
					this->operator()(/*vfirst*/vfirst ,
						/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						localToWorld);
				}
				break;
			}
		case(GL_POINTS):
		case(GL_LINES):
		case(GL_LINE_STRIP):
		case(GL_LINE_LOOP):
		default:
			// can't be converted into to triangles.
			break;
		}
#endif
    }

    virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices)
    {
#if 1
        if (indices==0 || count==0) return;

        typedef const GLuint* IndexPointer;

		switch(mode)
		{
		case(GL_TRIANGLES):
			{
				IndexPointer ilast = &indices[count];
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
				{
					this->operator()(vptr + *iptr/* _vertexArrayPtr[*iptr]*/,(vptr + *(iptr+1))/* _vertexArrayPtr[*(iptr+1)]*/,
						vptr + *(iptr + 2) /*_vertexArrayPtr[*(iptr+2)]*/,localToWorld);
				}
				break;
			}
		case(GL_TRIANGLE_STRIP):
			{
				IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				for(GLsizei i=2;i<count;++i,++iptr)
				{
					if ((i%2))
					{
						this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *iptr,
							/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
							/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
							localToWorld);

					}
					else
					{
						this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *iptr,
							/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
							/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
							localToWorld);
					}
				}
				break;
			}
		case(GL_QUADS):
			{
				IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				for(GLsizei i=3;i<count;i+=4,iptr+=4)
				{
					this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);

					this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						/*_vertexArrayPtr[*(iptr+3)]*/vptr + *(iptr + 3),
						localToWorld);
				}
				break;
			}
		case(GL_QUAD_STRIP):
			{
				IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				for(GLsizei i=3;i<count;i+=2,iptr+=2)
				{
					this->operator()(/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);

					this->operator()(/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+3)]*/vptr + *(iptr + 3),
						/*_vertexArrayPtr[*(iptr+2)]*/vptr + *(iptr + 2),
						localToWorld);
				}
				break;
			}
		case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
		case(GL_TRIANGLE_FAN):
			{
				IndexPointer iptr = indices;
				const osg::Vec3* vptr = &_vertexArrayPtr[0];
				/* const Vec3& vfirst = _vertexArrayPtr[*iptr];*/
				const osg::Vec3* vfirst = vptr + *iptr;
				++iptr;
				for(GLsizei i=2;i<count;++i,++iptr)
				{
					this->operator()(/*vfirst*/vfirst ,
						/*_vertexArrayPtr[*(iptr)]*/vptr + *(iptr),
						/*_vertexArrayPtr[*(iptr+1)]*/vptr + *(iptr + 1),
						localToWorld);
				}
				break;
			}
		case(GL_POINTS):
		case(GL_LINES):
		case(GL_LINE_STRIP):
		case(GL_LINE_LOOP):
		default:
			// can't be converted into to triangles.
			break;
		}
      
#endif
    }

    virtual void begin(GLenum mode)
    {
        _modeCache = mode;
        _vertexCache.clear();
    }

    virtual void vertex(const osg::Vec2& vert) { _vertexCache.push_back(osg::Vec3(vert[0],vert[1],0.0f)); }
    virtual void vertex(const osg::Vec3& vert) { _vertexCache.push_back(vert); }
    virtual void vertex(const osg::Vec4& vert) { _vertexCache.push_back(osg::Vec3(vert[0],vert[1],vert[2])/vert[3]); }
    virtual void vertex(float x,float y) { _vertexCache.push_back(osg::Vec3(x,y,0.0f)); }
    virtual void vertex(float x,float y,float z) { _vertexCache.push_back(osg::Vec3(x,y,z)); }
    virtual void vertex(float x,float y,float z,float w) { _vertexCache.push_back(osg::Vec3(x,y,z)/w); }
    virtual void end()
    {
        if (!_vertexCache.empty())
        {
            setVertexArray(_vertexCache.size(),&_vertexCache.front());
            _treatVertexDataAsTemporary = true;
            drawArrays(_modeCache,0,_vertexCache.size());
        }
    }

protected:

    unsigned int			_vertexArraySize;
    const osg::Vec3*         _vertexArrayPtr;

	osg::Matrixd			localToWorld;
    GLenum					_modeCache;
    std::vector<osg::Vec3>   _vertexCache;
    bool					_treatVertexDataAsTemporary;
};

}
