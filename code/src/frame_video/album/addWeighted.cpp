#include "pch.h"
#include "addWeighted.h"
#include "Mat_Buffer.h"
#include "xmmintrin.h"						//MMX/SSE/AVX指令函数头文件
#include "nmmintrin.h"						//MMX/SSE/AVX指令函数头文件


void myAddWeighted(cv::Mat src1, float alpha, cv::Mat src2, float beta, cv::Mat& dst)
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
	uchar* pBuffer3 = new uchar[nMemSize]();					//()表默认初始化为0
	//for (size_t i = 0; i < nMemSize; ++i)
	//{
	//	*(pBuffer3 + i) = uchar(*(pBuffer1 + i)*alpha + *(pBuffer2 + i)*beta);
	//}
/////////////////////SSE加速//////////////////////////////////////////////////////////////
	int i = 0;
	uchar* pBuffer4 = new uchar[nMemSize]();					//()表默认初始化为0
	uchar* pByte1 = new uchar[8]();
	uchar* pByte2 = new uchar[8]();
	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm9;
	__m128i xmm8 = _mm_setzero_si128();
	int fade = int(127*alpha);						//将加权因子转为整型，方便后续运算
	int nBlocks = nMemSize / 8;						//按8byte分块数量
	int nRem = nMemSize % 8;						//剩余bytes
	for (int i = 0; i < nBlocks; ++i)
	{
		memcpy(pByte1, pBuffer1 + i * 8, 8);
		memcpy(pByte2, pBuffer2 + i * 8, 8);
		//初始化寄存器
		xmm0 = _mm_set_epi16(fade, fade, fade, fade, fade, fade, fade, fade);//8个fade因子装入寄存器

		xmm1 = _mm_loadu_si128((__m128i*)pByte1);	//A的两个像素分量装入寄存器
		xmm1 = _mm_unpacklo_epi8(xmm1, xmm8);		//8个一位解紧缩至16位
		xmm2 = _mm_loadu_si128((__m128i*)pByte2);	//B的两个像素分量装入寄存器
		xmm2 = _mm_unpacklo_epi8(xmm2, xmm8);		//8个一位解紧缩至16位
		xmm1 = _mm_sub_epi16(xmm1, xmm2);			//A-B
		xmm1 = _mm_mullo_epi16(xmm1, xmm0);			//8个16位乘法
		xmm1 = _mm_srai_epi16(xmm1, 7);				//右移7位，相当于除127
		xmm1 = _mm_add_epi16(xmm1, xmm2);			//加法
		xmm1 = _mm_packus_epi16(xmm1, xmm8);		//16个一位紧缩为8位
		_mm_storel_epi64((__m128i*)pByte1, xmm1);	//把运算结果存到临时内存中
		memcpy(pBuffer4 + i * 8, pByte1, 8);		//拷贝到目标图像内存
	}
	for (i = nBlocks * 8; i < nMemSize; ++i)
	{
		*(pBuffer4 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}
//////////////////////////////////////////////////////////////////////////////////////////
	//将处理完毕的无符号数组转为opencv Mat类型
	dst = TransBufferToMat(pBuffer4, nWidth, nHeight, nBandNum, nBPB);
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
	if (NULL != pBuffer4)
	{
		delete[] pBuffer4;
	}	
	if (NULL != pByte1)
	{
		delete[] pByte1;
	}
	if (NULL != pByte2)
	{
		delete[] pByte2;
	}

}




//__m128 mSrc1 = _mm_setzero_ps();
//__m128 mSrc2 = _mm_setzero_ps();
//__m128 mDst = _mm_setzero_ps();
//__m128 xishu1 = _mm_setzero_ps();
//__m128 xishu2 = _mm_setzero_ps();
//int fade = 127;
//__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm9;
//__m128i xmm8 = _mm_setzero_si128();							//初始化寄存器
//xmm0 = _mm_set_epi16(fade, fade, fade, fade, fade, fade, fade, fade);
//uchar *pic1 = new uchar[8];
//uchar *pic2 = new uchar[8];
//memcpy_s(pic1, 8, pBuffer1, 8);
////8个fade因子装入寄存器
////把数据加载到MMX寄存器
//size_t i;
//size_t nBlockWidth = 4;    // 块宽. SSE寄存器能一次处理4个float
//size_t cntBlock = nMemSize / nBlockWidth;    // 块数.
//size_t cntRem = nMemSize % nBlockWidth;    // 剩余数量.
//const float* p1 = (const float*)pBuffer1;
//const float* p2 = (const float*)pBuffer2;
//float* p3 = (float*)pBuffer3;
//for (i = 0; i < cntBlock; ++i)
//{
//	mSrc1 = _mm_load_ps(p1 + i * 16);					//一次性加载128位共16个byte数据
//	mSrc2 = _mm_load_ps(p2 + i * 16);
//	mDst = _mm_sub_ps(mSrc1, mSrc2);
//	_mm_store_ps(p3 + i * 16, mDst);
//}
//for (i = nBlockWidth * 4; i < nMemSize; ++i)
//{
//	*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
//}