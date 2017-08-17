#include "PersonArea.h"

using namespace cv;

void PersonArea::drawOn(Mat &img)
{
	Scalar color(0, 0, 255);
	if (handUp) color = Scalar(0, 255, 0);
	Point pt1(min_x, min_y);
	Point pt2(max_x, max_y);
	rectangle(img, pt1, pt2, color, 5);
}


void PersonArea::update(Mat &bin)
{
	updatePeaks(bin);
	updateHand();
}


void PersonArea::updatePeaks(Mat &bin)
{
	int top = findTop(bin);
	peaks.push_back(top);
	peaks.pop_front();
}


int PersonArea::findTop(Mat &bin)
{
	for (int r = min_y; r < max_y; r++) {
		for (int c = min_x; c < max_x; c++) {
			if (bin.at<uchar>(r, c)) return r;
		}
	}
	return -1;
}


void PersonArea::updateHand()
{
	if (handUp && descending()) handUp = false;
	else if (!handUp && ascending()) handUp = true;
}

bool PersonArea::ascending()
{
	// y coordinates descend
	for (int i = 1; i < peaks.size(); i++) {
		if (peaks[i] >= peaks[i - 1]) return false;
	}
	return true;
}

bool PersonArea::descending()
{
	// y coordinates ascend
	for (int i = 1; i < peaks.size(); i++) {
		if (peaks[i] <= peaks[i - 1]) return false;
	}
	return true;
}