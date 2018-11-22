#pragma once
#include "pch.h"
#include <direct.h>

using namespace cv;
using namespace std;

int importImages(char* dir, vector<Mat>& vecMat);
void listFiles(char* dir, vector<char*>& vecSzFilename);
int getWorkDir(char* szProgramPath);















