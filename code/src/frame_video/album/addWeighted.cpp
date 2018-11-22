#include "pch.h"
#include "addWeighted.h"
#include "Mat_Buffer.h"
#include "xmmintrin.h"						//MMX/SSE/AVXָ���ͷ�ļ�
#include "nmmintrin.h"						//MMX/SSE/AVXָ���ͷ�ļ�


void myAddWeighted(cv::Mat src1, float alpha, cv::Mat src2, float beta, cv::Mat& dst)
{
	uchar* pBuffer1 = NULL;
	uchar* pBuffer2 = NULL;
	int nWidth = 0;
	int nHeight = 0;
	int nBandNum = 0;
	int nBPB = 0;
	size_t nMemSize = 0;
	//��opencv Mat����תΪ�޷���������д���
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	//�������㣬����ͼ���ں�
	uchar* pBuffer3 = new uchar[nMemSize]();					//()��Ĭ�ϳ�ʼ��Ϊ0
	//for (size_t i = 0; i < nMemSize; ++i)
	//{
	//	*(pBuffer3 + i) = uchar(*(pBuffer1 + i)*alpha + *(pBuffer2 + i)*beta);
	//}
/////////////////////SSE����//////////////////////////////////////////////////////////////
	int i = 0;
	uchar* pBuffer4 = new uchar[nMemSize]();					//()��Ĭ�ϳ�ʼ��Ϊ0
	uchar* pByte1 = new uchar[8]();
	uchar* pByte2 = new uchar[8]();
	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm9;
	__m128i xmm8 = _mm_setzero_si128();
	int fade = int(127*alpha);						//����Ȩ����תΪ���ͣ������������
	int nBlocks = nMemSize / 8;						//��8byte�ֿ�����
	int nRem = nMemSize % 8;						//ʣ��bytes
	for (int i = 0; i < nBlocks; ++i)
	{
		memcpy(pByte1, pBuffer1 + i * 8, 8);
		memcpy(pByte2, pBuffer2 + i * 8, 8);
		//��ʼ���Ĵ���
		xmm0 = _mm_set_epi16(fade, fade, fade, fade, fade, fade, fade, fade);//8��fade����װ��Ĵ���

		xmm1 = _mm_loadu_si128((__m128i*)pByte1);	//A���������ط���װ��Ĵ���
		xmm1 = _mm_unpacklo_epi8(xmm1, xmm8);		//8��һλ�������16λ
		xmm2 = _mm_loadu_si128((__m128i*)pByte2);	//B���������ط���װ��Ĵ���
		xmm2 = _mm_unpacklo_epi8(xmm2, xmm8);		//8��һλ�������16λ
		xmm1 = _mm_sub_epi16(xmm1, xmm2);			//A-B
		xmm1 = _mm_mullo_epi16(xmm1, xmm0);			//8��16λ�˷�
		xmm1 = _mm_srai_epi16(xmm1, 7);				//����7λ���൱�ڳ�127
		xmm1 = _mm_add_epi16(xmm1, xmm2);			//�ӷ�
		xmm1 = _mm_packus_epi16(xmm1, xmm8);		//16��һλ����Ϊ8λ
		_mm_storel_epi64((__m128i*)pByte1, xmm1);	//���������浽��ʱ�ڴ���
		memcpy(pBuffer4 + i * 8, pByte1, 8);		//������Ŀ��ͼ���ڴ�
	}
	for (i = nBlocks * 8; i < nMemSize; ++i)
	{
		*(pBuffer4 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}
//////////////////////////////////////////////////////////////////////////////////////////
	//��������ϵ��޷�������תΪopencv Mat����
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
//__m128i xmm8 = _mm_setzero_si128();							//��ʼ���Ĵ���
//xmm0 = _mm_set_epi16(fade, fade, fade, fade, fade, fade, fade, fade);
//uchar *pic1 = new uchar[8];
//uchar *pic2 = new uchar[8];
//memcpy_s(pic1, 8, pBuffer1, 8);
////8��fade����װ��Ĵ���
////�����ݼ��ص�MMX�Ĵ���
//size_t i;
//size_t nBlockWidth = 4;    // ���. SSE�Ĵ�����һ�δ���4��float
//size_t cntBlock = nMemSize / nBlockWidth;    // ����.
//size_t cntRem = nMemSize % nBlockWidth;    // ʣ������.
//const float* p1 = (const float*)pBuffer1;
//const float* p2 = (const float*)pBuffer2;
//float* p3 = (float*)pBuffer3;
//for (i = 0; i < cntBlock; ++i)
//{
//	mSrc1 = _mm_load_ps(p1 + i * 16);					//һ���Լ���128λ��16��byte����
//	mSrc2 = _mm_load_ps(p2 + i * 16);
//	mDst = _mm_sub_ps(mSrc1, mSrc2);
//	_mm_store_ps(p3 + i * 16, mDst);
//}
//for (i = nBlockWidth * 4; i < nMemSize; ++i)
//{
//	*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
//}