
#include "CHalconFuncSet.h"
#include<io.h>
//#define LOG
CHalconFuncSet::CHalconFuncSet()
{

}

CHalconFuncSet::~CHalconFuncSet()
{

}
void  CHalconFuncSet::SetWindowHandle(HTuple WindowHandle)
{
	//this->WindowHandle = WindowHandle;
}

int CHalconFuncSet::AdjustImage(HObject ImageIn, HObject* ImageOut, string Para, HTuple& Hom2D)
{
	HTuple hv_ModelID, hv_RowModel, hv_ColumnModel;
	HTuple  hv_AngleModel, hv_Score, hv_RowOrigin, hv_ColumnOrigin;
	if (_access((Para + "Model/Model.shm").c_str(), 0) != 0)
		return FILE_IS_NOT_FOUND;
	ReadShapeModel((Para + "Model/Model.shm").c_str(), &hv_ModelID);

	//做变换
	FindShapeModel(ImageIn, hv_ModelID, -0.39, 0.79, 0.2, 1, 0.5, "least_squares",
		0, 0.9, &hv_RowModel, &hv_ColumnModel, &hv_AngleModel, &hv_Score);
	if (hv_Score.TupleLength() == 0)
		return MODEL_NOT_FOUND;
	GetShapeModelOrigin(hv_ModelID, &hv_RowOrigin, &hv_ColumnOrigin);
	VectorAngleToRigid(hv_RowModel - hv_RowOrigin, hv_ColumnModel - hv_ColumnOrigin,
		hv_AngleModel, hv_RowOrigin, hv_ColumnOrigin, 0, &Hom2D);
	AffineTransImage(ImageIn, ImageOut, Hom2D, "constant", "false");
	ClearShapeModel(hv_ModelID);
	//WriteImage(*ImageOut, "bmp", 0, "ImageOut.bmp");
	return SUCCESS;
}

int CHalconFuncSet::FindLine(HObject Image, string Para, CLineModel* pLine)
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
	HObject ImageMean;
	if (_access((Para + "Roi.tup").c_str(), 0) != 0)
		return FILE_IS_NOT_FOUND;


	MeanImage(Image, &ImageMean, 3, 3);
	GetImageSize(ImageMean, &hv_Width, &hv_Height);

	ReadTuple((Para + "Roi.tup").c_str(), &hv_RoiTuple);


	//HTuple hv_ModelID, hv_RowModel, hv_ColumnModel, hv_HomMat2D;
	//HTuple  hv_AngleModel, hv_Score, hv_RowOrigin, hv_ColumnOrigin;
	//ReadShapeModel((Para + "../Model/Model.shm").c_str(), &hv_ModelID);

	////做变换
	//FindShapeModel(Image, hv_ModelID, -0.39, 0.79, 0.2, 1, 0.5, "least_squares",
	//	0, 0.9, &hv_RowModel, &hv_ColumnModel, &hv_AngleModel, &hv_Score);
	//if (hv_Score.TupleLength() == 0)
	//	return MODEL_NOT_FOUND;
	//GetShapeModelOrigin(hv_ModelID, &hv_RowOrigin, &hv_ColumnOrigin);
	//VectorAngleToRigid(hv_RowModel - hv_RowOrigin, hv_ColumnModel - hv_ColumnOrigin,
	//	hv_AngleModel, hv_RowOrigin, hv_ColumnOrigin, 0, &hv_HomMat2D);
	//AffineTransImage(Image, &Image, hv_HomMat2D, "constant", "false");


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

			MeasurePos(ImageMean, hv_MeasureHandle, HTuple(hv_RoiTuple[5]), HTuple(hv_RoiTuple[6]),
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
		return SUCCESS;
	}
	return NOT_ENOUGH_TUPLE;
}
int CHalconFuncSet::FindPair(HObject Image, string Para, CPairModel* Pair)
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

	if (_access((Para + "Roi.tup").c_str(), 0) != 0)
		return FILE_IS_NOT_FOUND;

	GetImageSize(Image, &hv_Width, &hv_Height);
	ReadTuple((Para + "Roi.tup").c_str(), &hv_RoiTuple);

	//HTuple hv_ModelID, hv_RowModel, hv_ColumnModel, hv_HomMat2D;
	//HTuple  hv_AngleModel, hv_Score, hv_RowOrigin, hv_ColumnOrigin;
	//ReadShapeModel((Para + "../Model/Model.shm").c_str(), &hv_ModelID);

	////做变换
	//FindShapeModel(Image, hv_ModelID, -0.39, 0.79, 0.2, 1, 0.5, "least_squares",
	//	0, 0.9, &hv_RowModel, &hv_ColumnModel, &hv_AngleModel, &hv_Score);
	//if (hv_Score.TupleLength() == 0)
	//	return MODEL_NOT_FOUND;

	//GetShapeModelOrigin(hv_ModelID, &hv_RowOrigin, &hv_ColumnOrigin);
	//VectorAngleToRigid(hv_RowModel - hv_RowOrigin, hv_ColumnModel - hv_ColumnOrigin,
	//	hv_AngleModel, hv_RowOrigin, hv_ColumnOrigin, 0, &hv_HomMat2D);
	//AffineTransImage(Image, &Image, hv_HomMat2D, "constant", "false");

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


	HTuple end_val41 = hv_CaliperNum;
	HTuple step_val41 = 1;
	for (hv_Index = 1; hv_Index.Continue(end_val41, step_val41); hv_Index += step_val41)
	{
		GenMeasureRectangle2(hv_newRow, hv_newCol, hv_Phi, hv_NewL1, hv_NewL2, hv_Width,
			hv_Height, "nearest_neighbor", &hv_MeasureHandle);

		//positive找白边，negative找黑边
		MeasurePairs(Image, hv_MeasureHandle, hv_RoiTuple[5], hv_RoiTuple[6], hv_RoiTuple[7],
			hv_RoiTuple[8], &hv_RowEdgeFirst, &hv_ColumnEdgeFirst, &hv_AmplitudeFirst, &hv_RowEdgeSecond,
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


	if (0 != ((hv_RowFirstList.TupleLength()) > 2))
	{

		GenContourPolygonXld(&ho_Contour, hv_RowFirstList, hv_ColFirstList);
		FitLineContourXld(ho_Contour, "gauss", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin,
			&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);
		Pair->Line1.StartPoint.X = hv_ColBegin.D();
		Pair->Line1.StartPoint.Y = hv_RowBegin.D();
		Pair->Line1.EndPoint.X = hv_ColEnd.D();
		Pair->Line1.EndPoint.Y = hv_RowEnd.D();

		GenContourPolygonXld(&ho_Contour1, hv_RowSecondList, hv_ColSecondList);

		FitLineContourXld(ho_Contour1, "gauss", -1, 0, 5, 2, &hv_RowBegin1, &hv_ColBegin1,
			&hv_RowEnd1, &hv_ColEnd1, &hv_Nr1, &hv_Nc1, &hv_Dist1);

		Pair->Line2.StartPoint.X = hv_ColBegin1.D();
		Pair->Line2.StartPoint.Y = hv_RowBegin1.D();
		Pair->Line2.EndPoint.X = hv_ColEnd1.D();
		Pair->Line2.EndPoint.Y = hv_RowEnd1.D();
		return SUCCESS;
	}
	return NOT_ENOUGH_TUPLE;

}
int CHalconFuncSet::FindCircle(HObject Image, string Para, CCircleModel* pCircle)
{
	// Local iconic variables
	HObject ho_ImageReduced, ho_Rectangle, ho_Regions, ho_ConnectedRegions;
	HObject  ho_SelectedRegions, ho_Regions1, ho_Contour, ho_Circle;

	// Local control variables
	HTuple  hv_Index1, hv_Group, hv_PoseIndex, hv_CircleIndex;
	HTuple  hv_PreFile, hv_ModelID, hv_RowModel, hv_ColumnModel;
	HTuple  hv_AngleModel, hv_Score, hv_RowOrigin, hv_ColumnOrigin;
	HTuple  hv_HomMat2D, hv_WindowHandle, hv_spoke_paras, hv_Row1;
	HTuple  hv_Column1, hv_RectRoiSearch, hv_AbsoluteHisto;
	HTuple  hv_RelativeHisto, hv_Indices, hv_IndexSelect, hv_SelectIndex;
	HTuple  hv_Area, hv_Row2, hv_Column2, hv_Row3, hv_Column3;
	HTuple  hv_ROIRows, hv_ROICols, hv_Direct, hv_ResultRow;
	HTuple  hv_ResultColumn, hv_ArcType, hv_Index, hv_Row, hv_Column;
	HTuple  hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;


	if (_access((Para + "RectRoiSearch.tup").c_str(), 0) != 0 || _access((Para + "spokeParas.tup").c_str(), 0) != 0)
		return FILE_IS_NOT_FOUND;


	//读取参数
	//ReadShapeModel((Para + "../Model/Model.shm").c_str(), &hv_ModelID);
	ReadTuple((Para + "RectRoiSearch.tup").c_str(), &hv_RectRoiSearch);
	ReadTuple((Para + "spokeParas.tup").c_str(), &hv_spoke_paras);



	////做变换
	//FindShapeModel(Image, hv_ModelID, -0.39, 0.79, 0.2, 1, 0.5, "least_squares",
	//	0, 0.9, &hv_RowModel, &hv_ColumnModel, &hv_AngleModel, &hv_Score);
	//if (hv_Score.TupleLength() == 0)
	//	return MODEL_NOT_FOUND;


	//GetShapeModelOrigin(hv_ModelID, &hv_RowOrigin, &hv_ColumnOrigin);
	//VectorAngleToRigid(hv_RowModel - hv_RowOrigin, hv_ColumnModel - hv_ColumnOrigin,
	//	hv_AngleModel, hv_RowOrigin, hv_ColumnOrigin, 0, &hv_HomMat2D);
	//AffineTransImage(Image, &Image, hv_HomMat2D, "constant", "false");



	GenRectangle2(&ho_Rectangle, HTuple(hv_RectRoiSearch[0]), HTuple(hv_RectRoiSearch[1]),
		HTuple(hv_RectRoiSearch[2]), HTuple(hv_RectRoiSearch[3]), HTuple(hv_RectRoiSearch[4]));
	ReduceDomain(Image, ho_Rectangle, &ho_ImageReduced);
	ScaleImageMax(ho_ImageReduced, &ho_ImageReduced);


	//找圆形区域
	GrayHisto(ho_Rectangle, ho_ImageReduced, &hv_AbsoluteHisto, &hv_RelativeHisto);
	TupleSortIndex(hv_AbsoluteHisto, &hv_Indices);
	hv_IndexSelect = 255;
	while (0 != ((HTuple(hv_Indices[hv_IndexSelect]) + 20) > 255))
	{
		hv_IndexSelect = hv_IndexSelect - 1;
	}
	if (HTuple(hv_Indices[hv_IndexSelect]) + 30 >= 255)
		return PARA_INVALID;

	Threshold(ho_ImageReduced, &ho_Regions, HTuple(hv_Indices[hv_IndexSelect]) + 30, 255);
	if (ho_Regions.CountObj() == 0)
		return PARA_INVALID;
	Connection(ho_Regions, &ho_ConnectedRegions);
	DilationCircle(ho_ConnectedRegions, &ho_ConnectedRegions, 20);
	Union1(ho_ConnectedRegions, &ho_ConnectedRegions);
	Connection(ho_ConnectedRegions, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, ((HTuple("area").Append("circularity")).Append("row")),
		"and", (HTuple(3165).Append(0.7)).TupleConcat(HTuple(hv_RectRoiSearch[0]) - 112),
		(HTuple(10000).Append(1)).TupleConcat(HTuple(hv_RectRoiSearch[0]) + 50));
	SortRegion(ho_SelectedRegions, &ho_SelectedRegions, "first_point", "true", "column");
	if (ho_SelectedRegions.CountObj() != 2)
		return PARA_INVALID;

	int nCircleIndex = Para.find("C1") != -1 ? 1 : 2;
	if (0 != (nCircleIndex == 1))
	{
		hv_SelectIndex = 0;
	}
	else
	{
		hv_SelectIndex = 1;
	}

	AreaCenter(ho_SelectedRegions, &hv_Area, &hv_Row1, &hv_Column1);
	IntersectionLineCircle(HTuple(hv_Row1[hv_SelectIndex]) - 50, HTuple(hv_Column1[hv_SelectIndex]),
		HTuple(hv_Row1[hv_SelectIndex]) + 50, HTuple(hv_Column1[hv_SelectIndex]), HTuple(hv_Row1[hv_SelectIndex]),
		HTuple(hv_Column1[hv_SelectIndex]), 26, 0, 6.28318, "positive", &hv_Row2,
		&hv_Column2);
	IntersectionLineCircle(HTuple(hv_Row1[hv_SelectIndex]), HTuple(hv_Column1[hv_SelectIndex]) - 50,
		HTuple(hv_Row1[hv_SelectIndex]), HTuple(hv_Column1[hv_SelectIndex]) + 50, HTuple(hv_Row1[hv_SelectIndex]),
		HTuple(hv_Column1[hv_SelectIndex]), 26, 0, 6.28318, "positive", &hv_Row3,
		&hv_Column3);

	//自动生成Spoke
	hv_ROIRows.Clear();
	hv_ROIRows.Append(hv_Row2);
	hv_ROIRows.Append(hv_Row3);
	hv_ROIRows.Append(HTuple(hv_Row2[0]));
	hv_ROICols.Clear();
	hv_ROICols.Append(hv_Column2);
	hv_ROICols.Append(hv_Column3);
	hv_ROICols.Append(HTuple(hv_Column2[0]));
	hv_Direct = "inner";

	spoke(ho_ImageReduced, &ho_Regions1, HTuple(hv_spoke_paras[0]), HTuple(hv_spoke_paras[1]),
		HTuple(hv_spoke_paras[2]), HTuple(hv_spoke_paras[3]), HTuple(hv_spoke_paras[4]),
		HTuple(hv_spoke_paras[5]), HTuple(hv_spoke_paras[6]), hv_ROIRows, hv_ROICols,
		hv_Direct, &hv_ResultRow, &hv_ResultColumn, &hv_ArcType);

	if (hv_ResultRow.TupleLength() < 3)
		return NOT_ENOUGH_OBJ;

	GenContourPolygonXld(&ho_Contour, hv_ResultRow, hv_ResultColumn);
	FitCircleContourXld(ho_Contour, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Column,
		&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	pCircle->CenterPoint.X = hv_Column.D();
	pCircle->CenterPoint.Y = hv_Row.D();
	pCircle->Radius = hv_Radius.D();
	return SUCCESS;
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

void CHalconFuncSet::GenImage(unsigned char* data, int width, int height)
{
	GenImage1(&m_ImageGen, "byte", width, height, (Hlong)data);
	WriteImage(m_ImageGen, "bmp", 0, "GrayImage");
}
void CHalconFuncSet::GenImage(char* FilePath)
{
	ReadImage(&m_ImageGen, FilePath);
}
void CHalconFuncSet::SaveImage(const char* FilePath)
{
	//EnterCriticalSection(&CS);
	HObject Image;
	if (this->m_ImageOutFore.IsInitialized() && this->m_ImageOutBk.IsInitialized())
	{
		Compose3(this->m_ImageOutFore, this->m_ImageOutBk, this->m_ImageOutBk, &Image);
		WriteImage(Image, "jpg", 0, FilePath);
		//Image.WriteObject(FilePath);
	}

	//Sleep(500);
	//LeaveCriticalSection(&CS);
}

void CHalconFuncSet::GetImageData(unsigned char* rgb_data, int width, int height)
{
	HTuple PtRed, PtGreen, PtBlue, ImageWidth, ImageHeight, ImageType;
	HObject Image;
	if (this->m_ImageOutFore.IsInitialized() && this->m_ImageOutBk.IsInitialized())
	{
		Compose3(this->m_ImageOutFore, this->m_ImageOutBk, this->m_ImageOutBk, &Image);
		GetImagePointer3(Image, &PtRed, &PtGreen, &PtBlue, &ImageType, &ImageWidth, &ImageHeight);
		if (width > (Hlong)ImageWidth || height > (Hlong)ImageHeight)
			throw new exception("Image width or height out of range!");
		BYTE *pR = (BYTE *)PtRed[0].L();
		BYTE *pG = (BYTE *)PtGreen[0].L();
		BYTE *pB = (BYTE *)PtBlue[0].L();

		memcpy(rgb_data, pR, width*height);
		memcpy(rgb_data + width * height, pG, width*height);
		memcpy(rgb_data + 2 * width*height, pB, width*height);
	}

}



//int CHalconFuncSet::ProcessImage(EnumPicType ImageType, double* Result, int& Num, bool draw_image)
//{
//	int n = 0;
//
//	HTuple Hom2D;
//	bool RetList[] = { false,false,false,false,false,false,false,false,false,false,false,false,false };
//	switch (ImageType)
//	{
//	case CHalconFuncSet::RP_POSE01:
//	case CHalconFuncSet::RP_POSE10:
//	{
//		string ParaPath = "";
//		if (ImageType == CHalconFuncSet::RP_POSE01)
//			ParaPath = "Pose1";
//		else
//			ParaPath = "Pose10";
//		string ParaP1 = "./Para/" + ParaPath + "/P1/";
//		CLineModel Line1;
//		CLineModel Line2;
//		if (AdjustImage(m_ImageGen, &m_ImageGen, "./Para/" + ParaPath + "/", Hom2D) != SUCCESS)
//			return FAILED;
//
//		this->m_ImageOutFore = m_ImageGen;
//		this->m_ImageOutBk = m_ImageGen;
//
//		if ((RetList[n] = (FindPair(m_ImageGen, ParaP1, &Line1, &Line2) == SUCCESS)) && draw_image)
//		{
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line1);
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line2);
//		}
//		double d = RetList[n] ? DistanceLineLine(Line1, Line2) : -1;
//		*(Result + n++) = d;
//		Num = 1;
//		return RetList[0] ? SUCCESS : FAILED;
//	}
//	break;
//	case CHalconFuncSet::RP_POSE03:
//	case CHalconFuncSet::RP_POSE08:
//	{
//		string ParaPath = "";
//		if (ImageType == CHalconFuncSet::RP_POSE03)
//			ParaPath = "Pose3";
//		else
//			ParaPath = "Pose8";
//
//		string ParaL1 = "./Para/" + ParaPath + "/L1/";
//		string ParaL2 = "./Para/" + ParaPath + "/L2/";
//		string ParaL3 = "./Para/" + ParaPath + "/L3/";
//		string ParaP1 = "./Para/" + ParaPath + "/P1/";
//		string ParaP2 = "./Para/" + ParaPath + "/P2/";
//		string ParaP3 = "./Para/" + ParaPath + "/P3/";
//		string ParaSpoke1 = "./Para/" + ParaPath + "/C1/";
//		string ParaSpoke2 = "./Para/" + ParaPath + "/C2/";
//
//		CLineModel Line1;
//		CLineModel Line2;
//		CLineModel Line3;
//
//		//Pair
//		CLineModel Line4;
//		CLineModel Line5;
//
//		//Pair
//		CLineModel Line6;
//		CLineModel Line7;
//
//		//Pair
//		CLineModel Line8;
//		CLineModel Line9;
//
//		CCircleModel Circle1;
//		CCircleModel Circle2;
//
//		if (AdjustImage(m_ImageGen, &m_ImageGen, "./Para/" + ParaPath + "/", Hom2D) != SUCCESS)
//			return FAILED;
//		this->m_ImageOutFore = m_ImageGen;
//		this->m_ImageOutBk = m_ImageGen;
//		int n = 0;
//		if ((RetList[n] = (FindLine(m_ImageGen, ParaL1, &Line1) == SUCCESS)) && draw_image)
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line1);
//		if ((RetList[n + 1] = (FindLine(m_ImageGen, ParaL2, &Line2) == SUCCESS)) && draw_image)
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line2);
//		if ((RetList[n + 2] = (FindLine(m_ImageGen, ParaL3, &Line3) == SUCCESS)) && draw_image)
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line3);
//		if ((RetList[n + 3] = (FindPair(m_ImageGen, ParaP1, &Line4, &Line5) == SUCCESS)) && draw_image)
//		{
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line4);
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line5);
//		}
//		if ((RetList[n + 4] = (FindCircle(m_ImageGen, ParaSpoke1, &Circle1) == SUCCESS)) && draw_image)
//			PaintCircle(this->m_ImageOutFore, this->m_ImageOutBk, Circle1);
//		if ((RetList[n + 5] = (FindCircle(m_ImageGen, ParaSpoke2, &Circle2) == SUCCESS)) && draw_image)
//			PaintCircle(this->m_ImageOutFore, this->m_ImageOutBk, Circle2);
//
//		if ((RetList[n + 6] = (FindPair(m_ImageGen, ParaP2, &Line6, &Line7) == SUCCESS)) && draw_image)
//		{
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line6);
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line7);
//			PaintLine(this->m_ImageOutFore, Line7, 0);
//			PaintLine(this->m_ImageOutFore, Line6, 0);
//			PaintLine(this->m_ImageOutBk, Line7, 255);
//			PaintLine(this->m_ImageOutBk, Line6, 255);
//		}
//
//		if ((RetList[n + 7] = (FindPair(m_ImageGen, ParaP3, &Line8, &Line9) == SUCCESS)) && draw_image)
//		{
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line8);
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line9);
//			PaintLine(this->m_ImageOutFore, Line8, 0);
//			PaintLine(this->m_ImageOutFore, Line9, 0);
//			PaintLine(this->m_ImageOutBk, Line8, 255);
//			PaintLine(this->m_ImageOutBk, Line9, 255);
//		}
//
//
//
//		//方通宽度
//		*(Result) = (RetList[n + 3] && RetList[n]) ? min(DistanceLineLine(Line1, Line4), DistanceLineLine(Line1, Line5)) : -1;
//		//缝隙宽度
//		*(Result + 1) = RetList[n + 3] ? DistanceLineLine(Line4, Line5) : -1;
//		//圆到三边的宽度
//
//		//n+4
//		*(Result + 2) = RetList[n + 4] ? DistanceCircleLine(Circle1, Line1) : -1;
//		*(Result + 3) = RetList[n + 4] ? DistanceCircleLine(Circle1, Line2) : -1;
//		*(Result + 4) = RetList[n + 4] ? DistanceCircleLine(Circle1, Line3) : -1;
//
//
//		//RetList[n + 5]
//		*(Result + 5) = RetList[n + 5] ? DistanceCircleLine(Circle2, Line1) : -1;
//		*(Result + 6) = RetList[n + 5] ? DistanceCircleLine(Circle2, Line2) : -1;
//		*(Result + 7) = RetList[n + 5] ? DistanceCircleLine(Circle2, Line3) : -1;
//
//		//上下缝隙的宽度
//		*(Result + 8) = RetList[n + 6] ? DistanceLineLine(Line6, Line7) : -1;
//		*(Result + 9) = RetList[n + 7] ? DistanceLineLine(Line8, Line9) : -1;
//
//
//
//		Num = 10;
//		bool Ret = true;
//		for (int i = 0;i < 8;i++)
//			Ret &= RetList[i];
//		return Ret ? SUCCESS : FAILED;
//	}
//	break;
//	case CHalconFuncSet::RP_POSE02:
//	case CHalconFuncSet::RP_POSE09:
//	case CHalconFuncSet::RP_POSE04:
//	case CHalconFuncSet::RP_POSE07:
//	case CHalconFuncSet::RP_POSE05:
//	case CHalconFuncSet::RP_POSE06:
//	{
//		string ParaPath = "";
//		if (ImageType == CHalconFuncSet::RP_POSE04)
//			ParaPath = "Pose4";
//		else if (ImageType == CHalconFuncSet::RP_POSE05)
//			ParaPath = "Pose5";
//		else if (ImageType == CHalconFuncSet::RP_POSE06)
//			ParaPath = "Pose6";
//		else if (ImageType == CHalconFuncSet::RP_POSE02)
//			ParaPath = "Pose2";
//		else if (ImageType == CHalconFuncSet::RP_POSE07)
//			ParaPath = "Pose7";
//		else
//			ParaPath = "Pose9";
//		string ParaL1 = "./Para/" + ParaPath + "/L1/";
//		string ParaP1 = "./Para/" + ParaPath + "/P1/";
//
//		CLineModel Line1;
//		CLineModel Line2;
//		CLineModel Line3;
//
//		if (AdjustImage(m_ImageGen, &m_ImageGen, "./Para/" + ParaPath + "/", Hom2D) != SUCCESS)
//			return FAILED;
//
//		this->m_ImageOutFore = m_ImageGen;
//		this->m_ImageOutBk = m_ImageGen;
//		if ((RetList[n] = (FindLine(m_ImageGen, ParaL1, &Line1) == SUCCESS)) && draw_image)
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line1);
//		if ((RetList[n + 1] = (FindPair(m_ImageGen, ParaP1, &Line2, &Line3) == SUCCESS)) && draw_image)
//		{
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line2);
//			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line3);
//		}
//
//		*(Result) = RetList[0] ? min(DistanceLineLine(Line1, Line2), DistanceLineLine(Line1, Line3)) : -1;
//		*(Result + 1) = RetList[1] ? DistanceLineLine(Line2, Line3) : -1;
//		Num = 2;
//		bool Ret = true;
//		for (int i = 0;i < 2;i++)
//			Ret &= RetList[i];
//		return Ret ? SUCCESS : FAILED;
//	}
//	break;
//
//	default:
//		break;
//	}
//	return FAILED;
//}

int CHalconFuncSet::ProcessImage1(EnumPicType ImageType, CPairModel& Pair, double* Result, int& Num, bool draw_image)
{
	int n = 0;

	HTuple Hom2D;
	bool RetList[] = { false,false,false,false,false,false,false,false,false,false,false,false,false };
	switch (ImageType)
	{
	case CHalconFuncSet::RP_POSE01:
	case CHalconFuncSet::RP_POSE10:
	{
		string ParaPath = "";
		if (ImageType == CHalconFuncSet::RP_POSE01)
			ParaPath = "Pose1";
		else
			ParaPath = "Pose10";
		string ParaP1 = "./Para/" + ParaPath + "/P1/";

		if (AdjustImage(m_ImageGen, &m_ImageGen, "./Para/" + ParaPath + "/", Hom2D) != SUCCESS)
			return FAILED;

		this->m_ImageOutFore = m_ImageGen;
		this->m_ImageOutBk = m_ImageGen;

		if ((RetList[0] = (FindPair(m_ImageGen, ParaP1, &Pair) == SUCCESS)) && draw_image)
		{
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Pair.Line1);
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Pair.Line2);
		}

		*(Result) = RetList[0] ? DistanceLineLine(Pair.Line1, Pair.Line2) : -1;
		*(Result+1) = RetList[0] ? AngleLineLine(Pair.Line1, Pair.Line2) : -1;


		Num = 2;
		return RetList[0] ? SUCCESS : FAILED;
	}
	break;
	}
}
int CHalconFuncSet::ProcessImage2(EnumPicType ImageType, CLineModel& Line, CPairModel& Pair, double* Result, int& Num, bool draw_image)
{
	int n = 0;

	HTuple Hom2D;
	bool RetList[] = { false,false,false,false,false,false,false,false,false,false,false,false,false };
	switch (ImageType)
	{
	case CHalconFuncSet::RP_POSE02:
	case CHalconFuncSet::RP_POSE09:
	case CHalconFuncSet::RP_POSE04:
	case CHalconFuncSet::RP_POSE07:
	case CHalconFuncSet::RP_POSE05:
	case CHalconFuncSet::RP_POSE06:
	{
		string ParaPath = "";
		if (ImageType == CHalconFuncSet::RP_POSE04)
			ParaPath = "Pose4";
		else if (ImageType == CHalconFuncSet::RP_POSE05)
			ParaPath = "Pose5";
		else if (ImageType == CHalconFuncSet::RP_POSE06)
			ParaPath = "Pose6";
		else if (ImageType == CHalconFuncSet::RP_POSE02)
			ParaPath = "Pose2";
		else if (ImageType == CHalconFuncSet::RP_POSE07)
			ParaPath = "Pose7";
		else
			ParaPath = "Pose9";
		string ParaL1 = "./Para/" + ParaPath + "/L1/";
		string ParaP1 = "./Para/" + ParaPath + "/P1/";

		if (AdjustImage(m_ImageGen, &m_ImageGen, "./Para/" + ParaPath + "/", Hom2D) != SUCCESS)
			return FAILED;

		this->m_ImageOutFore = m_ImageGen;
		this->m_ImageOutBk = m_ImageGen;
		if ((RetList[0] = (FindLine(m_ImageGen, ParaL1, &Line) == SUCCESS)) && draw_image)
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Line);
		if ((RetList[1] = (FindPair(m_ImageGen, ParaP1, &Pair) == SUCCESS)) && draw_image)
		{
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Pair.Line1);
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, Pair.Line2);
		}
		//方通宽度
		double D1 = DistanceLineLine(Line, Pair.Line1);
		double D2 = DistanceLineLine(Line, Pair.Line2);
		*(Result) = RetList[0] ? min(D1, D2) : -1;

		//方通夹角
		double A1 = (RetList[1] && RetList[0]) ? AngleLineLine(Line, Pair.Line1) : -1;
		double A2 = (RetList[1] && RetList[0]) ? AngleLineLine(Line, Pair.Line2) : -1;
		*(Result + 1) = (RetList[1] && RetList[0]) ? (D1 < D2 ? A1 : A2) : -1;

		//缝隙宽度
		*(Result + 2) = RetList[1] ? DistanceLineLine(Pair.Line1, Pair.Line2) : -1;

		//缝隙夹角
		*(Result + 3) = RetList[1] ? AngleLineLine(Pair.Line1, Pair.Line2) : -1;

		Num = 4;
		bool Ret = true;
		for (int i = 0;i < 2;i++)
			Ret &= RetList[i];
		return Ret ? SUCCESS : FAILED;
	}
	break;
	}
}
int CHalconFuncSet::ProcessImage3(EnumPicType ImageType, CLineModel& LineSide, CLineModel& LineUp, CLineModel& LineDown, CCircleModel& CircleLeft, CCircleModel& CircleRight,
	CPairModel& PairSide, CPairModel& PairUp, CPairModel& PairDown,  double* Result, int& Num, bool draw_image)
{
	HTuple Hom2D;
	bool RetList[] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
	switch (ImageType)
	{
	case CHalconFuncSet::RP_POSE03:
	case CHalconFuncSet::RP_POSE08:
	{
		string ParaPath = "";
		if (ImageType == CHalconFuncSet::RP_POSE03)
			ParaPath = "Pose3";
		else
			ParaPath = "Pose8";

		string ParaLineSide = "./Para/" + ParaPath + "/L1/";
		string ParaLineUp = "./Para/" + ParaPath + "/L2/";
		string ParaLineDown = "./Para/" + ParaPath + "/L3/";
		string ParaPirSide = "./Para/" + ParaPath + "/P1/";
		string ParaPairUp = "./Para/" + ParaPath + "/P2/";
		string ParaPairDown = "./Para/" + ParaPath + "/P3/";
		string ParaCircleLeft = "./Para/" + ParaPath + "/C1/";
		string ParaCircleRight = "./Para/" + ParaPath + "/C2/";




		if (AdjustImage(m_ImageGen, &m_ImageGen, "./Para/" + ParaPath + "/", Hom2D) != SUCCESS)
			return FAILED;
		this->m_ImageOutFore = m_ImageGen;
		this->m_ImageOutBk = m_ImageGen;
	
		//方通侧边线
		if ((RetList[0] = (FindLine(m_ImageGen, ParaLineSide, &LineSide) == SUCCESS)) && draw_image)
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, LineSide);
		//上边线
		if ((RetList[1] = (FindLine(m_ImageGen, ParaLineUp, &LineUp) == SUCCESS)) && draw_image)
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, LineUp);

		//下边线
		if ((RetList[2] = (FindLine(m_ImageGen, ParaLineDown, &LineDown) == SUCCESS)) && draw_image)
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, LineDown);

		//侧面缝隙
		if ((RetList[3] = (FindPair(m_ImageGen, ParaPirSide, &PairSide) == SUCCESS)) && draw_image)
		{
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, PairSide.Line1);
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, PairSide.Line2);
		}

		//左边圆
		if ((RetList[4] = (FindCircle(m_ImageGen, ParaCircleLeft, &CircleLeft) == SUCCESS)) && draw_image)
			PaintCircle(this->m_ImageOutFore, this->m_ImageOutBk, CircleLeft);

		//右边圆
		if ((RetList[5] = (FindCircle(m_ImageGen, ParaCircleRight, &CircleRight) == SUCCESS)) && draw_image)
			PaintCircle(this->m_ImageOutFore, this->m_ImageOutBk, CircleRight);

		//上面缝隙
		if ((RetList[6] = (FindPair(m_ImageGen, ParaPairUp, &PairUp) == SUCCESS)) && draw_image)
		{
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, PairUp.Line1);
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, PairUp.Line2);
			PaintLine(this->m_ImageOutFore, PairUp.Line2, 0);
			PaintLine(this->m_ImageOutFore, PairUp.Line1, 0);
			PaintLine(this->m_ImageOutBk, PairUp.Line2, 255);
			PaintLine(this->m_ImageOutBk, PairUp.Line1, 255);
		}

		//下面缝隙
		if ((RetList[7] = (FindPair(m_ImageGen, ParaPairDown, &PairDown) == SUCCESS)) && draw_image)
		{
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, PairDown.Line1);
			PaintLine(this->m_ImageOutFore, this->m_ImageOutBk, PairDown.Line2);
			PaintLine(this->m_ImageOutFore, PairDown.Line1, 0);
			PaintLine(this->m_ImageOutFore, PairDown.Line2, 0);
			PaintLine(this->m_ImageOutBk, PairDown.Line1, 255);
			PaintLine(this->m_ImageOutBk, PairDown.Line2, 255);
		}


		//方通宽度
		double D1 = DistanceLineLine(LineSide, PairSide.Line1);
		double D2 = DistanceLineLine(LineSide, PairSide.Line2);
		*(Result) = (RetList[3] && RetList[0]) ? min(D1, D2) : -1;

		//方通夹角
		double A1 = (RetList[3] && RetList[0]) ? AngleLineLine(LineSide, PairSide.Line1) : -1;
		double A2= (RetList[3] && RetList[0]) ? AngleLineLine(LineSide, PairSide.Line2) : -1;
		*(Result+1) = (RetList[3] && RetList[0]) ? (D1 < D2 ? A1 : A2) : -1;

		//侧面缝隙宽度
		*(Result + 2) = RetList[3] ? DistanceLineLine(PairSide.Line1, PairSide.Line2) : -1;

		//侧面缝隙夹角
		*(Result + 3) = RetList[3] ? AngleLineLine(PairSide.Line1, PairSide.Line2) : -1;

		//左圆到侧面距离
		*(Result + 4) = (RetList[0] && RetList[4]) ? DistanceCircleLine(CircleLeft, LineSide) : -1;

		//左圆到上面距离
		*(Result + 5) = (RetList[1] && RetList[4]) ? DistanceCircleLine(CircleLeft, LineUp) : -1;

		//左圆到下面距离
		*(Result + 6) = (RetList[2] && RetList[4]) ? DistanceCircleLine(CircleLeft, LineDown) : -1;

		//右圆到侧面距离
		*(Result + 7) = (RetList[0] && RetList[5]) ? DistanceCircleLine(CircleRight, LineSide) : -1;
		//右圆到上面距离
		*(Result + 8) = (RetList[1] && RetList[5]) ? DistanceCircleLine(CircleRight, LineUp) : -1;
		//右圆到下面距离
		*(Result + 9) = (RetList[2] && RetList[5]) ? DistanceCircleLine(CircleRight, LineDown) : -1;


		//上缝隙宽度
		*(Result + 10) = RetList[6] ? DistanceLineLine(PairUp.Line1, PairUp.Line2) : -1;
		//上缝隙角度
		*(Result + 11) = RetList[6] ? AngleLineLine(PairUp.Line1, PairUp.Line2) : -1;
		//下缝隙宽度
		*(Result + 12) = RetList[7] ? DistanceLineLine(PairDown.Line1, PairDown.Line2) : -1;
		//下缝隙角度
		*(Result + 13) = RetList[7] ? AngleLineLine(PairDown.Line1, PairDown.Line2) : -1;
		Num = 14;
		bool Ret = true;
		for (int i = 0;i < 8;i++)
			Ret &= RetList[i];
		return Ret ? SUCCESS : FAILED;
	}
	break;
	}
}






void CHalconFuncSet::PaintLine(HObject& ImageFore, HObject& ImageBk, CLineModel Line)
{
	HObject RegionLine1;
	GenRegionLine(&RegionLine1, Line.StartPoint.Y, Line.StartPoint.X, Line.EndPoint.Y, Line.EndPoint.X);
	PaintRegion(RegionLine1, ImageFore, &this->m_ImageOutFore, 255, "fill");

	PaintRegion(RegionLine1, ImageBk, &this->m_ImageOutBk, 0, "fill");
}

void CHalconFuncSet::PaintCircle(HObject& ImageFore, HObject& ImageBk, CCircleModel Circle)
{
	HObject RegionCircle, RegionPoint, RegionCircleErosion;
	GenRegionPoints(&RegionPoint, Circle.CenterPoint.Y, Circle.CenterPoint.X);
	GenCircle(&RegionCircle, Circle.CenterPoint.Y, Circle.CenterPoint.X, Circle.Radius);
	GenCircle(&RegionCircleErosion, Circle.CenterPoint.Y, Circle.CenterPoint.X, Circle.Radius - 2);
	//ErosionCircle(RegionCircle, &RegionCircleErosion, 4);
	Difference(RegionCircle, RegionCircleErosion, &RegionCircle);

	PaintRegion(RegionPoint, ImageFore, &this->m_ImageOutFore, 255, "fill");
	PaintRegion(RegionCircle, ImageFore, &this->m_ImageOutFore, 255, "fill");

	PaintRegion(RegionPoint, ImageBk, &this->m_ImageOutBk, 0, "fill");
	PaintRegion(RegionCircle, ImageBk, &this->m_ImageOutBk, 0, "fill");
}

void CHalconFuncSet::PaintLine(HObject& Image, CLineModel Line, int GrayValue)
{
	HObject RegionLine1;
	GenRegionLine(&RegionLine1, Line.StartPoint.Y, Line.StartPoint.X, Line.EndPoint.Y, Line.EndPoint.X);
	PaintRegion(RegionLine1, Image, &Image, GrayValue % 256, "fill");
}
void CHalconFuncSet::PaintCircle(HObject& Image, CCircleModel Circle, int GrayValue)
{
	HObject RegionCircle, RegionPoint, RegionCircleErosion;
	GenRegionPoints(&RegionPoint, Circle.CenterPoint.Y, Circle.CenterPoint.X);
	GenCircle(&RegionCircle, Circle.CenterPoint.Y, Circle.CenterPoint.X, Circle.Radius);
	GenCircle(&RegionCircleErosion, Circle.CenterPoint.Y, Circle.CenterPoint.X, Circle.Radius - 2);
	//ErosionCircle(RegionCircle, &RegionCircleErosion, 4);
	Difference(RegionCircle, RegionCircleErosion, &RegionCircle);

	PaintRegion(RegionPoint, Image, &Image, GrayValue % 256, "fill");
	PaintRegion(RegionCircle, Image, &Image, GrayValue % 256, "fill");
}

double CHalconFuncSet::DistanceLineLine(CLineModel& Line1, CLineModel& Line2)
{
	HTuple D1, D2;
	DistancePl(Line1.StartPoint.Y, Line1.StartPoint.X, Line2.StartPoint.Y, Line2.StartPoint.X, Line2.EndPoint.Y, Line2.EndPoint.X, &D1);
	DistancePl(Line1.EndPoint.Y, Line1.EndPoint.X, Line2.StartPoint.Y, Line2.StartPoint.X, Line2.EndPoint.Y, Line2.EndPoint.X, &D2);
	return ((D1 + D2) / 2).D();
}
double CHalconFuncSet::DistanceCircleLine(CCircleModel& Circle, CLineModel& Line)
{
	HTuple D;
	DistancePl(Circle.CenterPoint.Y, Circle.CenterPoint.X, Line.StartPoint.Y, Line.StartPoint.X, Line.EndPoint.Y, Line.EndPoint.X, &D);
	return D.D();
}
double CHalconFuncSet::AngleLineLine(CLineModel& Line1, CLineModel& Line2)
{
	HTuple Angle;
	AngleLl(Line1.StartPoint.Y, Line1.StartPoint.X, Line1.EndPoint.Y, Line1.EndPoint.X, Line2.StartPoint.Y, Line2.StartPoint.X, Line2.EndPoint.Y, Line2.EndPoint.X, &Angle);
	double A = RadToDeg(Angle);
	if (abs(A) > 90)
		A = abs(A) - 90;
	else
		A = abs(A);
	return A;
}