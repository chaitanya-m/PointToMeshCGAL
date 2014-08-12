/*
Started off with the example at http://doc.cgal.org/latest/AABB_tree/index.html
by Author(s) : Camille Wormser, Pierre Alliez
*/

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
#define vertexMax 5000

#define vertexDistOrigMax 100000 //Max distance of any vertex from origin, allowing for two decimal places
#define vertexDistDivisor 100   //For floating point values
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
typedef std::list<Triangle> TriangleMesh;
typedef std::list<Primitive> PrimitiveMesh;
typedef std::queue<std::pair<Point, Point> > EdgeQueue;
typedef std::pair<Point, Point> Edge;

Point randomPoint()
{
    double coords[3];
    for(int j = 0; j < 3; j ++)
    {
        coords[j] = (rand() % vertexDistOrigMax) / vertexDistDivisor;
    }
    return Point(coords[0], coords[1], coords[2]);

}


TriangleMesh meshGen()
{
    srand(0);   //Define seed for testing...
    //Let's generate a random mesh with vertexMin - vertexMax vertices
    TriangleMesh triangleMesh;

    int numVertices = rand() % vertexMax + vertexMin;
    std::vector<Point> vertices(numVertices); //Fixed backing data structure size. Save on address lookups.
    EdgeQueue edges;

    //Create a vertex pair first (an edge)
    Point p[2];

    for(int i = 0; i < 2; i ++)
    {
        p[i] = randomPoint();
        vertices.push_back(p[i]);
    }
    edges.push(Edge(p[0], p[1]));
    //create the triangle mesh
    for(int i = 0; i < numVertices - 2; i++) // we've just created 2 vertices
    {
        //Pick the first edge (vertex pair). Create two triangles off it, then retire it 
        Edge currentEdge = edges.front();
        Point p1 = randomPoint();        Point p2 = randomPoint();
        Point a = currentEdge.first;        Point b = currentEdge.second;

        Triangle t1(a, b, p1);        Triangle t2(a, b, p2);

        edges.push(Edge(a, p1));        edges.push(Edge(b, p1));
        edges.push(Edge(a, p2));        edges.push(Edge(b, p2));

        edges.pop();
        triangleMesh.push_back(t1);        triangleMesh.push_back(t2);
    }
    return triangleMesh;
}


int main()
{

    //Generate mesh
    //Generate queries
    //query mesh
    //print results
    TriangleMesh triangleMesh = meshGen();
    PrimitiveMesh primitiveMesh;

/*
    Point a(1.0, 0.0, 0.0);
    Point b(0.0, 1.0, 0.0);
    Point c(0.0, 0.0, 1.0);
    Point d(0.0, 0.0, 0.0);

    triangleMesh.push_back(Triangle(a,b,c));
    triangleMesh.push_back(Triangle(a,b,d));
    triangleMesh.push_back(Triangle(a,d,c));
*/
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

    for(std::list<Triangle>::iterator iter = triangleMesh.begin(); iter != triangleMesh.end(); iter++)
    {
        p = Primitive(iter);

        primitiveMesh.push_back(p);
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
Tree tree(triangleMesh.begin(), triangleMesh.end());

// compute closest point and squared distance
Point closest_point = tree.closest_point(queryPoint);
std::cerr << "AABB_tree closest point is: " << closest_point << std::endl;
FT sqd = tree.squared_distance(queryPoint);
std::cout << "AABB_tree squared distance: " << sqd << std::endl;
return EXIT_SUCCESS;
}

