#include "pch.h"
#include "CommonFunc.h"

int importImages(char * dir, vector<Mat>& vecMat)
{
	//���ͼƬ�����б�
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
	sprintf_s(szDir, _MAX_PATH, "%s%s", dir, "*.*");			//����ǰ����ͨ���
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(szDir, &findData);    // ����Ŀ¼�еĵ�һ���ļ�
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
			)    // �Ƿ�����Ŀ¼���Ҳ�Ϊ"."��".."
			cout << findData.name << "\t<dir>\n";
		else
		{
			cout << findData.name << "\t" << findData.size << endl;
			char* pBuffer = new char[_MAX_PATH]();
			sprintf_s(pBuffer, _MAX_PATH, "%s%s", dir, findData.name);
			vecSzFilename.push_back(pBuffer);
		}
	} while (_findnext(handle, &findData) == 0);    // ����Ŀ¼�е���һ���ļ�

	cout << "Done!\n";
	_findclose(handle);    // �ر��������
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
