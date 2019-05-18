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
	for (int i = 2;i < 3;i++)
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
			int nRet = 0;
			double pRes[20];
			switch (j)
			{
			case 0:
			{
				GAP2D gap;
				nRet = Vision.Compute1(gap, true);
				break;
			}
			case 1:
			{
				GAP2D gap_width, gap_narrow;
				nRet = Vision.Compute2(gap_width, gap_narrow, true);
	
				break;
			}
			case 2:
			{
				GAP2D GapSide, GapUp, GapDown;
				HOLEINFO HoleLeft, HoleDown;
				nRet = Vision.Compute3(GapSide, GapUp, GapDown, HoleLeft, HoleDown,true);
				break;
			}
			case 3:
			{
				GAP2D GapSide, GapUp, GapDown;
				HOLEINFO HoleLeft, HoleDown;
				nRet = Vision.Compute8(GapSide, GapUp, GapDown, HoleLeft, HoleDown,true);
				break;
			}
			case 4:
			{
				GAP2D gap_width, gap_narrow;
				nRet = Vision.Compute9(gap_width, gap_narrow, true);
				break;
			}
			case 5:
			{
				GAP2D gap;
				nRet = Vision.Compute10(gap, true);
				break;
			}
			default:
				break;
			}
			

			memset(Buff, 0, 100);
			sprintf_s(Buff, "%d.jpg", Index);
			Vision.SaveImage(Buff);
			//Sleep(200);
			unsigned char* ImageBuff = (unsigned char*)malloc(2592 * 2048 * 3);
			int ImageWidth=2592, ImageHeight=2048;
			//Vision.GetImageData(ImageBuff, ImageWidth, ImageHeight);
			cout << "-------Num=" << Num <<"--------  X:"<<nRet <<endl;
			for (int i = 0;i < Num;i++)
				cout << pRes[i] << endl;
		}
	}
}


