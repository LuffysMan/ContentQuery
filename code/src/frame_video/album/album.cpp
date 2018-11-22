#include "pch.h"
#include "addWeighted.h"
#include "CommonFunc.h"
#include <thread>
#include <random>

using namespace cv;
using namespace std;
// NORMAL, BIGLOOP, SSE, SSE_OMP
#define BIGLOOP
#define TEST

//Mat image, image1, image2;
//char windowName[32] = "Image Fusion";
//char trackBarName[32] = "TrackBar";
//int trackBarValue = 0;
//int trackBarMax = 100;

//控制条回调函数
//void TrackBarFunc(int, void(*));
void fusion_show(vector<Mat>& vecImage, char* windowName);
void performanceTest(vector<Mat>& vecImage, char* windowName);
void album(char* imgDir, char* windowName);
static int nThreadCount;
int main(int argc, char *argv[])
{
	char imgDir[_MAX_PATH] = { 0 };
	char imgDir1[_MAX_PATH] = { 0 };
	char imgDir2[_MAX_PATH] = { 0 };
	char winname1[32] = "asdf";
	char winname2[32] = "qwer";
	getWorkDir(imgDir);
	sprintf_s(imgDir1, _MAX_PATH, "%s%s", imgDir, "image\\a\\");
	sprintf_s(imgDir2, _MAX_PATH, "%s%s", imgDir, "image\\b\\");
	thread t1(album, imgDir1, winname1);
	//thread t2(album, imgDir2, winname2);
	t1.join();													//启动线程t, 并且阻塞主线程，等到线程t运行结束后，再继续运行主线程；
	//t2.join();													

	////判断读入是否成功
	//if (!image1.data | !image2.data)
	//{
	//	std::cout << "打开图片失败，请检查路径！" << std::endl;
	//	return 0;
	//}
	////调整image2的大小与image1的大小一致，融合函数addWeighted()要求输入的两个图形尺寸相同
	//resize(image2, image2, Size(image1.cols, image1.rows));
	////建立显示窗口
	//namedWindow(windowName);
	////在图像窗口上创建控制条
	//createTrackbar(trackBarName, windowName, &trackBarValue, trackBarMax, TrackBarFunc);
	//TrackBarFunc(0, 0);
	//waitKey();
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
	cout << "ThreadCount:" << nThreadCount << "\n"<< endl;
	vector<Mat> vecImage;
	importImages(imgDir, vecImage);
#ifdef TEST
	performanceTest(vecImage, windowName);
#else
	fusion_show(vecImage, windowName);
#endif // 

}

void fusion_show(vector<Mat>& vecImage, char* windowName)
{
	namedWindow(windowName, WINDOW_NORMAL);
	float rate = 0;
	vector<Mat>::iterator iter = vecImage.begin();
	Mat image1 = vecImage.at(0);
	Mat image2;
	Mat image;
	Size size = Size(image1.cols, image1.rows);
	imshow(windowName, image1);
	for (size_t idx = 0; ; ++idx)
	{
		idx %= vecImage.size() - 1;
		image2 = vecImage.at(idx);
		for (int nCnt = 100; nCnt > 0; --nCnt)
		{
			rate = nCnt / 100.0;
			resize(image2, image2, size);		//调整image2的大小与image1的大小一致
			imgFusionSSE(image1, rate, image2, image);		//图像融合
			imshow(windowName, image);
			int key = waitKey(10);					//每次渐变间隔10ms
			if (key == 'q' || key == 'Q' || key == 27)
			{
				break;
			}
		}
		image1 = image;
		waitKey(1000);						//每次渐变完等100ms，视觉感受更佳
	}
}
void performanceTest(vector<Mat>& vecImage, char* windowName)
{
	namedWindow(windowName, WINDOW_NORMAL);
	float rate = 0;
	vector<Mat>::iterator iter = vecImage.begin();
	Mat image1 = vecImage.at(0);
	Mat image2 = vecImage.at(1);
	Mat image;
	Size size = Size(image1.cols, image1.rows);
	imshow(windowName, image1);

	resize(image2, image2, size);		//调整image2的大小与image1的大小一致

	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;
	double average = 0.0;
	cout << "开始测试BIGLOOP方法: "<< endl;
	for (int nCnt = 100; nCnt > 0; --nCnt)
	{
		rate = nCnt / 100.0;
		QueryPerformanceFrequency(&nFreq);
		QueryPerformanceCounter(&nBeginTime);
#ifdef NORMAL
		imgFusionNormal(image1, rate, image2, image);		//图像融合
#endif	
#ifdef BIGLOOP
		imgFusionBigLoop(image1, rate, image2, image);		//图像融合
#endif 
#ifdef SSE
		imgFusionSSE(image1, rate, image2, image);		//图像融合
#endif
#ifdef SSE_OMP
		imgFusionSSE_OMP(image1, rate, image2, image);		//图像融合
#endif	

		QueryPerformanceCounter(&nEndTime);
		time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
		average += time;
		cout << "执行时间： " << time << endl;
		imshow(windowName, image);
		waitKey(10);
	}
	cout << "SSE_OMP平均执行时间: " << average / 100.0 << endl;
	waitKey(12313123);

}
