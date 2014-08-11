//Somewhat lax with function abstraction in this file

#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include "TriangleMesh.h"
#include "ClosestPointQuery.h"
#include "Common.h"

class TestClosestPointQuery
{
    public:
        bool Test1();
        bool Test2();

    private:
        TriangleMesh triangleMesh;
        Point CGAL_AABBTreeQuery(Point& queryPoint, TriangleMesh& m) const;
};

Point TestClosestPointQuery::CGAL_AABBTreeQuery(Point& queryPoint, TriangleMesh& m) const
{

    Triangles* triangles = m.getTriangles();

    // construct AABB tree
    Tree tree(triangles->begin(), triangles->end());

    // return closest point
    return tree.closest_point(queryPoint);

}

bool TestClosestPointQuery::Test1()
{

    std::cout << std::endl << "Running Test 1" << std::endl;

    //Generate mesh, generate queries, query mesh, get results
    TriangleMesh m = TriangleMesh();
    ClosestPointQuery query(m);

    Point queryPoint(2.0, 2.0, 2.0);
    Point result = query(queryPoint);
    Point expectedResult = CGAL_AABBTreeQuery(queryPoint, m);

    std::cout << "Queried point: " << queryPoint << std::endl;
    std::cout << "Computed closest point is: " << result << std::endl;
    std::cout << "Expected closest point is: " << expectedResult << std::endl;

    if (result == expectedResult)
    {
        std::cout << "Test 1: Result computed matches CGAL AABBTree result" << std::endl << std::endl;
        return true;
        //This should strictly be a floating point comparison within error bounds- the internal
        //implementation for Point may or may not do this
    }
    std::cerr << "Test 1 FAILED" << std::endl;
    return false;
}

bool TestClosestPointQuery::Test2()
{

#if !defined(DEBUG)

    std::cerr << "Test 2 MUST be run with DEBUG defined. It relies on a function that is not in the release interface. NOT running Test 2.\n";
    return false;
#else

    std::cout << std::endl << "Running test 2 " << std::endl;

    //Generate mesh, generate queries, query mesh, get results
    TriangleMesh m = TriangleMesh();
    ClosestPointQuery query(m);

    Point queryPoint(5000.2, 2000.2, 2000.3);
    Point queryPoint2(5000.2, 2000.2, 2000.3);


    Point result = query(queryPoint);
    Point expectedResult = CGAL_AABBTreeQuery(queryPoint, m);

    std::cout << "Queried point: " << queryPoint << std::endl;
    std::cout << "Computed closest point is: " << result << std::endl;
    std::cout << "Expected closest point is: " << expectedResult << std::endl;

    if (result == expectedResult)
    {
        std::cout << "Test 2: Result computed matches CGAL AABBTree result" << std::endl << std::endl;
    }
    else
    {
        return false;
    }


    Point result2 = query(queryPoint2);
    std::cout << "Queried point: " << queryPoint << std::endl;

    if (query.getMapSize() == 1) 
    {
        std::cout << "Point previously queried. Result successfully retrieved from table: " << result2 << std::endl << std::endl;
        return true;
    }

    std::cerr << "Test 2 FAILED";
    return false;

#endif

}





#endif
