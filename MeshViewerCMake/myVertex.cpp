#include "myVertex.h"
#include "myVector3D.h"
#include "myHalfedge.h"
#include "myFace.h"

myVertex::myVertex(void)
{
	point = NULL;
	originof = NULL;
	normal = new myVector3D(1.0,1.0,1.0);
}

myVertex::~myVertex(void)
{
	if (normal) delete normal;
}


void myVertex::computeNormal()
{
	if (originof == NULL) return;

	myVector3D n(0.0, 0.0, 0.0);
	myHalfedge* he = originof;

	do {
		if (he->adjacent_face != NULL && he->adjacent_face->normal != NULL) {
			n += *(he->adjacent_face->normal);
		}
		
		if (he->twin == NULL) break;
		he = he->twin->next;
		
	} while (he != originof && he != NULL);

	n.normalize();
	if (normal != NULL) {
		*normal = n;
	}
}

