/*
Started off with the example at http://doc.cgal.org/latest/AABB_tree/index.html
by Author(s) : Camille Wormser, Pierre Alliez
*/

#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "Common.h"

class TriangleMesh
{
    public:
        TriangleMesh();
        static void addTriangleAndEdges(Triangles* triangles, EdgeQueue &edges, const Edge &currentEdge);
        Triangles* getTriangles(); //tried to make this const but ran into issues with CGAL Primitives
    private:
        Triangles* triangles;
        static const int vertexDistOrigMax = 100000;   //Max distance of any vertex from origin, allowing for two decimal places
        static const double vertexDistDivisor = 100.0; //For floating point values
        static Point randomPoint();
};


Triangles* TriangleMesh::getTriangles()
{
    return triangles;
}


Point TriangleMesh::randomPoint()
{
    double coords[3];
    for(int j = 0; j < 3; j ++)
    {
        coords[j] = (rand() % vertexDistOrigMax) / vertexDistDivisor;
    }
    return Point(coords[0], coords[1], coords[2]);

}


void TriangleMesh::addTriangleAndEdges(Triangles* triangles, EdgeQueue &edges, const Edge &currentEdge)
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
    triangles->push_back(t);

}


TriangleMesh::TriangleMesh()
{

    triangles = new Triangles();
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

        TriangleMesh::addTriangleAndEdges(triangles, edges, currentEdge);
        TriangleMesh::addTriangleAndEdges(triangles, edges, currentEdge);

        edges.pop();
    }

}

#endif
