#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>

#include "MotionDetection.h"

#define QUEUE_LEN 5

using namespace cv;
using namespace std;

int findTop(Mat& bin) {
	for (int r = 0; r < bin.rows; r++) {
		for (int c = 0; c < bin.cols; c++) {
			if (bin.at<uchar>(r, c))
				return r;
		}
	}
	return -1;
}

Mat plot(deque<int> dq, int height) {
	int step = 20;
	Mat plot = Mat::zeros(height, step * (dq.size() + 2), CV_8UC3);
	for (int i = 0; i < dq.size(); i++) {
		circle(plot, Point(step + i * step, dq[i]), 5, Scalar(0, 0, 255), -1);
	}
	return plot;
}

double standardDeviation(vector<int> distances) {
	// calc mean
	double mean = 0;
	for (int d : distances)
		mean += d;
	mean /= distances.size();

	// calc variance
	double variance = 0;
	for (int d : distances)
		variance += pow(mean - d, 2);
	variance /= distances.size();

	// calc standard deviation
	double std = sqrt(variance);
	return std;
}

bool descending(deque<int> dq) {
	// y coordinates ascend
	int len = dq.size();
	vector<int> distances(len - 1);
	for (int i = 1; i < len; i++) {
		distances[i - 1] = abs(dq[i] - dq[i - 1]);
		if (dq[i] <= dq[i - 1])
			return false;
	}
	//int std = standardDeviation(distances);
	//cout << "down: " << std << endl;
	//return std < 20;
	return true;
}

bool ascending(deque<int> dq) {
	// y coordinates descend
	int len = dq.size();
	vector<int> distances(len - 1);
	for (int i = 1; i < len; i++) {
		distances[i - 1] = abs(dq[i] - dq[i - 1]);
		if (dq[i] >= dq[i - 1])
			return false;
	}
	//int std = standardDeviation(distances);
	//cout << "up: " << std << endl;
	//return std < 20;
	return true;
}


int main() {
	VideoCapture cam("three.mts");

	VideoWriter res;

	if (!cam.isOpened())
		return -1;

	namedWindow("cam", WINDOW_FREERATIO);
	namedWindow("result", WINDOW_FREERATIO);

	Mat previousFrame; // previous frame
	cam >> previousFrame;
	cvtColor(previousFrame, previousFrame, CV_BGR2GRAY);

	deque<int> peaks(QUEUE_LEN, -1);

	while (true) {
		Mat img;
		cam >> img;
		if (img.empty())
			break;

		imshow("cam", img);

		Mat motion = detectMotion(img, previousFrame);


		//if (ascending(peaks)) {
		//	handUp = true;
		//}
		//else if (descending(peaks)) {
		//	handUp = false;
		//}

		Mat result;
		cvtColor(motion, result, COLOR_GRAY2BGR);

		//Mat last5_plot = plot(last, result.rows);
		//Scalar dotColor = Scalar(0, 0, 255);
		//if (handUp) {
		//	dotColor = Scalar(0, 255, 0);
		//}
		//circle(result, Point(100, 100), 50, dotColor, -1);

		//hconcat(result, last5_plot, result);
		imshow("result", result);

		if (res.isOpened())
			res << result;
		else
			res = *new VideoWriter("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, result.size());


		int keyCode = waitKey(5);
		if (keyCode == 27)
			break;
	}
}