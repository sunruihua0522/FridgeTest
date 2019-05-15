#include "CHalconFuncSet.h"
#include "CVisionMetric2D.h"
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

void CVisionMetric2D::SetImageFile(const char* acFilePathNamefile_path_name)
{
	static_cast<CHalconFuncSet*> (m_pWapper)->GenImage(_strdup(acFilePathNamefile_path_name));
}
//! 计算
int CVisionMetric2D::Compute(EN_RobotPose robot_pose, double *param_list, int& num_param)
{
	try
	{
		return 0==static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage((CHalconFuncSet::EnumPicType)robot_pose, param_list, num_param)? EC_SUCCESS : EC_FAILED;
	}
	catch(exception ex)
	{
		return EC_FAILED;
	}
	
}

void CVisionMetric2D::SaveImage(const char* FilePath)
{
	static_cast<CHalconFuncSet*> (m_pWapper)->SaveImage(FilePath);
}

bool CVisionMetric2D::GetImageData(unsigned char* rgb_data, int width, int height)
{
	try
	{
		static_cast<CHalconFuncSet*>(m_pWapper)->GetImageData(rgb_data, width, height);
		return true;
	}
	catch(exception ex)
	{
		return false;
	}
}
