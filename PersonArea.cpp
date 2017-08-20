#include "PersonArea.h"

using namespace cv;

int PersonArea::minDist = 30;

void PersonArea::drawOn(Mat &img)
{
	Scalar color(0, 0, 255);
	if (handUp) color = Scalar(0, 255, 0);
	Point pt1(area.x, area.y);
	Point pt2(area.x + area.width, area.y + area.height);
	rectangle(img, pt1, pt2, color, 5);
}

void PersonArea::drawGraphOn(Mat &img)
{
	Scalar color(0, 0, 255);
	int steps = peaks.size() + 1;
	int step = area.width / steps;
	for (int i = 1; i < steps; i++) {
		if (peaks[i - 1] > 0)
			circle(img, Point(area.x + i * step, peaks[i - 1]), 5, color, -1);
	}
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
	for (int r = area.y; r < area.y + area.height; r++) {
		for (int c = area.x; c < area.x + area.width; c++) {
			if (bin.at<uchar>(r, c)) return r;
		}
	}
	return -1;
}


void PersonArea::updateHand()
{
	if (handUp && descending() && low() && farApart()) handUp = false;
	else if (!handUp && ascending() && high() && farApart()) handUp = true;
}

bool PersonArea::farApart()
{
	return abs(peaks.front() - peaks.back()) > minDist;
}

bool PersonArea::high()
{
	return peaks.back() < (area.y + area.height / 2);
}

bool PersonArea::low()
{
	return peaks.back() > (area.y + area.height / 2);
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