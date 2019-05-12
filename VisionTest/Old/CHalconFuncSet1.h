#ifdef MY_DLL_EXPORTS
#   define MY_DLL_EXP __declspec(dllexport)
#else
#   define MY_DLL_EXP __declspec(dllimport)
#endif

#include "HalconCpp.h"
#pragma comment(lib,"halconcpp.lib")
#include "CHalconCircleModel.h"
#include "CHalconLineDataModel.h"
#include "CHalconPointModel.h"

using namespace std;
using namespace HalconCpp;



class MY_DLL_EXP CHalconFuncSet
{
public:
	CHalconFuncSet();
	~CHalconFuncSet();
	void SetWindowHandle(HTuple WindowHandle);
	void FindLine(HObject Image, string Para, CHalconLineDataModel* pLine);
	void FindPair(HObject Image, string Para, CHalconLineDataModel* pStartLine, CHalconLineDataModel* pEndLine);
	void FindCircle(HObject Image, string Para, CHalconCircleModel* pCircle);
	enum EnumPicType
	{
		Robot_01,
		Robot_02,
		Robot_03,
		Robot_08,
		Robot_10,
	};
	void ProcessImage(EnumPicType ImageType, HObject Image, HObject* ImageOut);
	void ProcessImage(EnumPicType ImageType, string ImagePath, string ImageOutPath);

private:
	void spoke(HObject ho_Image, HObject *ho_Regions, HTuple hv_Elements, HTuple hv_DetectHeight,
		HTuple hv_DetectWidth, HTuple hv_Sigma, HTuple hv_Threshold, HTuple hv_Transition,
		HTuple hv_Select, HTuple hv_ROIRows, HTuple hv_ROICols, HTuple hv_Direct, HTuple *hv_ResultRow,
		HTuple *hv_ResultColumn, HTuple *hv_ArcType);
	void gen_arrow_contour_xld(HObject *ho_Arrow, HTuple hv_Row1, HTuple hv_Column1,
		HTuple hv_Row2, HTuple hv_Column2, HTuple hv_HeadLength, HTuple hv_HeadWidth);
	void dev_open_window_fit_image(HObject ho_Image, HTuple hv_Row, HTuple hv_Column,
		HTuple hv_WidthLimit, HTuple hv_HeightLimit, HTuple *hv_WindowHandle);
	HTuple WindowHandle;

};

