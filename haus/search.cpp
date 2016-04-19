#include "opencv2/opencv.hpp"
#include "iostream"
#include "CHausdorff.h"

using namespace std;
using namespace cv;
void searchPoints(IplImage *img);
void main0()
{
	
	IplImage * IMG = cvLoadImage("sgt.jpg",0);
	IplImage *Edge = cvCloneImage(IMG);
	cvCanny(IMG,Edge,50,150);
	cvSetImageROI(Edge,cvRect(20,0,30,20));
	searchPoints(Edge);
	//cvRectangle(IMG,cvPoint(20,0),cvPoint(30,20),cvScalar(255,0,0));
	cvWaitKey(0);
	system("pause");
}

void searchPoints(IplImage *img)
{
	IplImage *roiImg;
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

	roiImg = cvCreateImage(cvSize(rect.width,rect.height),img->depth,1);
	uchar *ptr, *ptr1;
	for (int i = rect.y; i < rect.y + rect.height; i++)
	{
		ptr = (uchar *) (img->imageData + i * img->widthStep);
		ptr1 = (uchar *)(roiImg->imageData + (i - rect.y) * roiImg->widthStep);
		for (int j = rect.x; j < rect.x + rect.width; j++)
		{
			ptr1[j - rect.x] = ptr[j];
			if (ptr[j] != 0)
			{
				printf_s("%d ,% d\n",i,j);
			}
		}
	}
	cvSaveImage("search.jpg",roiImg);

}/*searchPoints()*/