#include "myFace.h"
#include "myVector3D.h"
#include "myHalfedge.h"
#include "myVertex.h"
#include <GL/glew.h>

myFace::myFace(void)
{
	adjacent_halfedge = NULL;
	normal = new myVector3D(1.0, 1.0, 1.0);
}

myFace::~myFace(void)
{
	if (normal) delete normal;
}

void myFace::computeNormal()
{
	if (adjacent_halfedge == NULL) return;
	
	myVertex* p1 = adjacent_halfedge->source;
	myVertex* p2 = adjacent_halfedge->next->source;
	myVertex* p3 = adjacent_halfedge->next->next->source;

	if (p1 && p2 && p3 && p1->point && p2->point && p3->point) {
		normal->setNormal(p1->point, p2->point, p3->point);
	}
}
