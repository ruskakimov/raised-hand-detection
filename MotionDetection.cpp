#include "MotionDetection.h"

using namespace cv;
using namespace std;

Mat detectMotion(Mat &src, Mat &previousFrame) {
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	Mat res = Mat::zeros(gray.size(), CV_8U);
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			uchar srcCol = gray.at<uchar>(r, c);
			uchar prevCol = previousFrame.at<uchar>(r, c);

			if (abs(srcCol - prevCol) > 3) {
				res.at<uchar>(r, c) = 255;
			}
		}
	}
	previousFrame = gray; // save current frame as a reference
	erode(res, res, Mat(), Point(-1, -1), 3);
	dilate(res, res, Mat(), Point(-1, -1), 1);
	return res;
}