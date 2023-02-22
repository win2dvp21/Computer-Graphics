#pragma once

#include <glad/glad.h>
#include <stdlib.h> 
#include "tracer.h"
#include "raytraceData.h"

class shader {
public: 
	shader();
	~shader();

	raytraceData::material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb);
	void shade(raytraceData::point* p, raytraceData::vector* n, raytraceData::material* m, raytraceData::color* c, raytraceData::color* rfl);

	raytraceData::halfspace* h1;
	raytraceData::sphere* s1;
};
