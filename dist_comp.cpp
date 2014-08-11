/*
Started off with the example at http://doc.cgal.org/latest/AABB_tree/index.html 
by Author(s) : Camille Wormser, Pierre Alliez
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

typedef AABB_triangle_traits::Closest_point ClosestPointToTriangle;


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

    Point point_query(2.0, 2.0, 2.0);
    Point point_random(999999.0, 999999.0, 999999.0);

    /*
    So far so good. I'll need to construct an AABB_tree, but rather than use it's 
    closest point function, I'll use that of it's component triangle primitives (so
    I can still write the core algorithm).
    The triangle primitive can either store our triangle internally or reconstruct it on-the-fly.
    */

    // constructs AABB tree
    Tree tree(triangles.begin(),triangles.end());
    Primitive p(triangles.begin());

    ClosestPointToTriangle cp;
    
//    Point result = cp(point_query, tree.front(), point_random);
      Point result = cp(point_query, p, point_random);
      std::cerr << "------->" << result;

// counts #intersections
Ray ray_query(a,b);
std::cout << tree.number_of_intersected_primitives(ray_query)
<< " AABB_tree intersections(s) with ray query" << std::endl;
// compute closest point and squared distance
Point closest_point = tree.closest_point(point_query);
std::cerr << "AABB_tree closest point is: " << closest_point << std::endl;
FT sqd = tree.squared_distance(point_query);
std::cout << "AABB_tree squared distance: " << sqd << std::endl;
return EXIT_SUCCESS;
}

