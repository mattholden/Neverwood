#include "BoundingBox.h"
#include "shapes.h"

BoundingBox::BoundingBox() { iObjectType = SHAPE_BOX; }
BoundingBox::BoundingBox (float width, float height, float depth)
{
	iObjectType = SHAPE_BOX;
	fWidth = width;
	fHeight = height;
	fDepth = depth;
}

// CONVENIENCE METHODS
Quad BoundingBox::GetLeftFace()
{
	// left bottom back, left bottom front, left top front, left top back
	return Quad 
	(	
		Vector (GetLeft(), GetBottom(), GetBack()),
		Vector (GetLeft(), GetBottom(), GetFront()),
		Vector (GetLeft(), GetTop(), GetFront()),
		Vector (GetLeft(), GetTop(), GetBack())
	);
}
Quad BoundingBox::GetRightFace()
{
	//right bottom front, right bottom back, right top back, right top front
	return Quad 
	(	
		Vector (GetRight(), GetBottom(), GetFront()),
		Vector (GetRight(), GetBottom(), GetBack()),
		Vector (GetRight(), GetTop(), GetBack()),
		Vector (GetRight(), GetTop(), GetFront())
	);
}
Quad BoundingBox::GetTopFace()
{
	//left top front, right top front, right top back, left top back
	return Quad 
	(	
		Vector (GetLeft(), GetTop(), GetFront()),
		Vector (GetRight(), GetTop(), GetFront()),
		Vector (GetRight(), GetTop(), GetBack()),
		Vector (GetLeft(), GetTop(), GetBack())
	);
}
Quad BoundingBox::GetBottomFace()
{

	//left bottom back, right bottom back, right bottom front, left bottom front
	return Quad 
	(	
		Vector (GetLeft(), GetBottom(), GetBack()),
		Vector (GetRight(), GetBottom(), GetBack()),
		Vector (GetRight(), GetBottom(), GetFront()),
		Vector (GetLeft(), GetBottom(), GetFront())
	);
}
Quad BoundingBox::GetFrontFace()
{
	//left bottom front, right bottom front, right top front, left top front
	return Quad 
	(	
		Vector (GetLeft(), GetBottom(), GetFront()),
		Vector (GetRight(), GetBottom(), GetFront()),
		Vector (GetRight(), GetTop(), GetFront()),
		Vector (GetLeft(), GetTop(), GetFront())
	);
}
Quad BoundingBox::GetBackFace()
{
	//right bottom back, left bottom back, left top back, right top back
	return Quad 
	(	
		Vector (GetRight(), GetBottom(), GetBack()),
		Vector (GetLeft(), GetBottom(), GetBack()),
		Vector (GetLeft(), GetTop(), GetBack()),
		Vector (GetRight(), GetTop(), GetBack())
	);
}

// COLLISION DETECTION
bool BoundingBox::CollidesWith (BoundingShape* other, Vector* vIntersection)
{
	switch (other->iObjectType)
	{
	case SHAPE_SPHERE:
		return other->CollidesWith(this, vIntersection);
		break;
	case SHAPE_CYLINDER:
		//////////////////////////////////////////
		// The algorithm is to just run 6 cylinder to quad checks against the box's faces
		// (an alternative is to convert global to local coordinates, but that would be even more expensive)
		
		// this can get pretty expensive, so do a separate test first 
		// to see if the two shapes are even remotely close to each other
		if ( (other->GetPos()-GetPos()).Length() > fWidth + fHeight + fDepth + other->fWidth + other->fHeight)
			return false;

		//front quad
		if (other->CollidesWith (GetFrontFace(), vIntersection))
			return true;
		//back quad
		if (other->CollidesWith (GetBackFace(), vIntersection))
			return true;
		//left quad
		if (other->CollidesWith (GetLeftFace(), vIntersection))
			return true;
		//right quad
		if (other->CollidesWith (GetRightFace(), vIntersection))
			return true;
		//top quad
		if (other->CollidesWith (GetTopFace(), vIntersection))
			return true;
		//bottom quad
		if (other->CollidesWith (GetBottomFace(), vIntersection))
			return true;
		return false;	//note: at this point, the cylinder may be completely inside the box.
						//OPTIMIZATION: handle the case where the cylinder is completely inside the box if necessary
		break;

	case SHAPE_VERTICAL_CYLINDER:
		{
			//checking against a vertical cylinder is almost the same as checking against an axis aligned bounding box
		
			// box too far to the right
			if (GetLeft() > other->GetXPos() + other->fWidth * 0.5f)
				return false;
			// box too far to the left
			if (GetRight() < other->GetXPos() - other->fWidth * 0.5f)
				return false;
			// box too far back
			if (GetFront() > other->GetZPos() + other->fDepth * 0.5f)
				return false;
			// box too far forward
			if (GetBack() < other->GetZPos() - other->fDepth * 0.5f)
				return false;
			// box too high
			if (GetBottom() > other->GetYPos() + other->fHeight * 0.5f)
				return false;
			// box too low
			if (GetTop() < other->GetYPos() - other->fHeight * 0.5f)
				return false;
			return true;
		}
		break;
	case SHAPE_BOX:
		//check this box is too far to the right of the other box
		if (GetLeft() > ((BoundingBox*)other)->GetRight())
			return false;
		//too far to the left of the other box
		if (GetRight() < ((BoundingBox*)other)->GetLeft())
			return false;
		//too high
		if (GetBottom() > ((BoundingBox*)other)->GetTop())
			return false;
		//too low
		if (GetTop() < ((BoundingBox*)other)->GetBottom())
			return false;
		//too far to the front
		if (GetBack() > ((BoundingBox*)other)->GetFront())
			return false;
		//too far back
		if (GetFront() < ((BoundingBox*)other)->GetBack())
			return false;
		if (vIntersection)
			*vIntersection = (GetPos() + other->GetPos()) * 0.5f;
		return true;
		break;
	case SHAPE_FRUSTUM:
		return other->CollidesWith (this, vIntersection);
		break;
	}//switch (shape)
	return false;
}

bool BoundingBox::CollidesWith (Vector& vPoint)
{
	return (vPoint[0] >= GetLeft() && vPoint[0] <= GetRight()
			&& vPoint[1] >= GetBottom() && vPoint[1] <= GetTop()
			&& vPoint[2] >= GetBack() && vPoint[2] <= GetFront());
}

bool BoundingBox::CollidesWith (Line& line, Vector* vIntersection)
{
	//front quad
	if (GetFrontFace().CollidesWith (line, vIntersection))
		return true;
	//back quad
	if (GetBackFace().CollidesWith (line, vIntersection))
		return true;
	//left quad
	if (GetLeftFace().CollidesWith (line, vIntersection))
		return true;
	//right quad
	if (GetRightFace().CollidesWith (line, vIntersection))
		return true;
	//bottom quad
	if (GetBottomFace().CollidesWith (line, vIntersection))
		return true;
	//top quad
	if (GetTopFace().CollidesWith (line, vIntersection))
		return true;
	return false;
}
bool BoundingBox::CollidesWith (Triangle& triangle, Vector* vIntersection)
{
	if (CollidesWith (Line (triangle.v1, triangle.v2), vIntersection))
		return true;
	if (CollidesWith (Line (triangle.v2, triangle.v3), vIntersection))
		return true;
	if (CollidesWith (Line (triangle.v3, triangle.v1), vIntersection))
		return true;
	return false;
}
bool BoundingBox::CollidesWith (Quad& quad, Vector* vIntersection)
{
	if (CollidesWith (Line (quad.v1, quad.v2), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v2, quad.v3), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v3, quad.v4), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v4, quad.v1), vIntersection))
		return true;
	return false;
}

void BoundingBox::Draw()
{
	Shapes::SetColor(fRed, fGreen, fBlue);
	Shapes::Cube(&mGlobal, fWidth, fHeight, fDepth);
}

