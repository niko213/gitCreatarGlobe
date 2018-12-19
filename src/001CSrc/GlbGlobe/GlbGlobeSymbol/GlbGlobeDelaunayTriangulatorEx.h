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

#ifndef OSGUTIL_DELAUNAYTRIANGULATOR_EX
#define OSGUTIL_DELAUNAYTRIANGULATOR_EX

#include <list>

#include <osg/ref_ptr>
#include <osg/Array>
#include <osg/Referenced>
#include <osg/CopyOp>
#include <osg/PrimitiveSet>
#include <osg/Geometry>

//#include <osgUtil/Export>

namespace GlbGlobe
{
	/** DelaunayTriangulator: Utility class that triangulates an irregular network of sample points.
	Just create a DelaunayTriangulator, assign it the sample point array and call
	its triangulate() method to start the triangulation. Then you can obtain the
	generated primitive by calling the getTriangles() method.

	Add DelaunayConstraints (or derived class) to control the triangulation edges.
	*/
	class DelaunayConstraint: public osg::Geometry {
		// controls the edges in a Delaunay triangulation.
		// constraints can be linear (with width), areal (contains an area)
		// uses: to replace part of a terrain with an alternative textured model (roads, lakes).
		// the primitive sets in this are either LINE_LOOP or LINE_STRIP
	public:
		DelaunayConstraint() { }

		/** Each primitiveset is a list of vertices which may be closed by joining up to its start
		* to make a loop.  Constraints should be simple lines, not crossing themselves.
		* Constraints which cross other constraints can cause difficulties - see the example
		* for methods of dealing with them. */

		/** collect up indices of triangle from delaunay triangles.
		*  The delaunay triangles inside the DelaunayConstraint area can be used to fill
		*  the area or generate geometry that terrain follows the area in some way.
		*  These triangles can form a canopy or a field. */
		void addtriangle(int i1, int i2, int i3);

		/** Get the filling primitive. One:
		* triangulate must have bneen called and
		* two:  triangle list is filled when
		* DelaunayTriangulator::removeInternalTriangles is called.
		* These return the triangles removed from the delaunay triangulation by
		* DelaunayTriangulator::removeInternalTriangles. */
		inline const osg::DrawElementsUInt *getTriangles() const { return prim_tris_.get(); }

		inline osg::DrawElementsUInt *getTriangles() { return prim_tris_.get(); }

		/** Call BEFORE makeDrawable to reorder points to make optimised set
		*/
		osg::Vec3dArray *getPoints(const osg::Vec3dArray *points);

		/** converts simple list of triangles into a drawarray.
		*/
		osg::DrawElementsUInt *makeDrawable();

		/** Add vertices and constraint loops from dco
		* Can be used to generate extra vertices where dco crosses 'this' using
		* osgUtil::Tessellator to insert overlap vertices.
		*/
		void merge(DelaunayConstraint *dco);

		/** remove from line the vertices that are inside dco
		*/
		void removeVerticesInside(const DelaunayConstraint *dco);

		/** return winding number as a float of loop around testpoint; may use multiple loops
		* does not reject points on the edge or very very close to the edge */
		double windingNumber(const osg::Vec3d &testpoint) const ;

		/** true if testpoint is internal (or external) to constraint. */
		virtual bool contains(const osg::Vec3d &testpoint) const;
		virtual bool outside(const osg::Vec3d &testpoint) const;

		/** Tessellate the constraint loops so that the crossing points are interpolated
		* and added to the contraints for the triangulation. */
		void handleOverlaps(void);

	protected:
		virtual ~DelaunayConstraint();

		typedef std::vector< int* > trilist; // array of indices in points array defining triangles

		trilist _interiorTris; // list of triangles that fits the area.

		osg::ref_ptr<osg::DrawElementsUInt> prim_tris_; // returns a PrimitiveSet to draw the interior of this DC
	};


	class DelaunayTriangulatorEx: public osg::Referenced 
	{
	public:
		DelaunayTriangulatorEx();
		explicit DelaunayTriangulatorEx(osg::Vec3dArray *points, osg::Vec3dArray *normals = 0);
		DelaunayTriangulatorEx(const DelaunayTriangulatorEx &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

		typedef std::vector< osg::ref_ptr<DelaunayConstraint> > linelist;

		/** Set the input point array. */
		inline void setInputPointArray(osg::Vec3dArray* points) { points_ = points; }

		/** Get the const input point array. */
		inline const osg::Vec3dArray* getInputPointArray() const {  return points_.get(); }

		/** Get the input point array. */
		inline osg::Vec3dArray* getInputPointArray() {  return points_.get(); }


		/** Set the output normal array (optional). */
		inline void setOutputNormalArray(osg::Vec3dArray* normals) { normals_ = normals; }

		/** Get the const output normal array (optional). */
		inline const osg::Vec3dArray *getOutputNormalArray() const { return normals_.get(); }

		/** Get the output normal array (optional). */
		inline osg::Vec3dArray *getOutputNormalArray() { return normals_.get(); }


		/** Add an input constraint loop.
		** the edges of the loop will constrain the triangulation.
		** if remove!=0, the internal triangles of the constraint will be removed;
		** the user may the replace the constraint line with an equivalent geometry.
		** GWM July 2005 */
		void addInputConstraint(DelaunayConstraint *dc) { constraint_lines.push_back(dc); }


		/** Start triangulation. */
		bool triangulate();

		/** Get the generated primitive (call triangulate() first). */
		inline const osg::DrawElementsUInt *getTriangles() const { return prim_tris_.get(); }

		/** Get the generated primitive (call triangulate() first). */
		inline osg::DrawElementsUInt *getTriangles() { return prim_tris_.get(); }

		/** remove the triangles internal to the constraint loops.
		* (Line strips cannot remove any internal triangles). */
		void removeInternalTriangles(DelaunayConstraint *constraint);


	protected:
		virtual ~DelaunayTriangulatorEx();
		DelaunayTriangulatorEx &operator=(const DelaunayTriangulatorEx &) { return *this; }
		int getindex(const osg::Vec3d &pt,const osg::Vec3dArray *points);

	private:
		/*osg::ref_ptr<osg::Vec3Array> points_;
		osg::ref_ptr<osg::Vec3Array> normals_;*/
		osg::ref_ptr<osg::Vec3dArray> points_;
		osg::ref_ptr<osg::Vec3dArray> normals_;
		osg::ref_ptr<osg::DrawElementsUInt> prim_tris_;

		// GWM these lines provide required edges in the triangulated shape.
		linelist constraint_lines;

		void _uniqueifyPoints();
	};
}

#endif//OSGUTIL_DELAUNAYTRIANGULATOR_EX


