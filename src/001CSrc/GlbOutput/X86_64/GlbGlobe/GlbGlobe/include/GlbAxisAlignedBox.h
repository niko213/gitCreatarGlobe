/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbAxisAlignedBox.h
  * @brief   AABB包围盒类
  *
  * 这个档案定义CGlbAxisAlignedBox这个class
  *
  * @version 1.0
  * @author  马林
  * @date    2014-5-13 10:40
*********************************************************************/
#pragma once
#include <assert.h>
#include <limits>
#include "osg/Vec3"
namespace GlbGlobe
{
	/** A 3D box aligned with the x/y/z axes.
	@remarks
	This class represents a simple box which is aligned with the
	axes. Internally it only stores 2 points as the extremeties of
	the box, one which is the minima of all 3 axes, and the other
	which is the maxima of all 3 axes. This class is typically used
	for an axis-aligned bounding box (AABB) for collision and
	visibility determination.
	*/
	class GLB_DLLCLASS_EXPORT CGlbAxisAlignedBox
	{
	public:
		CGlbAxisAlignedBox(void);
		~CGlbAxisAlignedBox(void);

		enum Extent
		{
			EXTENT_NULL,
			EXTENT_FINITE,
			EXTENT_INFINITE
		};
				/*
		1-----2
		/|    /|
		/ |   / |
		5-----4  |
		|  0--|--3
		| /   | /
		|/    |/
		6-----7
		*/
		typedef enum {
			FAR_LEFT_BOTTOM = 0,
			FAR_LEFT_TOP = 1,
			FAR_RIGHT_TOP = 2,
			FAR_RIGHT_BOTTOM = 3,
			NEAR_RIGHT_BOTTOM = 7,
			NEAR_LEFT_BOTTOM = 6,
			NEAR_LEFT_TOP = 5,
			NEAR_RIGHT_TOP = 4
		} CornerEnum;
	private:

		osg::Vec3 mpr_minimum;
		osg::Vec3 mpr_maximum;
		Extent mpr_extent;
		mutable osg::Vec3* mpr_p_corners;

	public:
		inline CGlbAxisAlignedBox( const osg::Vec3& min, const osg::Vec3& max ) 
			: mpr_p_corners(NULL)
		{
			SetExtents( min, max );
		}
		inline CGlbAxisAlignedBox(	float mx, float my, float mz,
						float Mx, float My, float Mz ) 
			: mpr_p_corners(NULL)
		{
			SetExtents( mx, my, mz, Mx, My, Mz );
		}
		CGlbAxisAlignedBox& operator=(const CGlbAxisAlignedBox& rhs)
		{
			// Specifically override to avoid copying mCorners
			if (rhs.IsNull())
				SetNull();
			else if (rhs.IsInfinite())
				SetInfinite();
			else
				SetExtents(rhs.mpr_minimum, rhs.mpr_maximum);

			return *this;
		}

		/** Sets the minimum corner of the box.
		*/
		inline void SetMinimum( const osg::Vec3& vec )
		{
			mpr_extent = EXTENT_FINITE;
			mpr_minimum = vec;
		}

		inline void SetMinimum( float x, float y, float z )
		{
			mpr_extent = EXTENT_FINITE;
			mpr_minimum.set(x,y,z);
		}

		/** Gets a modifiable version of the minimum
		corner of the box.
		*/
		inline osg::Vec3& GetMinimum(void)
		{ 
			return mpr_minimum; 
		}

		
		/** Sets the maximum corner of the box.
		*/
		inline void SetMaximum( const osg::Vec3& vec )
		{
			mpr_extent = EXTENT_FINITE;
			mpr_maximum = vec;
		}

		inline void SetMaximum( float x, float y, float z )
		{
			mpr_extent = EXTENT_FINITE;
			mpr_maximum.set(x,y,z);
		}

		/** Gets a modifiable version of the minimum
		corner of the box.
		*/
		inline osg::Vec3& GetMaximum(void)
		{ 
			return mpr_maximum; 
		}

		/** Sets both minimum and maximum extents at once.
		*/
		inline void SetExtents( const osg::Vec3& min, const osg::Vec3& max )
		{
            assert( (min.x() <= max.x() && min.y() <= max.y() && min.z() <= max.z()) &&
                "The minimum corner of the box must be less than or equal to maximum corner" );

			mpr_extent = EXTENT_FINITE;
			mpr_minimum = min;
			mpr_maximum = max;
		}

		inline void SetExtents(	float mx, float my, float mz,
						float Mx, float My, float Mz )
		{
            assert( (mx <= Mx && my <= My && mz <= Mz) &&
                "The minimum corner of the box must be less than or equal to maximum corner" );

			mpr_extent = EXTENT_FINITE;

			mpr_minimum .set(mx,my,mz);
			mpr_maximum .set(Mx,My,Mz);
		}

		/** Returns a pointer to an array of 8 corner points, useful for
		collision vs. non-aligned objects.
		@remarks
		If the order of these corners is important, they are as
		follows: The 4 points of the minimum Z face (note that
		because Ogre uses right-handed coordinates, the minimum Z is
		at the 'back' of the box) starting with the minimum point of
		all, then anticlockwise around this face (if you are looking
		onto the face from outside the box). Then the 4 points of the
		maximum Z face, starting with maximum point of all, then
		anticlockwise around this face (looking onto the face from
		outside the box). Like this:
		<pre>
		1-----2
		/|    /|
		/ |   / |
		5-----4  |
		|  0--|--3
		| /   | /
		|/    |/
		6-----7
		</pre>
		@remarks as this implementation uses a static member, make sure to use your own copy !
		*/
		inline const osg::Vec3* getAllCorners(void) const
		{
			assert( (mpr_extent == EXTENT_FINITE) && "Can't get corners of a null or infinite AAB" );

			// The order of these items is, using right-handed co-ordinates:
			// Minimum Z face, starting with Min(all), then anticlockwise
			//   around face (looking onto the face)
			// Maximum Z face, starting with Max(all), then anticlockwise
			//   around face (looking onto the face)
			// Only for optimization/compatibility.
			if (!mpr_p_corners)
				mpr_p_corners = new osg::Vec3[8];

			mpr_p_corners[0] = mpr_minimum;
			mpr_p_corners[1].set(mpr_minimum.x(),mpr_maximum.y(),mpr_minimum.z());
			mpr_p_corners[2].set(mpr_maximum.x(),mpr_maximum.y(),mpr_minimum.z());
			mpr_p_corners[3].set(mpr_maximum.x(),mpr_minimum.y(),mpr_minimum.z());		
			mpr_p_corners[4] = mpr_maximum;
			mpr_p_corners[5].set(mpr_minimum.x(),mpr_maximum.y(),mpr_maximum.z());	
			mpr_p_corners[6].set(mpr_minimum.x(),mpr_minimum.y(),mpr_maximum.z());	
			mpr_p_corners[7].set(mpr_maximum.x(),mpr_minimum.y(),mpr_maximum.z());	

			return mpr_p_corners;
		}

		/** gets the position of one of the corners
		*/
		osg::Vec3 getCorner(CornerEnum cornerToGet) const
		{
			switch(cornerToGet)
			{
			case FAR_LEFT_BOTTOM:
				return mpr_minimum;
			case FAR_LEFT_TOP:
				return osg::Vec3(mpr_minimum.x(), mpr_maximum.y(), mpr_minimum.z());
			case FAR_RIGHT_TOP:
				return osg::Vec3(mpr_maximum.x(), mpr_maximum.y(), mpr_minimum.z());
			case FAR_RIGHT_BOTTOM:
				return osg::Vec3(mpr_maximum.x(), mpr_minimum.y(), mpr_minimum.z());
			case NEAR_RIGHT_BOTTOM:
				return osg::Vec3(mpr_maximum.x(), mpr_minimum.y(), mpr_maximum.z());
			case NEAR_LEFT_BOTTOM:
				return osg::Vec3(mpr_minimum.x(), mpr_minimum.y(), mpr_maximum.z());
			case NEAR_LEFT_TOP:
				return osg::Vec3(mpr_minimum.x(), mpr_maximum.y(), mpr_maximum.z());
			case NEAR_RIGHT_TOP:
				return mpr_maximum;
			default:
				return osg::Vec3();
			}
		}

		/** Sets the box to a 'null' value i.e. not a box.
		*/
		inline void SetNull()
		{
			mpr_extent = EXTENT_NULL;
		}

		/** Returns true if the box is null i.e. empty.
		*/
		inline bool IsNull(void) const
		{
			return (mpr_extent == EXTENT_NULL);
		}

		/** Returns true if the box is finite.
		*/
		bool IsFinite(void) const
		{
			return (mpr_extent == EXTENT_FINITE);
		}

		/** Sets the box to 'infinite'
		*/
		inline void SetInfinite()
		{
			mpr_extent = EXTENT_INFINITE;
		}

		/** Returns true if the box is infinite.
		*/
		bool IsInfinite(void) const
		{
			return (mpr_extent == EXTENT_INFINITE);
		}

		/// Gets the centre of the box
		osg::Vec3 GetCenter(void) const
		{
			assert( (mpr_extent == EXTENT_FINITE) && "Can't get center of a null or infinite AAB" );

			return osg::Vec3(
				(mpr_maximum.x() + mpr_minimum.x()) * 0.5f,
				(mpr_maximum.y() + mpr_minimum.y()) * 0.5f,
				(mpr_maximum.z() + mpr_minimum.z()) * 0.5f);
		}
		/// Gets the size of the box
		osg::Vec3 GetSize(void) const
		{
			switch (mpr_extent)
			{
			case EXTENT_NULL:
				return osg::Vec3();

			case EXTENT_FINITE:
				return mpr_maximum - mpr_minimum;

			case EXTENT_INFINITE:
				return osg::Vec3(
					std::numeric_limits<float>::infinity(),
					std::numeric_limits<float>::infinity(),
					std::numeric_limits<float>::infinity());

			default: // shut up compiler
				assert( false && "Never reached" );
				return osg::Vec3();
			}
		}
		/// Gets the half-size of the box
		osg::Vec3 GetHalfSize(void) const
		{
			switch (mpr_extent)
			{
			case EXTENT_NULL:
				return osg::Vec3();

			case EXTENT_FINITE:
				return (mpr_maximum - mpr_minimum) * 0.5;

			case EXTENT_INFINITE:
				return osg::Vec3(
					std::numeric_limits<float>::infinity(),
					std::numeric_limits<float>::infinity(),
					std::numeric_limits<float>::infinity());

			default: // shut up compiler
				assert( false && "Never reached" );
				return osg::Vec3();
			}
		}

		/** Merges the passed in box into the current box. The result is the
		box which encompasses both.
		*/
		void Merge( const CGlbAxisAlignedBox& rhs )
		{
			// Do nothing if rhs null, or this is infinite
			if ((rhs.mpr_extent == EXTENT_NULL) || (mpr_extent == EXTENT_INFINITE))
			{
				return;
			}
			// Otherwise if rhs is infinite, make this infinite, too
			else if (rhs.mpr_extent == EXTENT_INFINITE)
			{
				mpr_extent = EXTENT_INFINITE;
			}
			// Otherwise if current null, just take rhs
			else if (mpr_extent == EXTENT_NULL)
			{
				SetExtents(rhs.mpr_minimum, rhs.mpr_maximum);
			}
			// Otherwise merge
			else
			{
				osg::Vec3 _min = mpr_minimum;
				osg::Vec3 _max = mpr_maximum;
				_max.set(max(_max.x(),rhs.mpr_maximum.x()),max(_max.y(),rhs.mpr_maximum.y()),max(_max.z(),rhs.mpr_maximum.z()));
				_min.set(min(_min.x(),rhs.mpr_minimum.x()),min(_min.y(),rhs.mpr_minimum.y()),min(_min.z(),rhs.mpr_minimum.z()));
				SetExtents(_min, _max);
			}

		}

		/** Extends the box to encompass the specified point (if needed).
		*/
		inline void Merge( const osg::Vec3& point )
		{
			switch (mpr_extent)
			{
			case EXTENT_NULL: // if null, use this point
				SetExtents(point, point);
				return;

			case EXTENT_FINITE:
				{
					osg::Vec3 _max = mpr_maximum;
					osg::Vec3 _min = mpr_minimum;
					mpr_maximum.set(max(_max.x(),point.x()),max(_max.y(),point.y()),max(_max.z(),point.z()));
					mpr_minimum.set(min(_min.x(),point.x()),min(_min.y(),point.y()),min(_min.z(),point.z()));
				}
				return;

			case EXTENT_INFINITE: // if infinite, makes no difference
				return;
			}

			assert( false && "Never reached" );
		}

		/** Returns whether or not this box intersects another. */
		inline bool Intersects(const CGlbAxisAlignedBox& b2) const
		{
			// Early-fail for nulls
			if (this->IsNull() || b2.IsNull())
				return false;

			// Early-success for infinites
			if (this->IsInfinite() || b2.IsInfinite())
				return true;

			// Use up to 6 separating planes
			if (mpr_maximum.x() < b2.mpr_minimum.x())
				return false;
			if (mpr_maximum.y() < b2.mpr_minimum.y())
				return false;
			if (mpr_maximum.z() < b2.mpr_minimum.z())
				return false;

			if (mpr_minimum.x() > b2.mpr_maximum.x())
				return false;
			if (mpr_minimum.y() > b2.mpr_maximum.y())
				return false;
			if (mpr_minimum.z() > b2.mpr_maximum.z())
				return false;

			// otherwise, must be intersecting
			return true;

		}

		/** Returns whether or not this box intersects another. */
		inline bool Intersects2D(const CGlbAxisAlignedBox& b2) const
		{
			// Early-fail for nulls
			if (this->IsNull() || b2.IsNull())
				return false;

			// Early-success for infinites
			if (this->IsInfinite() || b2.IsInfinite())
				return true;

			// Use up to 6 separating planes
			if (mpr_maximum.x() < b2.mpr_minimum.x())
				return false;
			if (mpr_maximum.y() < b2.mpr_minimum.y())
				return false;			

			if (mpr_minimum.x() > b2.mpr_maximum.x())
				return false;
			if (mpr_minimum.y() > b2.mpr_maximum.y())
				return false;			

			// otherwise, must be intersecting
			return true;

		}

		/// Calculate the area of intersection of this box and another
		inline CGlbAxisAlignedBox Intersection(const CGlbAxisAlignedBox& b2) const
		{
			if (this->IsNull() || b2.IsNull())
			{
				return CGlbAxisAlignedBox();
			}
			else if (this->IsInfinite())
			{
				return b2;
			}
			else if (b2.IsInfinite())
			{
				return *this;
			}

			osg::Vec3 intMin = mpr_minimum;
			osg::Vec3 intMax = mpr_maximum;
	
			osg::Vec3 _min = b2.mpr_minimum;
			osg::Vec3 _max = b2.mpr_maximum;
			intMin.set(max(_min.x(),intMin.x()),max(_min.y(),intMin.y()),max(_min.z(),intMin.z()));
			intMax.set(min(_max.x(),intMax.x()),min(_max.y(),intMax.y()),min(_max.z(),intMax.z()));

			// Check intersection isn't null
			if (intMin.x() < intMax.x() &&
				intMin.y() < intMax.y() &&
				intMin.z() < intMax.z())
			{
				return CGlbAxisAlignedBox(intMin, intMax);
			}

			return CGlbAxisAlignedBox();
		}

		/** Tests whether the vector point is within this box. */
		bool Intersects(const osg::Vec3& v) const
		{
			switch (mpr_extent)
			{
			case EXTENT_NULL:
				return false;

			case EXTENT_FINITE:
				return(v.x() >= mpr_minimum.x()  &&  v.x() <= mpr_maximum.x()  && 
					v.y() >= mpr_minimum.y()  &&  v.y() <= mpr_maximum.y()  && 
					v.z() >= mpr_minimum.z()  &&  v.z() <= mpr_maximum.z());

			case EXTENT_INFINITE:
				return true;

			default: // shut up compiler
				assert( false && "Never reached" );
				return false;
			}
		}

		///** Tests whether this box intersects a sphere. */
		//bool intersects(const Sphere& s) const
		//{
		//	return Math::intersects(s, *this); 
		//}
		///** Tests whether this box intersects a plane. */
		//bool intersects(const Plane& p) const
		//{
		//	return Math::intersects(p, *this);
		//}

		/** Tests whether the given point contained by this box.
        */
        bool Contains(const osg::Vec3& v) const
        {
            if (IsNull())
                return false;
            if (IsInfinite())
                return true;

            return mpr_minimum.x() <= v.x() && v.x() <= mpr_maximum.x() &&
                   mpr_minimum.y() <= v.y() && v.y() <= mpr_maximum.y() &&
                   mpr_minimum.z() <= v.z() && v.z() <= mpr_maximum.z();
        }

		/** Tests whether the given point contained by this box.
        */
		bool Contains2D(const osg::Vec3& v) const
		{
			if (IsNull())
				return false;
			if (IsInfinite())
				return true;

			return mpr_minimum.x() <= v.x() && v.x() <= mpr_maximum.x() &&
				mpr_minimum.y() <= v.y() && v.y() <= mpr_maximum.y() ;
		}

				/** Tests whether the given point contained by this box.
        */
		bool Contains2D(const CGlbAxisAlignedBox& other) const
		{
			if (other.IsNull() || this->IsInfinite())
				return true;

			if (this->IsNull() || other.IsInfinite())
				return false;

			return this->mpr_minimum.x() <= other.mpr_minimum.x() &&
				this->mpr_minimum.y() <= other.mpr_minimum.y() &&				
				other.mpr_maximum.x() <= this->mpr_maximum.x() &&
				other.mpr_maximum.y() <= this->mpr_maximum.y();
		}
		
		
        /** Tests whether another box contained by this box.
        */
        bool Contains(const CGlbAxisAlignedBox& other) const
        {
            if (other.IsNull() || this->IsInfinite())
                return true;

            if (this->IsNull() || other.IsInfinite())
                return false;

            return this->mpr_minimum.x() <= other.mpr_minimum.x() &&
                   this->mpr_minimum.y() <= other.mpr_minimum.y() &&
                   this->mpr_minimum.z() <= other.mpr_minimum.z() &&
                   other.mpr_maximum.x() <= this->mpr_maximum.x() &&
                   other.mpr_maximum.y() <= this->mpr_maximum.y() &&
                   other.mpr_maximum.z() <= this->mpr_maximum.z();
        }

		  /** Tests 2 boxes for equality.
        */
        bool operator== (const CGlbAxisAlignedBox& rhs) const
        {
            if (this->mpr_extent != rhs.mpr_extent)
                return false;

            if (!this->IsFinite())
                return true;

            return this->mpr_minimum == rhs.mpr_minimum &&
                   this->mpr_maximum == rhs.mpr_maximum;
        }

        /** Tests 2 boxes for inequality.
        */
        bool operator!= (const CGlbAxisAlignedBox& rhs) const
        {
            return !(*this == rhs);
        }
	};

}

