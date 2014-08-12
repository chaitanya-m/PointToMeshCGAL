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
#define vertexDistDivisor 100.0   //For floating point values
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


void addTriangleAndEdges(TriangleMesh* triangleMesh, EdgeQueue &edges, const Edge &currentEdge)
{
    bool triangleIsDegenerate = true;
    Point a = currentEdge.first;
    Point b = currentEdge.second;
    Point p = randomPoint();
    Triangle t;

    while(triangleIsDegenerate) // Three Collinear points
    {
        t = Triangle(a, b, p);
        triangleIsDegenerate = t.is_degenerate();
    }

    edges.push(Edge(a, p));
    edges.push(Edge(b, p));
    triangleMesh->push_back(t);

}


TriangleMesh* meshGen()
{

    TriangleMesh* triangleMesh = new TriangleMesh();
    srand(0);   //Define seed for testing...
    //Generate a random mesh with vertexMin - vertexMax vertices

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

        addTriangleAndEdges(triangleMesh, edges, currentEdge);
        addTriangleAndEdges(triangleMesh, edges, currentEdge);

        edges.pop();
    }

    return triangleMesh;
}


class ClosestPointQuery
{
    public:

        ClosestPointQuery(TriangleMesh* m);
        //Type primitive unable to support const iterators? Should be looked into, this should ideally be const.
        Point operator() (const Point& queryPoint) const;
        // Return closest point on mesh to query point

    private:
        TriangleMesh* triangleMesh;
        std::map<Point, Point>* queryToClosestPointMap;
};

ClosestPointQuery::ClosestPointQuery(TriangleMesh* m)
{
    triangleMesh = m;
    queryToClosestPointMap = new std::map<Point, Point>();
}

Point ClosestPointQuery::operator() (const Point& queryPoint) const
{
    PrimitiveMesh primitiveMesh;
    //The triangle primitive can either store our triangle internally or reconstruct it on-the-fly, depending on space/lookup tradeoffs.

    Point maxPoint(LDBL_MAX, LDBL_MAX, LDBL_MAX);
    //Assumption: Any query is closer than point_max. Our mesh is not near point_max.

    ClosestPointToTriangle getClosestPoint;
    SquaredDistanceBetweenPoints getSquaredDistance;
    //This is an O(n) find(). Sorting points by in some arbitrary fashion will make it more efficient (Olog(n))
    //They are lexicographically sorted by default, as the '>', '<', ==, != comparators are all defined for the Point class.
    Point closestPointOnTriangle;
    Primitive p;
    FT squaredDistance;
    FT closestPointSquaredDistance = LDBL_MAX;
    Point closestPointOnMesh;

    for(TriangleMesh::iterator iter = triangleMesh->begin(); iter != triangleMesh->end(); iter++)
    {
        p = Primitive(iter);
        //doesn't support const iterators?
        primitiveMesh.push_back(p);
        closestPointOnTriangle = getClosestPoint(queryPoint, p, maxPoint);
        squaredDistance = getSquaredDistance(queryPoint, closestPointOnTriangle);
        if (squaredDistance < closestPointSquaredDistance)
        {
            closestPointSquaredDistance = squaredDistance;
            closestPointOnMesh = closestPointOnTriangle;
        }
        //We'll use point_max to save CPU cycles retrieving points on triangle
    //Store Query-Result pairs for quicker access later
    }
    queryToClosestPointMap->insert(std::pair<Point, Point>(queryPoint, closestPointOnMesh));
    return closestPointOnMesh;
}

class TestClosestPointQuery
{
    public:
        bool Test1();

    private:
        TriangleMesh* triangleMesh;
};

bool TestClosestPointQuery::Test1()
{

    //Generate mesh, generate queries, query mesh, get results
    TriangleMesh* m = meshGen();
    ClosestPointQuery query(m);

    Point queryPoint(2.0, 2.0, 2.0);
    Point result = query(queryPoint);


    // constructs AABB tree
    Tree tree(m->begin(), m->end());
    // compute closest point and squared distance
    Point expectedResult = tree.closest_point(queryPoint);

    std::cout << "Computed closest point is: " << result << std::endl;
    std::cout << "Expected closest point is: " << expectedResult << std::endl;

    if (result == expectedResult)
    {
        std::cout << "Test 1: Result is valid\n";
        return true;
        //This should strictly be a floating point comparison within error bounds- the internal
        //implementation for Point may or may not do this
    }
    std::cerr << "Test 1 FAILED";
    return false;
}

int main()
{
    TestClosestPointQuery testSuite;
    testSuite.Test1();
}

