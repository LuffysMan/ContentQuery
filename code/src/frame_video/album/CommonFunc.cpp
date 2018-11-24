#include "pch.h"
#include "CommonFunc.h"

int importImages(char * dir, vector<Mat>& vecMat)
{
	//获得图片名称列表
	vector<char*> vecSzFilename;
	listFiles(dir, vecSzFilename);
	for (size_t idx = 0; idx < vecSzFilename.size(); ++idx)
	{
		vecMat.push_back(imread(vecSzFilename.at(idx)));
	}
	for (vector<char*>::iterator iter = vecSzFilename.begin(); iter != vecSzFilename.end(); ++iter)
	{
		if (NULL != *iter)
		{
			delete[] * iter;
		}
	}
	vecSzFilename.clear();
	return 0;
}

void listFiles(char * dir, vector<char*>& vecSzFilename)
{
	char szDir[_MAX_PATH] = { 0 };
	sprintf_s(szDir, _MAX_PATH, "%s%s", dir, "*.*");			//遍历前加上通配符
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(szDir, &findData);    // 查找目录中的第一个文件
	if (handle == -1)
	{
		cout << "Failed to find first file!\n";
		return;
	}

	do
	{
		if (findData.attrib & _A_SUBDIR
			|| strcmp(findData.name, ".") == 0
			|| strcmp(findData.name, "..") == 0
			)    // 是否是子目录并且不为"."或".."
		{
			//cout << findData.name << "\t<dir>\n";
		}
		else
		{
			//cout << findData.name << "\t" << findData.size << endl;
			char* pBuffer = new char[_MAX_PATH]();
			sprintf_s(pBuffer, _MAX_PATH, "%s%s", dir, findData.name);
			vecSzFilename.push_back(pBuffer);
		}
	} while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件

	//cout << "Done!\n";
	_findclose(handle);    // 关闭搜索句柄
}
//枚举指定路径下文件夹
void listDir(char * dir, vector<char*>& vecSzDir)
{
	char szDir[_MAX_PATH] = { 0 };
	sprintf_s(szDir, _MAX_PATH, "%s%s", dir, "*.*");			//遍历前加上通配符
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(szDir, &findData);    // 查找目录中的第一个文件
	if (handle == -1)
	{
		cout << "Failed to find first file!\n";
		return;
	}
	do
	{
		if (findData.attrib & _A_SUBDIR && 
			strcmp(findData.name, ".") != 0 &&
			strcmp(findData.name, "..") != 0)// 只关注子目录
		{
			cout << findData.name << "\t<dir>\n";
			char* pBuffer = new char[_MAX_PATH]();
			sprintf_s(pBuffer, _MAX_PATH, "%s%s%s", dir, findData.name, "\\");
			vecSzDir.push_back(pBuffer);
		}
		else
		{
			////cout << findData.name << "\t" << findData.size << endl;
			//char* pBuffer = new char[_MAX_PATH]();
			//sprintf_s(pBuffer, _MAX_PATH, "%s%s", dir, findData.name);
			//vecSzFilename.push_back(pBuffer);
		}
	} while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件
	_findclose(handle);    // 关闭搜索句柄
}

void releasePointVec(vector<char*>& vecBuffer)
{
	for (vector<char*>::iterator iter = vecBuffer.begin(); iter != vecBuffer.end(); ++iter)
	{
		if (NULL != *iter)
		{
			delete[] * iter;
		}
	}
	vecBuffer.clear();
}

int getWorkDir(char* szProgramPath)
{
	char szPath[_MAX_PATH] = { 0 };
	char szDrive[_MAX_DRIVE] = { 0 };
	char szDir[_MAX_DIR] = { 0 };
	char szFname[_MAX_FNAME] = { 0 };
	char szExt[_MAX_EXT] = { 0 };

	GetModuleFileNameA(NULL, szPath, sizeof(szPath));
	//ZeroMemory(szProgramPath, strlen(szProgramPath));
	_splitpath_s(szPath, szDrive, szDir, szFname, szExt);
	sprintf_s(szProgramPath, _MAX_PATH, "%s%s", szDrive, szDir);

	return 0;
}

double getSpecificTime()
{
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);
	Sleep(1000);
	QueryPerformanceCounter(&nEndTime);
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	return time;
}

cv::Mat TransBufferToMat(unsigned char* pBuffer, int nWidth, int nHeight, int nBandNum, int nBPB)
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
		unsigned char* pSubBuffer = pBuffer + j * nWidth * nBandNum * nBPB;			//}end modify yuecui 2018/11/21
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