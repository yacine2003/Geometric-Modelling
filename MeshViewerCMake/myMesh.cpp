#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <cmath>
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


void myMesh::buildRevolutionSurface(std::vector<myPoint3D> profile, int steps)
{
	clear();
	int n = profile.size();
	double pi = 3.14159265358979;

	for (int i = 0; i < steps; i++) {
		double a = 2.0 * pi * i / steps;
		for (int j = 0; j < n; j++) {
			myVertex* v = new myVertex();
			v->point = new myPoint3D(profile[j].X * cos(a), profile[j].Y, profile[j].X * sin(a));
			v->index = vertices.size();
			vertices.push_back(v);
		}
	}

	std::vector<myHalfedge*> he(steps * (n - 1) * 4, NULL);

	for (int i = 0; i < steps; i++) {
		for (int j = 0; j < n - 1; j++) {
			int fi = i * (n - 1) + j;
			myFace* f = new myFace();
			f->index = faces.size();
			faces.push_back(f);

			int idx[4] = {
				i * n + j,
				i * n + j + 1,
				((i + 1) % steps) * n + j + 1,
				((i + 1) % steps) * n + j
			};

			for (int k = 0; k < 4; k++) {
				myHalfedge* h = new myHalfedge();
				h->source = vertices[idx[k]];
				h->adjacent_face = f;
				h->index = halfedges.size();
				halfedges.push_back(h);
				he[fi * 4 + k] = h;
				if (vertices[idx[k]]->originof == NULL)
					vertices[idx[k]]->originof = h;
			}

			f->adjacent_halfedge = he[fi * 4];
			for (int k = 0; k < 4; k++) {
				he[fi * 4 + k]->next = he[fi * 4 + (k + 1) % 4];
				he[fi * 4 + k]->prev = he[fi * 4 + (k + 3) % 4];
			}
		}
	}

	for (int i = 0; i < steps; i++) {
		for (int j = 0; j < n - 1; j++) {
			int fi = i * (n - 1) + j;
			int fp = ((i - 1 + steps) % steps) * (n - 1) + j;
			he[fi * 4]->twin     = he[fp * 4 + 2];
			he[fp * 4 + 2]->twin = he[fi * 4];
			if (j < n - 2) {
				int fa = i * (n - 1) + j + 1;
				he[fi * 4 + 1]->twin = he[fa * 4 + 3];
				he[fa * 4 + 3]->twin = he[fi * 4 + 1];
			}
		}
	}

	normalize();
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

	int N = 0;
	myHalfedge* curr = f->adjacent_halfedge;
	do {
		N++;
		curr = curr->next;
	} while (curr != f->adjacent_halfedge && curr != NULL);

	if (N <= 3) return false;

	// 1. Compute robust normal for the polygon (Newell's method)
	myVector3D normal(0.0, 0.0, 0.0);
	curr = f->adjacent_halfedge;
	do {
		myPoint3D* p1 = curr->source->point;
		myPoint3D* p2 = curr->next->source->point;
		normal.dX += (p1->Y - p2->Y) * (p1->Z + p2->Z);
		normal.dY += (p1->Z - p2->Z) * (p1->X + p2->X);
		normal.dZ += (p1->X - p2->X) * (p1->Y + p2->Y);
		curr = curr->next;
	} while (curr != f->adjacent_halfedge);
	normal.normalize();

	while (N > 3) {
		myHalfedge* ear_edge = NULL;

		// 2. Find an ear
		curr = f->adjacent_halfedge;
		for (int i = 0; i < N; ++i) {
			myHalfedge* prev = curr->prev;
			myHalfedge* next = curr->next;
			myPoint3D* p_prev = prev->source->point;
			myPoint3D* p_curr = curr->source->point;
			myPoint3D* p_next = next->source->point;

			// Check if angle is convex
			myVector3D v1(p_curr->X - p_prev->X, p_curr->Y - p_prev->Y, p_curr->Z - p_prev->Z);
			myVector3D v2(p_next->X - p_curr->X, p_next->Y - p_curr->Y, p_next->Z - p_curr->Z);
			myVector3D cross(
				v1.dY * v2.dZ - v1.dZ * v2.dY,
				v1.dZ * v2.dX - v1.dX * v2.dZ,
				v1.dX * v2.dY - v1.dY * v2.dX
			);
			
			if (cross * normal > 1e-5) { // Convex vertex
				// Check if any other vertex is inside the triangle
				bool is_ear = true;
				myHalfedge* test_edge = next->next;
				while (test_edge != prev) {
					myPoint3D* p_test = test_edge->source->point;
					
					// Barycentric test
					myVector3D v0(p_next->X - p_prev->X, p_next->Y - p_prev->Y, p_next->Z - p_prev->Z);
					myVector3D v1_(p_curr->X - p_prev->X, p_curr->Y - p_prev->Y, p_curr->Z - p_prev->Z);
					myVector3D v2_(p_test->X - p_prev->X, p_test->Y - p_prev->Y, p_test->Z - p_prev->Z);
					
					double dot00 = v0 * v0;
					double dot01 = v0 * v1_;
					double dot02 = v0 * v2_;
					double dot11 = v1_ * v1_;
					double dot12 = v1_ * v2_;
					
					double denom = (dot00 * dot11 - dot01 * dot01);
					if (denom > 1e-8 || denom < -1e-8) {
					    double invDenom = 1.0 / denom;
					    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
					    double v_bary = (dot00 * dot12 - dot01 * dot02) * invDenom;
					    
					    // Check if point is inside triangle
					    if ((u >= -1e-5) && (v_bary >= -1e-5) && (u + v_bary <= 1.0 + 1e-5)) {
						    is_ear = false;
						    break;
					    }
					}
					test_edge = test_edge->next;
				}
				
				if (is_ear) {
					ear_edge = prev; 
					break;
				}
			}
			curr = curr->next;
		}

		if (ear_edge == NULL) {
			// Fallback: if no valid ear found, find any convex corner anyway to guarantee termination
			curr = f->adjacent_halfedge;
			for (int i = 0; i < N; ++i) {
				myPoint3D* p_prev = curr->prev->source->point;
				myPoint3D* p_curr = curr->source->point;
				myPoint3D* p_next = curr->next->source->point;
				myVector3D v1(p_curr->X - p_prev->X, p_curr->Y - p_prev->Y, p_curr->Z - p_prev->Z);
				myVector3D v2(p_next->X - p_curr->X, p_next->Y - p_curr->Y, p_next->Z - p_curr->Z);
				myVector3D cross(
					v1.dY * v2.dZ - v1.dZ * v2.dY,
					v1.dZ * v2.dX - v1.dX * v2.dZ,
					v1.dX * v2.dY - v1.dY * v2.dX
				);
				if (cross * normal > -1e-5) {
					ear_edge = curr->prev;
					break;
				}
				curr = curr->next;
			}
			if (ear_edge == NULL) ear_edge = f->adjacent_halfedge;
		}

		// Clip the ear
		myHalfedge* h0 = ear_edge;
		myHalfedge* h1 = h0->next;
		myHalfedge* h2 = h1->next;
		myHalfedge* h_prev = h0->prev;

		myVertex* v0 = h0->source;
		myVertex* v2 = h2->source;

		myHalfedge* h_new = new myHalfedge();
		myHalfedge* h_new_twin = new myHalfedge();
		
		h_new->source = v2;
		h_new_twin->source = v0;
		
		h_new->twin = h_new_twin;
		h_new_twin->twin = h_new;
		
		h_new->index = halfedges.size(); halfedges.push_back(h_new);
		h_new_twin->index = halfedges.size(); halfedges.push_back(h_new_twin);

		myFace* f_tri = new myFace();
		f_tri->index = faces.size();
		faces.push_back(f_tri);
		f_tri->adjacent_halfedge = h0;

		h1->next = h_new; h_new->prev = h1;
		h_new->next = h0; h0->prev = h_new;
		
		h0->adjacent_face = f_tri;
		h1->adjacent_face = f_tri;
		h_new->adjacent_face = f_tri;

		f->adjacent_halfedge = h_new_twin; 
		
		h_prev->next = h_new_twin; h_new_twin->prev = h_prev;
		h_new_twin->next = h2; h2->prev = h_new_twin;
		
		h_new_twin->adjacent_face = f;
		
		N--;
	}

	return true;
}

