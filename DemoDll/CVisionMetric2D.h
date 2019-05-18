#pragma once
#include "Hua.h"
#ifdef MY_DLL_EXPORTS
#   define ML_DLL_EXP __declspec(dllexport)
#else
#   define ML_DLL_EXP __declspec(dllimport)
#endif
using namespace Hua;
class ML_DLL_EXP CVisionMetric2D
{
public:
	CVisionMetric2D();
	~CVisionMetric2D();

	enum EN_RobotPose
	{
		RP_POSE01,
		RP_POSE02,
		RP_POSE03,
		RP_POSE04,
		RP_POSE05,
		RP_POSE06,
		RP_POSE07,
		RP_POSE08,
		RP_POSE09,
		RP_POSE10
	};
	
	//! 输入图像文件
	void SetImageFile(const char* acFilePathNamefile_path_name);
	
	//! 输入图像数据
	void SetImageData(unsigned char* data, int width, int height);

	enum ErrorCode
	{
		// to do
		EC_FILE_NOT_EXIST = -3,
		EC_FILE_DATA_NOT_EXIST = -2,
		EC_FAILED = -1,
		EC_SUCCESS = 0
	};


	//int Compute(EN_RobotPose robot_pose, double *param_list, int& num_param, bool draw_image=false);
	
	//! 输出图像文件
	void SaveImage(const char* file_path_name);
	
	//! 输出图像数据
	bool GetImageData( unsigned char* rgb_data, int width ,int height);

	int Compute1(GAP2D& gap, bool draw_image = false);
	int Compute2(GAP2D& gap_Width, GAP2D& gap_Narrow, bool draw_image = false);
	int Compute3(GAP2D& gap_side, GAP2D& gap_up, GAP2D& gap_down, HOLEINFO& hole1, HOLEINFO& hole2, bool draw_image = false);
	int Compute8(GAP2D& gap_side, GAP2D& gap_up, GAP2D& gap_down, HOLEINFO& hole1, HOLEINFO& hole2, bool draw_image = false);
	int Compute9(GAP2D& gap_Width, GAP2D gap_Narrow, bool draw_image = false);
	int Compute10(GAP2D& gap, bool draw_image = false);
private:
	void*  m_pWapper;
	double m_DoubleBuff[20];
};

