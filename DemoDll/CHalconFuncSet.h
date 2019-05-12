#pragma once
#include "HalconCpp.h"
#pragma comment(lib,"halconcpp.lib")
#include "CCircleModel.h"
#include "CLineModel.h"
#include "CPointModel.h"

using namespace std;
using namespace HalconCpp;



class  CHalconFuncSet
{
public:
	CHalconFuncSet();
	~CHalconFuncSet();
	void SetWindowHandle(HTuple WindowHandle);
	void FindLine(HObject Image, string Para, CLineModel* pLine);
	void FindPair(HObject Image, string Para, CLineModel* pStartLine, CLineModel* pEndLine);
	void FindCircle(HObject Image, string Para, CCircleModel* pCircle);
	enum EnumPicType
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
	void ProcessImage(EnumPicType ImageType, double* Result);
	void GenImage(unsigned char* data, int width, int height);
	void GenImage(char* FilePath);
	void SaveImage(const char* FilePath);
private:
	void spoke(HObject ho_Image, HObject *ho_Regions, HTuple hv_Elements, HTuple hv_DetectHeight,
		HTuple hv_DetectWidth, HTuple hv_Sigma, HTuple hv_Threshold, HTuple hv_Transition,
		HTuple hv_Select, HTuple hv_ROIRows, HTuple hv_ROICols, HTuple hv_Direct, HTuple *hv_ResultRow,
		HTuple *hv_ResultColumn, HTuple *hv_ArcType);
	void gen_arrow_contour_xld(HObject *ho_Arrow, HTuple hv_Row1, HTuple hv_Column1,
		HTuple hv_Row2, HTuple hv_Column2, HTuple hv_HeadLength, HTuple hv_HeadWidth);
	void dev_open_window_fit_image(HObject ho_Image, HTuple hv_Row, HTuple hv_Column,
		HTuple hv_WidthLimit, HTuple hv_HeightLimit, HTuple *hv_WindowHandle);

	void PaintLine(HObject& ImageFore, HObject& ImageBk, CLineModel Line);
	void PaintCircle(HObject& ImageFore, HObject& ImageBk, CCircleModel Circle);

	double DistanceLineLine(CLineModel& Line1, CLineModel& Line2);
	double DistanceCircleLine(CCircleModel& Circle, CLineModel& Line);

	HObject m_ImageGen,m_ImageOutFore,m_ImageOutBk;

};

