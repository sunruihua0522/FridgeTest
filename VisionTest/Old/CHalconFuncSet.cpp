#include "pch.h"
#include "CHalconFuncSet.h"

CHalconFuncSet::CHalconFuncSet()
{
	this->WindowHandle =  -1;
}


CHalconFuncSet::~CHalconFuncSet()
{
}
void  CHalconFuncSet::SetWindowHandle(HTuple WindowHandle)
{
	this->WindowHandle = WindowHandle;
}

void CHalconFuncSet::FindLine(HObject Image,string Para, CHalconLineDataModel* pLine)
{
	HObject  ho_Rectangle, ho_Contour;
	HObject  ho_RegionLines;

	// Local control variables
	HTuple  hv_PreFile, hv_Width, hv_Height, hv_WindowHandle;
	HTuple  hv_Row, hv_Column, hv_Phi, hv_Length1, hv_Length2;
	HTuple  hv_RoiTuple, hv_CaliperNum, hv_NewL1, hv_NewL2;
	HTuple  hv_Sin, hv_Cos, hv_BaseRow, hv_BaseCol, hv_newRow;
	HTuple  hv_newCol, hv_RowFirstList, hv_ColFirstList, hv_Index;
	HTuple  hv_MeasureHandle, hv_RowEdge, hv_ColumnEdge, hv_Amplitude;
	HTuple  hv_Distance, hv_RowBegin, hv_ColBegin, hv_RowEnd;
	HTuple  hv_ColEnd, hv_Nr, hv_Nc, hv_Dist;

	MeanImage(Image, &Image, 3, 3);
	GetImageSize(Image, &hv_Width, &hv_Height);
	
	ReadTuple((Para + "Roi.tup").c_str(), &hv_RoiTuple);


	hv_Row = ((const HTuple&)hv_RoiTuple)[0];
	hv_Column = ((const HTuple&)hv_RoiTuple)[1];
	hv_Phi = ((const HTuple&)hv_RoiTuple)[2];
	hv_Length1 = ((const HTuple&)hv_RoiTuple)[3];
	hv_Length2 = ((const HTuple&)hv_RoiTuple)[4];

	GenRectangle2(&ho_Rectangle, hv_Row, hv_Column, hv_Phi, hv_Length1, hv_Length2);

	hv_CaliperNum = 100;

	hv_NewL1 = hv_Length1;
	hv_NewL2 = hv_Length2 / (hv_CaliperNum - 1);

	TupleSin(hv_Phi, &hv_Sin);
	TupleCos(hv_Phi, &hv_Cos);

	hv_BaseRow = hv_Row + (hv_Length2*hv_Cos);
	hv_BaseCol = hv_Column + (hv_Length2*hv_Sin);

	hv_newRow = hv_BaseRow;
	hv_newCol = hv_BaseCol;

	hv_RowFirstList = HTuple();
	hv_ColFirstList = HTuple();


	{
		HTuple end_val45 = hv_CaliperNum;
		HTuple step_val45 = 1;
		for (hv_Index = 1; hv_Index.Continue(end_val45, step_val45); hv_Index += step_val45)
		{
			GenMeasureRectangle2(hv_newRow, hv_newCol, hv_Phi, hv_NewL1, hv_NewL2, hv_Width,
				hv_Height, "nearest_neighbor", &hv_MeasureHandle);
			//positive  黑-》白 negative  白-》黑

			MeasurePos(Image, hv_MeasureHandle, HTuple(hv_RoiTuple[5]), HTuple(hv_RoiTuple[6]),
				HTuple(hv_RoiTuple[7]), HTuple(hv_RoiTuple[8]), &hv_RowEdge, &hv_ColumnEdge,
				&hv_Amplitude, &hv_Distance);

			hv_newRow = hv_BaseRow - (((hv_NewL2*hv_Cos)*hv_Index) * 2);
			hv_newCol = hv_BaseCol - (((hv_NewL2*hv_Sin)*hv_Index) * 2);

			if (0 != ((hv_RowEdge.TupleLength()) > 0))
			{
				hv_RowFirstList = hv_RowFirstList.TupleConcat(hv_RowEdge);
				hv_ColFirstList = hv_ColFirstList.TupleConcat(hv_ColumnEdge);
			}
			CloseMeasure(hv_MeasureHandle);
		}
	}



	if (0 != ((hv_RowFirstList.TupleLength()) > 2))
	{
		GenContourPolygonXld(&ho_Contour, hv_RowFirstList, hv_ColFirstList);
		FitLineContourXld(ho_Contour, "gauss", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin,
			&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);

		pLine->StartPoint.X = hv_ColBegin.D();
		pLine->StartPoint.Y = hv_RowBegin.D();
		pLine->EndPoint.X = hv_ColEnd.D();
		pLine->EndPoint.Y = hv_RowEnd.D();

	}
	
}
void CHalconFuncSet::FindPair(HObject Image, string Para, CHalconLineDataModel* pStartLine, CHalconLineDataModel* pEndLine)
{

	// Local iconic variables
	HObject  ho_Rectangle, ho_Contour;
	HObject  ho_Contour1;

	// Local control variables
	HTuple  hv_Width, hv_Height, hv_WindowHandle;
	HTuple  hv_PreFile, hv_Row, hv_Column, hv_Phi, hv_Length1;
	HTuple  hv_Length2, hv_RoiTuple, hv_CaliperNum, hv_ExpectPairNum;
	HTuple  hv_NewL1, hv_NewL2, hv_Sin, hv_Cos, hv_BaseRow;
	HTuple  hv_BaseCol, hv_newRow, hv_newCol, hv_RowFirstList;
	HTuple  hv_ColFirstList, hv_RowSecondList, hv_ColSecondList;
	HTuple  hv_Index, hv_MeasureHandle, hv_RowEdgeFirst, hv_ColumnEdgeFirst;
	HTuple  hv_AmplitudeFirst, hv_RowEdgeSecond, hv_ColumnEdgeSecond;
	HTuple  hv_AmplitudeSecond, hv_IntraDistance, hv_InterDistance;
	HTuple  hv_Length, hv_Length3, hv_RowBegin, hv_ColBegin;
	HTuple  hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc, hv_Dist, hv_RowBegin1;
	HTuple  hv_ColBegin1, hv_RowEnd1, hv_ColEnd1, hv_Nr1, hv_Nc1;
	HTuple  hv_Dist1;

	//ReadImage(&ho_Image11117455Cam0, "C:/Users/cn11321/source/Code/image/RawImage/Robot_01.png");

	GetImageSize(Image, &hv_Width, &hv_Height);
	

	ReadTuple((Para+"Roi.tup").c_str(), &hv_RoiTuple);
	hv_Row = ((const HTuple&)hv_RoiTuple)[0];
	hv_Column = ((const HTuple&)hv_RoiTuple)[1];
	hv_Phi = ((const HTuple&)hv_RoiTuple)[2];
	hv_Length1 = ((const HTuple&)hv_RoiTuple)[3];
	hv_Length2 = ((const HTuple&)hv_RoiTuple)[4];
	
	GenRectangle2(&ho_Rectangle, hv_Row, hv_Column, hv_Phi, hv_Length1, hv_Length2);

	hv_CaliperNum = 100;
	hv_ExpectPairNum = 1;

	hv_NewL1 = hv_Length1;
	hv_NewL2 = hv_Length2 / (hv_CaliperNum - 1);

	TupleSin(hv_Phi, &hv_Sin);
	TupleCos(hv_Phi, &hv_Cos);

	hv_BaseRow = hv_Row + (hv_Length2*hv_Cos);
	hv_BaseCol = hv_Column + (hv_Length2*hv_Sin);

	hv_newRow = hv_BaseRow;
	hv_newCol = hv_BaseCol;

	hv_RowFirstList = HTuple();
	hv_ColFirstList = HTuple();
	hv_RowSecondList = HTuple();
	hv_ColSecondList = HTuple();

	{
		HTuple end_val41 = hv_CaliperNum;
		HTuple step_val41 = 1;
		for (hv_Index = 1; hv_Index.Continue(end_val41, step_val41); hv_Index += step_val41)
		{
			GenMeasureRectangle2(hv_newRow, hv_newCol, hv_Phi, hv_NewL1, hv_NewL2, hv_Width,
				hv_Height, "nearest_neighbor", &hv_MeasureHandle);
			//positive找白边，negative找黑边
			MeasurePairs(Image, hv_MeasureHandle, 1.5, 30, "negative_strongest",
				"first", &hv_RowEdgeFirst, &hv_ColumnEdgeFirst, &hv_AmplitudeFirst, &hv_RowEdgeSecond,
				&hv_ColumnEdgeSecond, &hv_AmplitudeSecond, &hv_IntraDistance, &hv_InterDistance);
			hv_newRow = hv_BaseRow - (((hv_NewL2*hv_Cos)*hv_Index) * 2);
			hv_newCol = hv_BaseCol - (((hv_NewL2*hv_Sin)*hv_Index) * 2);

			TupleLength(hv_RowEdgeFirst, &hv_Length);
			TupleLength(hv_RowEdgeSecond, &hv_Length3);

			if (0 != (HTuple(HTuple((hv_RowEdgeFirst.TupleLength()) > 0).TupleAnd((hv_RowEdgeFirst.TupleLength()) == (hv_RowEdgeSecond.TupleLength()))).TupleAnd(hv_ExpectPairNum == (hv_RowEdgeFirst.TupleLength()))))
			{
				hv_RowFirstList = hv_RowFirstList.TupleConcat(hv_RowEdgeFirst);
				hv_ColFirstList = hv_ColFirstList.TupleConcat(hv_ColumnEdgeFirst);
				hv_RowSecondList = hv_RowSecondList.TupleConcat(hv_RowEdgeSecond);
				hv_ColSecondList = hv_ColSecondList.TupleConcat(hv_ColumnEdgeSecond);
			}
			CloseMeasure(hv_MeasureHandle);
		}
	}
	if (0 != ((hv_RowFirstList.TupleLength()) > 2))
	{
		GenContourPolygonXld(&ho_Contour, hv_RowFirstList, hv_ColFirstList);
		FitLineContourXld(ho_Contour, "gauss", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin,
			&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);

		pStartLine->StartPoint.X = hv_ColBegin.D();
		pStartLine->StartPoint.Y = hv_RowBegin.D();
		pStartLine->EndPoint.X = hv_ColEnd.D();
		pStartLine->EndPoint.Y = hv_RowEnd.D();

		GenContourPolygonXld(&ho_Contour1, hv_RowSecondList, hv_ColSecondList);
		FitLineContourXld(ho_Contour1, "gauss", -1, 0, 5, 2, &hv_RowBegin1, &hv_ColBegin1,
			&hv_RowEnd1, &hv_ColEnd1, &hv_Nr1, &hv_Nc1, &hv_Dist1);

		pEndLine->StartPoint.X = hv_ColBegin1.D();
		pEndLine->StartPoint.Y = hv_RowBegin1.D();
		pEndLine->EndPoint.X = hv_ColEnd1.D();
		pEndLine->EndPoint.Y = hv_RowEnd1.D();


	}

}
void CHalconFuncSet::FindCircle(HObject Image,string Para, CHalconCircleModel* pCircle)
{
	HObject  ho_Regions1;
	HObject  ho_Contour, ho_Circle;

	// Local control variables
	HTuple  hv_WindowHandle, hv_PreFile, hv_spoke_paras;
	HTuple  hv_ROIRows, hv_ROICols, hv_Direct, hv_ResultRow;
	HTuple  hv_ResultColumn, hv_ArcType, hv_Index, hv_Row, hv_Column;
	HTuple  hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;

	//ReadImage(&ho_Image, "C:/Users/cn11321/source/Code/image/RawImage/Robot_03.png");
	//MeanImage(Image, &Image, 3, 3);
	
	//hv_PreFile = "./Para/Spoke2/";
	
	ReadTuple((Para + "spokeParas.tup").c_str(), &hv_spoke_paras);
	ReadTuple((Para + "sopkeParaRow.tup").c_str(), &hv_ROIRows);
	ReadTuple((Para + "sopkeParaCol.tup").c_str(), &hv_ROICols);
	ReadTuple((Para + "sopkeParaDir.tup").c_str(), &hv_Direct);


	spoke(Image, &ho_Regions1, HTuple(hv_spoke_paras[0]), HTuple(hv_spoke_paras[1]),
		HTuple(hv_spoke_paras[2]), HTuple(hv_spoke_paras[3]), HTuple(hv_spoke_paras[4]),
		HTuple(hv_spoke_paras[5]), HTuple(hv_spoke_paras[6]), hv_ROIRows, hv_ROICols,
		hv_Direct, &hv_ResultRow, &hv_ResultColumn, &hv_ArcType);

	
	HTuple end_val27 = (hv_ResultRow.TupleLength()) - 1;
	HTuple step_val27 = 1;

	

	GenContourPolygonXld(&ho_Contour, hv_ResultRow, hv_ResultColumn);

	FitCircleContourXld(ho_Contour, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Column,
		&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	
	GenCircle(&ho_Circle, hv_Row, hv_Column, hv_Radius);
	pCircle->CenterPoint.X = hv_Column.D();
	pCircle->CenterPoint.Y = hv_Row.D();
	pCircle->Radius = hv_Radius.D();
}


void CHalconFuncSet::spoke(HObject ho_Image, HObject *ho_Regions, HTuple hv_Elements, HTuple hv_DetectHeight,
	HTuple hv_DetectWidth, HTuple hv_Sigma, HTuple hv_Threshold, HTuple hv_Transition,
	HTuple hv_Select, HTuple hv_ROIRows, HTuple hv_ROICols, HTuple hv_Direct, HTuple *hv_ResultRow,
	HTuple *hv_ResultColumn, HTuple *hv_ArcType)
{

	// Local iconic variables
	HObject  ho_Contour, ho_ContCircle, ho_Rectangle1;
	HObject  ho_Arrow1;

	// Local control variables
	HTuple  hv_Width, hv_Height, hv_RowC, hv_ColumnC;
	HTuple  hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;
	HTuple  hv_RowXLD, hv_ColXLD, hv_Length2, hv_i, hv_j, hv_RowE;
	HTuple  hv_ColE, hv_ATan, hv_RowL2, hv_RowL1, hv_ColL2;
	HTuple  hv_ColL1, hv_MsrHandle_Measure, hv_RowEdge, hv_ColEdge;
	HTuple  hv_Amplitude, hv_Distance, hv_tRow, hv_tCol, hv_t;
	HTuple  hv_Number, hv_k;

	//获取图像尺寸
	GetImageSize(ho_Image, &hv_Width, &hv_Height);
	//产生一个空显示对象，用于显示
	GenEmptyObj(&(*ho_Regions));
	//初始化边缘坐标数组
	(*hv_ResultRow) = HTuple();
	(*hv_ResultColumn) = HTuple();

	//产生xld
	GenContourPolygonXld(&ho_Contour, hv_ROIRows, hv_ROICols);
	//用回归线法（不抛出异常点，所有点权重一样）拟合圆
	FitCircleContourXld(ho_Contour, "algebraic", -1, 0, 0, 1, 2, &hv_RowC, &hv_ColumnC,
		&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	//根据拟合结果产生xld，并保持到显示对象
	GenCircleContourXld(&ho_ContCircle, hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi,
		hv_EndPhi, hv_PointOrder, 3);
	ConcatObj((*ho_Regions), ho_ContCircle, &(*ho_Regions));

	//获取圆或圆弧xld上的点坐标
	GetContourXld(ho_ContCircle, &hv_RowXLD, &hv_ColXLD);

	//求圆或圆弧xld上的点的数量
	TupleLength(hv_ColXLD, &hv_Length2);
	if (0 != (hv_Elements < 3))
	{
		//    disp_message (WindowHandle, '检测的边缘数量太少，请重新设置!', 'window', 52, 12, 'red', 'false')
		return;
	}
	//如果xld是圆弧，有Length2个点，从起点开始，等间距（间距为Length2/(Elements-1)）取Elements个点，作为卡尺工具的中点
	//如果xld是圆，有Length2个点，以0°为起点，从起点开始，等间距（间距为Length2/(Elements)）取Elements个点，作为卡尺工具的中点
	{
		HTuple end_val27 = hv_Elements - 1;
		HTuple step_val27 = 1;
		for (hv_i = 0; hv_i.Continue(end_val27, step_val27); hv_i += step_val27)
		{

			if (0 != (HTuple(hv_RowXLD[0]) == HTuple(hv_RowXLD[hv_Length2 - 1])))
			{
				//xld的起点和终点坐标相对，为圆
				TupleInt(((1.0*hv_Length2) / hv_Elements)*hv_i, &hv_j);
				(*hv_ArcType) = "circle";
			}
			else
			{
				//否则为圆弧
				TupleInt(((1.0*hv_Length2) / (hv_Elements - 1))*hv_i, &hv_j);
				(*hv_ArcType) = "arc";
			}
			//索引越界，强制赋值为最后一个索引
			if (0 != (hv_j >= hv_Length2))
			{
				hv_j = hv_Length2 - 1;
				//continue
			}
			//获取卡尺工具中心
			hv_RowE = HTuple(hv_RowXLD[hv_j]);
			hv_ColE = HTuple(hv_ColXLD[hv_j]);

			//超出图像区域，不检测，否则容易报异常
			if (0 != (HTuple(HTuple(HTuple(hv_RowE > (hv_Height - 1)).TupleOr(hv_RowE < 0)).TupleOr(hv_ColE > (hv_Width - 1))).TupleOr(hv_ColE < 0)))
			{
				continue;
			}
			//边缘搜索方向类型：'inner'搜索方向由圆外指向圆心；'outer'搜索方向由圆心指向圆外
			if (0 != (hv_Direct == HTuple("inner")))
			{
				//求卡尺工具的边缘搜索方向
				//求圆心指向边缘的矢量的角度
				TupleAtan2((-hv_RowE) + hv_RowC, hv_ColE - hv_ColumnC, &hv_ATan);
				//角度反向
				hv_ATan = (HTuple(180).TupleRad()) + hv_ATan;
			}
			else
			{
				//求卡尺工具的边缘搜索方向
				//求圆心指向边缘的矢量的角度
				TupleAtan2((-hv_RowE) + hv_RowC, hv_ColE - hv_ColumnC, &hv_ATan);
			}


			//产生卡尺xld，并保持到显示对象
			GenRectangle2ContourXld(&ho_Rectangle1, hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2,
				hv_DetectWidth / 2);
			ConcatObj((*ho_Regions), ho_Rectangle1, &(*ho_Regions));
			//用箭头xld指示边缘搜索方向，并保持到显示对象
			if (0 != (hv_i == 0))
			{
				hv_RowL2 = hv_RowE + ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_RowL1 = hv_RowE - ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_ColL2 = hv_ColE + ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				hv_ColL1 = hv_ColE - ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				gen_arrow_contour_xld(&ho_Arrow1, hv_RowL1, hv_ColL1, hv_RowL2, hv_ColL2, 25,
					25);
				ConcatObj((*ho_Regions), ho_Arrow1, &(*ho_Regions));
			}


			//产生测量对象句柄
			GenMeasureRectangle2(hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2, hv_DetectWidth / 2,
				hv_Width, hv_Height, "nearest_neighbor", &hv_MsrHandle_Measure);

			//设置极性
			if (0 != (hv_Transition == HTuple("negative")))
			{
				hv_Transition = "negative";
			}
			else
			{
				if (0 != (hv_Transition == HTuple("positive")))
				{

					hv_Transition = "positive";
				}
				else
				{
					hv_Transition = "all";
				}
			}
			//设置边缘位置。最强点是从所有边缘中选择幅度绝对值最大点，需要设置为'all'
			if (0 != (hv_Select == HTuple("first")))
			{
				hv_Select = "first";
			}
			else
			{
				if (0 != (hv_Select == HTuple("last")))
				{

					hv_Select = "last";
				}
				else
				{
					hv_Select = "all";
				}
			}
			//检测边缘
			MeasurePos(ho_Image, hv_MsrHandle_Measure, hv_Sigma, hv_Threshold, hv_Transition,
				hv_Select, &hv_RowEdge, &hv_ColEdge, &hv_Amplitude, &hv_Distance);
			//清除测量对象句柄
			CloseMeasure(hv_MsrHandle_Measure);
			//临时变量初始化
			//tRow，tCol保存找到指定边缘的坐标
			hv_tRow = 0;
			hv_tCol = 0;
			//t保存边缘的幅度绝对值
			hv_t = 0;
			TupleLength(hv_RowEdge, &hv_Number);
			//找到的边缘必须至少为1个
			if (0 != (hv_Number < 1))
			{
				continue;
			}
			//有多个边缘时，选择幅度绝对值最大的边缘
			{
				HTuple end_val120 = hv_Number - 1;
				HTuple step_val120 = 1;
				for (hv_k = 0; hv_k.Continue(end_val120, step_val120); hv_k += step_val120)
				{
					if (0 != ((HTuple(hv_Amplitude[hv_k]).TupleAbs()) > hv_t))
					{

						hv_tRow = HTuple(hv_RowEdge[hv_k]);
						hv_tCol = HTuple(hv_ColEdge[hv_k]);
						hv_t = HTuple(hv_Amplitude[hv_k]).TupleAbs();
					}
				}
			}
			//把找到的边缘保存在输出数组
			if (0 != (hv_t > 0))
			{

				(*hv_ResultRow) = (*hv_ResultRow).TupleConcat(hv_tRow);
				(*hv_ResultColumn) = (*hv_ResultColumn).TupleConcat(hv_tCol);
			}
		}
	}


	return;
}

void CHalconFuncSet::gen_arrow_contour_xld(HObject *ho_Arrow, HTuple hv_Row1, HTuple hv_Column1,
	HTuple hv_Row2, HTuple hv_Column2, HTuple hv_HeadLength, HTuple hv_HeadWidth)
{

	// Local iconic variables
	HObject  ho_TempArrow;

	// Local control variables
	HTuple  hv_Length, hv_ZeroLengthIndices, hv_DR;
	HTuple  hv_DC, hv_HalfHeadWidth, hv_RowP1, hv_ColP1, hv_RowP2;
	HTuple  hv_ColP2, hv_Index;

	//This procedure generates arrow shaped XLD contours,
	//pointing from (Row1, Column1) to (Row2, Column2).
	//If starting and end point are identical, a contour consisting
	//of a single point is returned.
	//
	//input parameteres:
	//Row1, Column1: Coordinates of the arrows' starting points
	//Row2, Column2: Coordinates of the arrows' end points
	//HeadLength, HeadWidth: Size of the arrow heads in pixels
	//
	//output parameter:
	//Arrow: The resulting XLD contour
	//
	//The input tuples Row1, Column1, Row2, and Column2 have to be of
	//the same length.
	//HeadLength and HeadWidth either have to be of the same length as
	//Row1, Column1, Row2, and Column2 or have to be a single element.
	//If one of the above restrictions is violated, an error will occur.
	//
	//
	//Init
	GenEmptyObj(&(*ho_Arrow));
	//
	//Calculate the arrow length
	DistancePp(hv_Row1, hv_Column1, hv_Row2, hv_Column2, &hv_Length);
	//
	//Mark arrows with identical start and end point
	//(set Length to -1 to avoid division-by-zero exception)
	hv_ZeroLengthIndices = hv_Length.TupleFind(0);
	if (0 != (hv_ZeroLengthIndices != -1))
	{
		hv_Length[hv_ZeroLengthIndices] = -1;
	}
	//
	//Calculate auxiliary variables.
	hv_DR = (1.0*(hv_Row2 - hv_Row1)) / hv_Length;
	hv_DC = (1.0*(hv_Column2 - hv_Column1)) / hv_Length;
	hv_HalfHeadWidth = hv_HeadWidth / 2.0;
	//
	//Calculate end points of the arrow head.
	hv_RowP1 = (hv_Row1 + ((hv_Length - hv_HeadLength)*hv_DR)) + (hv_HalfHeadWidth*hv_DC);
	hv_ColP1 = (hv_Column1 + ((hv_Length - hv_HeadLength)*hv_DC)) - (hv_HalfHeadWidth*hv_DR);
	hv_RowP2 = (hv_Row1 + ((hv_Length - hv_HeadLength)*hv_DR)) - (hv_HalfHeadWidth*hv_DC);
	hv_ColP2 = (hv_Column1 + ((hv_Length - hv_HeadLength)*hv_DC)) + (hv_HalfHeadWidth*hv_DR);
	//
	//Finally create output XLD contour for each input point pair
	{
		HTuple end_val45 = (hv_Length.TupleLength()) - 1;
		HTuple step_val45 = 1;
		for (hv_Index = 0; hv_Index.Continue(end_val45, step_val45); hv_Index += step_val45)
		{
			if (0 != (HTuple(hv_Length[hv_Index]) == -1))
			{
				//Create_ single points for arrows with identical start and end point
				GenContourPolygonXld(&ho_TempArrow, HTuple(hv_Row1[hv_Index]), HTuple(hv_Column1[hv_Index]));
			}
			else
			{
				//Create arrow contour
				GenContourPolygonXld(&ho_TempArrow, ((((HTuple(hv_Row1[hv_Index]).TupleConcat(HTuple(hv_Row2[hv_Index]))).TupleConcat(HTuple(hv_RowP1[hv_Index]))).TupleConcat(HTuple(hv_Row2[hv_Index]))).TupleConcat(HTuple(hv_RowP2[hv_Index]))).TupleConcat(HTuple(hv_Row2[hv_Index])),
					((((HTuple(hv_Column1[hv_Index]).TupleConcat(HTuple(hv_Column2[hv_Index]))).TupleConcat(HTuple(hv_ColP1[hv_Index]))).TupleConcat(HTuple(hv_Column2[hv_Index]))).TupleConcat(HTuple(hv_ColP2[hv_Index]))).TupleConcat(HTuple(hv_Column2[hv_Index])));
			}
			ConcatObj((*ho_Arrow), ho_TempArrow, &(*ho_Arrow));
		}
	}
	return;
}


void CHalconFuncSet::dev_open_window_fit_image(HObject ho_Image, HTuple hv_Row, HTuple hv_Column,
	HTuple hv_WidthLimit, HTuple hv_HeightLimit, HTuple *hv_WindowHandle)
{

	// Local iconic variables
	// Local control variables
	HTuple  hv_MinWidth, hv_MaxWidth, hv_MinHeight;
	HTuple  hv_MaxHeight, hv_ResizeFactor, hv_ImageWidth, hv_ImageHeight;
	HTuple  hv_TempWidth, hv_TempHeight, hv_WindowWidth, hv_WindowHeight;

	//This procedure opens a new graphics window and adjusts the size
	//such that it fits into the limits specified by WidthLimit
	//and HeightLimit, but also maintains the correct image aspect ratio.
	//
	//If it is impossible to match the minimum and maximum extent requirements
	//at the same time (f.e. if the image is very long but narrow),
	//the maximum value gets a higher priority,
	//
	//Parse input tuple WidthLimit
	if (0 != (HTuple((hv_WidthLimit.TupleLength()) == 0).TupleOr(hv_WidthLimit < 0)))
	{
		hv_MinWidth = 500;
		hv_MaxWidth = 800;
	}
	else if (0 != ((hv_WidthLimit.TupleLength()) == 1))
	{
		hv_MinWidth = 0;
		hv_MaxWidth = hv_WidthLimit;
	}
	else
	{
		hv_MinWidth = ((const HTuple&)hv_WidthLimit)[0];
		hv_MaxWidth = ((const HTuple&)hv_WidthLimit)[1];
	}
	//Parse input tuple HeightLimit
	if (0 != (HTuple((hv_HeightLimit.TupleLength()) == 0).TupleOr(hv_HeightLimit < 0)))
	{
		hv_MinHeight = 400;
		hv_MaxHeight = 600;
	}
	else if (0 != ((hv_HeightLimit.TupleLength()) == 1))
	{
		hv_MinHeight = 0;
		hv_MaxHeight = hv_HeightLimit;
	}
	else
	{
		hv_MinHeight = ((const HTuple&)hv_HeightLimit)[0];
		hv_MaxHeight = ((const HTuple&)hv_HeightLimit)[1];
	}
	//
	//Test, if window size has to be changed.
	hv_ResizeFactor = 1;
	GetImageSize(ho_Image, &hv_ImageWidth, &hv_ImageHeight);
	//First, expand window to the minimum extents (if necessary).
	if (0 != (HTuple(hv_MinWidth > hv_ImageWidth).TupleOr(hv_MinHeight > hv_ImageHeight)))
	{
		hv_ResizeFactor = (((hv_MinWidth.TupleReal()) / hv_ImageWidth).TupleConcat((hv_MinHeight.TupleReal()) / hv_ImageHeight)).TupleMax();
	}
	hv_TempWidth = hv_ImageWidth * hv_ResizeFactor;
	hv_TempHeight = hv_ImageHeight * hv_ResizeFactor;
	//Then, shrink window to maximum extents (if necessary).
	if (0 != (HTuple(hv_MaxWidth < hv_TempWidth).TupleOr(hv_MaxHeight < hv_TempHeight)))
	{
		hv_ResizeFactor = hv_ResizeFactor * ((((hv_MaxWidth.TupleReal()) / hv_TempWidth).TupleConcat((hv_MaxHeight.TupleReal()) / hv_TempHeight)).TupleMin());
	}
	hv_WindowWidth = hv_ImageWidth * hv_ResizeFactor;
	hv_WindowHeight = hv_ImageHeight * hv_ResizeFactor;
	//Resize window
	SetWindowAttr("background_color", "black");
	OpenWindow(hv_Row, hv_Column, hv_WindowWidth, hv_WindowHeight, 0, "visible", "", &(*hv_WindowHandle));
	HDevWindowStack::Push((*hv_WindowHandle));
	if (HDevWindowStack::IsOpen())
		SetPart(HDevWindowStack::GetActive(), 0, 0, hv_ImageHeight - 1, hv_ImageWidth - 1);
	return;
}
void CHalconFuncSet::ProcessImage(EnumPicType ImageType, string ImagePath, string ImageOutPath)
{
	HObject Image,ImageOut;
	ReadImage(&Image, ImagePath.c_str());
	ProcessImage(ImageType, Image, &ImageOut);
	WriteImage(ImageOut, "bmp", 0, ImageOutPath.c_str());
}
void CHalconFuncSet::ProcessImage(EnumPicType ImageType, HObject Image, HObject* ImageOut)
{
	switch (ImageType)
	{
	case CHalconFuncSet::Robot_01:	//找间隙
	{
		string ParaPath = "./Para/P1/";
		CHalconLineDataModel Line1;
		CHalconLineDataModel Line2;
		HObject LineRegion1;
		HObject LineRegion2;
		
		FindPair(Image, ParaPath, &Line1, &Line2);

		dev_open_window_fit_image(Image, 0, 0, -1, -1, &WindowHandle);
		DispObj(Image, WindowHandle);
		SetColor(WindowHandle, "red");
		SetDraw(WindowHandle, "margin");
		DispLine(WindowHandle, Line1.StartPoint.Y, Line1.StartPoint.X, Line1.EndPoint.Y, Line1.EndPoint.X);
		DispLine(WindowHandle, Line2.StartPoint.Y, Line2.StartPoint.X, Line2.EndPoint.Y, Line2.EndPoint.X);
		DumpWindowImage(ImageOut,WindowHandle);
		

	}
	break;
	
	case CHalconFuncSet::Robot_02:
	{
		string ParaPath = "./Para/P2/";
		CHalconLineDataModel Line1;
		CHalconLineDataModel Line2;
		FindPair(Image, ParaPath, &Line1, &Line2);

		
		dev_open_window_fit_image(Image, 0, 0, -1, -1, &WindowHandle);
		DispObj(Image, WindowHandle);
		SetColor(WindowHandle, "red");
		SetDraw(WindowHandle, "margin");
		DispLine(WindowHandle, Line1.StartPoint.Y, Line1.StartPoint.X, Line1.EndPoint.Y, Line1.EndPoint.X);
		DispLine(WindowHandle, Line2.StartPoint.Y, Line2.StartPoint.X, Line2.EndPoint.Y, Line2.EndPoint.X);
		DumpWindowImage(ImageOut, WindowHandle);
		
	}
	break;
	case CHalconFuncSet::Robot_03:
	{
		string ParaL1 = "./Para/L1/";
		string ParaL2 = "./Para/L2/";
		string ParaL3 = "./Para/L3/";
		string ParaSpoke1 = "./Para/Spoke1/";
		string ParaSpoke2 = "./Para/Spoke2/";

		CHalconLineDataModel Line1;
		CHalconLineDataModel Line2;
		CHalconLineDataModel Line3;
		CHalconCircleModel Circle1;
		CHalconCircleModel Circle2;

		FindLine(Image, ParaL1, &Line1);
		FindLine(Image, ParaL2, &Line2);
		FindLine(Image, ParaL3, &Line3);
		FindCircle(Image, ParaSpoke1,&Circle1);
		FindCircle(Image, ParaSpoke2, &Circle2);
	
		dev_open_window_fit_image(Image, 0, 0, -1, -1, &WindowHandle);
		DispObj(Image, WindowHandle);
		SetColor(WindowHandle, "red");
		SetDraw(WindowHandle, "margin");
		DispLine(WindowHandle, Line1.StartPoint.Y, Line1.StartPoint.X, Line1.EndPoint.Y, Line1.EndPoint.X);
		DispLine(WindowHandle, Line2.StartPoint.Y, Line2.StartPoint.X, Line2.EndPoint.Y, Line2.EndPoint.X);
		DispLine(WindowHandle, Line3.StartPoint.Y, Line3.StartPoint.X, Line3.EndPoint.Y, Line3.EndPoint.X);
		HObject RegionPoint1, RegionPoint2;

		GenRegionPoints(&RegionPoint1, Circle1.CenterPoint.Y, Circle1.CenterPoint.X);
		GenRegionPoints(&RegionPoint2, Circle2.CenterPoint.Y, Circle2.CenterPoint.X);

		DispObj(RegionPoint1, WindowHandle);
		DispObj(RegionPoint2, WindowHandle);
		DispCircle(WindowHandle, Circle1.CenterPoint.Y, Circle1.CenterPoint.X, Circle1.Radius);
		DispCircle(WindowHandle, Circle2.CenterPoint.Y, Circle2.CenterPoint.X, Circle2.Radius);

		DumpWindowImage(ImageOut, WindowHandle);
		
	}
	break;
	case CHalconFuncSet::Robot_08:
	{
		string ParaL1 = "./Para/L4/";
		string ParaL2 = "./Para/L5/";
		string ParaL3 = "./Para/L6/";
		string ParaSpoke1 = "./Para/Spoke3/";
		string ParaSpoke2 = "./Para/Spoke4/";

		CHalconLineDataModel Line1;
		CHalconLineDataModel Line2;
		CHalconLineDataModel Line3;
		CHalconCircleModel Circle1;
		CHalconCircleModel Circle2;

		FindLine(Image, ParaL1, &Line1);
		FindLine(Image, ParaL2, &Line2);
		FindLine(Image, ParaL3, &Line3);
		FindCircle(Image, ParaSpoke1, &Circle1);
		FindCircle(Image, ParaSpoke2, &Circle2);

		dev_open_window_fit_image(Image, 0, 0, -1, -1, &WindowHandle);
		DispObj(Image, WindowHandle);
		SetColor(WindowHandle, "red");
		SetDraw(WindowHandle, "margin");
		DispLine(WindowHandle, Line1.StartPoint.Y, Line1.StartPoint.X, Line1.EndPoint.Y, Line1.EndPoint.X);
		DispLine(WindowHandle, Line2.StartPoint.Y, Line2.StartPoint.X, Line2.EndPoint.Y, Line2.EndPoint.X);
		DispLine(WindowHandle, Line3.StartPoint.Y, Line3.StartPoint.X, Line3.EndPoint.Y, Line3.EndPoint.X);
		HObject RegionPoint1, RegionPoint2;

		GenRegionPoints(&RegionPoint1, Circle1.CenterPoint.Y, Circle1.CenterPoint.X);
		GenRegionPoints(&RegionPoint2, Circle2.CenterPoint.Y, Circle2.CenterPoint.X);
		DispObj(RegionPoint1, WindowHandle);
		DispObj(RegionPoint2, WindowHandle);
		DispCircle(WindowHandle, Circle1.CenterPoint.Y, Circle1.CenterPoint.X, Circle1.Radius);
		DispCircle(WindowHandle, Circle2.CenterPoint.Y, Circle2.CenterPoint.X, Circle2.Radius);

		DumpWindowImage(ImageOut, WindowHandle);
	}
	break;
	case CHalconFuncSet::Robot_10:
	{
		string ParaPath = "./Para/P3/";
		CHalconLineDataModel Line1;
		CHalconLineDataModel Line2;
		FindPair(Image, ParaPath, &Line1, &Line2);
		dev_open_window_fit_image(Image, 0, 0, -1, -1, &WindowHandle);
		DispObj(Image, WindowHandle);
		SetColor(WindowHandle, "red");
		SetDraw(WindowHandle, "margin");
		DispLine(WindowHandle, Line1.StartPoint.Y, Line1.StartPoint.X, Line1.EndPoint.Y, Line1.EndPoint.X);
		DispLine(WindowHandle, Line2.StartPoint.Y, Line2.StartPoint.X, Line2.EndPoint.Y, Line2.EndPoint.X);
		DumpWindowImage(ImageOut, WindowHandle);
	}
	break;
	default:
	break;
	}
}
