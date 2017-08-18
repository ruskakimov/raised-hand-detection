#pragma once
#include <opencv2\opencv.hpp>
#include <deque>

class PersonArea {
public:
	PersonArea(cv::Rect face, int peaksNum) :
		peaks(peaksNum, -1), handUp(false)
	{
		area = face;
		// expand to 3x3
		area.x -= area.width;
		area.y -= area.height;
		area.width *= 3;
		area.height *= 3;
	}
	void update(cv::Mat &bin);
	void drawOn(cv::Mat &img);

private:
	cv::Rect area;
	std::deque<int> peaks;
	bool handUp;

	void updatePeaks(cv::Mat &bin);
	int findTop(cv::Mat &bin);
	void updateHand();
	bool ascending();
	bool descending();
};