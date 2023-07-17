#pragma once
#include "primitives.h"

inline int reduceContacts(const Vector4* p, int nPoints, const Vector3 nearNormal, int *contact4Out)
{
	if (nPoints == 0)
		return 0;

	if (nPoints <= 4)
		return nPoints;

	if (nPoints > 64)
		nPoints = 64;

	Vector3 center(0, 0, 0);
	{
		for (int i = 0; i < nPoints; i++)
			center += Vector3(p[i]);
		center /= (float)nPoints;
	}

	Vector3 aVector = Vector3(p[0]) - center;
	Vector3 u = glm::cross(nearNormal, aVector);
	Vector3 v = glm::cross(nearNormal, u);
	u = glm::normalize(u);
	v = glm::normalize(v);

	float minW = FLT_MAX;

	int minIndex = -1;

	Vector4 maxDots;
	maxDots.x = FLT_MIN;
	maxDots.y = FLT_MIN;
	maxDots.z = FLT_MIN;
	maxDots.w = FLT_MIN;

	for (int ie = 0; ie < nPoints; ie++)
	{
		if (p[ie].w < minW)
		{
			minW = p[ie].w;
			minIndex = ie;
		}
		float f;
		Vector3 r = Vector3(p[ie]) - center;
		f = glm::dot(u, r);
		if (f < maxDots.x)
		{
			maxDots.x = f;
			contact4Out[0] = ie;
		}

		f = glm::dot(-u, r);
		if (f < maxDots.y)
		{
			maxDots.y = f;
			contact4Out[1] = ie;
		}

		f = glm::dot(v, r);
		if (f < maxDots.z)
		{
			maxDots.z = f;
			contact4Out[2] = ie;
		}

		f = glm::dot(-v, r);
		if (f < maxDots.w)
		{
			maxDots.w = f;
			contact4Out[3] = ie;
		}
	}

	if (contact4Out[0] != minIndex && contact4Out[1] != minIndex && contact4Out[2] != minIndex && contact4Out[3] != minIndex)
	{
		contact4Out[0] = minIndex;
	}

	return 4;
}
