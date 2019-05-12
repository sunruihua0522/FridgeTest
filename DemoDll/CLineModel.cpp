#include "CLineModel.h"
CLineModel::CLineModel() {}
CLineModel::CLineModel(CPointModel& StartPt, CPointModel& EndPt)
{
	this->StartPoint = StartPt;
	this->EndPoint = EndPt;
}

CLineModel::~CLineModel()
{

}
