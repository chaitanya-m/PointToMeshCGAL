
#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include "TriangleMesh.h"
#include "ClosestPointQuery.h"
#include "Common.h"

class TestClosestPointQuery
{
    public:
        bool Test1();

    private:
        TriangleMesh triangleMesh;
};


bool TestClosestPointQuery::Test1()
{

    //Generate mesh, generate queries, query mesh, get results
    TriangleMesh m = TriangleMesh();
    ClosestPointQuery query(m);

    Point queryPoint(2.0, 2.0, 2.0);
    Point result = query(queryPoint);


    // constructs AABB tree
    Triangles *triangles = m.getTriangles();
    Tree tree(triangles->begin(), triangles->end());
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


#endif
