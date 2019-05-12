#pragma once
#include "CPointModel.h"

class CCircleModel
{
public:
	CCircleModel();
	CCircleModel(CPointModel& CenterPt, double Radius);
	~CCircleModel();
	CPointModel CenterPoint;
	double Radius;
};

