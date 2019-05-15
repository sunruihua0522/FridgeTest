#include "CVisionMetric2D.h"
#include "CHalconFuncSet.h"
#include <stdio.h>

CVisionMetric2D::CVisionMetric2D()
{
	m_pWapper = new CHalconFuncSet();
	
}

CVisionMetric2D::~CVisionMetric2D()
{
	if (m_pWapper != NULL)
		delete m_pWapper;
}//! 设置图像数据

void CVisionMetric2D::SetImageData(unsigned char* data, int width, int height)
{
	static_cast<CHalconFuncSet*> (m_pWapper)->GenImage(data, width, height);
}

void CVisionMetric2D::SetImageData(char* FilePath)
{
	static_cast<CHalconFuncSet*> (m_pWapper)->GenImage(FilePath);
}
//! 计算
double* CVisionMetric2D::Compute(EN_RobotPose robot_pose)
{
	try
	{
		static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage((CHalconFuncSet::EnumPicType)robot_pose, m_pResult);
	}
	catch(exception ex)
	{
		return NULL;
	}
	for(int i=0;i<20;i++)
	{
		printf("%f\n", *(m_pResult+i));
		if (*(m_pResult+i) == -1)
			break;
	}
	return m_pResult;
}

void CVisionMetric2D::SaveImage(const char* FilePath)
{
	static_cast<CHalconFuncSet*> (m_pWapper)->SaveImage(FilePath);
}

void CVisionMetric2D:: Debug()
{
	MessageBox(0,"just for debug","",0);
}
