// Motion_Detection_V2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <iostream>
#include "String"

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{

	int frame_count = 0;							//Count the number of frames being processed
	int saved_image_count = 0;
	String names[10] = { "input1.jpg", "input2.jpg", "input3.jpg", "input4.jpg", "input5.jpg",
		"input6.jpg", "input7.jpg", "input8.jpg", "input9.jpg", "input10.jpg"
	};

	//Take in video input
	
	/*VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;


	cvNamedWindow("Video Input", CV_WINDOW_AUTOSIZE);				//Window to display input
	Mat previousframe;
	Mat frame;
	for (;;)
	{

		cap >> frame;												//Read in frame from video input

		if (frame_count == 0) {
			previousframe = frame.clone();
		}
		else {
			frame_count++;												//Increment frame_count by 1;
			cout << frame_count << endl;
			imshow("Video Input", frame);								//Output Image to window!

			if (waitKey(30) >= 0) {										//Wait 30ms for input to exit
				break;
			}
		}
	}


	imshow("Video Input", previousframe);*/


	cvWaitKey(0);

	/*	int frame_count = 0;
	VideoCapture cap(0); // open the default camera
	if(!cap.isOpened())  // check if we succeeded
	return -1;


	cvNamedWindow("AA",CV_WINDOW_AUTOSIZE);
	Mat edges;
	for(;;)
	{
	Mat frame;
	cap >> frame; // get a new frame from camera
	cvtColor(frame, edges, COLOR_BGR2GRAY);
	GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
	Canny(edges, edges, 0, 30, 3);
	//IplImage* image = cvCloneImage(&(IplImage)frame);
	//cvShowImage("AA", image);
	imshow("edges", edges);
	if (waitKey(30) >= 0) {
	break;
	}
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
	*/
	//Load Images saved in folder

	IplImage *imageA = cvLoadImage("imageG1.jpg");
	IplImage *imageB = cvLoadImage("imageG2.jpg");
	IplImage *imageC = cvLoadImage("imageG3.jpg");
	IplImage *imageD = cvLoadImage("imageG4.jpg");
	IplImage *imageE = cvLoadImage("imageG5.jpg");

	IplImage *imageA2 = cvLoadImage("imageG6.jpg");
	IplImage *imageB2 = cvLoadImage("imageG7.jpg");
	IplImage *imageC2 = cvLoadImage("imageG8.jpg");
	IplImage *imageD2 = cvLoadImage("imageG9.jpg");
	IplImage *imageE2 = cvLoadImage("imageG10.jpg");

	//Some constants, names of output images, framecount, frame size, frame depth

	String outputnames[4] = { "O1.jpg", "O2.jpg", "O3.jpg", "O4.jpg" };
	String outputnames2[4] = { "O5.jpg", "O6.jpg", "O7.jpg", "O8.jpg" };
	const int FRAME_COUNT = 5;
	const CvSize FRAME_SIZE = cvSize(imageA->width, imageA->height);
	const int FRAME_DEPTH = imageA->depth;
	int add_count = 0;

	//Store frames into an array

	IplImage frames[FRAME_COUNT];
	frames[0] = *imageA;
	frames[1] = *imageB;
	frames[2] = *imageC;
	frames[3] = *imageD;
	frames[4] = *imageE;

	IplImage frames2[FRAME_COUNT];
	frames2[0] = *imageA2;
	frames2[1] = *imageB2;
	frames2[2] = *imageC2;
	frames2[3] = *imageD2;
	frames2[4] = *imageE2;

	//Output Windows
	cvNamedWindow("Output", CV_WINDOW_AUTOSIZE);


	//COMPUTE THE AVERAGE IMAGE OVER MULTIPLE FRAMES
	//---------------------------------------------------------------------------------

	IplImage *AvgImg = cvCreateImage(cvSize(imageA->width, imageA->height), imageA->depth, imageA->nChannels);

	cvZero(AvgImg);


	IplImage *Divider = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 3);

	cvSet(Divider, CvScalar(2, 2, 2), NULL);

	for (int i = 0; i < FRAME_COUNT; i++) {
		cvDiv(&frames[i], Divider, &frames[i]);
		cvDiv(AvgImg, Divider, AvgImg);
		cvAdd(AvgImg, &frames[i], AvgImg);
		add_count++;
	}

	//---------------------------------------------------------------------------------
	//Motion Modelling Algorithm

	IplImage *OutputA = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 3);
	IplImage *Low = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 3);
	IplImage *High = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 3);

	IplImage *LowChannels[3];
	IplImage *HighChannels[3];
	IplImage *OutputChannels[3];

	for (int i = 0; i < 3; i++) {
		LowChannels[i] = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
		HighChannels[i] = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
		OutputChannels[i] = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
	}



	IplImage *Mask = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
	IplImage *MaskTwo = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);

	IplImage *Sum = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);

	//Set values of Sum, Low and High
	cvZero(Sum);
	cvSet(Low, cvScalar(20, 20, 20), NULL);
	cvSet(High, cvScalar(255, 255, 255), NULL);


	for (int i = 1; i < FRAME_COUNT; i++) {
		cvZero(Mask);													//Set both Mask and MaskTwo to ZERO
		cvZero(MaskTwo);
		cvAbsDiff(&frames[0], &frames[i], OutputA);						//Frame difference the first and ith frame
		cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_OPEN, 2);			//Apply MorphologyEX to remove noise from image
		cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_CLOSE, 2);

		cvSplit(OutputA, OutputChannels[0], OutputChannels[1], OutputChannels[2], 0);					//Split Output, High and Low into three separate channels
		cvSplit(High, HighChannels[0], HighChannels[1], HighChannels[2], 0);
		cvSplit(Low, LowChannels[0], LowChannels[1], LowChannels[2], 0);

		cvInRange(OutputChannels[0], LowChannels[0], HighChannels[0], Mask);							//Mark sufficient changes in pixel values
		cvOr(Mask, MaskTwo, Mask);										//CvOr to store into Mask

		cvInRange(OutputChannels[1], LowChannels[1], HighChannels[2], MaskTwo);
		cvOr(Mask, MaskTwo, Mask);

		cvInRange(OutputChannels[2], LowChannels[2], HighChannels[2], OutputChannels[2]);
		cvOr(Mask, MaskTwo, Mask);

		cvOr(Sum, Mask, Sum);											//CvOr the result of Mask into Sum, which accumulates all i masks

		cvSaveImage(outputnames[i - 1].c_str(), Mask);
	}

	cvSubRS(Sum, 255, Sum);												//Invert black and white using cvSubRS
	cvSaveImage("SumFinal.jpg", Sum, 0);								//Save Image


	///-----------------------------
	IplImage *Sum2 = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
	cvZero(Sum2);
	cvSet(Low, cvScalar(25, 25, 25), NULL);
	cvSet(High, cvScalar(255, 255, 255), NULL);


	for (int i = 1; i < FRAME_COUNT; i++) {
		cvZero(Mask);													//Set both Mask and MaskTwo to ZERO
		cvZero(MaskTwo);
		cvAbsDiff(&frames2[0], &frames2[i], OutputA);						//Frame difference the first and ith frame
		cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_OPEN, 2);			//Apply MorphologyEX to remove noise from image
		cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_CLOSE, 2);

		cvSplit(OutputA, OutputChannels[0], OutputChannels[1], OutputChannels[2], 0);					//Split Output, High and Low into three separate channels
		cvSplit(High, HighChannels[0], HighChannels[2], HighChannels[2], 0);
		cvSplit(Low, LowChannels[0], LowChannels[1], LowChannels[2], 0);

		cvInRange(OutputChannels[0], LowChannels[0], HighChannels[0], Mask);							//Mark sufficient changes in pixel values
		cvOr(Mask, MaskTwo, Mask);										//CvOr to store into Mask

		cvInRange(OutputChannels[1], LowChannels[1], HighChannels[2], MaskTwo);
		cvOr(Mask, MaskTwo, Mask);

		cvInRange(OutputChannels[2], LowChannels[2], HighChannels[2], OutputChannels[2]);
		cvOr(Mask, MaskTwo, Mask);

		cvOr(Sum2, Mask, Sum2);											//CvOr the result of Mask into Sum, which accumulates all i masks

		cvSaveImage(outputnames2[i - 1].c_str(), Mask);
	}

	cvSubRS(Sum2, 255, Sum2);												//Invert black and white using cvSubRS
	cvSaveImage("SumFinal2.jpg", Sum2, 0);


	//Output-----------------------------------------------

	cvShowImage("Output", Sum);
	cvWaitKey(0);
	cvShowImage("Output", Sum2);
	cvWaitKey(0);

	//Output-----------------------------------------------

}