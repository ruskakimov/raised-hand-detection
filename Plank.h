#pragma once

#include <opencv2\opencv.hpp>

struct Plank {
	int left_x;
	int right_x;
	int y;
	int activeDistance;
	Plank(int y, int lx, int rx, int activeDistance) : y(y), left_x(lx), right_x(rx), activeDistance(activeDistance) {}
	//void updatePlank(cv::Mat &img);
	void drawOn(cv::Mat &img, cv::Scalar color);
	void debug();
};