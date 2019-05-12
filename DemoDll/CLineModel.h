#pragma once
#include "CPointModel.h"
class CLineModel
{
public:
	CLineModel(CPointModel& StartPt, CPointModel& EndPt);
	CLineModel();
	~CLineModel();
	CPointModel StartPoint;
	CPointModel EndPoint;
};

