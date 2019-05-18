#pragma once
namespace Hua
{
	typedef struct INFOBASE
	{


	}* LPINFOBASE;

	typedef struct POINT2D : public INFOBASE
	{
		double x;
		double y;
		POINT2D(double _x, double _y) :x(_x), y(_y) {}
		POINT2D() :x(-1), y(-1) {}
	}* LPPOINT2D;

	typedef struct LINE2D : public INFOBASE
	{
		POINT2D P1;
		POINT2D P2;
		LINE2D(POINT2D p1, POINT2D p2) :P1(p1), P2(p2){}
		LINE2D() {};
	}* LPLINE2D;

	typedef struct GAP2D : public INFOBASE
	{
		LINE2D L1;
		LINE2D L2;
		double Angle;
		double Distance;
		GAP2D(LINE2D l1, LINE2D l2) :L1(l1), L2(l2), Angle(-1), Distance(-1) {}
		GAP2D():Angle(-1), Distance(-1) {}

	}* LPGAP2D;

	typedef struct HOLEINFO : public INFOBASE
	{
		POINT2D CenterPt;
		double Radius;

		LINE2D LineSide;
		LINE2D LineUp;
		LINE2D LineDown;

		double DistanceSide;
		double DistanceUp;
		double DistanceDown;

		HOLEINFO() :Radius(-1) {}
		HOLEINFO(POINT2D pt, double R) { CenterPt = pt; Radius = R; }

	}* LPHOLEINFO;

	
}