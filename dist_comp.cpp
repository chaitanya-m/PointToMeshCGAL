/*

================================================================================
The question: 
1.The wording asks for an [Efficient] closest-point-[ON]-mesh query.
2.But the sample interface and comment in it ask for closest-point-[TO]-mesh.
(presumably to the closest point in a point cloud from a query point on mesh)

Efficiency: 
For either problem, I can think of corner cases that would force us 
to evaluate distance to every polygon in the mesh. 

(Short of writing a bounding-box framework...)
================================================================================

Started off with the example at http://doc.cgal.org/latest/AABB_tree/index.html 
by Author(s) : Camille Wormser, Pierre Alliez

The example computes the closest point on a mesh consisting of triangles (stored in a list) using CGAL functions. 
The CGAL example translates the list of trinagles into a tree of Axis-Aligned Bounding-Boxes(AABBs). 
There are convenient distance functions that compute closest distance.

================================================================================
Task 1 would require me not to use such a library function, but to implement the 
algorithm myself, as this would be the core algorithm.

Task 2 would simply require me to find the closest point to the mesh 
as efficiently as possible, so I could use this function, as the core algorithm
would involve evaluating points.
================================================================================


++++++++++++++++++++++++++++++
Problem 1 Notes
++++++++++++++++++++++++++++++

We can't use CGAL's AABB trees as we like. The interface is quite limited.

Problem synopsis: 
1. There exist a mesh M and a fixed-point P. Find the closest point P'
on the mesh to the point P.
OR
2. There exist a mesh M and a set of fixed points S. Find the closest point P in S to mesh M.

Problem specification: I shall interpret the problem as follows:

We are working in a 3-dimensional Euclidean space.

Mesh M is a disjoint forest (!) (overgeneralisation) of triangles, where:
A triangle is an area enclosed by three non-collinear points, including vertices, edges, 
and enclosed area("face").

Tree structure: 
We shall overgeneralize what is acceptable.
Triangles may be connected to each other in any way- they may share an edge, they may share
just one vertex(!!) or may just have intersecting faces(!!!). We will allow for ridiculous meshes.

However, the mesh we actually generate will be one that has triangles sharing a whole edge.


-----------
Efficiency
-----------

Algorithms and data structures

What would be the best data structure to store our triangles?

We can't really skip any triangle in the mesh given the conditions we've set. Consider a triangle ABC which contains 
both the closest vertex C and farthest vertices A and B to our point P. 

The triangle preceding it, let's call it ABD, may be the furthest triangle, AB may be the farthest edge, but 
ABC is closer to P than is ABD. No increasing or decreasing pattern here, nothing to prune in our search.

We'll have to evaluate distance to each triangle in the mesh... unless we can slice the volume somehow
(which is what the AABB's seem to do). We can't do better than O(n). No matter what heuristic we use to 
'predict' closer triangles, there is always a corner case that breaks it. We cannot prune. We cannot avoid 
an exhaustive search.

Distance to a triangle: can be distance to a vertex, edge, or face.

Efficiency is limited to avoiding re-evaluating distances and CPU/memory advantages gained from using one data structure over another.

Using a vector backed by an array to store triangles is fine- our mesh is static. 
It gives us a bit of an efficiency jump over using a List.

There's no point using other kinds of graphs...


Their code is kept for comparison, to test how different their results are from mine.


*/

#include <iostream>
#include <list>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>

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
int main()
{
Point a(1.0, 0.0, 0.0);
Point b(0.0, 1.0, 0.0);
Point c(0.0, 0.0, 1.0);
Point d(0.0, 0.0, 0.0);
std::list<Triangle> triangles;
triangles.push_back(Triangle(a,b,c));
triangles.push_back(Triangle(a,b,d));
triangles.push_back(Triangle(a,d,c));
// constructs AABB tree
Tree tree(triangles.begin(),triangles.end());
// counts #intersections
Ray ray_query(a,b);
std::cout << tree.number_of_intersected_primitives(ray_query)
<< " intersections(s) with ray query" << std::endl;
// compute closest point and squared distance
Point point_query(2.0, 2.0, 2.0);
Point closest_point = tree.closest_point(point_query);
std::cerr << "closest point is: " << closest_point << std::endl;
FT sqd = tree.squared_distance(point_query);
std::cout << "squared distance: " << sqd << std::endl;
return EXIT_SUCCESS;
}

