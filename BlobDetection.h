#pragma once
#include <opencv2\opencv.hpp>
#include <vector>

struct Blob {
	int minx;
	int miny;
	int maxx;
	int maxy;
	Blob(int x, int y) : minx(x), maxx(x), miny(y), maxy(y) {}
	int squareDistanceTo(int x, int y);
	int width();
	int area();
	void insert(int x, int y);
	void merge(Blob *b);
	float overlapPercent(Blob *b);
};

std::vector<Blob*> detectBlobs(cv::Mat &src);