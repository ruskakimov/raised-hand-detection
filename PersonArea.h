#pragma once
#include <opencv2\opencv.hpp>
#include <deque>
#include <string>

class PersonArea {
public:
	static int minWaveLength;
	static int minGap;
	PersonArea(std::string personName, cv::Rect face, int peaksNum, cv::Size size) :
		personName(personName), peaks(peaksNum, -1), handUp(false), pos(-1), animationFrame(0)
	{
		area = face;
		// expand to 3x3
		area.x -= area.width;
		area.y -= area.height;
		area.width *= 3;
		area.height *= 3;
		if (area.x < 0) {
			area.width += area.x;
			area.x = 0;
		}
		if (area.y < 0) {
			area.width += area.y;
			area.y = 0;
		}
		if (area.x + area.width > size.width) {
			area.width = size.width - area.x;
		}
		if (area.y + area.height > size.height) {
			area.height = size.height - area.y;
		}
	}
	bool isRaised();
	void update(cv::Mat &bin);
	void drawOn(cv::Mat &img);
	void drawGraphOn(cv::Mat &img);
	void removeOverlap(PersonArea* other);
	void increaseGap(PersonArea* other);
	cv::Rect getArea()
	{
		return area;
	}
	std::string getName()
	{
		return personName;
	}
	void setPos(int new_pos)
	{
		pos = new_pos;
	};

private:
	std::string personName;
	cv::Rect area;
	std::deque<int> peaks;
	bool handUp;
	int pos;
	int animationFrame;

	void updatePeaks(cv::Mat &bin);
	int findTop(cv::Mat &bin);
	void updateHand();
	bool ascending();
	bool descending();
	bool high();
	bool low();
	bool farApart();
	void removeOverlap(cv::Rect &overlap);
	cv::Rect getOverlap(cv::Rect &rect);
	int gapBetween(PersonArea* other);
	int gapBetween(cv::Rect &rect);
};