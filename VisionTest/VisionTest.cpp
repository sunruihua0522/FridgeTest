// VisionTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "CVisionMetric2D.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"DemoDll.lib")
int main()
{
	int IndexBuff[] = { 1,2,3,8,9,10 };
	int i = 1;
	//for (int i = 1;i < 10;i++)
	{
		//int j = 1;
		for (int j = 0;j < 6;j++)
		{
			int PoseIndex = 1;
			int Index = IndexBuff[j];
			CVisionMetric2D Vision;
			char Buff[100];
			memset(Buff, 0, 100);
			if(Index!=10)
				sprintf_s(Buff, "C:\\Users\\cn11321\\source\\repos\\TestDemo\\Halcon\\image\\RawImage\\%d\\Robot_0%d.png", PoseIndex, Index);
			else
				sprintf_s(Buff, "C:\\Users\\cn11321\\source\\repos\\TestDemo\\Halcon\\image\\RawImage\\%d\\Robot_%d.png", PoseIndex, Index);
			Vision.SetImageData(Buff);

			double* Res = Vision.Compute((CVisionMetric2D::EN_RobotPose)(Index - 1));

			memset(Buff, 0, 100);
			sprintf_s(Buff, "%d.bmp", Index);
			Vision.SaveImage(Buff);
		}
	}
}


