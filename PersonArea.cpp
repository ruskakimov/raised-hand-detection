#include "PersonArea.h"
#include <iostream>

using namespace cv;
using namespace std;

int PersonArea::minWaveLength = 30;
int PersonArea::minGap = 30;

bool PersonArea::isRaised()
{
	return handUp;
}

void PersonArea::drawOn(Mat &img)
{
	Scalar color(0, 0, 255);
	if (handUp) color = Scalar(0, 255, 255);
	Point pt1(area.x, area.y);
	Point pt2(area.x + area.width, area.y + area.height);

	int border = 3;
	// border animation for the first in the queue
	if (pos == 1) {
		color = Scalar(0, 255, 0);

		// draw name
		Point name_start(area.x, area.y - 20);
		putText(img, personName, name_start, FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 0), 5);
		putText(img, personName, name_start, FONT_HERSHEY_PLAIN, 4, color, 2);

		int increase = 2 * (animationFrame / 5);
		if (increase > 10) increase = 20 - increase;
		border = 3 + increase;
		animationFrame++;
		animationFrame %= 50;
	}
	rectangle(img, pt1, pt2, color, border);

	if (pos > 0)
	{
		Point text_start(area.x + 10, area.y + 60);
		putText(img, to_string(pos), text_start, FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 0), 5);
		putText(img, to_string(pos), text_start, FONT_HERSHEY_PLAIN, 4, color, 2);
	}
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
	if (handUp && descending() && low() && farApart())
	{
		handUp = false;
		animationFrame = 0;
	}
	else if (!handUp && ascending() && high() && farApart()) handUp = true;
}

bool PersonArea::farApart()
{
	return abs(peaks.front() - peaks.back()) > minWaveLength;
}

bool PersonArea::high()
{
	return peaks.back() < (area.y + area.height / 3) && peaks.back() != -1;
}

bool PersonArea::low()
{
	return peaks.back() > (area.y + area.height / 2) && peaks.back() != -1;
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

Rect PersonArea::getOverlap(Rect &rect)
{
	Point p1(
		max(area.x, rect.x),
		max(area.y, rect.y)
	);
	Point p2(
		min(area.x + area.width, rect.x + rect.width),
		min(area.y + area.height, rect.y + rect.height)
	);
	if (p1.x >= p2.x || p1.y >= p2.y) {
		return Rect(Point(0, 0), Point(0, 0));
	}
	Rect overlap(p1, p2);
	return overlap;
}

void PersonArea::removeOverlap(PersonArea* other)
{
	Rect overlap = other->getOverlap(area);
	if (overlap.area() > 0)
	{
		removeOverlap(overlap);
		other->removeOverlap(overlap);
	}
}

void PersonArea::removeOverlap(Rect &overlap)
{
	if (overlap.height >= overlap.width)
	{
		// remove vertical slice
		area.width -= overlap.width;

		if (overlap.x == area.x) // overlap on left side
		{
			area.x += overlap.width;
		}
	}
	else
	{
		// remove horizontal slice
		area.height -= overlap.height;

		if (overlap.y == area.y) // overlap on top
		{
			area.y += overlap.height;
		}
	}
}

void PersonArea::increaseGap(PersonArea* other)
{
	int remove = minGap - gapBetween(other);
	if (remove < 0) return;
	Rect otherArea = other->getArea();
	if (area.x < otherArea.x)
	{
		// remove from area
		area.width -= remove;
	}
	else
	{
		// remove from other area
		otherArea.width -= remove;
		otherArea.x += remove;
	}
}


int PersonArea::gapBetween(PersonArea* other)
{
	return other->gapBetween(area);
}

int PersonArea::gapBetween(Rect &rect)
{
	int maxx0 = area.x + area.width;
	int minx0 = area.x;
	int maxx1 = rect.x + rect.width;
	int minx1 = rect.x;
	return max(minx0, minx1) - min(maxx0, maxx1);
}