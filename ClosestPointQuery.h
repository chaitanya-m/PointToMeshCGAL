
#ifndef CLOSEST_POINT_QUERY_H
#define CLOSEST_POINT_QUERY_H

#include "TriangleMesh.h"
#include "Common.h"

class ClosestPointQuery
{
    public:

        ClosestPointQuery(TriangleMesh& m);
        //Type primitive unable to support const iterators? Should be looked into, this should ideally be const.

        Point operator() (const Point& queryPoint) const;
        // Return closest point on mesh to query point
#ifdef DEBUG
        inline int getMapSize()     //Should only be used for debugging/testing...
        {
            return queryToClosestPointMap->size();
        }
#endif
    private:
        TriangleMesh* triangleMesh;
        Triangles* triangles;
        std::map<Point, Point>* queryToClosestPointMap;
};


ClosestPointQuery::ClosestPointQuery(TriangleMesh& m)
{
    triangleMesh = &m;
    triangles = m.getTriangles();
    queryToClosestPointMap = new std::map<Point, Point>();
}


Point ClosestPointQuery::operator() (const Point& queryPoint) const
{

    std::map<Point, Point>::iterator mapIter = queryToClosestPointMap->find(queryPoint);
    if (mapIter != queryToClosestPointMap->end())
    {
        return mapIter->second;
    }
    //This is an O(n) find(). Sorting points by in some arbitrary fashion will make it more efficient (Olog(n))
    //They are lexicographically sorted by default, as the '>', '<', ==, != comparators are all defined for the Point class.

    Primitives primitives;
    //The triangle primitive can either store our triangle internally or reconstruct it on-the-fly, depending on space/lookup tradeoffs.

    Point maxPoint(LDBL_MAX, LDBL_MAX, LDBL_MAX);
    //Assumption: Any query is closer than point_max. Our mesh is not near point_max.

    ClosestPointToTriangle getClosestPoint;
    SquaredDistanceBetweenPoints getSquaredDistance;

    Point closestPointOnTriangle;
    Primitive p;
    FT squaredDistance;
    FT closestPointSquaredDistance = LDBL_MAX;
    Point closestPointOnMesh;

    for(Triangles::iterator iter = triangles->begin(); iter != triangles->end(); iter++)
    {
        p = Primitive(iter);        //doesn't support const iterators?
        primitives.push_back(p);
        closestPointOnTriangle = getClosestPoint(queryPoint, p, maxPoint);
        squaredDistance = getSquaredDistance(queryPoint, closestPointOnTriangle);
        if (squaredDistance < closestPointSquaredDistance)
        {
            closestPointSquaredDistance = squaredDistance;
            closestPointOnMesh = closestPointOnTriangle;
        }
        //We'll use maxPoint to save CPU cycles retrieving points on triangle
    //Store Query-Result pairs for quicker access later
    }
    queryToClosestPointMap->insert(std::pair<Point, Point>(queryPoint, closestPointOnMesh));
    return closestPointOnMesh;
}

#endif
