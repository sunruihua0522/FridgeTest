#include "pch.h"
#include "CHalconCircleModel.h"

CHalconCircleModel::CHalconCircleModel() {}
CHalconCircleModel::CHalconCircleModel(CHalconPointModel& CenterPt,double Radius)
{
	this->CenterPoint = CenterPt;
	this->Radius = Radius;
}


CHalconCircleModel::~CHalconCircleModel()
{
}
