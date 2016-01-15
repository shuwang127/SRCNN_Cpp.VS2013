/* Program 	: Image Super-Resolution using deep Convolutional Neural Networks
* Author  	: Wang Shu
* Date		: Wed 22 Jul, 2015
* Descrip.	:
* */
#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <cv.h>
#include <highgui.h>
#include "Header.h"
#include "convolution1.h"
#include "convolution2.h"
#include "convolution3.h"

using namespace std;

#define IMAGE_WIDTH		960		//
#define IMAGE_HEIGHT	540		//
#define UP_SCALE		2
#define CONV1_FILTERS	64
#define CONV2_FILTERS	32

int _tmain(int argc, _TCHAR* argv[])
{
	/*��ȡ����ʾԭʼͼ��*/
	IplImage* pImgOrigin;
	pImgOrigin = cvLoadImage("Pictures\\test.jpg");
	//cvNamedWindow("OriginImage");
	//cvShowImage("OriginImage", pImgOrigin);
	cout << "Read Image Successfully..." << endl;
	
	/*��BGR�ռ�ת����YCbCr�ռ�*/
	IplImage* pImgYCbCr = cvCreateImage(cvGetSize(pImgOrigin), IPL_DEPTH_8U, 3);
	cvCvtColor(pImgOrigin, pImgYCbCr, CV_BGR2YCrCb);

	/*����Y-Cb-Cr��ͨ��������ʾYͨ��*/
	IplImage* pImgY = cvCreateImage(cvGetSize(pImgYCbCr), IPL_DEPTH_8U, 1);
	IplImage* pImgCb = cvCreateImage(cvGetSize(pImgYCbCr), IPL_DEPTH_8U, 1);
	IplImage* pImgCr = cvCreateImage(cvGetSize(pImgYCbCr), IPL_DEPTH_8U, 1);
	cvSplit(pImgYCbCr, pImgY, pImgCb, pImgCr, 0);
	cvSaveImage("Pictures\\Luma.bmp", pImgY);
	//cvNamedWindow("Luma");
	//cvShowImage("Luma", pImgY);
	cout << "Spliting the Y-Cb-Cr Channel..." << endl;

	/*��Yͨ������2��˫���β�ֵ������ʾͼƬ*/
	IplImage* pImg = cvCreateImage(CvSize(UP_SCALE * pImgY->width, UP_SCALE * pImgY->height), IPL_DEPTH_8U, 1);
	cvResize(pImgY, pImg, CV_INTER_CUBIC);
	cvSaveImage("Pictures\\LumaCubic.bmp", pImg);
	//cvNamedWindow("CubicInter");
	//cvShowImage("CubicInter", pImg);

	/*��һ�����*/
	IplImage* pImgConv1[CONV1_FILTERS];
	for (int i = 0; i < CONV1_FILTERS; i++)
	{
		pImgConv1[i] = cvCreateImage(cvGetSize(pImg), IPL_DEPTH_16U, 1);
		Convolution99(pImg, pImgConv1[i], weights_conv1_data[i], biases_conv1[i]);
		cout << "Convolution Layer I : " << setw(2) << i+1 << "/64 Cell Complete..." << endl;
	}
	//cvNamedWindow("Conv1");
	//cvShowImage("Conv1", pImgConv1[8]);
	//ShowImgData(pImgConv1[8]);
	cvSaveImage("Pictures\\Conv1.bmp", pImgConv1[8]);

	/*�ڶ������*/
	IplImage* pImgConv2[CONV2_FILTERS];
	for (int i = 0; i < CONV2_FILTERS; i++)
	{
		pImgConv2[i] = cvCreateImage(cvGetSize(pImg), IPL_DEPTH_16U, 1);
		Convolution11(pImgConv1, pImgConv2[i], weights_conv2_data[i], biases_conv2[i]);
		cout << "Convolution Layer II : " << setw(2) << i+1 << "/32 Cell Complete..." << endl;
	}
	//cvNamedWindow("Conv2");
	//cvShowImage("Conv2", pImgConv2[31]);
	//ShowImgData(pImgConv2[31]);
	cvSaveImage("Pictures\\Conv2.bmp", pImgConv2[31]);

	/*���������*/
	IplImage* pImgConv3 = cvCreateImage(cvGetSize(pImg), IPL_DEPTH_8U, 1);
	Convolution55(pImgConv2, pImgConv3, weights_conv3_data, biases_conv3);
	//cvNamedWindow("Conv3");
	//cvShowImage("Conv3", pImgConv3);
	//ShowImgData(pImgConv3);
	cvSaveImage("Pictures\\Conv3.bmp", pImgConv3);
	cout << "Convolution Layer III : 100% Complete..." << endl;

	/*�ϳ����*/
	IplImage* pImgCb2 = cvCreateImage(CvSize(UP_SCALE * pImgCb->width, UP_SCALE * pImgCb->height), IPL_DEPTH_8U, 1);
	cvResize(pImgCb, pImgCb2, CV_INTER_CUBIC);
	IplImage* pImgCr2 = cvCreateImage(CvSize(UP_SCALE * pImgCr->width, UP_SCALE * pImgCr->height), IPL_DEPTH_8U, 1);
	cvResize(pImgCr, pImgCr2, CV_INTER_CUBIC);
	IplImage* pImgYCbCrOut = cvCreateImage(cvGetSize(pImg), IPL_DEPTH_8U, 3);
	cvMerge(pImgConv3, pImgCb2, pImgCr2, 0, pImgYCbCrOut);
	IplImage* pImgBGROut = cvCreateImage(cvGetSize(pImg), IPL_DEPTH_8U, 3);
	cvCvtColor(pImgYCbCrOut, pImgBGROut, CV_YCrCb2BGR);
	//cvNamedWindow("Output");
	//cvShowImage("Output", pImgBGROut);
	cvSaveImage("Pictures\\Output.bmp", pImgBGROut);
	cout << "Reconstruction Complete..." << endl;

	cvMerge(pImg, pImgCb2, pImgCr2, 0, pImgYCbCrOut);
	cvCvtColor(pImgYCbCrOut, pImgBGROut, CV_YCrCb2BGR);
	//cvNamedWindow("Cubic");
	//cvShowImage("Cubic", pImgBGROut);
	cvSaveImage("Pictures\\Cubic.bmp", pImgBGROut);

	/*�ȴ�����*/
	cvWaitKey(0);

	/*ע��ͼ��*/
	cvReleaseImage(&pImgOrigin);
	cvReleaseImage(&pImgYCbCr);
	cvReleaseImage(&pImgY);
	cvReleaseImage(&pImgCb);
	cvReleaseImage(&pImgCr);

	cvReleaseImage(&pImg);
	for (int i = 0; i < CONV1_FILTERS; i++)
	{
		cvReleaseImage(&pImgConv1[i]);
	}
	for (int i = 0; i < CONV2_FILTERS; i++)
	{
		cvReleaseImage(&pImgConv2[i]);
	}
	cvReleaseImage(&pImgConv3);

	cvReleaseImage(&pImgCb2);
	cvReleaseImage(&pImgCr2);
	cvReleaseImage(&pImgYCbCrOut);
	cvReleaseImage(&pImgBGROut);

	/*ע������*/
	cvDestroyAllWindows();

	return (0);
}