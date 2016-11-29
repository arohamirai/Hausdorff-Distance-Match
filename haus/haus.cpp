/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* 文件名：haus.cpp
* 摘要：包含main函数的主文件，用于测试CHausdorff类, CFeatures类
* 历史记录：V2.0,2015年9月14日,添加CFeatures类，并使用CFeatures类来获取特征点
* 历史记录：V1.0,2015年9月7日，HolaMirai,创建该文件
******************************************/

#include "opencv2/opencv.hpp"
#include "iostream"
#include "CHausdorff.h"
#include "CFeatures.h"
#include "windows.h"

using namespace std;
using namespace cv;

//void main()
//{
//	char modelName[] = "m1.jpg";
//
//	CvCapture *capture = cvCreateCameraCapture(0);
//	IplImage *srcColor = cvQueryFrame(capture);
//	IplImage *modelColor = cvLoadImage(modelName,1);
//
//	IplImage *srcGray = cvCreateImage(cvGetSize(srcColor),IPL_DEPTH_8U,1);
//	IplImage *modelGray = cvLoadImage(modelName,0);
//
//	IplImage *srcFeat = cvCreateImage(cvGetSize(srcColor),IPL_DEPTH_8U,1);
//	IplImage *modelFeat = cvCreateImage(cvGetSize(modelColor),IPL_DEPTH_8U,1);
//
//	CFeatures features(FEATURE_HARRIS);
//	features.getFeatures(modelGray,modelFeat);
//
//	char winSrc[] = "src";
//	cvNamedWindow("src",1);
//
//	CHausdorff haus(NULL,modelFeat,8,8);
//
//	for (int i = 0; i < 30; i++)
//	{
//		srcColor = cvQueryFrame(capture);
//		cvShowImage(winSrc,srcColor);
//		cvWaitKey(30);
//	}
//
//	while (cvWaitKey(30) != 'q')
//	{
//		srcColor = cvQueryFrame(capture);
//
//		features.getFeatures(srcGray,srcFeat);
//		features.drawFeatures("src",srcColor);
//
//		cvCvtColor(srcColor,srcGray,CV_BGR2GRAY);
//		features.getFeatures(srcGray,srcFeat);
//
//		haus.m_srcImg = srcFeat;
//
//		DWORD start_t = GetTickCount(); 
//		haus.match();
//		DWORD end_t = GetTickCount();
//
//		cout<<"匹配耗时："<<end_t - start_t<<"ms"<<endl;
//
//		haus.drawMatch(srcColor);
//
//
//	}
//
//
//
//	
//
//	
//
//	cvWaitKey(0);
//	//system("pause");
//	cvReleaseImage(&srcColor);
//	cvReleaseImage(&modelColor);
//	cvReleaseImage(&srcGray);
//	cvReleaseImage(&modelGray);
//	cvReleaseImage(&srcFeat);
//	cvReleaseImage(&modelFeat);
//	cvDestroyAllWindows();
//
//}






////单张图片匹配
void main()
{
	char srcName[] = "ls.jpg";
	char modelName[] = "lsm0.jpg";

	IplImage *srcColor = cvLoadImage(srcName,1);
	IplImage *modelColor = cvLoadImage(modelName,1);

	IplImage *srcGray = cvLoadImage(srcName,0);
	IplImage *modelGray = cvLoadImage(modelName,0);

	//cvShowImage("src",srcColor);
	//cvShowImage("model",modelColor);

	IplImage *srcFeat = cvCreateImage(cvGetSize(srcGray),IPL_DEPTH_8U,1);
	IplImage *modelFeat = cvCreateImage(cvGetSize(modelGray),IPL_DEPTH_8U,1);

	CFeatures features(FEATURE_HARRIS);

	features.getFeatures(srcGray,srcFeat);
	features.drawFeatures("src",srcColor);
	features.getFeatures(modelGray,modelFeat);
	features.drawFeatures("model",modelColor);
	//cvWaitKey(0);

	CHausdorff haus(srcFeat,modelFeat,8,8);

	DWORD start_t = GetTickCount(); 
	haus.match();
	DWORD end_t = GetTickCount();

	cout<<"匹配耗时："<<end_t - start_t<<"ms"<<endl;

	haus.drawMatch(srcColor);

	cvWaitKey(0);
	//system("pause");
	cvReleaseImage(&srcColor);
	cvReleaseImage(&modelColor);
	cvReleaseImage(&srcGray);
	cvReleaseImage(&modelGray);
	cvReleaseImage(&srcFeat);
	cvReleaseImage(&modelFeat);
	cvDestroyAllWindows();

}
























//void main()
//{
//	char srcName[] = "w2.jpg";
//	char modelName[] = "water_m.jpg";
//
//	IplImage *srcColor = cvLoadImage(srcName,1);
//	IplImage *modelColor = cvLoadImage(modelName,1);
//
//	IplImage *srcGray = cvLoadImage(srcName,0);
//	IplImage *modelGray = cvLoadImage(modelName,0);
//
//	cvShowImage("src",srcColor);
//	cvShowImage("model",modelColor);
//
//	IplImage *srcEdge ;//= cvCloneImage(srcGray);
//	IplImage *modelEdge = cvCloneImage(modelGray);
//
//	cvCanny(srcGray,srcEdge,50,150);
//	cvCanny(modelGray,modelEdge,50,150);
//
//	CHausdorff haus(srcEdge,modelEdge,8,8);
//
//	DWORD start_t = GetTickCount(); 
//	haus.match();
//	DWORD end_t = GetTickCount();
//
//	cout<<"匹配耗时："<<end_t - start_t<<"ms"<<endl;
//
//	haus.drawMatch(srcColor);
//
//	cvWaitKey(0);
//	system("pause");
//}
