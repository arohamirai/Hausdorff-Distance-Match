/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* 文件名：hausdorff.h
* 摘要：寻找Hausdorff Distance度量的最匹配位置和最匹配位置的距离
* 当前版本：<V1.0>,<完成日期：2015年9月7日>，<HolaMirai>,<创建该文件>
* 历史记录：...
******************************************/


/*
* 类定义说明
*/
/********************************************
* CHausforff类
* CHausdorff类接受带匹配图、模板图的二值化图像，其他参数都带有默认值
* 使用match函数可以直接获取匹配结果
* hausdorff distance 采用部分距离法，对遮挡、噪声效果良好
* 匹配过程分为粗匹配和精匹配个过程
* 各函数已经解耦，可以单独调用某个函数以观察hausdorff distance 方法的各部分
********************************************/

#ifndef CHAUSDORFF_H
#define CHAUSDORFF_H

/*
* 宏定义
*/
// 最大距离
#define MAX_DISTANCE INT_MAX

#include "cv.h"

/*
*  类定义
*/
class CHausdorff
{
public:
	CHausdorff(){};
	CHausdorff(IplImage *srcImg, IplImage *modelImge, 
				int xInterval = 1, int yInterval = 1, 
				double frontRange = 0.8, double backRange = 0.7);
	~CHausdorff();

public:
	void setScanInterval_X(int xInterval);
	 void setScanInterval_Y(int yInterval);

	 void setFrontRange(double Range);
	 void setBackRange(double Range);

	 double computeDirDistance(CvPoint Apoints[], CvPoint Bpoints[], int numA, int numB, double Range);
	 double distance(CvPoint Apoints[], CvPoint Bpoints[], int numA, int numB);
	 
	 void match();
	 void drawMatch(IplImage *img = NULL);

private:
	int searchPoints(IplImage *img, CvPoint pPoints[]);

	void sort(double a[],int n);
	double meanDistance(double a[], int n);

	void createTable();
	void freeTable();

public:
	//记录匹配结果
	CvPoint m_matchPoint;
	double  m_matchDistance;

private:
	int m_xInterval;
	int m_yInterval;
	double m_frontRange;
	double m_backRange;

	double **m_disTable;

public:
	IplImage *m_srcImg;
	IplImage *m_modelImg;
};

#endif