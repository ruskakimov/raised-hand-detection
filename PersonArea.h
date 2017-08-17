#pragma once
#include <opencv2\opencv.hpp>
#include <deque>

class PersonArea {
public:
	PersonArea(int minx, int miny, int maxx, int maxy, int peaksNum) :
		min_x(minx), min_y(miny), max_x(maxx), max_y(maxy), peaks(peaksNum, -1), handUp(false)
	{
	}
	void update(cv::Mat &bin);
	void drawOn(cv::Mat &img);

private:
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	std::deque<int> peaks;
	bool handUp;

	void updatePeaks(cv::Mat &bin);
	int findTop(cv::Mat &bin);
	void updateHand();
	bool ascending();
	bool descending();
};