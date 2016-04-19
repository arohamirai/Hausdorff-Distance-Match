/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* 文件名：hausdorff.cpp
* 摘要：寻找Hausdorff Distance度量的最匹配位置和最匹配位置的距离
* 当前版本： v2.1.1,2016年1月1日，修正了CHausdorff类的构造函数未接收带匹配图像的错误
*			为Harris、SIFT、SURF特征点返回时添加std::move优化，VS2010下具体效果不明显
* 历史记录：V2.1, 2015年9月11日，HolaMirai, 添加 createTable()、freeTable()函数，添加平均hausdorff距离方法,添加注释
* 历史记录：V2.0, 2015年9月10日，HolaMirai, 调试完成该文件&添加meanDistance()函数
*		  ：V1.0, 2015年9月7日， HolaMirai, 创建该文件
******************************************/

/*
*              可改进处
* 改用Voronoi图进行Hausdorff距离计算
*
*/

#include "CHausdorff.h"
#include "highgui.h"

/*
* 函数名称：CHausdorff
* 函数功能：类构造函数
* 函数入口： 
* 输入参数： 待匹配图、模板二值化图像，粗匹配横、纵坐标扫描间隔 xInterval，yInterval 
*            前向距离因子 frontRange，后向距离因子 backRange
* 输出参数：无
* 返 回 值：
* 其它说明： 
*/
CHausdorff::CHausdorff(IplImage *srcImg, IplImage *modelImge, int xInterval, int yInterval, double frontRange, double backRange)
{
	m_srcImg = srcImg;
	m_modelImg = cvCloneImage(modelImge);

	m_frontRange = frontRange;
	m_backRange = backRange;
	m_xInterval = xInterval;
	m_yInterval = yInterval;

	createTable();
}

CHausdorff::~CHausdorff()
{
	freeTable();
	//cvReleaseImage(&m_srcImg);
	cvReleaseImage(&m_modelImg);

	
}

/*
* 函数名称：setScanInterval_X
* 函数功能：设置粗匹配X坐标扫描间隔
* 函数入口： 
* 输入参数： 粗匹配横坐标扫描间隔 xInterval
* 输出参数：无
* 返 回 值：void
* 其它说明： 
*/
void CHausdorff::setScanInterval_X(int xInterval)
{
	m_xInterval = xInterval;
}

/*
* 函数名称：setScanInterval_Y
* 函数功能：设置粗匹配Y坐标扫描间隔
* 函数入口： 
* 输入参数： 粗匹配横坐标扫描间隔 yInterval
* 输出参数：无
* 返 回 值：void
* 其它说明： 
*/
void CHausdorff::setScanInterval_Y(int yInterval)
{
	m_yInterval = yInterval;
}

/*
* 函数名称：setFrontRange
* 函数功能：设置前向匹配距离因子
* 函数入口： 
* 输入参数： 前向匹配距离因子 Range
* 输出参数：无
* 返 回 值：void
* 其它说明： 
*/
void CHausdorff::setFrontRange(double Range)
{
	m_frontRange = Range;
}

/*
* 函数名称：setFrontRange
* 函数功能：设置后向匹配距离因子
* 函数入口： 
* 输入参数： 前向匹配距离因子 Range
* 输出参数：无
* 返 回 值：void
* 其它说明： 
*/
void CHausdorff::setBackRange(double Range)
{
	m_backRange = Range;
}

/*
* 函数名称：searchPoints
* 函数功能：获得特征点坐标和特征点的个数
* 函数入口： 
* 输入参数： 灰度图 img， 接受特征点的数组指针
* 输出参数：无
* 返 回 值： 特征点个数
* 其它说明： 支持图片 ROI
*/
int CHausdorff::searchPoints(IplImage *img, CvPoint pPoints[])
{
	CvRect rect;
	if ( !img->roi)
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = img->width;
		rect.height = img->height;
	}
	else
	{
		rect.x = img->roi->xOffset;
		rect.y = img->roi->yOffset;
		rect.width = img->roi->width;
		rect.height = img->roi->height;
	}

	//统计特征点
	uchar *ptr;
	int num = 0;
	for (int i = rect.y; i < rect.y + rect.height; i++)
	{
		ptr = (uchar *) (img->imageData + i * img->widthStep);
		for (int j = rect.x; j < rect.x + rect.width; j++)
		{
			if (ptr[j] != 0)
			{
				num++;
			}
		}
	}

	if (num == 0)
	{
		return 0;
	}

	num = 0;
	for (int i = rect.y; i < rect.y + rect.height; i++)
	{
		ptr = (uchar *) (img->imageData + i * img->widthStep);
		for (int j = rect.x; j < rect.x + rect.width; j++)
		{
			if (ptr[j] != 0)
			{
				pPoints[num].x = j - rect.x;
				pPoints[num].y = i - rect.y;
				num++;
			}
		}
	}

	//结果返回
	return num;
}/*searchPoints()*/


/*
* 函数名称：match
* 函数功能：利用 Hausdorff distance 值来匹配图片
* 函数入口： 
* 输入参数：无
* 输出参数：最佳匹配位置 和最佳匹配距离
* 返 回 值： void
* 其它说明： 
*/
void CHausdorff::match()
{
	int ws, hs, wm, hm, numModel, numSrc;
	CvRect rect;
	double *matchDis = new double[100];  //假定最大会有100个最佳位置，这里并未实现，留着以后改进
	CvPoint *matchPoints = new CvPoint[100];
	double temp = MAX_DISTANCE;
	double dis = MAX_DISTANCE;
	int n = 0;  //只有一个最佳位置，本文的假设前提


	ws = m_srcImg->width;
	hs = m_srcImg->height;
	wm = m_modelImg->width;
	hm = m_modelImg->height;

	// 存储模板特征点和原图像特征点
	CvPoint *pMPoints = new CvPoint[wm * hm];
	CvPoint *pSPoints = new CvPoint[wm * hm];

	//获得模板特征点
	numModel = searchPoints(m_modelImg,pMPoints);

	if (numModel == 0)
	{
		printf_s("find no points in model");
		return;
	}

	//搜索最佳匹配
	rect.width = wm;
	rect.height = hm;
	
	// 粗匹配
	//#pragma omp parallel for firstprivate(m_srcImg)     //并行运算
	for (int i = 0; i < ws; i += m_xInterval)
	{
		rect.x = i;
		//#pragma omp parallel for 
		for (int j = 0; j < hs; j += m_yInterval)
		{
			rect.y = j;
			cvSetImageROI(m_srcImg,rect);
			numSrc = searchPoints(m_srcImg,pSPoints);
			if (numSrc == 0)
			{
				continue;
			}
			// 特征点数据量相差太大则忽略该位置，可以依具体情况调整参数
			if (double(numSrc)/numModel < 0.8 || (double)numSrc/numModel > 1.25)
			{
				continue;
			}

			temp = distance(pMPoints, pSPoints, numModel, numSrc);
			// min
			if (temp < dis)
			{
				dis = temp;
				matchDis[n] = temp;
				matchPoints[n].x = i;
				matchPoints[n].y = j;
			} 	
		}
	}

	//精匹配
	for (int i = matchPoints[n].x - m_xInterval; i < matchPoints[n].x + m_xInterval; i++)
	{
		rect.x = i;
		for (int j = matchPoints[n].y - m_yInterval; j < matchPoints[n].y + m_yInterval; j++)
		{
			rect.y = j;
			cvSetImageROI(m_srcImg,rect);
			numSrc = searchPoints(m_srcImg,pSPoints);
			if (numSrc == 0)
			{
				continue;
			}

			temp = distance(pMPoints, pSPoints, numModel, numSrc);
			// min
			if (temp < dis)
			{
				dis = temp;
				matchDis[n] = temp;
				matchPoints[n].x = i;
				matchPoints[n].y = j;
			} 	
		}
	}


	m_matchPoint = matchPoints[0];
	m_matchDistance = matchDis[0];

	cvResetImageROI(m_srcImg);
	drawMatch();

	delete []pMPoints;
	delete []pSPoints;
}/* match() */


/*
* 函数名称：distance
* 函数功能：获取某位置的Hausdorff distance值
* 函数入口： 
* 输入参数：两组特征点数组指针，特征点个数 numA, numB
* 输出参数：
* 返 回 值： 该位置的Hausdorff distance值
* 其它说明： 支持图片ROI
*/
double CHausdorff::distance(CvPoint Apoints[], CvPoint Bpoints[], int numA, int numB)
{
	double hAB;
	double hBA;
	hAB = computeDirDistance(Apoints, Bpoints, numA, numB, m_frontRange);
	hBA = computeDirDistance(Bpoints, Apoints, numB, numA, m_backRange);

	// H(A, B) = max(h(A, B), h(B,A))
	return std::max(hAB, hBA);	
}/*distance()*/

/*
* 函数名称：computeDirDistance
* 函数功能：计算单向 Hausdorff distance 值
* 函数入口： 
* 输入参数：两组特征点数组指针 Apoints, Bpoints, 特征点个数numA, int numB, 单向距离因子 Range
* 输出参数：
* 返 回 值： 单向 Hausdorff distance 值
* 其它说明：
*/
double CHausdorff::computeDirDistance(CvPoint Apoints[], CvPoint Bpoints[], int numA, int numB, double Range)
{
	double *disA = new double[numA];

	double temp;
	double aB = MAX_DISTANCE;
	for (int i = 0; i < numA; i++)
	{
		for (int j = 0; j < numB; j++)
		{
			temp = (Apoints[i].x - Bpoints[j].x) * (Apoints[i].x - Bpoints[j].x)
					+ (Apoints[i].y - Bpoints[j].y) * (Apoints[i].y - Bpoints[j].y);
			//或者使用查表法
			//temp = m_disTable[abs(Apoints[i].x - Bpoints[j].x)][abs(Apoints[i].y - Bpoints[j].y)];

			// while b in B, get min|| a - b ||
			aB = std::min(temp,aB);
		}
		disA[i] = aB;
		// 注意下面这条语句
		aB = MAX_DISTANCE;
	}

	sort(disA, numA);
	double dis;
	// hausdorff 距离改进版：部分hausdorff距离
	dis = disA[(int)(numA * Range) - 1];
	// 也可以尝试平均距离:meanDistance()，使用meanDistance()则不需要对距离数组排序
	// dis = meanDistance(disA, numA);

	delete []disA;
	return dis;
}/*computeDirDistance()*/

/*
* 函数名称：drawMatch()
* 函数功能：在待匹配图中画出匹配结果区域
* 函数入口： 
* 输入参数： img指针默认指向m_srcImg，可以将待匹配图像的jpeg格式图像指针传入，以使结果更加直观
* 输出参数：
* 返 回 值： void
* 其它说明： 
*/
void CHausdorff::drawMatch(IplImage *img)
{
	CvPoint pt;
	pt.x = m_matchPoint.x + m_modelImg->width;
	pt.y = m_matchPoint.y + m_modelImg->height;

	if (!img)
	{
		img = m_srcImg;
	}

	cvRectangle(img, m_matchPoint, pt, cvScalar(0,0,255));
	cvShowImage("resultMatch", img);
}

/*
* 函数名称：sort
* 函数功能：对数组进行升序排序
* 函数入口： 
* 输入参数：待排序数组指针及数组个数
* 输出参数：
* 返 回 值： void
* 其它说明： 
*/
void CHausdorff::sort(double a[],int n) //升序
{ 
	double temp;
	for(int i=0;i<n;i++) 
		for(int j=i+1;j<n;j++) 
		{ 
			if(a[i]>a[j]) 
			{  
				temp=a[i]; 
				a[i]=a[j]; 
				a[j]=temp; 
			} 
		} 
}

/*
* 函数名称：meanDistance
* 函数功能：对距离数组求平均值
* 函数入口： 
* 输入参数：距离数组指针及数组个数
* 输出参数：
* 返 回 值： 距离数组的平均值
* 其它说明： 
*/
double CHausdorff::meanDistance(double a[], int n)
{
	double sum = 0;

	for (int i = 0; i < n; i++)
	{
		sum += a[i];
	}
	sum /= n;
	return sum;
}

/*
* 函数名称：createTable
* 函数功能：创建距离变换表
* 函数入口： 
* 输入参数：NULL
* 输出参数：距离变换表的指针
* 返 回 值： void
* 其它说明： 
*/
void CHausdorff::createTable()
{
	int wm, hm, r;
	wm = m_modelImg->width;
	hm = m_modelImg->height;

	r = std::max(wm,hm);

	m_disTable = new double*[r];
	for (int i = 0; i < r; i++)
	{
		m_disTable[i] = new double[r];
	}

	for (int j = 0; j < r; j++)
	{
		for(int i = 0; i < r; i++)
		{
			m_disTable[j][i] = i*i + j*j;
			m_disTable[i][j] = m_disTable[j][i];
		}
	}
}

/*
* 函数名称：freeTable
* 函数功能：释放距离变换表内存
* 函数入口： 
* 输入参数：
* 输出参数：
* 返 回 值： void
* 其它说明： 
*/
void CHausdorff::freeTable()
{
	int wm, hm, r;
	wm = m_modelImg->width;
	hm = m_modelImg->height;

	r = std::max(wm,hm);

	for (int i = 0; i < r; i++)
	{
		delete []m_disTable[i];
	}
	delete []m_disTable;
}