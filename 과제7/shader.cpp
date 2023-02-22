#include "shader.h"
#include "raytrace.h"
#include "tracer.h"

#include "math.h"

using namespace raytraceData;

shader::shader()
{
}

shader::~shader()
{
}

point* normalize(point* p) {
	point* norm = new point();

	norm->x = (p->x / p->w) / sqrt(pow(p->x / p->w, 2) + pow(p->y / p->w, 2) + pow(p->z / p->w, 2));
	norm->y = (p->y / p->w) / sqrt(pow(p->x / p->w, 2) + pow(p->y / p->w, 2) + pow(p->z / p->w, 2));
	norm->z = (p->z / p->w) / sqrt(pow(p->x / p->w, 2) + pow(p->y / p->w, 2) + pow(p->z / p->w, 2));
	norm->w = 1.0f;
	
	return(norm);
}

material* shader::makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb) {
	material* m;

	/* allocate memory */
	m = new material();

	/* put stuff in it */
	m->c.r = r;
	m->c.g = g;
	m->c.b = b;
	m->amb = amb;
	return(m);
}

/* LIGHTING CALCULATIONS */

/* shade */
/* color of point p with normal vector n and material m returned in c */
void shader::shade(point* p, vector* n, material* m, color* c, color* rfl) {

	tracer tracer;
	ray ray_s;

	point l1;
	l1.x = -2.0;	l1.y = -2.0;	l1.z = -2.0;	l1.w = 1.0;

	point norm_p;
	point* norm_l;
	point v, r;
	point* norm_v;

	float f_radial, f_angular;
	float i_diff, i_spec;
	float nxl;
	float ns;
	float vis_i = 1, t = 0;

	color kd, ks;
	kd.r = 0.5f;	kd.g = 0.5f;	kd.b = 0.5f;
	ks.r = 1.0f;	ks.g = 1.0f;	ks.b = 1.0f;

	int hit_s = 0;

	norm_p.x = p->x - l1.x;
	norm_p.y = p->y - l1.y;
	norm_p.z = p->z - l1.z;
	norm_l = normalize(&norm_p);

	float len_x = p->x - l1.x;
	float len_y = p->y - l1.y;
	float len_z = p->z - l1.z;
	float len = sqrt(pow(len_x, 2) + pow(len_y, 2) + pow(len_z, 2));

	f_radial = 1 / (1 + pow(len,2));
	f_angular = 1.0;

	v.x = 0.0f - p->x;
	v.y = 0.0f - p->y;
	v.z = 0.0f - p->z;
	v.w = 1.0f;
	norm_v = normalize(&v);

	nxl = n->x * norm_l->x + n->y * norm_l->y + n->z * norm_l->z;

	r.x = nxl * n->x - norm_l->x;
	r.y = nxl * n->y - norm_l->y;
	r.z = nxl * n->z - norm_l->z;

	i_diff = (norm_l->x * n->x + norm_l->y * n->y + norm_l->z * n->z) * 3;
	i_spec = (norm_v->x * r.x + norm_v->y * r.y + norm_v->z * r.z) * 3;

	if (i_diff < 0.0f) {
		i_diff = 0.0f; i_spec = 0.0f;
	}

	if (i_spec < 0.0f) {
		i_spec = 0.0f;
	}
	else {
		ns = 5.0f;
		i_spec = pow(i_spec, ns);
	}

	ray_s.start = p;
	ray_s.end = norm_l;
	hit_s = tracer.raySphereIntersect(&ray_s, s1, &t);

	if (hit_s) {
		vis_i = 0.0;
	}

	c->r = (m->amb * m->c.r + (f_radial * f_angular * (i_diff * kd.r + i_spec * ks.r) * 2) * vis_i) + rfl->r * 0.5;
	c->g = (m->amb * m->c.g + (f_radial * f_angular * (i_diff * kd.g + i_spec * ks.g) * 2) * vis_i) + rfl->g * 0.5;
	c->b = (m->amb * m->c.b + (f_radial * f_angular * (i_diff * kd.b + i_spec * ks.b) * 2) * vis_i) + rfl->b * 0.5;

	/* clamp color values to 1.0 */
	if (c->r > 1.0) c->r = 1.0;
	if (c->g > 1.0) c->g = 1.0;
	if (c->b > 1.0) c->b = 1.0;
}