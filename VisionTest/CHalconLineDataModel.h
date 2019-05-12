#pragma once
#include "CHalconPointModel.h"
class CHalconLineDataModel
{
public:
	CHalconLineDataModel(CHalconPointModel& StartPt, CHalconPointModel& EndPt);
	CHalconLineDataModel();
	~CHalconLineDataModel();
	CHalconPointModel StartPoint;
	CHalconPointModel EndPoint;
};

