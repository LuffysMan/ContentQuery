#pragma once
#include "pch.h"

cv::Mat TransBufferToMat(unsigned char* pBuffer, int nWidth, int nHeight, int nBandNum, int nBPB = 1);

int TransMatToBuffer(cv::Mat mSrc, unsigned char*& ppBuffer, int& nWidth, int& nHeight, int& nBandNum, int& nBPB, size_t& nMemSize);

