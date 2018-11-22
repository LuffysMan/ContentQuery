#pragma once
#include "pch.h"
#include <direct.h>

using namespace cv;
using namespace std;

int importImages(char* dir, vector<Mat>& vecMat);
void listFiles(char* dir, vector<char*>& vecSzFilename);
int getWorkDir(char* szProgramPath);
double getSpecificTime();

cv::Mat TransBufferToMat(unsigned char* pBuffer, int nWidth, int nHeight, int nBandNum, int nBPB = 1);
int TransMatToBuffer(cv::Mat mSrc, unsigned char*& ppBuffer, int& nWidth, int& nHeight, int& nBandNum, int& nBPB, size_t& nMemSize);















