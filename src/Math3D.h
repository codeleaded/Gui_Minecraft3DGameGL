#ifndef MATH3D_H
#define MATH3D_H

#include "/home/codeleaded/System/Static/Container/Vector.h"

typedef struct Vec2D{
	float u;
	float v;
	float w;
} Vec2D;

Vec2D Vec2D_New(){
    return (Vec2D){ 0.0f,0.0f,1.0f };
}
Vec2D Vec2D_Make(float u,float v){
    return (Vec2D){ u,v,1.0f };
}


typedef struct Vec3D{
	float x;
	float y;
	float z;
	float w;
} Vec3D;

Vec3D Vec3D_new(float x,float y,float z){
	return (Vec3D){ x,y,z,1.0f };
}
Vec3D Vec3D_Add(Vec3D v1, Vec3D v2)
{
	return Vec3D_new( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}
Vec3D Vec3D_Sub(Vec3D v1, Vec3D v2)
{
	return Vec3D_new( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}
Vec3D Vec3D_Mul(Vec3D v1, float k)
{
	return Vec3D_new( v1.x * k, v1.y * k, v1.z * k );
}
Vec3D Vec3D_Div(Vec3D v1, float k)
{
	if(k==0.0f) return Vec3D_new( 0.0f,0.0f,0.0f );
	return Vec3D_new( v1.x / k, v1.y / k, v1.z / k );
}
float Vec3D_DotProduct(Vec3D v1, Vec3D v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}
float Vec3D_Length(Vec3D v)
{
	return sqrtf(Vec3D_DotProduct(v, v));
}
Vec3D Vec3D_Normalise(Vec3D v){
	float l = Vec3D_Length(v);
	return Vec3D_new( v.x / l, v.y / l, v.z / l );
}
Vec3D Vec3D_Perp(Vec3D v){
	return Vec3D_new( -v.z, v.y, v.x );
}
Vec3D Vec3D_Round(Vec3D v){
	return Vec3D_new( roundf(v.x),roundf(v.y),roundf(v.z) );
}

Vec3D Vec3D_CrossProduct(Vec3D v1, Vec3D v2){
	Vec3D v = Vec3D_new(0.0f,0.0f,0.0f);
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}
Vec3D Vec3D_IntersectPlane(Vec3D plane_p, Vec3D* plane_n, Vec3D lineStart, Vec3D lineEnd){
	*plane_n = Vec3D_Normalise(*plane_n);
	float plane_d = -Vec3D_DotProduct(*plane_n, plane_p);
	float ad = Vec3D_DotProduct(lineStart, *plane_n);
	float bd = Vec3D_DotProduct(lineEnd, *plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	Vec3D lineStartToEnd = Vec3D_Sub(lineEnd, lineStart);
	Vec3D lineToIntersect = Vec3D_Mul(lineStartToEnd, t);
	return Vec3D_Add(lineStart, lineToIntersect);
}

typedef struct Tri3D{
	Vec3D p[3];
	Vec3D n;
	unsigned int c;
} Tri3D;

void Triangle_CalcNorm(Tri3D* t){
	Vec3D line1 = Vec3D_Sub(t->p[1],t->p[0]);
	Vec3D line2 = Vec3D_Sub(t->p[2],t->p[0]);
	Vec3D normal = Vec3D_CrossProduct(line1,line2);

	t->n = Vec3D_Normalise(normal);
}

#define CUBE_SIDE_SOUTH      0
#define CUBE_SIDE_WEST       1
#define CUBE_SIDE_NORTH      2
#define CUBE_SIDE_EAST       3
#define CUBE_SIDE_TOP        4
#define CUBE_SIDE_BOTTOM     5

#define CUBE_SIDE_000		0
#define CUBE_SIDE_001		1
#define CUBE_SIDE_100		2
#define CUBE_SIDE_101		3
#define CUBE_SIDE_002		4
#define CUBE_SIDE_102		5
#define CUBE_SIDE_200		6
#define CUBE_SIDE_201		7
#define CUBE_SIDE_202		8
#define CUBE_SIDE_010		9
#define CUBE_SIDE_011		10
#define CUBE_SIDE_110		11
#define CUBE_SIDE_111		12
#define CUBE_SIDE_012		13
#define CUBE_SIDE_112		14
#define CUBE_SIDE_210		15
#define CUBE_SIDE_211		16
#define CUBE_SIDE_212		17
#define CUBE_SIDE_020		18
#define CUBE_SIDE_021		19
#define CUBE_SIDE_120		20
#define CUBE_SIDE_121		21
#define CUBE_SIDE_022		22
#define CUBE_SIDE_122		23
#define CUBE_SIDE_220		24
#define CUBE_SIDE_221		25
#define CUBE_SIDE_222		26

Vec3D Neighbour_Side(int s){
	switch (s){
		case CUBE_SIDE_NORTH: 	return (Vec3D){ 0.0f, 0.0f,-1.0f,1.0f };
		case CUBE_SIDE_EAST: 	return (Vec3D){ 1.0f, 0.0f, 0.0f,1.0f };
		case CUBE_SIDE_SOUTH: 	return (Vec3D){ 0.0f, 0.0f, 1.0f,1.0f };
		case CUBE_SIDE_WEST: 	return (Vec3D){-1.0f, 0.0f, 0.0f,1.0f };
		case CUBE_SIDE_TOP: 	return (Vec3D){ 0.0f, 1.0f, 0.0f,1.0f };
		case CUBE_SIDE_BOTTOM: 	return (Vec3D){ 0.0f,-1.0f, 0.0f,1.0f };
	}
	return (Vec3D){ 0.0f,0.0f,0.0f,1.0f };
}
int Side_Other(int s){
	switch (s){
	case CUBE_SIDE_SOUTH: 	return CUBE_SIDE_NORTH;
	case CUBE_SIDE_EAST: 	return CUBE_SIDE_WEST;
	case CUBE_SIDE_NORTH: 	return CUBE_SIDE_SOUTH;
	case CUBE_SIDE_WEST: 	return CUBE_SIDE_EAST;
	case CUBE_SIDE_TOP: 	return CUBE_SIDE_BOTTOM;
	case CUBE_SIDE_BOTTOM: 	return CUBE_SIDE_TOP;
	}
	return CUBE_SIDE_SOUTH;
}
Vec3D Neighbour_AllSide(int s){
	switch (s){
		case CUBE_SIDE_000: return (Vec3D){-1.0f,-1.0f,-1.0f,1.0f };
		case CUBE_SIDE_001: return (Vec3D){-1.0f,-1.0f, 0.0f,1.0f };
		case CUBE_SIDE_100: return (Vec3D){ 0.0f,-1.0f,-1.0f,1.0f };
		case CUBE_SIDE_101: return (Vec3D){ 0.0f,-1.0f, 0.0f,1.0f };
		case CUBE_SIDE_002: return (Vec3D){-1.0f,-1.0f, 1.0f,1.0f };
		case CUBE_SIDE_102: return (Vec3D){ 0.0f,-1.0f, 1.0f,1.0f };
		case CUBE_SIDE_200: return (Vec3D){ 1.0f,-1.0f,-1.0f,1.0f };
		case CUBE_SIDE_201: return (Vec3D){ 1.0f,-1.0f, 0.0f,1.0f };
		case CUBE_SIDE_202: return (Vec3D){ 1.0f,-1.0f, 1.0f,1.0f };
		case CUBE_SIDE_010: return (Vec3D){-1.0f, 0.0f,-1.0f,1.0f };
		case CUBE_SIDE_011: return (Vec3D){-1.0f, 0.0f, 0.0f,1.0f };
		case CUBE_SIDE_110: return (Vec3D){ 0.0f, 0.0f,-1.0f,1.0f };
		case CUBE_SIDE_111: return (Vec3D){ 0.0f, 0.0f, 0.0f,1.0f };
		case CUBE_SIDE_012: return (Vec3D){-1.0f, 0.0f, 1.0f,1.0f };
		case CUBE_SIDE_112: return (Vec3D){ 0.0f, 0.0f, 1.0f,1.0f };
		case CUBE_SIDE_210: return (Vec3D){ 1.0f, 0.0f,-1.0f,1.0f };
		case CUBE_SIDE_211: return (Vec3D){ 1.0f, 0.0f, 0.0f,1.0f };
		case CUBE_SIDE_212: return (Vec3D){ 1.0f, 0.0f, 1.0f,1.0f };
		case CUBE_SIDE_020: return (Vec3D){-1.0f, 1.0f,-1.0f,1.0f };
		case CUBE_SIDE_021: return (Vec3D){-1.0f, 1.0f, 0.0f,1.0f };
		case CUBE_SIDE_120: return (Vec3D){ 0.0f, 1.0f,-1.0f,1.0f };
		case CUBE_SIDE_121: return (Vec3D){ 0.0f, 1.0f, 0.0f,1.0f };
		case CUBE_SIDE_022: return (Vec3D){-1.0f, 1.0f, 1.0f,1.0f };
		case CUBE_SIDE_122: return (Vec3D){ 0.0f, 1.0f, 1.0f,1.0f };
		case CUBE_SIDE_220: return (Vec3D){ 1.0f, 1.0f,-1.0f,1.0f };
		case CUBE_SIDE_221: return (Vec3D){ 1.0f, 1.0f, 0.0f,1.0f };
		case CUBE_SIDE_222: return (Vec3D){ 1.0f, 1.0f, 1.0f,1.0f };
	}
	return (Vec3D){ 0.0f,0.0f,0.0f,1.0f };
}

typedef struct mesh{
	Vector tris;
} mesh;

typedef struct M4x4D{
	float m[4][4];
} M4x4D;

M4x4D Matrix_Null()
{
	return (M4x4D){{
		{ 0.0f,0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f,0.0f }
	}};
}
Vec3D Matrix_MultiplyVector(M4x4D m, Vec3D i)
{
	Vec3D v;
	v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
	return v;
}
M4x4D Matrix_MakeIdentity()
{
	M4x4D matrix = Matrix_Null();
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}
M4x4D Matrix_MakeRotationX(float fAngleRad)
{
	M4x4D matrix = Matrix_Null();
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[1][2] = sinf(fAngleRad);
	matrix.m[2][1] = -sinf(fAngleRad);
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}
M4x4D Matrix_MakeRotationY(float fAngleRad)
{
	M4x4D matrix = Matrix_Null();
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][2] = sinf(fAngleRad);
	matrix.m[2][0] = -sinf(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}
M4x4D Matrix_MakeRotationZ(float fAngleRad)
{
	M4x4D matrix = Matrix_Null();
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][1] = sinf(fAngleRad);
	matrix.m[1][0] = -sinf(fAngleRad);
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}
M4x4D Matrix_MakeTranslation(float x, float y, float z)
{
	M4x4D matrix = Matrix_Null();
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}
M4x4D Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	M4x4D matrix = Matrix_Null();
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}
M4x4D Matrix_MultiplyMatrix(M4x4D m1, M4x4D m2)
{
	M4x4D matrix = Matrix_Null();
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}

float dist(Vec3D plane_p, Vec3D plane_n,Vec3D p){
	Vec3D n = Vec3D_Normalise(p);
	return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vec3D_DotProduct(plane_n, plane_p));
}

int Triangle_ClipAgainstPlane(Vec3D plane_p, Vec3D plane_n, Tri3D in_tri, Tri3D* out_tri1, Tri3D* out_tri2){
	// Make sure plane normal is indeed normal
	plane_n = Vec3D_Normalise(plane_n);
	
	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	Vec3D* inCUBE_SIDE_points[3];  int nInsidePointCount = 0;
	Vec3D* outCUBE_SIDE_points[3]; int nOutsidePointCount = 0;
	// Get signed distance of each point in Tri3D to plane
	float d0 = dist(plane_p,plane_n,in_tri.p[0]);
	float d1 = dist(plane_p,plane_n,in_tri.p[1]);
	float d2 = dist(plane_p,plane_n,in_tri.p[2]);
	if (d0 >= 0) { inCUBE_SIDE_points[nInsidePointCount++] = &in_tri.p[0]; }
	else { outCUBE_SIDE_points[nOutsidePointCount++] = &in_tri.p[0]; }
	if (d1 >= 0) { inCUBE_SIDE_points[nInsidePointCount++] = &in_tri.p[1]; }
	else { outCUBE_SIDE_points[nOutsidePointCount++] = &in_tri.p[1]; }
	if (d2 >= 0) { inCUBE_SIDE_points[nInsidePointCount++] = &in_tri.p[2]; }
	else { outCUBE_SIDE_points[nOutsidePointCount++] = &in_tri.p[2]; }
	// Now classify Tri3D points, and break the input Tri3D into 
	// smaller output Tri3Ds if required. There are four possible
	// outcomes...
	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole Tri3D
		// It ceases to exist
		return 0; // No returned Tri3Ds are valid
	}
	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the Tri3D to simply pass through
		*out_tri1 = in_tri;
		return 1; // Just the one returned original Tri3D is valid
	}
	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the Tri3D simply becomes a smaller Tri3D
		// Copy appearance info to new Tri3D
		out_tri1->c =  in_tri.c;
		// The inside point is valid, so keep that...
		out_tri1->p[0] = *inCUBE_SIDE_points[0];
		// but the two new points are at the locations where the 
		// original sides of the Tri3D (lines) intersect with the plane
		out_tri1->p[1] = Vec3D_IntersectPlane(plane_p, &plane_n, *inCUBE_SIDE_points[0], *outCUBE_SIDE_points[0]);
		out_tri1->p[2] = Vec3D_IntersectPlane(plane_p, &plane_n, *inCUBE_SIDE_points[0], *outCUBE_SIDE_points[1]);
		return 1; // Return the newly formed single Tri3D
	}
	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped Tri3D becomes a "quad". Fortunately, we can
		// represent a quad with two new Tri3Ds
		// Copy appearance info to new Tri3Ds
		out_tri1->c =  in_tri.c;
		out_tri2->c =  in_tri.c;
		// The first Tri3D consists of the two inside points and a new
		// point determined by the location where one side of the Tri3D
		// intersects with the plane
		out_tri1->p[0] = *inCUBE_SIDE_points[0];
		out_tri1->p[1] = *inCUBE_SIDE_points[1];
		out_tri1->p[2] = Vec3D_IntersectPlane(plane_p, &plane_n, *inCUBE_SIDE_points[0], *outCUBE_SIDE_points[0]);
		// The second Tri3D is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// Tri3D and the plane, and the newly created point above
		out_tri2->p[0] = *inCUBE_SIDE_points[1];
		out_tri2->p[1] = out_tri1->p[2];
		out_tri2->p[2] = Vec3D_IntersectPlane(plane_p, &plane_n, *inCUBE_SIDE_points[1], *outCUBE_SIDE_points[0]);
		return 2; // Return two newly formed Tri3Ds which form a quad
	}
}

M4x4D Matrix_PointAt(Vec3D pos, Vec3D target, Vec3D up){
	// Calculate new forward direction
	Vec3D newForward = Vec3D_Sub(target, pos);
	newForward = Vec3D_Normalise(newForward);
	// Calculate new Up direction
	Vec3D a = Vec3D_Mul(newForward, Vec3D_DotProduct(up,newForward));
	Vec3D newUp = Vec3D_Sub(up, a);
	newUp = Vec3D_Normalise(newUp);
	// New Right direction is easy, its just cross product
	Vec3D newRight = Vec3D_CrossProduct(newUp, newForward);
	// Construct Dimensioning and Translation Matrix	
	M4x4D matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;
}
M4x4D Matrix_QuickInverse(M4x4D m) // Only for Rotation/Translation Matrices
{
	M4x4D matrix;
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

#endif