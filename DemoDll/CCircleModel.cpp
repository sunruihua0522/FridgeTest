#include "CCircleModel.h"

CCircleModel::CCircleModel() {}
CCircleModel::CCircleModel(CPointModel& CenterPt,double Radius)
{
	this->CenterPoint = CenterPt;
	this->Radius = Radius;
}


CCircleModel::~CCircleModel()
{
}
