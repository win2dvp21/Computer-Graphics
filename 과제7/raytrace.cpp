#include "raytrace.h"
#include <math.h>

using namespace raytraceData;

raytrace::raytrace(int w, int h)
{
	/* low-level graphics setup */
	lowlevel.initCanvas(w, h);

	/* raytracer setup */
	initCamera(w, h);
	initScene();
}

raytrace::~raytrace()
{
	if (s1 != NULL) delete s1;
	if (viewpoint != NULL) delete viewpoint;
}

GLubyte* raytrace::display(void)
{	
	drawScene();  /* draws the picture in the canvas */

	return lowlevel.flushCanvas();  /* draw the canvas to the OpenGL window */	
}

void raytrace::initScene()
{
	s1 = makeSphere(0.0, 0.0, -2.0, 0.25);
	s1->m = shader.makeMaterial(0.8, 0.1, 0.15, 0.3);

	tracer.s1 = s1;
	shader.s1 = s1;

	h1 = makeHalfspace(0.0, 0.3, 0.1, 0.0, -1.0, 0.03);
	h1->m = shader.makeMaterial(0.5, 0.5, 0.5, 0.5);

	tracer.h1 = h1;
	shader.h1 = h1;
}

void raytrace::initCamera(int w, int h)
{
	viewpoint = makePoint(0.0, 0.0, 0.0, 1.0);
	pnear = -1.0;
	fovx = M_PI / 6;
	/* window dimensions */
	width = w;
	height = h;
}

void raytrace::drawScene(void)
{
	int i, j;
	GLfloat imageWidth;

	/* declare data structures on stack to avoid malloc & free */
	point worldPix;  /* current pixel in world coordinates */
	point direction;
	ray r;
	color c;

	/* z and w are constant for all pixels */
	worldPix.w = 1.0;
	worldPix.z = pnear;

	r.start = &worldPix;
	r.end = &direction;

	imageWidth = 2 * pnear * tan(fovx / 2);

	/* trace a ray for every pixel */
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {

			/* find position of pixel in world coordinates */
			/* y position = (pixel height/middle) scaled to world coords */
			worldPix.y = (j - (height / 2)) * imageWidth / width;
			/* x position = (pixel width/middle) scaled to world coords */
			worldPix.x = (i - (width / 2)) * imageWidth / width;

			/* find direction */
			/* note: direction vector is NOT NORMALIZED */
			calculateDirection(viewpoint, &worldPix, &direction);

			/* Find color for pixel */
			rayColor(&r, &c, 0);
			/* write the pixel! */
			lowlevel.drawPixel(i, j, c.r, c.g, c.b);
		}
	}
}

point* raytrace::makePoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	point* p;
	/* allocate memory */	
	p = new point();
	/* put stuff in it */
	p->x = x; p->y = y; p->z = z; p->w = w;
	return (p);
}

sphere* raytrace::makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {
	sphere* s;
	/* allocate memory */
	s = new sphere();

	/* put stuff in it */
	s->c = makePoint(x, y, z, 1.0);   /* center */
	s->r = r;   /* radius */
	s->m = NULL;   /* material */
	return(s);
}

halfspace* raytrace::makeHalfspace(GLfloat x, GLfloat y, GLfloat z, GLfloat norm_x, GLfloat norm_y, GLfloat norm_z) {
	halfspace* hs;
	hs = new halfspace();

	hs->c = makePoint(x, y, z, 1.0);
	hs->n = makePoint(norm_x, norm_y, norm_z, 1.0); //normal point
	hs->m = NULL;
	return(hs);
}

void reflect(ray* ray, point* p, vector* n) {
	ray->start = p;
	ray->end->x = (ray->end->x - 2 * n->x * (-ray->end->x * n->x + ray->end->y * n->y + ray->end->z * n->z));
	ray->end->y = (ray->end->y - 2 * n->y * (-ray->end->x * n->x + ray->end->y * n->y + ray->end->z * n->z));
	ray->end->z = (ray->end->z - 2 * n->z * (-ray->end->x * n->x + ray->end->y * n->y + ray->end->z * n->z));
}

/* returns the color seen by ray r in parameter c */
void raytrace::rayColor(ray* r, color* c, int num) {
	point p;	
	p.w = 0.0;  /* inialize to "no intersection" */

	vector n;
	material* m;
	ray rfl_ray = *r;

	color rfl_color;
	rfl_color.r = 0.0, rfl_color.g = 0.0, rfl_color.b = 0.0;

	bool reflective = false;
	tracer.trace(r, &p, &n, &m, &reflective);

	if (p.w != 0.0) {

		if (num < 1 && reflective) {
			
			reflect(&rfl_ray, &p, &n);
			rfl_ray.end->z += 0.02;
			num += 1;

			rayColor(&rfl_ray, &rfl_color, num);
			shader.shade(&p, &n, m, c, &rfl_color);
			
			return;
		}
		shader.shade(&p, &n, m, c, &rfl_color);
	}
	else { /* nothing was hit */
		c->r = 0.0;
		c->g = 0.0;
		c->b = 0.0;
	}
	
}

/* vector from point p to point q is returned in v */
void raytrace::calculateDirection(point* p, point* q, point* v) {
	float length;

	v->x = q->x - p->x;
	v->y = q->y - p->y;
	v->z = q->z - p->z;
	/* a direction is a point at infinity */
	v->w = 0.0;
}
