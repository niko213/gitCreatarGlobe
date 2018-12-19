/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbAlgorithm.h
  * @brief   Globe算法类头文件
  *
  * 这个档案定义CGlbAlgorithm这个class,
  *
  * @version 1.0
  * @author  马林
  * @date    2017-8-7 10:40
*********************************************************************/
#pragma once
#include "GlbExportDef.h"
#include "GlbCommTypes.h"
#include "GlbReference.h"
#include <math.h>
//算法（平面和空间）库
// 参考 http://geomalgorithms.com
//a. 空间点到线段的最短距离， 点到直线的距离
//b. 空间线段与线段之间的最短距离
//c. 面积计算

//d. 容积计算
//e. 缓冲区分析。 点，线，面
//f. 叠置分析
//h. 碰撞分析

// 参考 http://geomalgorithms.com/code.html
// Primitive Functions
// isLeft()	- test if a point is Left|On|Right of an infinite 2D line

// orientation2D_Triangle()	- test the orientation of a 2D triangle

// area2D_Triangle() - compute the area of a 2D triangle

//	orientation2D_Polygon()	- test the orientation of a simple 2D polygon

//	area2D_Polygon() - compute the area of a 2D polygon

//	area3D_Polygon() - compute the area of a 3D planar polygon

//	closest2D_Point_to_Line() - find the closest point to a line (in 2D)

//	dist_Point_to_Line() - get the distance of a point to a line (any D)

//	dist_Point_to_Segment() - get the distance of a point to a segment (any D)

//	cn_PnPoly() - crossing number test for a point in a 2D polygon

//	wn_PnPoly()	- winding number test for a point in a 2D polygon

//	dist_Point_to_Plane() - get distance (and perp base) from a point to a 3D plane

//	intersect2D_2Segments()	- find the intersection of 2 finite 2D segments

//	inSegment()	- determine if a point is inside a segment (any D)

//	intersect3D_SegmentPlane()	- find the 3D intersection of a segment and a plane

//	intersect3D_2Planes() - find the 3D intersection of two planes

//	intersect3D_RayTriangle() - find the 3D intersection of a ray with a triangle

//	dist3D_Line_to_Line() - get the 3D minimum distance between 2 lines

//	dist3D_Segment_to_Segment() - get the 3D minimum distance between 2 segments

//	cpa_time() - compute the time of CPA for two tracks (in 3D)

//	cpa_distance() - compute the distance at CPA for two tracks (in 3D)	

// Application Level Routines
// fastBall() - get a fast approximation for the 2D bounding ball

// simple_Polygon() - test if a 2D Polygon is simple or not

// chainHull_2D() - Andrew's monotone chain 2D convex hull algorithm

// nearHull_2D() - the BFP fast approximate 2D convex hull algorithm

// simpleHull_2D() - Melkman's 2D simple polyline O(n) convex hull algorithm

// intersect2D_SegPoly() - intersect a 2D segment with a convex polygon

// polyMax_2D() - find a 2D polygon's max vertex in a specified direction

// dist2D_Poly_to_Line() - find the distance from a 2D polygon to a line

// tangent_PointPoly() - find a 2D polygon's exterior tangents (brute force)

// tangent_PointPolyC() - fast binary search for tangents to a convex polygon

// RLtangent_PolyPolyC() - get the RL tangent between two convex polygons

// poly_decimate() - remove vertices to get a smaller approximate polygon


namespace GlbGlobe
{
	// 算法类
	class GLB_DLLCLASS_EXPORT CGlbAlgorithm : public CGlbReference
	{
	public:
		CGlbAlgorithm(void);
		~CGlbAlgorithm(void);
		// 向量叉积
		//inline glbDouble dot(glbVector P0, glbVector P1)
		//{
		//	return (P0.x*P1.x+P0.y*P1.y+P0.z*P1.z);
		//}
		/*
		* @brief 计算两点间距离
		* @param [in] [x0,y0,z0] [x1,y1,z1] 两点坐标
		* @return 距离
		*/
		inline glbDouble distance(glbDouble x0,glbDouble y0,glbDouble z0, glbDouble x1,glbDouble y1,glbDouble z1)
		{
			return sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) + (z1-z0)*(z1-z0));
		}
		inline glbDouble distance(glbVector P0, glbVector P1)
		{
			return sqrt( (P0.x-P1.x)*(P0.x-P1.x) + (P0.y-P1.y)*(P0.y-P1.y) + (P0.z-P1.z)*(P0.z-P1.z) );
		}
		
		// isLeft(): test if a point is Left|On|Right of an infinite 2D line.
		//    Input:  three points P0, P1, and P2
		//    Return: >0 for P2 left of the line through P0 to P1
		//          =0 for P2 on the line
		//          <0 for P2 right of the line
		inline glbInt32 isLeft( glbDouble P0_x,glbDouble P0_y, glbDouble P1_x, glbDouble P1_y, glbDouble P2_x, glbDouble P2_y)
		{
			return ( (P1_x - P0_x) * (P2_y - P0_y)	- (P2_x - P0_x) * (P1_y - P0_y) );
		}
		inline glbInt32 isLeft( glbVector P0, glbVector P1, glbVector P2 )
		{
			return ( (P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y) );
		}
		// orientation2D_Triangle(): test the orientation of a 2D triangle
		//  Input:  three vertex points V0, V1, V2
		//  Return: >0 for counterclockwise
		//          =0 for none (degenerate)
		//          <0 for clockwise
		inline glbInt32 orientation2D_Triangle( glbDouble V0_x, glbDouble V0_y, glbDouble V1_x, glbDouble V1_y, glbDouble V2_x, glbDouble V2_y )
		{
			return isLeft(V0_x,V0_y, V1_x,V1_y, V2_x, V2_y);
		}
		inline glbInt32	orientation2D_Triangle( glbVector V0, glbVector V1, glbVector V2 )
		{
			return isLeft(V0, V1, V2);
		}
		// area2D_Triangle(): compute the area of a 2D triangle
		//  Input:  three vertex points V0, V1, V2
		//  Return: the (double) area of triangle T
		inline glbDouble area2D_Triangle( glbDouble V0_x, glbDouble V0_y, glbDouble V1_x, glbDouble V1_y, glbDouble V2_x, glbDouble V2_y )
		{
			return isLeft(V0_x,V0_y, V1_x,V1_y, V2_x, V2_y) / 2.0;
		}
		inline glbDouble area2D_Triangle( glbVector V0, glbVector V1, glbVector V2 )
		{
			return isLeft(V0, V1, V2) / 2.0;
		}
		// area2D_Polygon(): compute the area of a 2D polygon
		//  Input:  int n = the number of vertices in the polygon
		//          Point* V = an array of n+1 vertex points with V[n]=V[0]
		//  Return: the (float) area of the polygon
		glbDouble area2D_Polygon( glbInt32 n, glbVector* V );
		
		// area3D_Polygon(): compute the area of a 3D planar polygon
		//  Input:  int n = the number of vertices in the polygon
		//          Point* V = an array of n+1 points in a 2D plane with V[n]=V[0]
		//          Point N = a normal vector of the polygon's plane
		//  Return: the (float) area of the polygon
		glbDouble area3D_Polygon( glbInt32 n, glbVector* V, glbVector N);

		// closest2D_Point_to_Line(): find the closest 2D Point to a Line
		//     Input:  an array P[] of n points, and a Line L
		//     Return: the index i of the Point P[i] closest to L
		glbInt32	closest2D_Point_to_Line( glbVector P[], glbInt32 n, glbLine L);
		// dist_Point_to_Line(): get the distance of a point to a line
		//     Input:  a Point P and a Line L (in any dimension)
		//     Return: the shortest distance from P to L
		glbDouble	dist_Point_to_Line( glbVector P, glbLine L);

		// dist_Point_to_Segment(): get the distance of a point to a segment
		//     Input:  a Point P and a Segment S (in any dimension)
		//     Return: the shortest distance from P to S
		glbDouble dist_Point_to_Segment( glbVector P, glbSegment S);	

		// cn_PnPoly(): crossing number test for a point in a polygon
		//      Input:   P = a point,
		//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
		//      Return:  0 = outside, 1 = inside
		// This code is patterned after [Franklin, 2000]
		glbInt32	cn_PnPoly( glbVector P, glbVector* V, glbInt32 n );
		
		// wn_PnPoly(): winding number test for a point in a polygon
		//      Input:   P = a point,
		//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
		//      Return:  wn = the winding number (=0 only when P is outside)
		glbInt32	wn_PnPoly( glbVector P, glbVector* V, glbInt32 n );

		// dist_Point_to_Plane(): get distance (and perp base) from a point to a plane
		//    Input:  P  = a 3D point
		//            PL = a  plane with point V0 and normal n
		//    Output: *B = base point on PL of perpendicular from P
		//    Return: the distance from P to the plane PL
		glbDouble	dist_Point_to_Plane( glbVector P, glbPlane PL, glbVector* B);		
		
		// inSegment(): determine if a point is inside a segment
		//    Input:  a point P, and a collinear segment S
		//    Return: 1 = P is inside S
		//            0 = P is  not inside S
		glbInt32		inSegment( glbVector P, glbSegment S);
		
		// intersect2D_2Segments(): find the 2D intersection of 2 finite segments
		//    Input:  two finite segments S1 and S2
		//    Output: *I0 = intersect point (when it exists)
		//            *I1 =  endpoint of intersect segment [I0,I1] (when it exists)
		//    Return: 0=disjoint (no intersect)
		//            1=intersect  in unique point I0
		//            2=overlap  in segment from I0 to I1
		glbInt32	intersect2D_2Segments( glbSegment S1, glbSegment S2, glbVector* I0, glbVector* I1 );	

		// intersect3D_SegmentPlane(): find the 3D intersection of a segment and a plane
		//    Input:  S = a segment, and Pn = a plane = {Point V0;  Vector n;}
		//    Output: *I0 = the intersect point (when it exists)
		//    Return: 0 = disjoint (no intersection)
		//            1 =  intersection in the unique point *I0
		//            2 = the  segment lies in the plane
		glbInt32	intersect3D_SegmentPlane( glbSegment S, glbPlane Pn, glbVector* I );
		
		// intersect3D_2Planes(): find the 3D intersection of two planes
		//    Input:  two planes Pn1 and Pn2
		//    Output: *L = the intersection line (when it exists)
		//    Return: 0 = disjoint (no intersection)
		//            1 = the two  planes coincide
		//            2 =  intersection in the unique line *L
		glbInt32	intersect3D_2Planes( glbPlane Pn1, glbPlane Pn2, glbLine* L );
		// intersect3D_RayTriangle(): find the 3D intersection of a ray with a triangle
		//    Input:  a ray R, and a triangle T
		//    Output: *I = intersection point (when it exists)
		//    Return: -1 = triangle is degenerate (a segment or point)
		//             0 =  disjoint (no intersect)
		//             1 =  intersect in unique point I1
		//             2 =  are in the same plane
		glbInt32	intersect3D_RayTriangle( glbRay R, glbTriangle T, glbVector* I );
		// dist3D_Line_to_Line(): get the 3D minimum distance between 2 lines
		//    Input:  two 3D lines L1 and L2
		//    Return: the shortest distance between L1 and L2
		glbDouble	dist3D_Line_to_Line( glbLine L1, glbLine L2);

		// dist3D_Segment_to_Segment(): get the 3D minimum distance between 2 segments
		//    Input:  two 3D line segments S1 and S2
		//    Return: the shortest distance between S1 and S2
		glbDouble	dist3D_Segment_to_Segment( glbSegment S1, glbSegment S2);			

		// cpa_time(): compute the time of CPA for two tracks
		//    Input:  two tracks Tr1 and Tr2
		//    Return: the time at which the two tracks are closest
		glbDouble	cpa_time( glbTrack Tr1, glbTrack Tr2 );

		// cpa_distance(): compute the distance at CPA for two tracks
		//    Input:  two tracks Tr1 and Tr2
		//    Return: the distance for which the two tracks are closest
		glbDouble	cpa_distance( glbTrack Tr1, glbTrack Tr2 );		

		//===================================================================

	};
}


