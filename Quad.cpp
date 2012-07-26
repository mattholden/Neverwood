// TO DO: REMOVE THIS AFTER THE RENDERER HAS BEEN IMPLEMENTED
#include <windows.h>
#include <gl/gl.h>

#include "Quad.h"

Quad::Quad (Vector& V1, Vector& V2, Vector& V3, Vector& V4)
{
	v1 = V1;
	v2 = V2;
	v3 = V3;
	v4 = V4;
}


///////////////////////
// COLLISION DETECTION
///////////////////////

bool Quad::CollidesWith (Plane& plane, Vector* vIntersection)
{
	// check if any of the quad's edges intersect the plane
	if (plane.CollidesWith (Line (v1,v2), vIntersection))
		return true;
	if (plane.CollidesWith (Line (v2,v3), vIntersection))
		return true;
	if (plane.CollidesWith (Line (v3,v4), vIntersection))
		return true;
	if (plane.CollidesWith (Line (v4,v1), vIntersection))
		return true;

	// no intersection: return false
	return false;
}

bool Quad::CollidesWith (Line& line, Vector* vIntersection)
{
	if (GetAsyncKeyState (0x41 + 'b' - 'a'))
		int blah = 0;
	// first, check if the line even intersects the plane containing the quad
	Vector vNormal = (v2 - v1) & (v4 - v1);
	Vector vPlaneIntersection;
	if (Plane(vNormal, v1).CollidesWith (line, &vPlaneIntersection) == false)
		return false;

	// now check of the plane's intersection point is in the quad
	if (!CollidesWith (vPlaneIntersection))
		return false;

	// the line intersects the quad, so return the result
	if (vIntersection)
		*vIntersection = vPlaneIntersection;
	return true;
}

bool Quad::CollidesWith (Triangle& other, Vector* vIntersection )
{
	// just do a line to quad check with all of the other triangle's lines
	if (other.CollidesWith (Line (v1,v2), vIntersection))
		return true;
	if (other.CollidesWith (Line (v2,v3), vIntersection))
		return true;
	if (other.CollidesWith (Line (v3,v4), vIntersection))
		return true;
	if (other.CollidesWith (Line (v4,v1), vIntersection))
		return true;

	return false;
}

bool Quad::CollidesWith (Quad& other, Vector* vIntersection )
{
	// just do a line to quad check with all of the other quad's lines
	if (other.CollidesWith (Line (v1,v2), vIntersection))
		return true;
	if (other.CollidesWith (Line (v2,v3), vIntersection))
		return true;
	if (other.CollidesWith (Line (v3,v4), vIntersection))
		return true;
	if (other.CollidesWith (Line (v4,v1), vIntersection))
		return true;

	return false;
}

bool Quad::CollidesWith (Vector& point)
{
	if (Line (v1,v2).SameSide (v3, point) == false)
		return false;
	if (Line (v2,v3).SameSide (v4, point) == false)
		return false;
	if (Line (v3,v4).SameSide (v1, point) == false)
		return false;
	if (Line (v4,v1).SameSide (v2, point) == false)
		return false;

	//the point passed all the half space tests, so check to see if it's co-planar
	/*
	Vector vToPoint = point - v1;
	Vector vNormal = (v2-v1) & (v3 - v1);
	if (!FloatEqual ( vToPoint * vNormal, 0.0f))
		return false;
	*/
	return true;
}

void Quad::Draw()
{
	/*glPushMatrix();
	glLoadIdentity();
	glBegin (GL_QUADS);
		glVertex3f (v1[0], v1[1], v1[2]);
		glVertex3f (v2[0], v2[1], v2[2]);
		glVertex3f (v3[0], v3[1], v3[2]);
		glVertex3f (v4[0], v4[1], v4[2]);
	glPopMatrix();*/
}


