#include "pch.h"

cv::Mat TransBufferToMat(unsigned char* pBuffer, int nWidth, int nHeight, int nBandNum, int nBPB = 1)
{
	cv::Mat mDst;
	if (nBandNum == 4)
	{
		if (nBPB == 1)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_8UC4);
		}
		else if (nBPB == 2)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_16UC4);
		}
	}
	else if (nBandNum == 3)
	{
		if (nBPB == 1)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_8UC3);
		}
		else if (nBPB == 2)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_16UC3);
		}
	}
	else if (nBandNum == 1)
	{
		if (nBPB == 1)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_8UC1);
		}
		else if (nBPB == 2)
		{
			mDst = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_16UC1);
		}
	}

	for (int j = 0; j < nHeight; ++j)
	{
		unsigned char* data = mDst.ptr<unsigned char>(j);
		//unsigned char* pSubBuffer = pBuffer + (nHeight - 1 - j) * nWidth * nBandNum * nBPB;		//{modified by yuecui 2018/11/21
		unsigned char* pSubBuffer = pBuffer + j* nWidth * nBandNum * nBPB;			//}end modify yuecui 2018/11/21
		memcpy(data, pSubBuffer, nWidth * nBandNum * nBPB);
	}
	if (nBandNum == 1)
	{
		cv::cvtColor(mDst, mDst, CV_GRAY2BGR);
	}
	else if (nBandNum == 3)
	{
		cv::cvtColor(mDst, mDst, CV_RGB2BGR);
	}
	else if (nBandNum == 4)
	{
		cv::cvtColor(mDst, mDst, CV_RGBA2BGR);
	}

	return mDst;
}

int TransMatToBuffer(cv::Mat mSrc, unsigned char*& ppBuffer, int& nWidth, int& nHeight, int& nBandNum, int& nBPB, size_t& nMemSize)
{
	if (ppBuffer)
	{
		delete[] ppBuffer;
		ppBuffer = nullptr;
	}

	nWidth = mSrc.cols;
	//nWidth = ((nWidth + 3) / 4) * 4;
	nHeight = mSrc.rows;
	nBandNum = mSrc.channels();
	nBPB = (mSrc.depth() >> 1) + 1;

	//size_t nMemSize = nWidth * nHeight * nBandNum * nBPB;				// {modified by yue.cui 2018/11/21
	nMemSize = nWidth * nHeight * nBandNum * nBPB;						// }end modify 2018/11/21
	//这样可以改变外部*pBuffer的值
	ppBuffer = new uchar[nMemSize];
	memset(ppBuffer, 0, nMemSize);
	uchar* pT = ppBuffer;
	for (int j = 0; j < nHeight; ++j)
	{
		unsigned char* data = mSrc.ptr<unsigned char>(j);
		unsigned char* pSubBuffer = ppBuffer + (j)* nWidth * nBandNum * nBPB;
		memcpy(pSubBuffer, data, nWidth * nBandNum * nBPB);
	}

	return 0;
}
