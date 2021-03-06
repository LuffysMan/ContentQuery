#include "pch.h"
#include "addWeighted.h"
#include "CommonFunc.h"
#include <thread>
#include <random>

using namespace cv;
using namespace std;
// NORMAL, BIGLOOP, SSE, SSE_OMP

//Mat image, image1, image2;
//char windowName[32] = "Image Fusion";
//char trackBarName[32] = "TrackBar";
//int trackBarValue = 0;
//int trackBarMax = 100;
char g_szParam1[32] = {0};
char g_szParam2[32] = {0};
void(*g_pFunc)(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat& dst);		//函数指针

//控制条回调函数
//void TrackBarFunc(int, void(*));
void fusion_show(vector<Mat>& vecImage, char* windowName);
void performanceTest(vector<Mat>& vecImage, char* windowName);
void album(char* imgDir, char* windowName);
static int nThreadCount;
int main(int argc, char *argv[])
{
	char imgDir[_MAX_PATH] = { 0 };
	getWorkDir(imgDir);
	sprintf_s(imgDir, _MAX_PATH, "%s%s", imgDir, "image\\");
	vector<char*> vecImgDir;
	listDir(imgDir, vecImgDir);
	if (argc > 2) 
	{
		sprintf_s(g_szParam1, 32, "%s", argv[1]);
		sprintf_s(g_szParam2, 32, "%s", argv[2]);
	}
	if (strcmp(g_szParam1, "TEST") == 0)
	{
		album(vecImgDir[0], g_szParam1);
	}
	else
	{
		std::vector<std::thread> pool;
		for (size_t i = 0; i < vecImgDir.size(); ++i)
		{
			char* szWinname = new char[32]();
			sprintf_s(szWinname, 32, "%d", i);
			pool.push_back(thread(album, vecImgDir[i], szWinname));
		}
		for (size_t i = 0; i < pool.size(); ++i)
		{
			pool.at(i).join();
		}
	}
	releasePointVec(vecImgDir);
	return 0;
}
//void TrackBarFunc(int, void(*))
//{
//	//转换成融合比例
//	float rate = (float)trackBarValue / trackBarMax;
//	addWeighted(image1, rate, image2, 1 - rate, 0, image);
//	imshow(windowName, image);
//}
void album(char* imgDir, char* windowName)
{
	nThreadCount++;
	//cout << "ThreadCount:" << nThreadCount << "\n"<< endl;
	vector<Mat> vecImage;
	importImages(imgDir, vecImage);
	if (strcmp(g_szParam1, "TEST") == 0)
	{
		performanceTest(vecImage, windowName);
	}
	else
	{
		fusion_show(vecImage, windowName);
	}
	delete[] windowName;
}

void fusion_show(vector<Mat>& vecImage, char* windowName)
{
	if (vecImage.size() < 2)
	{
		return;
	}
	namedWindow(windowName, WINDOW_NORMAL);
	float rate = 0;
	Mat image1 = vecImage.at(0);
	Mat image2;
	Mat image;
	Size size = Size(image1.cols, image1.rows);
	imshow(windowName, image1);
	for (size_t idx = 0 ,vecSize = vecImage.size(); ; ++idx)
	{
		image1 = vecImage.at(idx%vecSize);
		image2 = vecImage.at((idx+1)%vecSize);
		resize(image2, image2, size);		//调整image2的大小与image1的大小一致
		for (int nCnt = 100; nCnt > 0; --nCnt)
		{
			rate = nCnt / 100.0;
			imgFusionSSE(image1, rate, image2, image);		//调整image2的大小与image1的大小一致
			imshow(windowName, image);
			waitKey(10);
		}
		image1 = image;
		waitKey(1000);						//每次渐变完等100ms，视觉感受更佳
	}
}
void performanceTest(vector<Mat>& vecImage, char* windowName)
{
	if (vecImage.size() < 2)
	{
		return;
	}
	namedWindow(windowName, WINDOW_NORMAL);
	float rate = 0;
	Mat image1 = vecImage.at(0);
	Mat image2 = vecImage.at(1);
	Mat image;
	Size size = Size(image1.cols, image1.rows);
	imshow(windowName, image1);
	resize(image2, image2, size);		//调整image2的大小与image1的大小一致

	LARGE_INTEGER nFreq;				//用于精确计时
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;
	double average = 0.0;
	if (strcmp(g_szParam2, "NORMAL") == 0)
	{
		g_pFunc = &imgFusionNormal;
	}
	else if (strcmp(g_szParam2, "NORMAL_LONGSTEP") == 0)
	{
		g_pFunc = &imgFusionLongStep;
	}
	else if (strcmp(g_szParam2, "NORMAL_OMP") == 0)
	{
		g_pFunc = &imgFusionNormal_OMP;
	}
	else if (strcmp(g_szParam2, "SSE") == 0)
	{
		g_pFunc = &imgFusionSSE;
	}
	for (int nCnt = 100; nCnt > 0; --nCnt)
	{
		rate = nCnt / 100.0;
		QueryPerformanceFrequency(&nFreq);
		QueryPerformanceCounter(&nBeginTime);
		g_pFunc(image1, rate, image2, image);		//使用函数指针调用对应融合方法
		QueryPerformanceCounter(&nEndTime);
		time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
		average += time;
		cout << "执行时间： " << time << endl;
		imshow(windowName, image);
		waitKey(10);
	}
	cout <<g_szParam2<<"100次平均执行时间: " << average / 100.0 << endl;
	waitKey(1111111);
}
