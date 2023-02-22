#include "tracer.h"

#include <math.h>

using namespace raytraceData;

tracer::tracer():
	s1(NULL)
{
}

tracer::~tracer()
{
}

/* point on ray r parameterized by t is returned in p */
void tracer::findPointOnRay(ray* r, float t, point* p) {
	p->x = r->start->x + t * r->end->x;
	p->y = r->start->y + t * r->end->y;
	p->z = r->start->z + t * r->end->z;
	p->w = 1.0;
}

/* raySphereIntersect */
/* returns TRUE if ray r hits sphere s, with parameter value in t */
int tracer::raySphereIntersect(ray* r, sphere* s, float* t) {
	point p;   /* start of transformed ray */
	float a, b, c;  /* coefficients of quadratic equation */
	float D;    /* discriminant */
	point* v;

	/* transform ray so that sphere center is at origin */
	/* don't use matrix, just translate! */
	p.x = r->start->x - s->c->x;
	p.y = r->start->y - s->c->y;
	p.z = r->start->z - s->c->z;
	v = r->end; /* point to direction vector */

	a = v->x * v->x + v->y * v->y + v->z * v->z;
	b = 2 * (v->x * p.x + v->y * p.y + v->z * p.z);
	c = p.x * p.x + p.y * p.y + p.z * p.z - s->r * s->r;

	D = b * b - 4 * a * c;

	if (D < 0) {  /* no intersection */
		return (FALSE);
	}
	else {
		D = static_cast<float>(sqrt(D));
		/* First check the root with the lower value of t: */
		/* this one, since D is positive */
		*t = (-b - D) / (2 * a);
		/* ignore roots which are less than zero (behind viewpoint) */
		if (*t < 0) {
			*t = (-b + D) / (2 * a);
		}
		if (*t < 0) { return(FALSE); }
		else return(TRUE);
	}
}

/* normal vector of s at p is returned in n */
/* note: dividing by radius normalizes */
void tracer::findSphereNormal(sphere* s, point* p, vector* n) {
	n->x = (p->x - s->c->x) / s->r;
	n->y = (p->y - s->c->y) / s->r;
	n->z = (p->z - s->c->z) / s->r;
	n->w = 0.0;
}

int tracer::rayHalfspaceIntersect(ray* r, halfspace* hs, float* t) {
	float a, b; 
	float D;

	a = ((hs->c->x - r->start->x) * hs->n->x + (hs->c->y - r->start->y) * hs->n->y + (hs->c->z - r->start->z) * hs->n->z);
	b = r->end->x * hs->n->x + r->end->y * hs->n->y + r->end->z * hs->n->z;

	D = a / b;

	if (D < 0) {
		return FALSE;
	}

	else {
		*t = D;
		return TRUE;
	}
}

void findHalfspaceNormal(halfspace* hs, vector* n) {
	n->x = hs->n->x / pow(hs->n->x * hs->n->x + hs->n->y * hs->n->y + hs->n->z * hs->n->z, 0.5);
	n->y = hs->n->y / pow(hs->n->x * hs->n->x + hs->n->y * hs->n->y + hs->n->z * hs->n->z, 0.5);
	n->z = hs->n->z / pow(hs->n->x * hs->n->x + hs->n->y * hs->n->y + hs->n->z * hs->n->z, 0.5);
	n->w = 0.0;
}

/* trace */
/* If something is hit, returns the finite intersection point p,
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
void tracer::trace(ray* r, point* p, vector* n, material** m, bool* reflective) {
	ray* rfl_ray = r;

	float t1 = 0, t2 = 0;
	int hit_s = 0, hit_hs = 0;

	hit_s = raySphereIntersect(r, s1, &t1);
	hit_hs = rayHalfspaceIntersect(r, h1, &t2);

	if (hit_s) {
		*m = s1->m;
		*reflective = false;
		findPointOnRay(r, t1, p);
		findSphereNormal(s1, p, n);
	}

	if (hit_hs) {
		*m = h1->m;
		*reflective = false;
		findPointOnRay(r, t2, p);
		findHalfspaceNormal(h1, n);	
	}

	if (!hit_s && !hit_hs) {
		p->w = 0.0;
	}
}