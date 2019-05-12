#pragma once
#include "CHalconPointModel.h"

class CHalconCircleModel
{
public:
	CHalconCircleModel();
	CHalconCircleModel(CHalconPointModel& CenterPt, double Radius);
	~CHalconCircleModel();
	CHalconPointModel CenterPoint;
	double Radius;
};

