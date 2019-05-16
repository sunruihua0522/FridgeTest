// VisionTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define Log
#include "CVisionMetric2D.h"
#include <Windows.h>
#include <iostream>
using namespace std;
#pragma comment(lib,"DemoDll.lib")
int main()
{
	
	int IndexBuff[] = { 1,2,3,8,9,10 };
	for (int i = 1;i < 10;i++)
	{
		cout << "#############################################################" << endl;
		for (int j = 0;j < 6;j++)
		{
			int Index = IndexBuff[j];
			CVisionMetric2D Vision;
			char Buff[100] = {0};
			if(Index!=10)
				sprintf_s(Buff, "C:\\Users\\cn11321\\source\\repos\\TestDemo\\Halcon\\image\\RawImage\\%d\\Robot_0%d.png", i, Index);
			else
				sprintf_s(Buff, "C:\\Users\\cn11321\\source\\repos\\TestDemo\\Halcon\\image\\RawImage\\%d\\Robot_%d.png", i, Index);
			Vision.SetImageFile(Buff);		
			int Num = 0;
			double pRes[20];
			int x=Vision.Compute((CVisionMetric2D::EN_RobotPose)(Index - 1),pRes,Num,true);

			memset(Buff, 0, 100);
			sprintf_s(Buff, "C:\\jjj.jpg", Index);
			Vision.SaveImage(Buff);
			//Sleep(200);
			unsigned char* ImageBuff = (unsigned char*)malloc(2592 * 2048 * 3);
			int ImageWidth=2592, ImageHeight=2048;
			//Vision.GetImageData(ImageBuff, ImageWidth, ImageHeight);
			cout << "-------Num=" << Num <<"--------" <<endl;
			for (int i = 0;i < Num;i++)
				cout << pRes[i] << endl;
		}
	}
}


