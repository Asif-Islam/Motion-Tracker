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
	/* The following code prepares the webcam and reads in frames, frame differencing between adjacent frames. The result is a black and white image of
	only notable changes in pixel values. The following step is approach a machine learning algorithm, likely using Classification, and marking features of the motion
	to try and train the computer to recognize what TYPE of motion
	*/



	
	VideoCapture cap(0);																	//Initialize the videocapture to the default webcam															


	
	if (!cap.isOpened()) {																	//If the webcam cannot be accessed, break from the code
		return -1;
	}


	
	cvNamedWindow("Output", CV_WINDOW_AUTOSIZE);											//Instantiate the Output Windows




	int video_frame_count = 0;																//Initialize frame counter


	//TEMPORARY - Get Depth 
	IplImage *imageX = cvLoadImage("imageG1.jpg");
	cout << imageX->depth << endl;

	
	

	Mat frame;																				//Initialize the input frame of type Mat
	IplImage* previous_frame;																//Initialize IplImage to hold the previous frame
	IplImage* current_frame;																//Initialize IplImage to hold current frame



	

	const CvSize FRAME_SIZE = cvSize(640, 480);												//Two constants, size and depth, when defining all of our images!
	const int FRAME_DEPTH = imageX->depth;





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




//*******************************************************************************************************************
	//Take in video input and run the frame differencing algorithm!



	while (1)
	{

		cap >> frame;																			//Using >> operator, retreive a frame from the videocapture																				
		current_frame = cvCloneImage(&(IplImage)frame);											//clone the mat structure into an iplimage data structure
																								
		video_frame_count++;																	//Increment the frame counter by one
	

		if (video_frame_count > 1 && video_frame_count % 2 == 0) {
			cvZero(Sum);
			cvZero(Mask);																		//Set both Sum, Mask and MaskTwo equal to 0
			cvZero(MaskTwo);
			cvAbsDiff(current_frame, previous_frame, OutputA);									//Frame difference the first and previous frame
			cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_OPEN, 2);								//Apply morphology algorithm that will remove general noise from image
			cvMorphologyEx(OutputA, OutputA, 0, 0, CV_MOP_CLOSE, 2);

			cvSplit(OutputA, OutputChannels[0], OutputChannels[1], OutputChannels[2], 0);		//Split Output to three channels
			cvSplit(High, HighChannels[0], HighChannels[1], HighChannels[2], 0);				//Split High to three channels
			cvSplit(Low, LowChannels[0], LowChannels[1], LowChannels[2], 0);					//Split Low to three channels

			cvInRange(OutputChannels[0], LowChannels[0], HighChannels[0], Mask);				//Mark pixels in range and store them into Mask 
			cvOr(Mask, MaskTwo, Mask);															//Or operator Mask, which was used last line, and MaskTwo which is zero right now

			cvInRange(OutputChannels[1], LowChannels[1], HighChannels[2], MaskTwo);				//Mark pixels in range and store them into MaskTwo
			cvOr(Mask, MaskTwo, Mask);															//Or operator Mask and MaskTwo again to overlay the results onto Mask

			cvInRange(OutputChannels[2], LowChannels[2], HighChannels[2], MaskTwo);				//Mark pixels in range store them into maskTwo
			cvOr(Mask, MaskTwo, Mask);															//Or operator Mask and MaskTwo again to overlap results onto Mask

			cvOr(Sum, Mask, Sum);																//CvOr the result of Mask into Sum, which accumulates all i masks




			cvSubRS(Sum, 255, Sum);																//Invert Black and White
		}

		previous_frame = cvCloneImage(&(IplImage)frame);										//Store the previous frame as the current frame

		cvShowImage("Output", Sum);																//Output the Result!



		if (waitKey(30) >= 0) {																	//If any key is pressed in a delay of 30 milliscends, then EXIT
			break;
		}
	}


		
	
//Being Clean about memory! deleting all of the pointers

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


}