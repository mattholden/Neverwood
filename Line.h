///////////////////////////////////////
// Line.h
//
// Defines functionality for a line segment in 3D space.
// A line segment is reprented by its start and end points
///////////////////////////////////////

#ifndef LINE_H
#define LINE_H

#include "Vector.h"

class Line
{
public:
	Vector vBegin;
	Vector vEnd;

	// creating lines
	Line (){}
	Line (Vector& begin, Vector& end);

	// convenience methods
	float GetLength();

	////////////////////////////
	// DistanceTo
	//
	// Determines how far apart two lines are
	// The computation uses infinite lines that contain the line segment
	////////////////////////////
	float DistanceTo (Line& other);

	// take a wild guess what this does
	float DistanceTo (Vector& point);

	////////////////////////////
	// ClosestPoint
	//
	// Given a point in space (P), ClosestPoint computes the point on this line that is closest to P
	// ClosestPoint does not take the length of the line segment into account: it outputs the closest point
	// on the INFINITE line containing a line segment
	////////////////////////////
	Vector ClosestPoint (Vector& point);

	// same as above, but computes the closest point to a line instead
	Vector ClosestPoint (Line& line);

	////////////////////////////
	// SameSide
	//
	// Determines whether two points are on the same side of a line
	// This is equivalent to a 3D half space test
	///////////////////////////
	bool SameSide (Vector& v1, Vector& v2);

	//////////////////////////////
	// ContainsPoint
	//
	// Checks if a given COLINEAR point is on a line segment
	// i.e. given a point on the infinite line containing this line segment,
	// ContainsPoint outputs whether this point is between the line segment's starting and ending points
	///////////////////////////////
	bool ContainsPoint (Vector& point);

	/////////////////////////////
	// Draw
	//
	// Draws the line in a given color. Color defaults to white
	//  colors are specified by their red, green, and blue components on a scale from 0 to 1
	/////////////////////////////
	void Draw (float Red=1.0f, float Green=1.0f, float Blue=1.0f);
};

#endif
