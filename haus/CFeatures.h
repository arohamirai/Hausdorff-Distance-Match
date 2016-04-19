/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* 文件名：CFeatures.h
* 摘要：图像前置处理，用于获取特征点
* 当前版本：V1.0,2015年9月13日，HolaMirai,创建该文件
* 历史记录：...
******************************************/

/*
* 类定义说明
*/
/********************************************
* CFeatures类
* CFeatures类接受一个int值，表示特征点类型
* 使用getFeatures()函数可以得到特征点的二值化图像
* 一个工程只需定义一个CFeatures类的实例
********************************************/

#ifndef CFEATURES_H
#define CFEATURES_H

enum feature_type
{
	FEATURE_CANNY,		//特征点为CANNY边缘点
	FEATURE_HARRIS,		//特征点为Harris特征点
	FEATURE_SIFT,		//特征点为SIFT特征点
	FEATURE_SURF,		//特征点为SURF特征点
};

#include "cv.h"
#include <vector>

/*
* 类定义
*/
class CFeatures
{
public:
	CFeatures(int feature_type);
	~CFeatures();

public:
	void getFeatures(IplImage *grayImg, IplImage *featuresImg);
	void drawFeatures(const char *name, IplImage *img);

private:
	void vecPointsToImage(IplImage *featuresImg);

public:
	IplImage *m_featuresImg;
	IplImage *m_imgShow;
	std::vector<cv::KeyPoint> m_vecPoints;

private:
	int m_type;
};

#endif