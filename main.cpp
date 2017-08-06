#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>

#include "MotionDetection.h"
#include "BlobDetection.h"
#include "Plank.h"

#define PLANK_ACTIVE_DISTANCE 150

using namespace cv;
using namespace std;

void updatePlank(vector<Blob*> blobs, Plank *plank) {
	int min_y = plank->y - plank->activeDistance;
	int max_y = plank->y + plank->activeDistance;
	int new_y = INT_MAX;
	for (Blob *b : blobs) {
		if (b->miny < max_y && b->maxy > min_y) { // blob intersects active area 
			new_y = min(new_y, max(b->miny, min_y));
		}
	}
	// blob top could be out of reach
	if (new_y != INT_MAX)
		plank->y = new_y;
}

int main() {
	VideoCapture cam(0);

	Mat sampleImg;
	cam >> sampleImg;

	VideoWriter res("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, sampleImg.size());

	if (!cam.isOpened())
		return -1;

	//namedWindow("cam");
	//namedWindow("skin");
	namedWindow("motion");
	//namedWindow("overlap");

	Mat previousFrame; // previous frame
	cam >> previousFrame;
	cvtColor(previousFrame, previousFrame, CV_BGR2GRAY);

	//Blob* oldBlob = new Blob(0, 0); // previous frame blob
	Plank *plank = new Plank(previousFrame.rows - 1, 0, previousFrame.cols - 1, PLANK_ACTIVE_DISTANCE);

	while (true) {
		Mat img;
		cam >> img;
		if (img.empty())
			break;

		//imshow("cam", img);

		//Mat skin;
		//IplImage* image2 = cvCloneImage(&(IplImage)img);
		//detectSkin(skin, image2);

		//Mat skin = detectSkin(img);
		Mat motion = detectMotion(img, previousFrame);

		//imshow("skin", skin);

		vector<Blob*> blobs = detectBlobs(motion);
		updatePlank(blobs, plank);

		//Mat both = overlap(skin, motion);
		//imshow("overlap", both);

		Mat result;
		cvtColor(motion, result, COLOR_GRAY2BGR);

		plank->drawOn(result, Scalar(0,255,0));

		imshow("motion", result);
		res << result;

		int keyCode = waitKey(5);
		if (keyCode == 27)
			break;
	}
}