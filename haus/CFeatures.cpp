/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* 文件名：CFeatures.cpp
* 摘要：图像前置处理，用于获取特征点
* 当前版本：V2.0,2015年9月14日，HolaMirai,完成四种特征点获取方法
* 历史记录：V1.0,2015年9月13日，HolaMirai,创建该文件
******************************************/

#include "CFeatures.h"
#include "highgui.h"
#include <opencv2/nonfree/nonfree.hpp>  
using namespace cv;

/***********************************************/

/*
* 函数名称：CFeatures
* 函数功能：CFeatures类构造函数
* 函数入口： 
* 输入参数： 需要获取的特征点的特征点类型，值为enum feature_type其中一个
* 输出参数：无
* 返 回 值：
* 其它说明： 
*/
CFeatures::CFeatures(int feature_type)
{
	m_type = feature_type;
	initModule_nonfree();//if use SIFT or SURF
}

/*
* 函数名称：~CFeatures
* 函数功能：CFeatures类析构函数
* 函数入口： 
* 输入参数： 
* 输出参数：
* 返 回 值：
* 其它说明： 
*/
CFeatures::~CFeatures()
{

}

/*
* 函数名称：getFeatures
* 函数功能：获取特征点
* 函数入口： 
* 输入参数： 待搜索图像的灰度图 grayImg 指针, 接受特征点的灰度图像指针 featuresImg, featuresImg与 grayImg尺寸相同
* 输出参数：特征点的灰度图像 featuresImg
* 返 回 值：
* 其它说明： 
*/
void CFeatures::getFeatures(IplImage *grayImg, IplImage *featuresImg)
{
	switch (m_type)
	{
	case FEATURE_CANNY:
		{
			assert(grayImg->width == featuresImg->width &&
				grayImg->height == featuresImg->height &&
				grayImg->nChannels == grayImg->nChannels);

			cvCanny(grayImg,featuresImg,50,150);
			break;
		}
	case FEATURE_HARRIS:
		{
			//std::vector<cv::KeyPoint> vecPoints;
			Ptr<FeatureDetector> detector = FeatureDetector::create( "HARRIS" );    //特征点寻找
			detector->detect(grayImg, m_vecPoints);
			//m_vecPoints = std::move(vecPoints);
			vecPointsToImage(featuresImg);
			break;
		}

	case FEATURE_SIFT:
		{
			std::vector<cv::KeyPoint> vecPoints;
			Ptr<FeatureDetector> detector = FeatureDetector::create( "SIFT" );    //特征点寻找
			detector->detect(grayImg, vecPoints);
			m_vecPoints = std::move(vecPoints);
			vecPointsToImage(featuresImg);
			break;
		}

	case FEATURE_SURF:
		{
			std::vector<cv::KeyPoint> vecPoints;
			Ptr<FeatureDetector> detector = FeatureDetector::create( "SURF" );    //特征点寻找
			detector->detect(grayImg, vecPoints);
			m_vecPoints = std::move(vecPoints);
			vecPointsToImage(featuresImg);
			break;
		}
	default:
		break;
	}
} /*getFeatures()*/

/*
* 函数名称：vecPointsToImage
* 函数功能：将vector类型的特征点变换成 IplImage 图像类型
* 函数入口： 
* 输入参数： 特征点向量m_vecPoints, 接受特征点的灰度图像指针 featuresImg
* 输出参数：特征点的灰度图像 featuresImg
* 返 回 值：
* 其它说明： 
*/
void CFeatures::vecPointsToImage(IplImage *featuresImg)
{
	vector<KeyPoint>::iterator it;
	vector<KeyPoint>::iterator end_it = m_vecPoints.end();

	cvZero(featuresImg);

	int w,h;
	uchar *ptr;
	for (it = m_vecPoints.begin(); it != end_it; it++)
	{
		w= (int)it->pt.x;
		h = (int)it->pt.y;
		ptr = (uchar *)(featuresImg->imageData + h*featuresImg->widthStep + w);
		// 特征点置255，其他置0
		*ptr = 255;
	}
	ptr = NULL;
}/*vecPointsToImage()*/

/*
* 函数名称：drawFeatures
* 函数功能：画出特征点
* 函数入口： 
* 输入参数： 显示窗口 name, 在哪副图像上画出特征点的图像指针 img
* 输出参数：
* 返 回 值：void
* 其它说明：画出的是最近一次处理的图片的特征点
*/
void CFeatures::drawFeatures(const char *name, IplImage *img)
{
	IplImage *m_imgShow = cvCloneImage(img);

	vector<KeyPoint>::iterator it;
	vector<KeyPoint>::iterator end_it = m_vecPoints.end();

	CvPoint center;
	for (it = m_vecPoints.begin(); it != end_it; it++)
	{
		center.x = (int)it->pt.x;
		center.y  = (int)it->pt.y;

		cvCircle(m_imgShow,center,3,cvScalar(rand()%255,rand()%255,rand()%255));
	}
	cvShowImage(name,m_imgShow);
	cvReleaseImage(&m_imgShow);
}/*drawFeatures()*/