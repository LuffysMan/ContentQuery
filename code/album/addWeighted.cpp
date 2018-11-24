#include "pch.h"
#include "addWeighted.h"
#include "CommonFunc.h"
#include "xmmintrin.h"						//MMX/SSE/AVXָ���ͷ�ļ�
#include "nmmintrin.h"						//MMX/SSE/AVXָ���ͷ�ļ�


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
	//��opencv Mat����תΪ�޷���������д���
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
#pragma endregion
///////////////�������㣬����ͼ���ں�////////SSE����//////////////////////////////////////////////////////////////
	uchar* pBuffer3 = new uchar[nMemSize]();					//()��Ĭ�ϳ�ʼ��Ϊ0
	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm9;
	__m128i xmm8 = _mm_setzero_si128();
	int fade = int(127*alpha);						//����Ȩ����תΪ���ͣ������������
	xmm0 = _mm_set_epi16(fade, fade, fade, fade, fade, fade, fade, fade);	//8��fade����װ��Ĵ���
	int nBlocks = nMemSize / 8;						//��8byte�ֿ�����
	int nRem = nMemSize % 8;						//ʣ��bytes
	for (int i = 0; i < nBlocks; ++i)				//��ʼ���Ĵ���
	{
		xmm1 = _mm_loadu_si128((__m128i*)(pBuffer1 + i * 8));	//A���������ط���װ��Ĵ���
		xmm1 = _mm_unpacklo_epi8(xmm1, xmm8);		//8��һλ�������16λ
		//xmm2 = _mm_loadu_si128((__m128i*)pByte2);	//B���������ط���װ��Ĵ���
		xmm2 = _mm_loadu_si128((__m128i*)(pBuffer2 + i * 8));	//B���������ط���װ��Ĵ���
		xmm2 = _mm_unpacklo_epi8(xmm2, xmm8);		//8��һλ�������16λ
		xmm1 = _mm_sub_epi16(xmm1, xmm2);			//A-B
		xmm1 = _mm_mullo_epi16(xmm1, xmm0);			//8��16λ�˷�
		xmm1 = _mm_srai_epi16(xmm1, 7);				//����7λ���൱�ڳ�127
		xmm1 = _mm_add_epi16(xmm1, xmm2);			//�ӷ�
		xmm1 = _mm_packus_epi16(xmm1, xmm8);		//16��һλ����Ϊ8λ
		_mm_storel_epi64((__m128i*)(pBuffer3 + i * 8), xmm1);	//���������浽��ʱ�ڴ���
	}
	for (int i = nBlocks * 8; i < nMemSize; ++i)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}
//////////////////////////////////////////////////////////////////////////////////////////
#pragma region MyRegion
		//��������ϵ��޷�������תΪopencv Mat����
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
	//��opencv Mat����תΪ�޷���������д���
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	//�������㣬����ͼ���ں�
/////////////////////SSE����//////////////////////////////////////////////////////////////
	int i = 0;
	uchar* pBuffer3 = new uchar[nMemSize]();					//()��Ĭ�ϳ�ʼ��Ϊ0
	uchar* pByte1 = new uchar[8]();
	uchar* pByte2 = new uchar[8]();
	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm9;
	__m128i xmm8 = _mm_setzero_si128();
	int fade = int(127 * alpha);						//����Ȩ����תΪ���ͣ������������
	xmm0 = _mm_set_epi16(fade, fade, fade, fade, fade, fade, fade, fade);	//8��fade����װ��Ĵ���
	int nBlocks = nMemSize / 8;						//��8byte�ֿ�����
	int nRem = nMemSize % 8;						//ʣ��bytes
#pragma	omp parallel for num_threads(8)
	for (int i = 0; i < nBlocks; ++i)
	{
		//memcpy(pByte1, pBuffer1 + i * 8, 8);
		//memcpy(pByte2, pBuffer2 + i * 8, 8);
		//��ʼ���Ĵ���
		//xmm1 = _mm_loadu_si128((__m128i*)pByte1);	//A���������ط���װ��Ĵ���
		xmm1 = _mm_loadu_si128((__m128i*)(pBuffer1 + i * 8));	//A���������ط���װ��Ĵ���
		xmm1 = _mm_unpacklo_epi8(xmm1, xmm8);		//8��һλ�������16λ
		//xmm2 = _mm_loadu_si128((__m128i*)pByte2);	//B���������ط���װ��Ĵ���
		xmm2 = _mm_loadu_si128((__m128i*)(pBuffer2 + i * 8));	//B���������ط���װ��Ĵ���
		xmm2 = _mm_unpacklo_epi8(xmm2, xmm8);		//8��һλ�������16λ
		xmm1 = _mm_sub_epi16(xmm1, xmm2);			//A-B
		xmm1 = _mm_mullo_epi16(xmm1, xmm0);			//8��16λ�˷�
		xmm1 = _mm_srai_epi16(xmm1, 7);				//����7λ���൱�ڳ�127
		xmm1 = _mm_add_epi16(xmm1, xmm2);			//�ӷ�
		xmm1 = _mm_packus_epi16(xmm1, xmm8);		//16��һλ����Ϊ8λ
		//_mm_storel_epi64((__m128i*)pByte1, xmm1);	//���������浽��ʱ�ڴ���
		_mm_storel_epi64((__m128i*)(pBuffer3 + i * 8), xmm1);	//���������浽��ʱ�ڴ���
		//memcpy(pBuffer4 + i * 8, pByte1, 8);		//������Ŀ��ͼ���ڴ�
	}
	for (i = nBlocks * 8; i < nMemSize; ++i)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}
	//////////////////////////////////////////////////////////////////////////////////////////
		//��������ϵ��޷�������תΪopencv Mat����
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
#pragma region MyRegion
	uchar* pBuffer1 = NULL;
	uchar* pBuffer2 = NULL;
	int nWidth = 0;
	int nHeight = 0;
	int nBandNum = 0;
	int nBPB = 0;
	size_t nMemSize = 0;
#pragma endregion
	//��opencv Mat����תΪ�޷���������д���
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	//�������㣬����ͼ���ں�
	uchar* pBuffer3 = new uchar[nMemSize]();								//()��Ĭ�ϳ�ʼ��Ϊ0
	for (int i = 0; i < nMemSize; ++i)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}
	dst = TransBufferToMat(pBuffer3, nWidth, nHeight, nBandNum, nBPB);		//��������ϵ��޷�������תΪopencv Mat����
#pragma region MyRegion
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

void imgFusionNormal_OMP(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat & dst)
{
#pragma region MyRegion
	uchar* pBuffer1 = NULL;
	uchar* pBuffer2 = NULL;
	int nWidth = 0;
	int nHeight = 0;
	int nBandNum = 0;
	int nBPB = 0;
#pragma endregion
	size_t nMemSize = 0;
	//��opencv Mat����תΪ�޷���������д���
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	//�������㣬����ͼ���ں�
	uchar* pBuffer3 = new uchar[nMemSize]();								//()��Ĭ�ϳ�ʼ��Ϊ0
#pragma	omp parallel for num_threads(8)
	for (int i = 0; i < nMemSize; ++i)
	{
		*(pBuffer3 + i) = uchar((*(pBuffer1 + i) - *(pBuffer2 + i))*alpha + *(pBuffer2 + i));
	}

	dst = TransBufferToMat(pBuffer3, nWidth, nHeight, nBandNum, nBPB);		//��������ϵ��޷�������תΪopencv Mat����
#pragma region MyRegion
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

void imgFusionLongStep(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat & dst)
{
#pragma region MyRegion
	uchar* pBuffer1 = NULL;
	uchar* pBuffer2 = NULL;
	int nWidth = 0;
	int nHeight = 0;
	int nBandNum = 0;
	int nBPB = 0;
#pragma endregion
	size_t nMemSize = 0;
	//��opencv Mat����תΪ�޷���������д���
	TransMatToBuffer(src1, pBuffer1, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	TransMatToBuffer(src2, pBuffer2, nWidth, nHeight, nBandNum, nBPB, nMemSize);
	//�������㣬����ͼ���ں�
	uchar* pBuffer3 = new uchar[nMemSize]();								//()��Ĭ�ϳ�ʼ��Ϊ0
	int nBlocks = nMemSize / 8;						//��8byte�ֿ�����
	int nBlocksX = nBlocks * 8;
	int nRem = nMemSize % 8;						//ʣ��bytes
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
	dst = TransBufferToMat(pBuffer3, nWidth, nHeight, nBandNum, nBPB);		//��������ϵ��޷�������תΪopencv Mat����
#pragma region MyRegion
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



