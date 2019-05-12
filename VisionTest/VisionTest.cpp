// VisionTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "CVisionMetric2D.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"DemoDll.lib")
int main()
{
	int Index = 8;
	CVisionMetric2D Vision;
	char Buff[100];
	memset(Buff, 0, 100);

	sprintf_s(Buff, "C:\\Users\\cn11321\\source\\Code\\image\\RawImage\\Robot_0%d.png", Index);

	Vision.SetImageData(Buff);

	double* Res=Vision.Compute((CVisionMetric2D::EN_RobotPose)(Index-1));

	memset(Buff, 0, 100);
	sprintf_s(Buff, "%d.bmp", Index);
	Vision.SaveImage(Buff);
}


