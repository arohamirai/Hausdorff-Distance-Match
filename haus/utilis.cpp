#include "opencv2/opencv.hpp"

using namespace cv;

void main1()
{
	CvCapture *capture = cvCreateCameraCapture(0);

	IplImage *img = cvQueryFrame(capture);

	while (cvWaitKey(30) != 'q')
	{
		img = cvQueryFrame(capture);
		cvShowImage("11",img);
	}
	cvSaveImage("water_m1.jpg",img);
}