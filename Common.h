/*
Started off with the example at http://doc.cgal.org/latest/AABB_tree/index.html
by Author(s) : Camille Wormser, Pierre Alliez
*/

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <stdlib.h>
#include <CGAL/Simple_cartesian.h>
#include <queue>
#include <map>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>
#include <cfloat>

#define vertexMin 100   //better than a static declaration within class in terms of lookup efficiency
#define vertexMax 5000  //but might not be a logical placement

typedef CGAL::Simple_cartesian<double> K;
typedef K::FT FT;
typedef K::Ray_3 Ray;
typedef K::Line_3 Line;
typedef K::Point_3 Point;
typedef K::Triangle_3 Triangle;
typedef std::list<Triangle>::iterator Iterator;
typedef CGAL::AABB_triangle_primitive<K, Iterator> Primitive;
typedef CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
typedef CGAL::AABB_tree<AABB_triangle_traits> Tree;

typedef AABB_triangle_traits::Closest_point ClosestPointToTriangle;
typedef AABB_triangle_traits::Squared_distance SquaredDistanceBetweenPoints;
typedef std::pair<Point, Point> Edge;
typedef std::list<Triangle> Triangles;
typedef std::list<Primitive> Primitives;
typedef std::queue<std::pair<Point, Point> > EdgeQueue;

#endif
