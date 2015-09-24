#include <stdio.h>
#include <string.h>
#include <png.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <math.h>

#define PI 3.14159265
#define SPHERE 0
#define POLYGON 1
#define BOUNCEDEPTH 2
#define WIDTH 900
#define HEIGHT 900




///////////////////////////
// My Structs
///////////////////////////



typedef struct {
  float x, y, z;
} point;

typedef struct {
  int numverts;               // number of vertices
  point vertices[100];        // array of vertices (from 0 to numverts-1)
} polygon;

// Object that is either a sphere or a polygon
struct object {
  // an id number that corresponds to the index in objlist
  short oid;
  // type is eithere SPHERE or POLYGON
  short type;
  // color of the object
  float r,g,b;
  // reflectance coefficient;
  float reflectanceCoeff;
  // Degree of specularity;
  char specularity;
  // These are only defined if type == SPHERE
  // coordinates of the center and radius
  point center;
  float radius;
  // polygon is only defined type==POLYGON
  polygon p;
};


// This parametric point has an extra value for t
// If t == -1, the point is invalid
typedef struct {
  float x,y,z,t;
  object obj;   // This is the object the ppoint lies on
} ppoint;


// Light
struct light {
  // a light object
  point source;
  // RBG components of the light
  float r,g,b;
};

// pixel color structure for easy access and indexing
typedef struct {
    png_byte red;
    png_byte green;
    png_byte blue;
} rgb_pixel;



////////////////////////////
// My Variables
////////////////////////////
int numobjs;         // number of objects
object objlist[10];  // array of objects
int numlights;
light lightlist[10]; // array of lights
point eye;           // The eye point
float pixelplane;    // the z-value of the pixel plane (x & y bounds are the viewport)
float ambientr;
float ambientg;
float ambientb;
rgb_pixel **pixel_data;


ppoint inObj (point start, point dir, object obj);
point arbaxisrot(point pt, point axis, float angle);
point yaxisrot(point pt, float angle);
point zaxisrot(point pt, float angle);
light calculateSpecularity (point source, point dir, object obj, int depth);
light getPixelColor(int x, int y, int depth);
point findNormal (point pt, object obj);
point findNormal (ppoint pt, object obj);
int pointInPolygonTest (ppoint q, polygon poly);





// This is three colored spheres and a square.  My test layout.
void initializeObject1() {
  // Set ambient illumination constant;
  ambientr = 0.3f;
  ambientg = 0.3f;
  ambientb = 0.3f;

  object sphere;
  point center;
  object polygon;
  point p1, p2, p3, p4;


  sphere.type = SPHERE;
  center.x = 200;
  center.y = 200;
  center.z = 0;
  sphere.r = 1.0f;
  sphere.g = 0.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 200;
  sphere.oid = 0;
  objlist[0] = sphere;

  sphere.type = SPHERE;
  center.x = 650;
  center.y = 300;
  center.z = -200;
  sphere.r = 0.0f;
  sphere.g = 1.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 14;
  sphere.center = center;
  sphere.radius = 200;
  sphere.oid = 1;
  objlist[1] = sphere;

  sphere.type = SPHERE;
  center.x = 1100;
  center.y = 400;
  center.z = -400;
  sphere.r = 0.0f;
  sphere.g = 0.0f;
  sphere.b = 1.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 14;
  sphere.center = center;
  sphere.radius = 200;
  sphere.oid = 2;
  objlist[2] = sphere;

  polygon.type = POLYGON;
  polygon.r = 1.0;
  polygon.g = 1.0;
  polygon.b = 1.0;
  p1.x = 200;
  p1.y = 100;
  p1.z = 500;
  p2.x = 1400;
  p2.y = 100;
  p2.z = 500;
  p3.x = 1400;
  p3.y = 150;
  p3.z = -1800;
  p4.x = 200;
  p4.y = 150;
  p4.z = -1800;
  polygon.p.vertices[0] = p1;
  polygon.p.vertices[1] = p2;
  polygon.p.vertices[2] = p3;
  polygon.p.vertices[3] = p4;
  polygon.p.numverts = 4;
  polygon.reflectanceCoeff = 0.7f;
  polygon.specularity = 15;
  polygon.oid = 3;
  objlist[3] = polygon;



  light light;
  light.source.x = 200;
  light.source.y = 200;
  light.source.z = 3000;
  light.r = 1.0f;
  light.g = 1.0f;
  light.b = 1.0f;
  lightlist[0] = light;

  light.source.x = 1000;
  light.source.y = 350;
  light.source.z = -3000;
  light.r = 1.0f;
  light.g = 1.0f;
  light.b = 1.0f;
  lightlist[1] = light;

  numobjs = 4;
  numlights = 2;



}





// More complicated one.... spheres in a box
void initializeObject2() {
  // Set ambient illumination constant;
  ambientr = 0.3f;
  ambientg = 0.3f;
  ambientb = 0.3f;

  object sphere;
  point center;
  object polygon;
  point p1, p2, p3, p4;


  sphere.type = SPHERE;
  center.x = 400;
  center.y = 500;
  center.z = 0;
  sphere.r = 1.0f;
  sphere.g = 0.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 100;
  sphere.oid = 0;
  objlist[0] = sphere;

  sphere.type = SPHERE;
  center.x = 600;
  center.y = 500;
  center.z = 0;
  sphere.r = 0.0f;
  sphere.g = 1.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 14;
  sphere.center = center;
  sphere.radius = 100;
  sphere.oid = 1;
  objlist[1] = sphere;

  sphere.type = SPHERE;
  center.x = 500;
  center.y = 700;
  center.z = 200;
  sphere.r = 0.0f;
  sphere.g = 0.0f;
  sphere.b = 1.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 14;
  sphere.center = center;
  sphere.radius = 100;
  sphere.oid = 2;

  objlist[2] = sphere;
  sphere.type = SPHERE;
  center.x = 500;
  center.y = 250;
  center.z = 200;
  sphere.r = 0.0f;
  sphere.g = 0.5f;
  sphere.b = 0.8f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 14;
  sphere.center = center;
  sphere.radius = 100;
  sphere.oid = 3;
  objlist[3] = sphere;

  polygon.type = POLYGON;
  polygon.r = 0.8;
  polygon.g = 0.8;
  polygon.b = 0.0;
  p1.x = 200;
  p1.y = 200;
  p1.z = 400;
  p2.x = 800;
  p2.y = 200;
  p2.z = 400;
  p3.x = 700;
  p3.y = 300;
  p3.z = 0;
  p4.x = 300;
  p4.y = 300;
  p4.z = 0;
  polygon.p.vertices[0] = p1;
  polygon.p.vertices[1] = p2;
  polygon.p.vertices[2] = p3;
  polygon.p.vertices[3] = p4;
  polygon.p.numverts = 4;
  polygon.reflectanceCoeff = 0.7f;
  polygon.specularity = 15;
  polygon.oid = 4;
  objlist[4] = polygon;

  polygon.type = POLYGON;
  polygon.r = 0.8;
  polygon.g = 0.8;
  polygon.b = 0.0;
  p1.x = 700;
  p1.y = 300;
  p1.z = 0;
  p2.x = 800;
  p2.y = 200;
  p2.z = 400;
  p3.x = 800;
  p3.y = 800;
  p3.z = 400;
  p4.x = 700;
  p4.y = 700;
  p4.z = 0;
  polygon.p.vertices[0] = p1;
  polygon.p.vertices[1] = p2;
  polygon.p.vertices[2] = p3;
  polygon.p.vertices[3] = p4;
  polygon.p.numverts = 4;
  polygon.reflectanceCoeff = 0.7f;
  polygon.specularity = 15;
  polygon.oid = 5;
  objlist[5] = polygon;

  polygon.type = POLYGON;
  polygon.r = 0.8;
  polygon.g = 0.8;
  polygon.b = 0.0;
  p1.x = 300;
  p1.y = 700;
  p1.z = 0;
  p2.x = 700;
  p2.y = 700;
  p2.z = 0;
  p3.x = 800;
  p3.y = 800;
  p3.z = 400;
  p4.x = 200;
  p4.y = 800;
  p4.z = 400;
  polygon.p.vertices[0] = p1;
  polygon.p.vertices[1] = p2;
  polygon.p.vertices[2] = p3;
  polygon.p.vertices[3] = p4;
  polygon.p.numverts = 4;
  polygon.reflectanceCoeff = 0.7f;
  polygon.specularity = 15;
  polygon.oid = 6;
  objlist[6] = polygon;

  polygon.type = POLYGON;
  polygon.r = 0.8;
  polygon.g = 0.8;
  polygon.b = 0.0;
  p1.x = 200;
  p1.y = 200;
  p1.z = 400;
  p2.x = 300;
  p2.y = 300;
  p2.z = 0;
  p3.x = 300;
  p3.y = 700;
  p3.z = 0;
  p4.x = 200;
  p4.y = 800;
  p4.z = 400;
  polygon.p.vertices[0] = p1;
  polygon.p.vertices[1] = p2;
  polygon.p.vertices[2] = p3;
  polygon.p.vertices[3] = p4;
  polygon.p.numverts = 4;
  polygon.reflectanceCoeff = 0.7f;
  polygon.specularity = 15;
  polygon.oid = 7;
  objlist[7] = polygon;

  polygon.type = POLYGON;
  polygon.r = 0.8;
  polygon.g = 0.8;
  polygon.b = 0.8;
  p1.x = 300;
  p1.y = 300;
  p1.z = 0;
  p2.x = 700;
  p2.y = 300;
  p2.z = 0;
  p3.x = 700;
  p3.y = 700;
  p3.z = 0;
  p4.x = 300;
  p4.y = 700;
  p4.z = 0;
  polygon.p.vertices[0] = p1;
  polygon.p.vertices[1] = p2;
  polygon.p.vertices[2] = p3;
  polygon.p.vertices[3] = p4;
  polygon.p.numverts = 4;
  polygon.reflectanceCoeff = 0.7f;
  polygon.specularity = 15;
  polygon.oid = 8;
  objlist[8] = polygon;



  light light;
  light.source.x = 1000;
  light.source.y = 1000;
  light.source.z = 1000;
  light.r = 1.0f;
  light.g = 1.0f;
  light.b = 1.0f;
  lightlist[0] = light;

  numobjs = 9;
  numlights = 1;



}






// Spheres in a corner
void initializeObject3() {
  // Set ambient illumination constant;
  ambientr = 0.3f;
  ambientg = 0.3f;
  ambientb = 0.3f;

  object sphere;
  point center;
  object polygon;
  point p1, p2, p3, p4;


  sphere.type = SPHERE;
  center.x = 250;
  center.y = 650;
  center.z = 200;
  sphere.r = 0.7f;
  sphere.g = 0.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 200;
  sphere.oid = 0;
  objlist[0] = sphere;

  sphere.type = SPHERE;
  center.x = 400;
  center.y = 750;
  center.z = 100;
  sphere.r = 0.7f;
  sphere.g = 0.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 14;
  sphere.center = center;
  sphere.radius = 100;
  sphere.oid = 1;
  objlist[1] = sphere;

  sphere.type = SPHERE;
  center.x = 475;
  center.y = 800;
  center.z = 50;
  sphere.r = 0.7f;
  sphere.g = 0.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.7f;
  sphere.specularity = 14;
  sphere.center = center;
  sphere.radius = 50;
  sphere.oid = 2;
  objlist[2] = sphere;


  light light;
  light.source.x = 100;
  light.source.y = 1000;
  light.source.z = 500;
  light.r = 1.0f;
  light.g = 0.0f;
  light.b = 1.0f;
  lightlist[0] = light;

  light.source.x = 100;
  light.source.y = -1000;
  light.source.z = 700;
  light.r = 1.0f;
  light.g = 1.0f;
  light.b = 0.0f;
  lightlist[1] = light;

  numobjs = 4;
  numlights = 2;



}


// multicolored lights
void initializeObject4() {
  // Set ambient illumination constant;
  ambientr = 0.3f;
  ambientg = 0.3f;
  ambientb = 0.3f;

  object sphere;
  point center;
  object polygon;
  point p1, p2, p3, p4;


  sphere.type = SPHERE;
  center.x = 200;
  center.y = 800;
  center.z = 0;
  sphere.r = 1.0f;
  sphere.g = 1.0f;
  sphere.b = 1.0f;
  sphere.reflectanceCoeff = 0.5f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 300;
  sphere.oid = 0;
  objlist[0] = sphere;

  sphere.type = SPHERE;
  center.x = 800;
  center.y = 500;
  center.z = 0;
  sphere.r = 1.0f;
  sphere.g = 1.0f;
  sphere.b = 1.0f;
  sphere.reflectanceCoeff = 0.5f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 300;
  sphere.oid = 1;
  objlist[1] = sphere;


  light light;
  light.source.x = 500;
  light.source.y = 200;
  light.source.z = 500;
  light.r = 1.0f;
  light.g = 1.0f;
  light.b = 0.0f;
  lightlist[0] = light;

  light.source.x = 500;
  light.source.y = 800;
  light.source.z = 500;
  light.r = 1.0f;
  light.g = 0.0f;
  light.b = 1.0f;
  lightlist[1] = light;

  light.source.x = 1200;
  light.source.y = 500;
  light.source.z = -200;
  light.r = 1.0f;
  light.g = 0.0f;
  light.b = 0.0f;
  lightlist[2] = light;

  light.source.x = -200;
  light.source.y = 500;
  light.source.z = -200;
  light.r = 1.0f;
  light.g = 0.0f;
  light.b = 0.0f;
  lightlist[3] = light;

  numobjs = 2;
  numlights = 4;



}




// Gouraud Mouse with ray tracing!
void initializeObject5() {
  // Set ambient illumination constant;
  ambientr = 0.3f;
  ambientg = 0.3f;
  ambientb = 0.3f;

  object sphere;
  point center;
  object polygon;
  point p1, p2, p3, p4;


  sphere.type = SPHERE;
  center.x = 500;
  center.y = 500;
  center.z = 0;
  sphere.r = 1.0f;
  sphere.g = 0.6f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.5f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 300;
  sphere.oid = 0;
  objlist[0] = sphere;

  sphere.type = SPHERE;
  center.x = 200;
  center.y = 300;
  center.z = 0;
  sphere.r = 1.0f;
  sphere.g = 0.5f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.5f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 150;
  sphere.oid = 1;
  objlist[1] = sphere;

  sphere.type = SPHERE;
  center.x = 800;
  center.y = 300;
  center.z = 0;
  sphere.r = 1.0f;
  sphere.g = 0.5f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.5f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 150;
  sphere.oid = 2;
  objlist[2] = sphere;
  sphere.type = SPHERE;

  center.x = 650;
  center.y = 550;
  center.z = 210;
  sphere.r = 0.0f;
  sphere.g = 1.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.5f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 90;
  sphere.oid = 3;
  objlist[3] = sphere;

  sphere.type = SPHERE;
  center.x = 350;
  center.y = 550;
  center.z = 210;
  sphere.r = 0.0f;
  sphere.g = 1.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.5f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 90;
  sphere.oid = 4;
  objlist[4] = sphere;

  sphere.type = SPHERE;
  center.x = 500;
  center.y = 600;
  center.z = 260;
  sphere.r = 1.0f;
  sphere.g = 0.0f;
  sphere.b = 0.0f;
  sphere.reflectanceCoeff = 0.5f;
  sphere.specularity = 15;
  sphere.center = center;
  sphere.radius = 30;
  sphere.oid = 5;
  objlist[5] = sphere;


  light light;
  light.source.x = 500;
  light.source.y = 1000;
  light.source.z = 300;
  light.r = 1.0f;
  light.g = 0.2f;
  light.b = 0.2f;
  lightlist[0] = light;

  numobjs = 6;
  numlights = 1;

}



// Given a point on the pixel plane, return a normalized direction ray
// from the eye to the point on the pixel plane
point pixelplaneToDir(float px, float py) {
  point dir;
  dir.x = px - eye.x;
  dir.y = py - eye.y;
  dir.z = pixelplane - eye.z;
  float length = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
  dir.x /= length;
  dir.y /= length;
  dir.z /= length;

  return dir;
}


// Given an object and a point on that object, calculate the surface normal
// Note: If the point is not on the object, cannot be held responsible for results.
point findNormal (ppoint ppoint, object obj) {

  point pt;
  pt.x = ppoint.x;
  pt.y = ppoint.y;
  pt.z = ppoint.z;
  return findNormal(pt, obj);
}

point findNormal (point pt, object obj) {

  point normal;

  if (obj.type == SPHERE) {
    object sphere = obj;
    // Move point relative to origin
    normal.x = pt.x - sphere.center.x;
    normal.y = pt.y - sphere.center.y;
    normal.z = pt.z - sphere.center.z;

    // Normalize the point
    normal.x /= sphere.radius;
    normal.y /= sphere.radius;
    normal.z /= sphere.radius;

    return normal;

  } else if (obj.type == POLYGON) {
    polygon polygon = obj.p;
    point v1, v2;

    // Get two vectors from the first three vertices
    v1.x = polygon.vertices[1].x - polygon.vertices[0].x;
    v1.y = polygon.vertices[1].y - polygon.vertices[0].y;
    v1.z = polygon.vertices[1].z - polygon.vertices[0].z;
    v2.x = polygon.vertices[2].x - polygon.vertices[0].x;
    v2.y = polygon.vertices[2].y - polygon.vertices[0].y;
    v2.z = polygon.vertices[2].z - polygon.vertices[0].z;

    // Take the cross product of these two vectors to get the normal
    point normal;
    normal.x = v1.y*v2.z - v1.z*v2.y;
    normal.y = v1.z*v2.x - v1.x*v2.z;
    normal.z = v1.x*v2.y - v1.y*v2.x;

    // Normalize
    float v1length = sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);
    float v2length = sqrt(v2.x*v2.x + v2.y*v2.y + v2.z*v2.z);
    normal.x /= v1length*v2length;
    normal.y /= v1length*v2length;
    normal.z /= v1length*v2length;

    return normal;

  }

}


void plotPixel(int x, int y, int depth) {

  light pixelColor = getPixelColor(x, y, depth);
  int r,g,b;
  int i;


  r = 255*pixelColor.r;
  g = 255*pixelColor.g;
  b = 255*pixelColor.b;

  pixel_data[y][x].red = r;
  pixel_data[y][x].green = g;
  pixel_data[y][x].blue = b;

}





light getPixelColor(int x, int y, int depth) {


  ppoint intersection;
  ppoint tempintersection;

  for (int o = 0 ; o < numobjs ; o++) {
    object objct = objlist[o];
    float px = 0.0 + x;
    float py = 0.0 + y;

    // This is the first object
    if (o == 0) {
      intersection = inObj(eye, pixelplaneToDir(px, py), objct);
    } else {
      tempintersection = inObj(eye, pixelplaneToDir(px, py), objct);
      if (tempintersection.t >= 0) {
	if ((intersection.t >= 0 && tempintersection.t < intersection.t) || intersection.t < 0) {
	  intersection = tempintersection;
	}
      }
    }
  }


  if (intersection.t > 0) {
    
    // The point is in the object, so we need to find the intensity at that point
    
    object obj = intersection.obj;
    // First, we calculate the normal at that point
    point normal = findNormal(intersection, obj);
    
    float rintensity = 0.0f;
    float gintensity = 0.0f;
    float bintensity = 0.0f;
    float cosTheta;
    float nx = normal.x;
    float ny = normal.y;
    float nz = normal.z;
    
    // Only plot if the z of the normal is positive
    if (nz >= 0) {
      for (int i = 0 ; i < numlights ; i++) {
	light light = lightlist[i];
	// lightv is the light vector relative to the intersection point
	point lightv;
	lightv.x = light.source.x - intersection.x;
	lightv.y = light.source.y - intersection.y;
	lightv.z = light.source.z - intersection.z;
	float tempintensity = 1.0f;
	cosTheta = nx*lightv.x + ny*lightv.y + nz*lightv.z;
	cosTheta /= sqrt(lightv.x*lightv.x +
			 lightv.y*lightv.y +
			 lightv.z*lightv.z);
	cosTheta /= sqrt(nx*nx + ny*ny + nz*nz);
	tempintensity *= cosTheta*obj.reflectanceCoeff;
	if (tempintensity < 0.0f) tempintensity = 0.0f;
	rintensity += obj.r*tempintensity*light.r;
	gintensity += obj.g*tempintensity*light.g;
	bintensity += obj.b*tempintensity*light.b;


	// Determine whether object is in the shadow
	point shadowpoint;
	point lightray;
	shadowpoint.x = intersection.x;
	shadowpoint.y = intersection.y;
	shadowpoint.z = intersection.z;
	lightray.x = lightv.x;
	lightray.y = lightv.y;
	lightray.z = lightv.z;
	float raylength = sqrt(lightray.x*lightray.x +
			       lightray.y*lightray.y +
			       lightray.z*lightray.z);
	lightray.x /= raylength;
	lightray.y /= raylength;
	lightray.z /= raylength;
	for (int o = 0 ; o < numobjs ; o++) {
	  object objct = objlist[o];
	  // Don't count intersections with itself
	  if (objct.oid == obj.oid) continue;
	  ppoint shadowintersection = inObj(shadowpoint, lightray, objct);
	  if (shadowintersection.t > 0.0) {
	    rintensity -= obj.r*tempintensity*light.r;
	    gintensity -= obj.g*tempintensity*light.g;
	    bintensity -= obj.b*tempintensity*light.b;
	    break;
	  }
	}

      }



      rintensity += obj.r*obj.reflectanceCoeff*ambientr;
      gintensity += obj.g*obj.reflectanceCoeff*ambientg;
      bintensity += obj.b*obj.reflectanceCoeff*ambientb;
      if (rintensity > 1.0f) rintensity = 1.0f;
      if (gintensity > 1.0f) gintensity = 1.0f;
      if (bintensity > 1.0f) bintensity = 1.0f;

      point specpt;
      specpt.x = intersection.x;
      specpt.y = intersection.y;
      specpt.z = intersection.z;

      point dir;
      float px = 0.0 + x;
      float py = 0.0 + y;
      dir = arbaxisrot(pixelplaneToDir(px, py), findNormal(intersection, obj), PI);
      light specularity = calculateSpecularity (specpt, dir, obj, depth);

      rintensity += specularity.r;
      gintensity += specularity.g;
      bintensity += specularity.b;
      
      if (rintensity > 1) rintensity = 1.0f;
      if (gintensity > 1) gintensity = 1.0f;
      if (bintensity > 1) bintensity = 1.0f;

      light pixelColor;
      pixelColor.r = rintensity;
      pixelColor.g = gintensity;
      pixelColor.b = bintensity;
      return pixelColor;
    }
  }
  light pixelColor;
  pixelColor.r = 0;
  pixelColor.g = 0;
  pixelColor.b = 0;
  return pixelColor;
}



light getObjColor(ppoint intersection, object obj, point incomingray, int depth) {


if (1) {
    
    // The point is in the object, so we need to find the intensity at that point
    
    // First, we calculate the normal at that point
    point normal = findNormal(intersection, obj);
    
    float rintensity = 0.0f;
    float gintensity = 0.0f;
    float bintensity = 0.0f;
    float cosTheta;
    float nx = normal.x;
    float ny = normal.y;
    float nz = normal.z;
    
    // Only plot if the z of the normal is positive
    if (1) {
      for (int i = 0 ; i < numlights ; i++) {
	light light = lightlist[i];
	// lightv is the light vector relative to the intersection point
	point lightv;
	lightv.x = light.source.x - intersection.x;
	lightv.y = light.source.y - intersection.y;
	lightv.z = light.source.z - intersection.z;
	float tempintensity = 1.0f;
	cosTheta = nx*lightv.x + ny*lightv.y + nz*lightv.z;
	cosTheta /= sqrt(lightv.x*lightv.x +
			 lightv.y*lightv.y +
			 lightv.z*lightv.z);
	cosTheta /= sqrt(nx*nx + ny*ny + nz*nz);
	tempintensity *= cosTheta*obj.reflectanceCoeff;
	if (tempintensity < 0.0f) tempintensity = 0.0f;
	rintensity += obj.r*tempintensity*light.r;
	gintensity += obj.g*tempintensity*light.g;
	bintensity += obj.b*tempintensity*light.b;

	// Determine whether object is in the shadow
	point shadowpoint;
	point lightray;
	shadowpoint.x = intersection.x;
	shadowpoint.y = intersection.y;
	shadowpoint.z = intersection.z;
	lightray.x = lightv.x;
	lightray.y = lightv.y;
	lightray.z = lightv.z;
	float raylength = sqrt(lightray.x*lightray.x +
			       lightray.y*lightray.y +
			       lightray.z*lightray.z);
	lightray.x /= raylength;
	lightray.y /= raylength;
	lightray.z /= raylength;
	for (int o = 0 ; o < numobjs ; o++) {
	  object objct = objlist[o];
	  // Don't count intersections with itself
	  if (objct.oid == obj.oid) continue;
	  ppoint shadowintersection = inObj(shadowpoint, lightray, objct);
	  if (shadowintersection.t > 0.0) {
	    rintensity -= obj.r*tempintensity*light.r;
	    gintensity -= obj.g*tempintensity*light.g;
	    bintensity -= obj.b*tempintensity*light.b;
	    break;
	  }
	}

      }

      rintensity += obj.r*obj.reflectanceCoeff*ambientr;
      gintensity += obj.g*obj.reflectanceCoeff*ambientg;
      bintensity += obj.b*obj.reflectanceCoeff*ambientb;
      if (rintensity > 1.0f) rintensity = 1.0f;
      if (gintensity > 1.0f) gintensity = 1.0f;
      if (bintensity > 1.0f) bintensity = 1.0f;

      point specpt;
      specpt.x = intersection.x;
      specpt.y = intersection.y;
      specpt.z = intersection.z;

      point dir;
      dir = arbaxisrot(incomingray, findNormal(intersection, obj), PI);
      light specularity = calculateSpecularity (specpt, dir, obj, depth);

      rintensity += specularity.r;
      gintensity += specularity.g;
      bintensity += specularity.b;

      light pixelColor;
      pixelColor.r = rintensity;
      pixelColor.g = gintensity;
      pixelColor.b = bintensity;
      return pixelColor;
    }
  }
  light pixelColor;
  pixelColor.r = 0;
  pixelColor.g = 0;
  pixelColor.b = 0;
  return pixelColor;
}












// Given a point (representing the direction vector of the ray)
// and an object, return a ppoint where the pixel ray hits the object.
// If it doesn't intersect, return 0.
ppoint inObj (point start, point dir, object obj) {
  if (obj.type == SPHERE) {
    object sphere = obj;
    // This is from the ray tracing slides
    float B = 2*(start.x - sphere.center.x)*dir.x +
      2*(start.y - sphere.center.y)*dir.y +
      2*(start.z - sphere.center.z)*dir.z;
    float C = (start.x - sphere.center.x)*(start.x - sphere.center.x) +
      (start.y - sphere.center.y)*(start.y - sphere.center.y) +
      (start.z - sphere.center.z)*(start.z - sphere.center.z) -
      sphere.radius * sphere.radius;

    float discriminant = B*B - 4*C;
    if (discriminant < 0) {
      // ray doesn't intersect sphere
      ppoint intersection;
      intersection.obj = obj;
      intersection.t = -1;
      return intersection;
    } else if (discriminant == 0) {
      // ray grazes sphere
      ppoint intersection;
      intersection.obj = obj;
      intersection.t = -B/2;
      intersection.x = start.x + intersection.t*dir.x;
      intersection.y = start.y + intersection.t*dir.y;
      intersection.z = start.z + intersection.t*dir.z;
      return intersection;
    } else {
      // ray intersects sphere in 2 places
      float bigt = (-B + sqrt(discriminant))/2;
      float smallt = (-B - sqrt(discriminant))/2;
      // Select smallest positive t
      if (smallt > 0) {
	// smallt is > 0, so use this value
	ppoint intersection;
	intersection.obj = obj;
	intersection.t = smallt;
	intersection.x = start.x + smallt*dir.x;
	intersection.y = start.y + smallt*dir.y;
	intersection.z = start.z + smallt*dir.z;
	return intersection;
      } else if (bigt > 0) {
	// big is > 0, so use this value
	ppoint intersection;
	intersection.obj = obj;
	intersection.t = bigt;
	intersection.x = start.x + bigt*dir.x;
	intersection.y = start.y + bigt*dir.y;
	intersection.z = start.z + bigt*dir.z;
	return intersection;
      } else {
	// both bigt and smallt <= 0, so return 0
	ppoint intersection;
	intersection.t = -1;
	return intersection;
      }
    }
  } else if (obj.type == POLYGON) {
    polygon polygon = obj.p;
    // Find the intersection with the infinite plane of the polygon
    // This is from the ray tracing slides
    point pt = polygon.vertices[0];
    point normal = findNormal(pt, obj);
    float d = -(pt.x*normal.x + pt.y*normal.y + pt.z*normal.z);
    float NdotP = normal.x*start.x + normal.y*start.y + normal.z*start.z;
    float NdotU = normal.x*dir.x + normal.y*dir.y + normal.z*dir.z;
    if (NdotU == 0) {
      ppoint intersection;
      intersection.obj = obj;
      intersection.t = -1;
      return intersection;
    }
    float t = -(d + NdotP)/NdotU;
    if (t < 0) {
      ppoint intersection;
      intersection.obj = obj;
      intersection.t = -1;
      return intersection;
    }

    ppoint intersection;
    intersection.t = t;
    intersection.x = start.x + t*dir.x;
    intersection.y = start.y + t*dir.y;
    intersection.z = start.z + t*dir.z;
    intersection.obj = obj;

    if (pointInPolygonTest(intersection, polygon)) 
      return intersection;
    else {
      intersection.t = -1;
      return intersection;
    }

  }
}

float myabs(float n) {
  if (n < 0) return -n;
  else return n;
}

int pointInPolygonTest (ppoint q, polygon poly) {

  float nodeepsilon = 0.0001; // Node margin of error
  float totalepsilon = 0.01; // Total margin of error
  float anglesum, costheta, m1, m2;
  point p1, p2;
  int n = poly.numverts;

  anglesum = 0.0;
  for (int i = 0 ; i < n ; i++) {
    p1.x = poly.vertices[i].x - q.x;
    p1.y = poly.vertices[i].y - q.y;
    p1.z = poly.vertices[i].z - q.z;
    p2.x = poly.vertices[(i+1)%n].x - q.x;
    p2.y = poly.vertices[(i+1)%n].y - q.y;
    p2.z = poly.vertices[(i+1)%n].z - q.z;

    m1 = sqrt(p1.x*p1.x + p1.y*p1.y + p1.z*p1.z);
    m2 = sqrt(p2.x*p2.x + p2.y*p2.y + p2.z*p2.z);
    // The epsilon calculation is if we are on a vertex.
    // Consider this inside.
    if (m1*m2 <= nodeepsilon) return 1;
    else costheta = (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z) / (m1*m2);
    anglesum += acos(costheta);
  }
  if (myabs(anglesum - 2*PI) < totalepsilon) return 1;
  else return 0;
}


// Calculate the specular coefficient.
// I just made up this function, but it is similar to that of silver
float specularCoefficient (float theta) {
  return (0.7 + 0.3*theta*theta/(4*PI*PI));
  //return (theta/PI);
}

// Calculate the specularity intensity given the principal reflection ray.
// I'm returning a light just to hold the rgb intensity values.
// source is the point which we are calculating
// dir is the direction of the outgoing ray from that point
// depth is the current bounce depth
light calculateSpecularity (point source, point dir, object obj, int depth) {
  light specularI;
  if (depth == 1) {
    specularI.r = 0.0;
    specularI.g = 0.0;
    specularI.b = 0.0;
    for (int l = 0 ; l < numlights ; l++) {
      light light = lightlist[l];
      // lightv is the light vector relative to the intersection point
      point lightv;
      lightv.x = light.source.x - source.x;
      lightv.y = light.source.y - source.y;
      lightv.z = light.source.z - source.z;
      // calculate cosPhi
      float cosPhi = dir.x*-lightv.x + dir.y*-lightv.y + dir.z*-lightv.z;
      cosPhi /= sqrt(lightv.x*lightv.x +
		     lightv.y*lightv.y +
		     lightv.z*lightv.z);
      cosPhi /= sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
      if (cosPhi < 0) cosPhi = 0.0;

      float cosPhiExp = 1.0;
      for (int i = 0 ; i < obj.specularity ; i++) cosPhiExp *= cosPhi;
      // calculate theta
      point normal = findNormal(source, obj);
      float theta = dir.x*normal.x + dir.y*normal.y + dir.z*normal.z;
      theta /= sqrt(normal.x*normal.x +
		     normal.y*normal.y +
		     normal.z*normal.z);
      theta /= sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
      theta = acos(theta);
      // I'm not sure why, but sometimes theta is NaN when it should be PI....
      if (theta != theta) theta = PI;
      specularI.r += cosPhiExp * specularCoefficient(theta) * light.r;
      specularI.g += cosPhiExp * specularCoefficient(theta) * light.g;
      specularI.b += cosPhiExp * specularCoefficient(theta) * light.b;
      if (specularI.r < 0) specularI.r = 0;
      if (specularI.g < 0) specularI.r = 0;
      if (specularI.b < 0) specularI.r = 0;

      // Determine whether object is in the shadow
      point shadowpoint;
      shadowpoint.x = source.x;
      shadowpoint.y = source.y;
      shadowpoint.z = source.z;
      float raylength = sqrt(lightv.x*lightv.x +
			     lightv.y*lightv.y +
			     lightv.z*lightv.z);
      // Note: lightv gets normalized here
      lightv.x /= raylength;
      lightv.y /= raylength;
      lightv.z /= raylength;
      for (int o = 0 ; o < numobjs ; o++) {
	object objct = objlist[o];
	// Don't count intersections with itself
	if (objct.oid == obj.oid) continue;
	ppoint shadowintersection = inObj(shadowpoint, lightv, objct);
	if (shadowintersection.t > 0.0) {
	  // I can optimize shadow ray stuff if I need to later
	  specularI.r -= cosPhiExp * specularCoefficient(theta) * light.r;
	  specularI.g -= cosPhiExp * specularCoefficient(theta) * light.g;
	  specularI.b -= cosPhiExp * specularCoefficient(theta) * light.b;
	  break;
	}
      }

    }
    return specularI;
  } else {
    // Find the intersection with the object coming from the ray
    ppoint intersection;
    ppoint tempintersection;

    // The direction of dir (the outgoing light) needs to be reversed
    dir.x *= -1;
    dir.y *= -1;
    dir.z *= -1;

    intersection.y = -1;

    for (int o = 0 ; o < numobjs ; o++) {
      object objct = objlist[o];
      // This is the first object
      if (obj.oid != o && intersection.y == -1) {
	intersection = inObj(source, dir, objct);
      } else if (obj.oid != o) {
	tempintersection = inObj(source, dir, objct);
	if (tempintersection.t > 0) {
	  if ((intersection.t > 0 && tempintersection.t < intersection.t) || intersection.t < 0) {
	    intersection = tempintersection;
	  }
	}
      }
    }

    if (intersection.t <= 0) {
      // If there is no intersection, we get the specularity from the light

      dir.x *= -1;
      dir.y *= -1;
      dir.z *= -1;

      return calculateSpecularity(source, dir, obj, 1);
    } else {

      point normal = findNormal(source, obj);
      float theta = dir.x*normal.x + dir.y*normal.y + dir.z*normal.z;
      theta /= sqrt(normal.x*normal.x +
		     normal.y*normal.y +
		     normal.z*normal.z);
      theta /= sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
      theta = acos(theta);
      // I'm not sure why, but sometimes theta is NaN when it should be PI....
      if (theta != theta) theta = PI;

      // Otherwise, we find the intensity at the intersection point and return that
      light retlight = getObjColor(intersection, intersection.obj, dir, depth-1);
      retlight.r *= specularCoefficient(theta);
      retlight.g *= specularCoefficient(theta);
      retlight.b *= specularCoefficient(theta);
      return retlight;
    }

  }
}


// rotating about an arbitrary axis
point arbaxisrot(point pt, point axis, float angle) {
  point ret, axistransformed;
  float theta, phi;
  if (axis.z == 0 && axis.x == 0) theta = 0.0;
  else theta = atan(axis.z/axis.x);
  axistransformed = yaxisrot(axis, theta);
  //printf ("  axistransformed1: (%f, %f, %f)\n", axistransformed.x, axistransformed.y, axistransformed.z);
  if (axistransformed.x == 0 && axistransformed.y == 0) phi = 0.0;
  else  phi = -atan(axistransformed.x/axistransformed.y);
  axistransformed = zaxisrot(axistransformed, phi);
  //printf ("  axistransformed2: (%f, %f, %f)\n", axistransformed.x, axistransformed.y, axistransformed.z);
  //printf ("arbaxisrot(PI): (%f, %f, %f)\n", pt.x, pt.y, pt.z);
  //printf ("  theta = %f, phi = %f\n", theta, phi);
  ret = yaxisrot(pt, theta);
  //printf ("  yaxisrot(%f): (%f, %f, %f)\n", theta, ret.x, ret.y, ret.z);
  ret = zaxisrot(ret, phi);
  //printf ("  zaxisrot(%f): (%f, %f, %f)\n", phi, ret.x, ret.y, ret.z);
  ret = yaxisrot(ret, angle);
  //printf ("  yaxisrot(%f): (%f, %f, %f)\n", angle, ret.x, ret.y, ret.z);
  ret = zaxisrot(ret, -phi);
  //printf ("  zaxisrot(%f): (%f, %f, %f)\n", -phi, ret.x, ret.y, ret.z);
  ret = yaxisrot(ret, -theta);
  //printf ("  yaxisrot(%f): (%f, %f, %f)\n", -theta, ret.x, ret.y, ret.z);
  return ret;
}

point yaxisrot(point pt, float angle) {
  point ret;
  ret.x = pt.z*sin(angle) + pt.x*cos(angle);
  ret.y = pt.y;
  ret.z = pt.z*cos(angle) - pt.x*sin(angle);
  return ret;
}

point zaxisrot(point pt, float angle) {
  point ret;
  ret.x = pt.x*cos(angle) + pt.y*sin(angle);
  ret.y = -pt.x*sin(angle) + pt.y*cos(angle);
  ret.z = pt.z;
  return ret;
}



int main(int argc, char** argv) {

  if (argc != 2 || !(argv[1][0] > '0' && argv[1][0] < '6' && argv[1][1] == 0)) {
    printf("Please type ./ray #, where # is the following:\n");
    printf(" 1: Three colored spheres and a square\n");
    printf(" 2: Spheres in a box\n");
    printf(" 3: Spheres in a line\n");
    printf(" 4: Multicolored lights\n");
    printf(" 5: Gouraud Mouse\n\n");
    return 0;
  }

  // This tells which image to print
  int imageindex = argv[1][0] - '0';

  eye.x = 500;
  eye.y = 500;
  eye.z = 2000;
  pixelplane = 1000;


    // image parameters
    char *img_filename;

    if (imageindex == 1) {
      img_filename = "image-01.png";
      initializeObject1();
    } else if (imageindex == 2) {
      img_filename = "image-02.png";
      initializeObject2();
    } else if (imageindex == 3) {
      img_filename = "image-03.png";
      initializeObject3();
    } else if (imageindex == 4) {
      img_filename = "image-04.png";
      initializeObject4();
    } else if (imageindex == 5) {
      img_filename = "image-05.png";
      initializeObject5();
    }

    int img_width = WIDTH, img_height = HEIGHT;
    
    
    // open file for writing
    FILE *fp = fopen(img_filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Error: could not open %s for writing\n", img_filename);
        return 1;
    }
    
    // set up PNG structures
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        fprintf(stderr, "Error: png_create_write_struct() failed\n");
        return 2;
    }    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        fprintf(stderr, "Error: png_create_info_struct() failed\n");
        return 3;
    }
    
    // set up jump point for error handling
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        fprintf(stderr, "Error: libpng encountered an error\n");
        return 4;
    }
    
    // set the output file for writing PNG data
    png_init_io(png_ptr, fp);
    
    // set image parameters - width, height, 8 bits per channel, RGB mode
    png_set_IHDR(png_ptr, info_ptr, img_width, img_height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    // allocate and set image memory - 2D array of pixels accessed by pixel_data[y][x]
    pixel_data = (rgb_pixel**)png_malloc(png_ptr, img_height * sizeof(png_bytep));
    int row_idx;
    for (row_idx = 0; row_idx < img_height; row_idx++) {
        pixel_data[row_idx] = (rgb_pixel*)png_malloc(png_ptr, img_width * sizeof(rgb_pixel));
        memset(pixel_data[row_idx], 0, img_width * sizeof(rgb_pixel)); // fill with black
    }
    png_set_rows(png_ptr, info_ptr, (png_bytepp)pixel_data);
    
    
    // TODO: fill in pixel data, example:
    int x, y;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
	  plotPixel(x,y,BOUNCEDEPTH);
        }
    }
    
    
    // write PNG data to file
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    
    // finished successfully - clean up and exit
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    printf("Successfully wrote %s\n", img_filename);
    return 0;
}
