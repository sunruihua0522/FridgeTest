#ifdef MY_DLL_EXPORTS
#   define ML_DLL_EXP __declspec(dllexport)
#else
#   define ML_DLL_EXP __declspec(dllimport)
#endif

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

	//! 设置图像数据
	void SetImageData(unsigned char* data, int width, int height);
	void SetImageData(char* FilePath);

	//! 计算,-1为终止符号
	double* Compute(EN_RobotPose robot_pose);

	void SaveImage(const char* FilePath);

private:
	void* m_pWapper;
	double m_pResult[20];
};

