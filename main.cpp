#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>

#include "MotionDetection.h"
#include "FaceDetection.h"
#include "PersonArea.h"

#define QUEUE_LEN 7

using namespace cv;
using namespace std;

Mat plot(deque<int> dq, int height) {
	int step = 20;
	Mat plot = Mat::zeros(height, step * (dq.size() + 2), CV_8UC3);
	for (int i = 0; i < dq.size(); i++) {
		circle(plot, Point(step + i * step, dq[i]), 5, Scalar(0, 0, 255), -1);
	}
	return plot;
}

int main() {
	VideoCapture cam("two.mp4");
	VideoWriter res;
	if (!cam.isOpened())
		return -1;
	namedWindow("result", WINDOW_FREERATIO);

	Mat previousFrame; // previous frame for motion detection
	cam >> previousFrame;
	cvtColor(previousFrame, previousFrame, CV_BGR2GRAY);

	vector<Rect> faces = detectFaces(previousFrame);
	vector<PersonArea*> person_areas;
	for (Rect face : faces) {
		person_areas.push_back(new PersonArea(face, QUEUE_LEN));
	}

	while (true) {
		Mat img;
		cam >> img;
		if (img.empty())
			break;

		Mat motion = detectMotion(img, previousFrame);

		for (auto pa : person_areas) {
			pa->update(motion);
		}

		Mat result;
		cvtColor(motion, result, COLOR_GRAY2BGR);

		for (auto pa : person_areas) {
			pa->drawOn(img);
		}

		if (res.isOpened())
			res << img;
		else
			res = *new VideoWriter("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 50, result.size());
		imshow("result", img);
		int keyCode = waitKey(5);
		if (keyCode == 27)
			break;
	}
}