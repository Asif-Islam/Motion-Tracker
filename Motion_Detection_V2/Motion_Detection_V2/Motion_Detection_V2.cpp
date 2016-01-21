/* MOTIION DETECTION PROGRAM

This is a program in construction with the intention to not only detect motion but also recognize what type of motion is occuring.
The motion detection is done using a basic frame differencing algorithm which thresholding on pixel values, following by an accumulation of pixels that were
deemed as "motion"
The type of motion is to be recognized by first "Training" the computer of the type of motion with a working data set first to deveop a model first

*/

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
	/* The following piece of code implements the webcam of the computer. Ideally, the frame differencing algorithm will run between frames of the
		video as it gets frames from the webcam. For now, the below code runs the webcam and also stores the first image seems into an IplImage data structure.
		In upcoming updates, I plan to bring the frame-differencing algorithm down below at line 146 into the logic of the frame retrieval.
	*/

	//Open the camera to the default, i.e. the computer webcam
	VideoCapture cap(0); 

	//If the webcam cannot be accessed, break from the code
	
	if (!cap.isOpened()) {
		return -1;
	}


	//Instantiate the Output Windows
	cvNamedWindow("Output", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("PreviousFrame", CV_WINDOW_AUTOSIZE);

	//Initialize frame counter
	int video_frame_count = 0;

	
	//Infinite loop until any key is pressed, continuously retreving frames

	while (1)
	{
		Mat frame;
		cap >> frame;																//Using >> operator, retreive a frame from the videocapture									
	
		if (video_frame_count == 0) {												
			IplImage* firstframe = cvCloneImage(&(IplImage)frame);					//clone the mat structure into an iplimage data structure
			cvShowImage("PreviousFrame", firstframe);									//Display the first frame of the image
		}

		video_frame_count++;														//Increment the frame counter by one

		imshow("Output", frame);													//Display the video output


		if (waitKey(30) >= 0) {														//If any key is pressed in a delay of 30 milliscends, then EXIT
			break;
		}
	}

		
	//Initialize some basic frame counting variables
	
	int frame_count = 0;
	int saved_image_count = 0;


	//both set of 5 frames contains a collection of images of me swiping my hand down.

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


	//Store the loaded images into two array of IplImages
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

	
	



	
/*
The following code is currently for testing purposes; It is creating an Average over multiple frames. To do this, an IplImage named Divider is created
We Set the dividers 3 channels equal to 2 respectively. Using the cvDiv function, which divides the pixels from one image to another, we accumulate the image
into an IplImage "AvgImg", which was initialized to zero using cvZero
*/

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
	
/*
The following code repeats twice. This is not intentional, but only for testing purposes to have two sets of data to compare to. This algorithm essentially maps
difference of frames and store what the computer considers as "motion" as black and static as white. Afterwards, the code will compile all the frame differences and
put them together into one image. This means that any pixel that was regarded as "motion" in any of the frame differences will be considered black. However, for testing purposes,
each image of a frame difference between adjacent images have been saved to the project as well as the accumulated frame difference image
*/


	//Instantiate an Output Image, a Image that has the lower threshold for pixels and an Image that has the higher threshold for pixels
	IplImage *OutputA = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 3);
	IplImage *Low = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 3);
	IplImage *High = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 3);

	
	
	//Instantiate 3 channels for each of the above
	IplImage *LowChannels[3];
	IplImage *HighChannels[3];
	IplImage *OutputChannels[3];

	
	
	//Loop through and intialize the channels; Note that they are initialized to ONE channel in the last parameter
	for (int i = 0; i < 3; i++) {
		LowChannels[i] = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
		HighChannels[i] = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
		OutputChannels[i] = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
	}
	


	//Two Images that will be used to accumulate the motion
	IplImage *Mask = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
	IplImage *MaskTwo = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);

	
	
	//The final product image, that is, this image holds the accumulation of all frame differences
	IplImage *Sum = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);

	
	
	//Set values of Sum, Low and High. Sum is set to 0, Low is set to an arbitrarily value of 20 as the lower threshold value
	cvZero(Sum);
	cvSet(Low, cvScalar(20, 20, 20), NULL);
	cvSet(High, cvScalar(255, 255, 255), NULL);

	
	

	for (int i = 1; i < FRAME_COUNT; i++) {
		
		cvZero(Mask);																					//Set both Mask and MaskTwo to ZERO
		cvZero(MaskTwo);
		cvAbsDiff(&frames[0], &frames[i], OutputA);														//Frame difference the first and ith frame
		cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_OPEN, 2);											//Apply morphology algorithm that will remove general noise from image
		cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_CLOSE, 2);

		cvSplit(OutputA, OutputChannels[0], OutputChannels[1], OutputChannels[2], 0);					//Split Output to three channels
		cvSplit(High, HighChannels[0], HighChannels[1], HighChannels[2], 0);							//Split High to three channels
		cvSplit(Low, LowChannels[0], LowChannels[1], LowChannels[2], 0);								//Split Low to three channels

		cvInRange(OutputChannels[0], LowChannels[0], HighChannels[0], Mask);							//Mark pixels in range and store them into Mask 
		cvOr(Mask, MaskTwo, Mask);																		//Or operator Mask, which was used last line, and MaskTwo which is zero right now

		cvInRange(OutputChannels[1], LowChannels[1], HighChannels[2], MaskTwo);							//Mark pixels in range and store them into MaskTwo
		cvOr(Mask, MaskTwo, Mask);																		//Or operator Mask and MaskTwo again to overlay the results onto Mask

		cvInRange(OutputChannels[2], LowChannels[2], HighChannels[2], MaskTwo);							//Mark pixels in range store them into maskTwo
		cvOr(Mask, MaskTwo, Mask);																		//Or operator Mask and MaskTwo again to overlap results onto Mask

		cvOr(Sum, Mask, Sum);																			//CvOr the result of Mask into Sum, which accumulates all i masks

		cvSaveImage(outputnames[i - 1].c_str(), Mask);													//Save the Image, testing purposes
	}

	cvSubRS(Sum, 255, Sum);																				//Invert black and white using scalar subtraction
	cvSaveImage("SumFinal.jpg", Sum, 0);																//Save Image


	
	
	
/*
The following code repeats twice. This is not intentional, but only for testing purposes to have two sets of data to compare to. This algorithm essentially maps
difference of frames and store what the computer considers as "motion" as black and static as white. Afterwards, the code will compile all the frame differences and
put them together into one image. This means that any pixel that was regarded as "motion" in any of the frame differences will be considered black. However, for testing purposes,
each image of a frame difference between adjacent images have been saved to the project as well as the accumulated frame difference image
*/

	IplImage *Sum2 = cvCreateImage(FRAME_SIZE, FRAME_DEPTH, 1);
	cvZero(Sum2);


	for (int i = 1; i < FRAME_COUNT; i++) {
		cvZero(Mask);													
		cvZero(MaskTwo);
		cvAbsDiff(&frames2[0], &frames2[i], OutputA);						
		cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_OPEN, 2);			
		cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_CLOSE, 2);

		cvSplit(OutputA, OutputChannels[0], OutputChannels[1], OutputChannels[2], 0);					
		cvSplit(High, HighChannels[0], HighChannels[2], HighChannels[2], 0);
		cvSplit(Low, LowChannels[0], LowChannels[1], LowChannels[2], 0);

		cvInRange(OutputChannels[0], LowChannels[0], HighChannels[0], Mask);							
		cvOr(Mask, MaskTwo, Mask);										

		cvInRange(OutputChannels[1], LowChannels[1], HighChannels[2], MaskTwo);
		cvOr(Mask, MaskTwo, Mask);

		cvInRange(OutputChannels[2], LowChannels[2], HighChannels[2], MaskTwo);
		cvOr(Mask, MaskTwo, Mask);

		cvOr(Sum2, Mask, Sum2);											

		cvSaveImage(outputnames2[i - 1].c_str(), Mask);
	}

	cvSubRS(Sum2, 255, Sum2);												
	cvSaveImage("SumFinal2.jpg", Sum2, 0);

	
	
//-----------------------------------------------

//Output the two Images using the Window instantiated initially


	cvShowImage("Output", Sum);
	cvWaitKey(0);
	cvShowImage("Output", Sum2);
	cvWaitKey(0);



//Being Clean about memory! deleting all of the pointers

	cvReleaseImage(&imageA);
	cvReleaseImage(&imageB);
	cvReleaseImage(&imageC);
	cvReleaseImage(&imageD);
	cvReleaseImage(&imageE);

	cvReleaseImage(&imageA2);
	cvReleaseImage(&imageB2);
	cvReleaseImage(&imageC2);
	cvReleaseImage(&imageD2);
	cvReleaseImage(&imageE2);

	cvReleaseImage(&AvgImg);
	cvReleaseImage(&Divider);

	cvReleaseImage(&OutputA);
	cvReleaseImage(&High);
	cvReleaseImage(&Low);

	for (int i = 0; i < 3; i++) {
		cvReleaseImage(&OutputChannels[i]);
		cvReleaseImage(&HighChannels[i]);
		cvReleaseImage(&LowChannels[i]);
	}

	cvReleaseImage(&Mask);
	cvReleaseImage(&MaskTwo);
	cvReleaseImage(&Sum);
	cvReleaseImage(&Sum2);

}