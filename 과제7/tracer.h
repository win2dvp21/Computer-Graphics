#pragma once

#include "raytraceData.h"

class tracer {
public:
	tracer();
	~tracer();

	void findPointOnRay(raytraceData::ray* r, float t, raytraceData::point* p);
	int raySphereIntersect(raytraceData::ray* r, raytraceData::sphere* s, float* t);
	int rayHalfspaceIntersect(raytraceData::ray* r, raytraceData::halfspace* hs, float* t);
	void findSphereNormal(raytraceData::sphere* s, raytraceData::point* p, raytraceData::vector* n);
	void trace(raytraceData::ray* r, raytraceData::point* p, raytraceData::vector* n, raytraceData::material** m, bool* reflective);
	
	raytraceData::halfspace* h1;
	raytraceData::sphere* s1;
};