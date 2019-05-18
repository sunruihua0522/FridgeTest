#include "CHalconFuncSet.h"
#include "CVisionMetric2D.h"
#include "CPairModel.h"
#include "CLineModel.h"
#include "CPointModel.h"
#include "CCircleModel.h"
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
void MovePairToGap(CPairModel& Pair, GAP2D& Gap)
{
	Gap.L1.P1.x = Pair.Line1.StartPoint.X;
	Gap.L1.P1.y = Pair.Line1.StartPoint.Y;

	Gap.L1.P2.x = Pair.Line1.EndPoint.X;
	Gap.L1.P2.y = Pair.Line1.EndPoint.Y;

	Gap.L2.P1.x = Pair.Line2.StartPoint.X;
	Gap.L2.P1.y = Pair.Line2.StartPoint.Y;

	Gap.L2.P2.x = Pair.Line2.EndPoint.X;
	Gap.L2.P2.y = Pair.Line2.EndPoint.Y;
}
void MoveLineModelToLine2D(CLineModel& LineModel, LINE2D& Line2D)
{
	Line2D.P1.x = LineModel.StartPoint.X;
	Line2D.P1.y = LineModel.StartPoint.Y;
	Line2D.P2.x = LineModel.EndPoint.X;
	Line2D.P2.y = LineModel.EndPoint.Y;
}

void MovePointModelToPoint2D(CPointModel& PointModel, POINT2D& Point2D)
{
	Point2D.x = PointModel.X;
	Point2D.y = PointModel.Y;
}
void CVisionMetric2D::SetImageData(unsigned char* data, int width, int height)
{
	static_cast<CHalconFuncSet*> (m_pWapper)->GenImage(data, width, height);
}

void CVisionMetric2D::SetImageFile(const char* acFilePathNamefile_path_name)
{
	static_cast<CHalconFuncSet*> (m_pWapper)->GenImage(_strdup(acFilePathNamefile_path_name));
}
//! 计算
//int CVisionMetric2D::Compute(EN_RobotPose robot_pose, double *param_list, int& num_param,bool draw_image)
//{
//	try
//	{
//		return 0==static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage((CHalconFuncSet::EnumPicType)robot_pose, param_list, num_param,draw_image)? EC_SUCCESS : EC_FAILED;
//	}
//	catch(exception ex)
//	{
//		return EC_FAILED;
//	}
//	
//}

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

int CVisionMetric2D::Compute1(GAP2D& gap, bool draw_image)
{
	try
	{
		CPairModel Pair;
		int Num;
		int nRet=static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage1((CHalconFuncSet::EnumPicType)RP_POSE01,Pair, m_DoubleBuff, Num,draw_image);
		MovePairToGap(Pair, gap);
		gap.Distance = *m_DoubleBuff;
		gap.Angle= *(m_DoubleBuff+1);
		return nRet == 0? EC_SUCCESS : EC_FAILED;
	}
	catch(exception ex)
	{
		return EC_FAILED;
	}
	
}
int CVisionMetric2D::Compute2(GAP2D& gap_Width, GAP2D& gap_Narrow, bool draw_image)
{
	try
	{
		CPairModel Pair,PairWidth;
		CLineModel Line;
		int Num;
		int nRet = static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage2((CHalconFuncSet::EnumPicType)RP_POSE02, Line, Pair, m_DoubleBuff, Num, draw_image);
		
		double D1 = static_cast<CHalconFuncSet*> (m_pWapper)->DistanceLineLine(Line, Pair.Line1);
		double D2= static_cast<CHalconFuncSet*> (m_pWapper)->DistanceLineLine(Line, Pair.Line2);

		CLineModel LineNear = D1 > D2 ? Pair.Line1 : Pair.Line2;
		PairWidth.Line1 = Line;
		PairWidth.Line2 = LineNear;
		MovePairToGap(PairWidth, gap_Width);
		gap_Width.Distance = *m_DoubleBuff;
		gap_Width.Angle = *(m_DoubleBuff + 1);

		MovePairToGap(Pair, gap_Narrow);
		gap_Narrow.Distance = *(m_DoubleBuff+2);
		gap_Narrow.Angle = *(m_DoubleBuff + 3);

		return nRet == 0 ? EC_SUCCESS : EC_FAILED;
	}
	catch (exception ex)
	{
		return EC_FAILED;
	}

}
int CVisionMetric2D::Compute3(GAP2D& gap_side, GAP2D& gap_up, GAP2D& gap_down, HOLEINFO& hole1, HOLEINFO& hole2, bool draw_image)
{
	try
	{
		CLineModel LineSide, LineUp, LineDown;
		CCircleModel CircleLeft, CircleRight;
		CPairModel PairSide, PairUp, PairDown;
		int Num;
		int nRet = static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage3((CHalconFuncSet::EnumPicType)RP_POSE03,LineSide,LineUp,LineDown,CircleLeft,CircleRight, PairSide,PairUp, PairDown,m_DoubleBuff, Num, draw_image);
		MovePairToGap(PairSide, gap_side);
		gap_side.Distance = *(m_DoubleBuff+2);
		gap_side.Angle= *(m_DoubleBuff+3);

		MovePairToGap(PairUp, gap_up);
		gap_up.Distance = *(m_DoubleBuff+10);
		gap_up.Angle = *(m_DoubleBuff + 11);

		MovePairToGap(PairDown, gap_down);
		gap_down.Distance = *(m_DoubleBuff + 12);
		gap_down.Angle = *(m_DoubleBuff + 13);


		MovePointModelToPoint2D(CircleLeft.CenterPoint, hole1.CenterPt);
		hole1.Radius = CircleLeft.Radius;
		hole1.DistanceSide= *(m_DoubleBuff + 4);
		hole1.DistanceUp= *(m_DoubleBuff + 5);
		hole1.DistanceDown = *(m_DoubleBuff + 6);
		MoveLineModelToLine2D(LineSide, hole1.LineSide);
		MoveLineModelToLine2D(LineUp, hole1.LineUp);
		MoveLineModelToLine2D(LineDown, hole1.LineDown);

		MovePointModelToPoint2D(CircleRight.CenterPoint, hole2.CenterPt);
		hole2.Radius = CircleRight.Radius;
		hole2.DistanceSide = *(m_DoubleBuff + 7);
		hole2.DistanceUp = *(m_DoubleBuff + 8);
		hole2.DistanceDown = *(m_DoubleBuff + 9);
		MoveLineModelToLine2D(LineSide, hole2.LineSide);
		MoveLineModelToLine2D(LineUp, hole2.LineUp);
		MoveLineModelToLine2D(LineDown, hole2.LineDown);

		return nRet == 0 ? EC_SUCCESS : EC_FAILED;
	}
	catch (exception ex)
	{
		return EC_FAILED;
	}
}
int CVisionMetric2D::Compute8(GAP2D& gap_side, GAP2D& gap_up, GAP2D& gap_down, HOLEINFO& hole1, HOLEINFO& hole2, bool draw_image)
{
	try
	{
		CLineModel LineSide, LineUp, LineDown;
		CCircleModel CircleLeft, CircleRight;
		CPairModel PairSide, PairUp, PairDown;
		int Num;
		int nRet = static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage3((CHalconFuncSet::EnumPicType)RP_POSE08, LineSide, LineUp, LineDown, CircleLeft, CircleRight, PairSide,PairUp,PairDown,m_DoubleBuff, Num, draw_image);
		MovePairToGap(PairSide, gap_side);
		gap_side.Distance = *(m_DoubleBuff + 2);
		gap_side.Angle = *(m_DoubleBuff + 3);

		MovePairToGap(PairUp, gap_up);
		gap_up.Distance = *(m_DoubleBuff + 10);
		gap_up.Angle = *(m_DoubleBuff + 11);

		MovePairToGap(PairDown, gap_down);
		gap_down.Distance = *(m_DoubleBuff + 12);
		gap_down.Angle = *(m_DoubleBuff + 13);


		MovePointModelToPoint2D(CircleLeft.CenterPoint, hole1.CenterPt);
		hole1.Radius = CircleLeft.Radius;
		hole1.DistanceSide = *(m_DoubleBuff + 4);
		hole1.DistanceUp = *(m_DoubleBuff + 5);
		hole1.DistanceDown = *(m_DoubleBuff + 6);
		MoveLineModelToLine2D(LineSide, hole1.LineSide);
		MoveLineModelToLine2D(LineUp, hole1.LineUp);
		MoveLineModelToLine2D(LineDown, hole1.LineDown);

		MovePointModelToPoint2D(CircleRight.CenterPoint, hole2.CenterPt);
		hole2.Radius = CircleRight.Radius;
		hole2.DistanceSide = *(m_DoubleBuff + 7);
		hole2.DistanceUp = *(m_DoubleBuff + 8);
		hole2.DistanceDown = *(m_DoubleBuff + 9);
		MoveLineModelToLine2D(LineSide, hole2.LineSide);
		MoveLineModelToLine2D(LineUp, hole2.LineUp);
		MoveLineModelToLine2D(LineDown, hole2.LineDown);

		return nRet == 0 ? EC_SUCCESS : EC_FAILED;
	}
	catch (exception ex)
	{
		return EC_FAILED;
	}
}
int CVisionMetric2D::Compute9(GAP2D& gap_Width, GAP2D gap_Narrow, bool draw_image)
{
	try
	{
		CPairModel Pair, PairWidth;
		CLineModel Line;
		int Num;
		int nRet = static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage2((CHalconFuncSet::EnumPicType)RP_POSE09, Line, Pair, m_DoubleBuff, Num, draw_image);

		double D1 = static_cast<CHalconFuncSet*> (m_pWapper)->DistanceLineLine(Line, Pair.Line1);
		double D2 = static_cast<CHalconFuncSet*> (m_pWapper)->DistanceLineLine(Line, Pair.Line2);

		CLineModel LineNear = D1 > D2 ? Pair.Line1 : Pair.Line2;
		PairWidth.Line1 = Line;
		PairWidth.Line2 = LineNear;
		MovePairToGap(PairWidth, gap_Width);
		gap_Width.Distance = *m_DoubleBuff;
		gap_Width.Angle = *(m_DoubleBuff + 1);

		MovePairToGap(Pair, gap_Narrow);
		gap_Narrow.Distance = *(m_DoubleBuff + 2);
		gap_Narrow.Angle = *(m_DoubleBuff + 3);

		return nRet == 0 ? EC_SUCCESS : EC_FAILED;
	}
	catch (exception ex)
	{
		return EC_FAILED;
	}

}
int CVisionMetric2D::Compute10(GAP2D& gap, bool draw_image)
{
	try
	{
		CPairModel Pair;
		int Num;
		int nRet = static_cast<CHalconFuncSet*> (m_pWapper)->ProcessImage1((CHalconFuncSet::EnumPicType)RP_POSE10, Pair, m_DoubleBuff, Num, draw_image);
		MovePairToGap(Pair, gap);
		gap.Distance = *m_DoubleBuff;
		gap.Angle = *(m_DoubleBuff + 1);
		return nRet == 0 ? EC_SUCCESS : EC_FAILED;
	}
	catch (exception ex)
	{
		return EC_FAILED;
	}
}

