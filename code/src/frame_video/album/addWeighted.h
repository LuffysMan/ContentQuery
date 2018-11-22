#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

void imgFusionSSE(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat& dst);
void imgFusionSSE_OMP(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat& dst);
void imgFusionNormal(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat& dst);
void imgFusionBigLoop(cv::Mat src1, float alpha, cv::Mat src2, cv::Mat& dst);














