#include "pch.h"
#include "CHalconLineDataModel.h"
CHalconLineDataModel::CHalconLineDataModel() {}
CHalconLineDataModel::CHalconLineDataModel(CHalconPointModel& StartPt, CHalconPointModel& EndPt)
{
	this->StartPoint = StartPt;
	this->EndPoint = EndPt;
}

CHalconLineDataModel::~CHalconLineDataModel()
{

}
