/*
Started off with the example at http://doc.cgal.org/latest/AABB_tree/index.html
by Author(s) : Camille Wormser, Pierre Alliez
*/

#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <list>
#include <map>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>
#include <cfloat>

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

int main()
{

    Point a(1.0, 0.0, 0.0);
    Point b(0.0, 1.0, 0.0);
    Point c(0.0, 0.0, 1.0);
    Point d(0.0, 0.0, 0.0);

    std::list<Triangle> mesh;
    std::list<Primitive> primitives;
    mesh.push_back(Triangle(a,b,c));
    mesh.push_back(Triangle(a,b,d));
    mesh.push_back(Triangle(a,d,c));

    Point queryPoint(2.0, 2.0, 2.0);
    Point point_max(LDBL_MAX, LDBL_MAX, LDBL_MAX);
    //Assumption: Any query is closer than point_max. Our mesh is not near point_max.
    /*
    So far so good. I'll need to construct an AABB_tree, but rather than use it's
    closest point function, I'll use that of its component triangle primitives (so
    I can still write the core algorithm).
    The triangle primitive can either store our triangle internally or reconstruct
    it on-the-fly, depending on space/lookup tradeoffs.
    */

    ClosestPointToTriangle getClosestPoint;
    SquaredDistanceBetweenPoints getSquaredDistance;
    std::map<Point, Point> queryToClosestPointMap; 
//This is an O(n) find(). Sorting points by in some arbitrary fashion will make it more efficient (Olog(n))
//They are lexicographically sorted by default, as the '>', '<', ==, != comparators are all defined for the Point class.
    Point closestPointOnTriangle; //Not the right place for these declarations, but saves on unnecessary reallocation in loop
    Primitive p;
    FT squaredDistance;
    FT closestPointSquaredDistance = LDBL_MAX;
    Point closestPointOnMesh;

    for(std::list<Triangle>::iterator iter = mesh.begin(); iter != mesh.end(); iter++)
    {
        p = Primitive(iter);
        primitives.push_back(p);
        closestPointOnTriangle = getClosestPoint(queryPoint, p, point_max);
        squaredDistance = getSquaredDistance(queryPoint, closestPointOnTriangle);
        if (squaredDistance < closestPointSquaredDistance)
        {
            closestPointSquaredDistance = squaredDistance;
            closestPointOnMesh = closestPointOnTriangle;
        }
        //We'll use point_max to save CPU cycles retrieving points
    //Store Query-Result pairs for quicker access later
    }
    queryToClosestPointMap.insert(std::pair<Point, Point>(queryPoint, closestPointOnMesh));

        std::cerr << "------->" << closestPointSquaredDistance <<"------" << closestPointOnMesh << std::endl;



// constructs AABB tree
Tree tree(mesh.begin(),mesh.end());

// counts #intersections
Ray ray_query(a,b);
std::cout << tree.number_of_intersected_primitives(ray_query)
<< " AABB_tree intersections(s) with ray query" << std::endl;
// compute closest point and squared distance
Point closest_point = tree.closest_point(queryPoint);
std::cerr << "AABB_tree closest point is: " << closest_point << std::endl;
FT sqd = tree.squared_distance(queryPoint);
std::cout << "AABB_tree squared distance: " << sqd << std::endl;
return EXIT_SUCCESS;
}

