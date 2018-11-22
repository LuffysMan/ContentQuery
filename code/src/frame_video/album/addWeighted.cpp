#include "pch.h"
#include "addWeighted.h"
#include "CommonFunc.h"
#include "xmmintrin.h"						//MMX/SSE/AVX指令函数头文件
#include "nmmintrin.h"						//MMX/SSE/AVX指令函数头文件


void imgFusionSSE(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat& dst)
{
#pragma region MyRegion
	uchar* pBuffer1 = NULL;
	uchar* pBuffer2 = NULL;
	int nWidth = 0;
	int nHeight = 0;
	int nBandNum = 0;
	int nBPB = 0;
	size_t nMemSize = 0;
	//将opencv Mat类型转为无符号数组进行处理
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
#pragma endregion
///////////////矩阵运算，两个图像融合////////SSE加速//////////////////////////////////////////////////////////////
	uchar* pBuffer3 = new uchar[nMemSize]();					//()表默认初始化为0
	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm9;
	__m128i xmm8 = _mm_setzero_si128();
	int fade = int(127*alpha);						//将加权因子转为整型，方便后续运算
	xmm0 = _mm_set_epi16(fade, fade, fade, fade, fade, fade, fade, fade);	//8个fade因子装入寄存器
	int nBlocks = nMemSize / 8;						//按8byte分块数量
	int nRem = nMemSize % 8;						//剩余bytes
	for (int i = 0; i < nBlocks; ++i)				//初始化寄存器
	{
		xmm1 = _mm_loadu_si128((__m128i*)(pBuffer1 + i * 8));	//A的两个像素分量装入寄存器
		xmm1 = _mm_unpacklo_epi8(xmm1, xmm8);		//8个一位解紧缩至16位
		//xmm2 = _mm_loadu_si128((__m128i*)pByte2);	//B的两个像素分量装入寄存器
		xmm2 = _mm_loadu_si128((__m128i*)(pBuffer2 + i * 8));	//B的两个像素分量装入寄存器
		xmm2 = _mm_unpacklo_epi8(xmm2, xmm8);		//8个一位解紧缩至16位
		xmm1 = _mm_sub_epi16(xmm1, xmm2);			//A-B
		xmm1 = _mm_mullo_epi16(xmm1, xmm0);			//8个16位乘法
		xmm1 = _mm_srai_epi16(xmm1, 7);				//右移7位，相当于除127
		xmm1 = _mm_add_epi16(xmm1, xmm2);			//加法
		xmm1 = _mm_packus_epi16(xmm1, xmm8);		//16个一位紧缩为8位
		_mm_storel_epi64((__m128i*)(pBuffer3 + i * 8), xmm1);	//把运算结果存到临时内存中
	}
	for (int i = nBlocks * 8; i < nMemSize; ++i)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}
//////////////////////////////////////////////////////////////////////////////////////////
#pragma region MyRegion
		//将处理完毕的无符号数组转为opencv Mat类型
	dst = TransBufferToMat(pBuffer3, nWidth, nHeight, nBandNum, nBPB);
	if (NULL != pBuffer1)
	{
		delete[] pBuffer1;
	}
	if (NULL != pBuffer2)
	{
		delete[] pBuffer2;
	}
	if (NULL != pBuffer3)
	{
		delete[] pBuffer3;
	}
#pragma endregion
}

void imgFusionSSE_OMP(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat & dst)
{
	uchar* pBuffer1 = NULL;
	uchar* pBuffer2 = NULL;
	int nWidth = 0;
	int nHeight = 0;
	int nBandNum = 0;
	int nBPB = 0;
	size_t nMemSize = 0;
	//将opencv Mat类型转为无符号数组进行处理
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	//矩阵运算，两个图像融合
/////////////////////SSE加速//////////////////////////////////////////////////////////////
	int i = 0;
	uchar* pBuffer3 = new uchar[nMemSize]();					//()表默认初始化为0
	uchar* pByte1 = new uchar[8]();
	uchar* pByte2 = new uchar[8]();
	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm9;
	__m128i xmm8 = _mm_setzero_si128();
	int fade = int(127 * alpha);						//将加权因子转为整型，方便后续运算
	xmm0 = _mm_set_epi16(fade, fade, fade, fade, fade, fade, fade, fade);	//8个fade因子装入寄存器
	int nBlocks = nMemSize / 8;						//按8byte分块数量
	int nRem = nMemSize % 8;						//剩余bytes
#pragma	omp parallel for
	for (int i = 0; i < nBlocks; ++i)
	{
		//memcpy(pByte1, pBuffer1 + i * 8, 8);
		//memcpy(pByte2, pBuffer2 + i * 8, 8);
		//初始化寄存器
		//xmm1 = _mm_loadu_si128((__m128i*)pByte1);	//A的两个像素分量装入寄存器
		xmm1 = _mm_loadu_si128((__m128i*)(pBuffer1 + i * 8));	//A的两个像素分量装入寄存器
		xmm1 = _mm_unpacklo_epi8(xmm1, xmm8);		//8个一位解紧缩至16位
		//xmm2 = _mm_loadu_si128((__m128i*)pByte2);	//B的两个像素分量装入寄存器
		xmm2 = _mm_loadu_si128((__m128i*)(pBuffer2 + i * 8));	//B的两个像素分量装入寄存器
		xmm2 = _mm_unpacklo_epi8(xmm2, xmm8);		//8个一位解紧缩至16位
		xmm1 = _mm_sub_epi16(xmm1, xmm2);			//A-B
		xmm1 = _mm_mullo_epi16(xmm1, xmm0);			//8个16位乘法
		xmm1 = _mm_srai_epi16(xmm1, 7);				//右移7位，相当于除127
		xmm1 = _mm_add_epi16(xmm1, xmm2);			//加法
		xmm1 = _mm_packus_epi16(xmm1, xmm8);		//16个一位紧缩为8位
		//_mm_storel_epi64((__m128i*)pByte1, xmm1);	//把运算结果存到临时内存中
		_mm_storel_epi64((__m128i*)(pBuffer3 + i * 8), xmm1);	//把运算结果存到临时内存中
		//memcpy(pBuffer4 + i * 8, pByte1, 8);		//拷贝到目标图像内存
	}
	for (i = nBlocks * 8; i < nMemSize; ++i)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}
	//////////////////////////////////////////////////////////////////////////////////////////
		//将处理完毕的无符号数组转为opencv Mat类型
	dst = TransBufferToMat(pBuffer3, nWidth, nHeight, nBandNum, nBPB);
	if (NULL != pBuffer1)
	{
		delete[] pBuffer1;
	}
	if (NULL != pBuffer2)
	{
		delete[] pBuffer2;
	}
	if (NULL != pBuffer3)
	{
		delete[] pBuffer3;
	}


}

void imgFusionNormal(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat & dst)
{
	uchar* pBuffer1 = NULL;
	uchar* pBuffer2 = NULL;
	int nWidth = 0;
	int nHeight = 0;
	int nBandNum = 0;
	int nBPB = 0;
	size_t nMemSize = 0;
	//将opencv Mat类型转为无符号数组进行处理
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	//矩阵运算，两个图像融合
	uchar* pBuffer3 = new uchar[nMemSize]();								//()表默认初始化为0
	for (int i = 0; i < nMemSize; ++i)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}

	dst = TransBufferToMat(pBuffer3, nWidth, nHeight, nBandNum, nBPB);		//将处理完毕的无符号数组转为opencv Mat类型
	if (NULL != pBuffer1)
	{
		delete[] pBuffer1;
	}
	if (NULL != pBuffer2)
	{
		delete[] pBuffer2;
	}
	if (NULL != pBuffer3)
	{
		delete[] pBuffer3;
	}
}

void imgFusionBigLoop(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat & dst)
{
	uchar* pBuffer1 = NULL;
	uchar* pBuffer2 = NULL;
	int nWidth = 0;
	int nHeight = 0;
	int nBandNum = 0;
	int nBPB = 0;
	size_t nMemSize = 0;
	//将opencv Mat类型转为无符号数组进行处理
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	//矩阵运算，两个图像融合
	uchar* pBuffer3 = new uchar[nMemSize]();								//()表默认初始化为0
	int nBlocks = nMemSize / 8;						//按8byte分块数量
	int nBlocksX = nBlocks * 8;
	int nRem = nMemSize % 8;						//剩余bytes
	for (int i = 0; i < nBlocksX;  i+= 8)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
		*(pBuffer3 + i+1) = uchar((*(pBuffer1 + i + 1) - *(pBuffer2 + i + 1))*alpha + *(pBuffer2 + i + 1));
		*(pBuffer3 + i+2) = uchar((*(pBuffer1 + i + 2) - *(pBuffer2 + i + 2))*alpha + *(pBuffer2 + i + 2));
		*(pBuffer3 + i+3) = uchar((*(pBuffer1 + i + 3) - *(pBuffer2 + i + 3))*alpha + *(pBuffer2 + i + 3));
		*(pBuffer3 + i+4) = uchar((*(pBuffer1 + i + 4) - *(pBuffer2 + i + 4))*alpha + *(pBuffer2 + i + 4));
		*(pBuffer3 + i+5) = uchar((*(pBuffer1 + i + 5) - *(pBuffer2 + i + 5))*alpha + *(pBuffer2 + i + 5));
		*(pBuffer3 + i+6) = uchar((*(pBuffer1 + i + 6) - *(pBuffer2 + i + 6))*alpha + *(pBuffer2 + i + 6));
		*(pBuffer3 + i+7) = uchar((*(pBuffer1 + i + 7) - *(pBuffer2 + i + 7))*alpha + *(pBuffer2 + i + 7));

	}
	for (int i = nBlocksX; i < nMemSize; ++i)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}
	dst = TransBufferToMat(pBuffer3, nWidth, nHeight, nBandNum, nBPB);		//将处理完毕的无符号数组转为opencv Mat类型
	if (NULL != pBuffer1)
	{
		delete[] pBuffer1;
	}
	if (NULL != pBuffer2)
	{
		delete[] pBuffer2;
	}
	if (NULL != pBuffer3)
	{
		delete[] pBuffer3;
	}
}



