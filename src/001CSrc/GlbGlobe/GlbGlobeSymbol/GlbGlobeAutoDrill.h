#pragma once
#include "GlbGlobeSymbolExport.h"
#include <osg/AutoTransform>
using namespace osg;
// 注释： 系统中目前没有用到，似乎是想实现自动朝向相机的钻孔对象---在GlobeDrill的ReadData函数中用到了！
namespace GlbGlobe
{
	class GLB_SYMBOLDLL_CLASSEXPORT GlbGlobeAutoDrill : public osg::AutoTransform
	{
	public :
		GlbGlobeAutoDrill();

		GlbGlobeAutoDrill(const GlbGlobeAutoDrill& pat,const CopyOp& copyop=CopyOp::SHALLOW_COPY);

		virtual osg::Object* cloneType() const { return new GlbGlobeAutoDrill (); }
		virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new GlbGlobeAutoDrill (*this,copyop); }
		virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const GlbGlobeAutoDrill *>(obj)!=NULL; }
		virtual const char* className() const { return "GlbGlobeAutoDrill"; }
		virtual const char* libraryName() const { return "osg"; }

		virtual void accept(NodeVisitor& nv);

		virtual GlbGlobeAutoDrill* asAutoTransform() { return this; }
		virtual const GlbGlobeAutoDrill* asAutoTransform() const { return this; }

		inline void setPosition(const Vec3d& pos) { _position = pos; _matrixDirty=true; dirtyBound(); }
		inline const Vec3d& getPosition() const { return _position; }


		inline void setRotation(const Quat& quat) { _rotation = quat; _matrixDirty=true; dirtyBound(); }
		inline const Quat& getRotation() const { return _rotation; }

		inline void setScale(double scale) { setScale(osg::Vec3(scale,scale,scale)); }

		void setScale(const Vec3d& scale);
		inline const Vec3d& getScale() const { return _scale; }

		void setMinimumScale(double minimumScale) { _minimumScale = minimumScale; }
		double getMinimumScale() const { return _minimumScale; }

		void setMaximumScale(double maximumScale) { _maximumScale = maximumScale; }
		double getMaximumScale() const { return _maximumScale; }

		inline void setPivotPoint(const Vec3d& pivot) { _pivotPoint = pivot; _matrixDirty=true; dirtyBound(); }
		inline const Vec3d& getPivotPoint() const { return _pivotPoint; }


		void setAutoUpdateEyeMovementTolerance(float tolerance) { _autoUpdateEyeMovementTolerance = tolerance; }
		float getAutoUpdateEyeMovementTolerance() const { return _autoUpdateEyeMovementTolerance; }


		enum AutoRotateMode
		{
			NO_ROTATION,
			ROTATE_TO_SCREEN,
			ROTATE_TO_CAMERA,
			ROTATE_TO_AXIS
		};

		void setAutoRotateMode(AutoRotateMode mode);

		AutoRotateMode getAutoRotateMode() const { return _autoRotateMode; }

		/** Set the rotation axis for the AutoTransform's child nodes.
		* Only utilized when _autoRotateMode==ROTATE_TO_AXIS. */
		void setAxis(const Vec3& axis);
		/** Get the rotation axis. */
		inline const Vec3& getAxis() const { return _axis; }

		/** This normal defines child Nodes' front face direction when unrotated. */
		void setNormal(const Vec3& normal);
		/** Get the front face direction normal. */
		inline const Vec3& getNormal() const { return _normal; }

		void setAutoScaleToScreen(bool autoScaleToScreen) { _autoScaleToScreen = autoScaleToScreen; _matrixDirty=true; }

		bool getAutoScaleToScreen() const { return _autoScaleToScreen; }

		void setAutoScaleTransitionWidthRatio(float ratio) { _autoScaleTransitionWidthRatio = ratio; }
		float getAutoScaleTransitionWidthRatio() const { return _autoScaleTransitionWidthRatio; }


		virtual bool computeLocalToWorldMatrix(Matrix& matrix,NodeVisitor* nv) const;

		virtual bool computeWorldToLocalMatrix(Matrix& matrix,NodeVisitor* nv) const;

		virtual BoundingSphere computeBound() const;


	protected :

		virtual ~GlbGlobeAutoDrill() {}

		Vec3d                           _position;
		Vec3d                           _pivotPoint;
		double                          _autoUpdateEyeMovementTolerance;

		AutoRotateMode                  _autoRotateMode;

		bool                            _autoScaleToScreen;

		mutable Quat                    _rotation;
		mutable Vec3d                   _scale;
		mutable bool                    _firstTimeToInitEyePoint;
		mutable osg::Vec3               _previousEyePoint;
		mutable osg::Vec3               _previousLocalUp;
		mutable Viewport::value_type    _previousWidth;
		mutable Viewport::value_type    _previousHeight;
		mutable osg::Matrixd            _previousProjection;
		mutable osg::Vec3d              _previousPosition;

		double                          _minimumScale;
		double                          _maximumScale;
		double                          _autoScaleTransitionWidthRatio;

		void computeMatrix() const;

		mutable bool                    _matrixDirty;
		mutable osg::Matrixd            _cachedMatrix;

		enum AxisAligned
		{
			AXIAL_ROT_X_AXIS=ROTATE_TO_AXIS+1,
			AXIAL_ROT_Y_AXIS,
			AXIAL_ROT_Z_AXIS,
			CACHE_DIRTY
		};

		Vec3                            _axis;
		Vec3                            _normal;

		// used internally as cache of which what _axis is aligned to help
		// decide which method of rotation to use.
		int                             _cachedMode;
		Vec3                            _side;
		void updateCache();

	};
}