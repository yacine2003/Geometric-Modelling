#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <GL/glew.h>
#include "myVector3D.h"

using namespace std;

myMesh::myMesh(void)
{
	/**** TODO ****/
}


myMesh::~myMesh(void)
{
	/**** TODO ****/
}

void myMesh::clear()
{
	for (unsigned int i = 0; i < vertices.size(); i++) if (vertices[i]) delete vertices[i];
	for (unsigned int i = 0; i < halfedges.size(); i++) if (halfedges[i]) delete halfedges[i];
	for (unsigned int i = 0; i < faces.size(); i++) if (faces[i]) delete faces[i];

	vector<myVertex *> empty_vertices;    vertices.swap(empty_vertices);
	vector<myHalfedge *> empty_halfedges; halfedges.swap(empty_halfedges);
	vector<myFace *> empty_faces;         faces.swap(empty_faces);
}

void myMesh::checkMesh()
{
	vector<myHalfedge *>::iterator it;
	for (it = halfedges.begin(); it != halfedges.end(); it++)
	{
		if ((*it)->twin == NULL)
			break;
	}
	if (it != halfedges.end())
		cout << "Error! Not all edges have their twins!\n";
	else cout << "Each edge has a twin!\n";
}


bool myMesh::readFile(std::string filename)
{
	string s, t, u;
	vector<int> faceids;
	myHalfedge **hedges;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	name = filename;

	map<pair<int, int>, myHalfedge *> twin_map;
	map<pair<int, int>, myHalfedge *>::iterator it;

	while (getline(fin, s))
	{
		stringstream myline(s);
		myline >> t;
		if (t == "g") {}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;
			myVertex *v = new myVertex();
			v->point = new myPoint3D(x, y, z);
			v->index = vertices.size();
			vertices.push_back(v);
		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			faceids.clear();
			while (myline >> u) {
				int vi = atoi((u.substr(0, u.find("/"))).c_str()) - 1;
				faceids.push_back(vi);
			}

			if (faceids.size() < 3) continue;

			myFace *f = new myFace();
			f->index = faces.size();
			faces.push_back(f);

			hedges = new myHalfedge*[faceids.size()];
			for (size_t i = 0; i < faceids.size(); i++) {
				hedges[i] = new myHalfedge();
				hedges[i]->source = vertices[faceids[i]];
				hedges[i]->adjacent_face = f;
				hedges[i]->index = halfedges.size();
				halfedges.push_back(hedges[i]);

				if (vertices[faceids[i]]->originof == NULL) {
					vertices[faceids[i]]->originof = hedges[i];
				}
			}

			f->adjacent_halfedge = hedges[0];

			for (size_t i = 0; i < faceids.size(); i++) {
				hedges[i]->next = hedges[(i + 1) % faceids.size()];
				hedges[i]->prev = hedges[(i + faceids.size() - 1) % faceids.size()];

				int v1 = faceids[i];
				int v2 = faceids[(i + 1) % faceids.size()];

				twin_map[make_pair(v1, v2)] = hedges[i];

				it = twin_map.find(make_pair(v2, v1));
				if (it != twin_map.end()) {
					hedges[i]->twin = it->second;
					it->second->twin = hedges[i];
				}
			}
			delete[] hedges;
		}
	}

	checkMesh();
	normalize();

	return true;
}


void myMesh::computeNormals()
{
	for (unsigned int i = 0; i < faces.size(); i++) {
		if (faces[i] != NULL) faces[i]->computeNormal();
	}
	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i] != NULL) vertices[i]->computeNormal();
	}
}

void myMesh::normalize()
{
	if (vertices.size() < 1) return;

	int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->point->X < vertices[tmpxmin]->point->X) tmpxmin = i;
		if (vertices[i]->point->X > vertices[tmpxmax]->point->X) tmpxmax = i;

		if (vertices[i]->point->Y < vertices[tmpymin]->point->Y) tmpymin = i;
		if (vertices[i]->point->Y > vertices[tmpymax]->point->Y) tmpymax = i;

		if (vertices[i]->point->Z < vertices[tmpzmin]->point->Z) tmpzmin = i;
		if (vertices[i]->point->Z > vertices[tmpzmax]->point->Z) tmpzmax = i;
	}

	double xmin = vertices[tmpxmin]->point->X, xmax = vertices[tmpxmax]->point->X,
		ymin = vertices[tmpymin]->point->Y, ymax = vertices[tmpymax]->point->Y,
		zmin = vertices[tmpzmin]->point->Z, zmax = vertices[tmpzmax]->point->Z;

	double scale = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	scale = scale > (zmax - zmin) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->point->X -= (xmax + xmin) / 2;
		vertices[i]->point->Y -= (ymax + ymin) / 2;
		vertices[i]->point->Z -= (zmax + zmin) / 2;

		vertices[i]->point->X /= scale;
		vertices[i]->point->Y /= scale;
		vertices[i]->point->Z /= scale;
	}
}


void myMesh::splitFaceTRIS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}

void myMesh::splitEdge(myHalfedge *e1, myPoint3D *p)
{

	/**** TODO ****/
}

void myMesh::splitFaceQUADS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}


void myMesh::subdivisionCatmullClark()
{
	/**** TODO ****/
}

void myMesh::simplify()
{
	/**** TODO ****/
}

void myMesh::simplify(myVertex *)
{
	/**** TODO ****/
}

void myMesh::triangulate()
{
	int original_size = faces.size();
	for (int i = 0; i < original_size; ++i) {
		if (faces[i] != NULL) {
			triangulate(faces[i]);
		}
	}
}

//return false if already triangle, true othewise.
bool myMesh::triangulate(myFace *f)
{
	if (f == NULL || f->adjacent_halfedge == NULL) return false;

	vector<myHalfedge*> bnd;
	myHalfedge* curr = f->adjacent_halfedge;
	do {
		bnd.push_back(curr);
		curr = curr->next;
	} while (curr != f->adjacent_halfedge && curr != NULL);

	int N = bnd.size();
	if (N <= 3) return false;

	double sumX = 0, sumY = 0, sumZ = 0;
	for (int i = 0; i < N; ++i) {
		sumX += bnd[i]->source->point->X;
		sumY += bnd[i]->source->point->Y;
		sumZ += bnd[i]->source->point->Z;
	}

	myVertex *vc = new myVertex();
	vc->point = new myPoint3D(sumX / N, sumY / N, sumZ / N);
	vc->index = vertices.size();
	vertices.push_back(vc);

	vector<myHalfedge*> In(N), Out(N);
	for (int i = 0; i < N; ++i) {
		In[i] = new myHalfedge();
		Out[i] = new myHalfedge();
		In[i]->source = bnd[(i + 1) % N]->source;
		Out[i]->source = vc;

		In[i]->index = halfedges.size(); halfedges.push_back(In[i]);
		Out[i]->index = halfedges.size(); halfedges.push_back(Out[i]);
	}

	vc->originof = Out[0];

	for (int i = 0; i < N; ++i) {
		myHalfedge *he_i = bnd[i];
		myHalfedge *in_i = In[i];
		myHalfedge *out_i = Out[i];

		he_i->next = in_i; in_i->prev = he_i;
		in_i->next = out_i; out_i->prev = in_i;
		out_i->next = he_i; he_i->prev = out_i;

		in_i->twin = Out[(i + 1) % N];
		Out[(i + 1) % N]->twin = in_i;

		myFace *face_i = (i == 0) ? f : new myFace();
		if (i > 0) {
			face_i->index = faces.size();
			faces.push_back(face_i);
		}
		face_i->adjacent_halfedge = he_i;

		he_i->adjacent_face = face_i;
		in_i->adjacent_face = face_i;
		out_i->adjacent_face = face_i;
	}

	return true;
}

