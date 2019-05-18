#pragma once
#include "HalconCpp.h"
#pragma comment(lib,"halconcpp.lib")
#include "CCircleModel.h"
#include "CLineModel.h"
#include "CPointModel.h"
#include "CPairModel.h"
#include "Hua.h"
using namespace std;
using namespace HalconCpp;



class  CHalconFuncSet
{
public:
	CHalconFuncSet();
	~CHalconFuncSet();
	void SetWindowHandle(HTuple WindowHandle);
	int FindLine(HObject Image, string Para, CLineModel* pLine);
	int FindPair(HObject Image, string Para, CPairModel* Pair);
	int FindCircle(HObject Image, string Para, CCircleModel* pCircle);
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

	enum ErrorCode
	{
		SUCCESS,
		FILE_IS_NOT_FOUND,
		MODEL_NOT_FOUND,
		NOT_ENOUGH_TUPLE,
		NOT_ENOUGH_OBJ,
		PARA_INVALID,
		FAILED,
	};
	//int ProcessImage(EnumPicType ImageType, double* Result,int& Num, bool draw_image);

	int ProcessImage1(EnumPicType ImageType,CPairModel& Pair, double* Result, int& Num,bool draw_image);
	int ProcessImage2(EnumPicType ImageType,CLineModel& Line, CPairModel& Pair, double* Result, int& Num, bool draw_image);
	int ProcessImage3(EnumPicType ImageType,CLineModel& LineSide, CLineModel& LineUp, CLineModel& LineDown, CCircleModel& CircleLeft, CCircleModel& CircleRight, CPairModel& PairSide, CPairModel& PairUp, CPairModel& PairDown, double* Result, int& Num, bool draw_image);



	double DistanceLineLine(CLineModel& Line1, CLineModel& Line2);
	double DistanceCircleLine(CCircleModel& Circle, CLineModel& Line);
	double AngleLineLine(CLineModel& Line1, CLineModel& Line2);
	

	void GenImage(unsigned char* data, int width, int height);
	void GenImage(char* FilePath);
	void SaveImage(const char* FilePath);
	void GetImageData(unsigned char* rgb_data,int width, int height);
	int AdjustImage(HObject ImageIn, HObject* ImageOut, string Para, HTuple& Hom2D);
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
	void PaintLine(HObject& ImageFore, CLineModel Line,int GrayValue);
	void PaintCircle(HObject& ImageFore, CCircleModel Circle, int GrayValue);
	

	HObject m_ImageGen,m_ImageOutFore,m_ImageOutBk;
	CRITICAL_SECTION CS;

};

