#include "BlobDetection.h"

using namespace cv;
using namespace std;

#define MIN_BLOB_DISTANCE_SQUARED 900
#define MIN_BLOB_AREA 1000
#define MIN_BLOB_WIDTH 50

int Blob::squareDistanceTo(int x, int y) {
	// clamp point to the side
	int clampx = max(min(maxx, x), minx);
	int clampy = max(min(maxy, y), miny);
	return pow(x - clampx, 2) + pow(y - clampy, 2);
}

int Blob::area() {
	return (maxx - minx) * (maxy - miny);
}

int Blob::width() {
	return maxx - minx;
}

void Blob::insert(int x, int y) {
	maxx = max(maxx, x);
	minx = min(minx, x);
	maxy = max(maxy, y);
	miny = min(miny, y);
}

void Blob::merge(Blob *b) {
	insert(b->minx, b->miny);
	insert(b->maxx, b->maxy);
}

float Blob::overlapPercent(Blob *b) {
	int ominx = max(minx, b->minx);
	int omaxx = min(maxx, b->maxx);
	int ominy = max(miny, b->miny);
	int omaxy = min(maxy, b->maxy);
	float oarea = (omaxx - ominx) * (omaxy - ominy);
	return oarea / area();
}

vector<Blob*> detectBlobs(Mat &src) {
	vector<Blob*> allBlobs;
	// get square blobs
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			uchar color = src.at<uchar>(r, c);
			if (color == 255) {
				// find blob this point belongs to
				bool foundHome = false;
				for (Blob *b : allBlobs) {
					if (b->squareDistanceTo(c, r) < MIN_BLOB_DISTANCE_SQUARED) {
						b->insert(c, r);
						foundHome = true;
						break;
					}
				}
				if (!foundHome) {
					allBlobs.push_back(new Blob(c, r));
				}
			}
		}
	}

	vector<Blob*> bigBlobs;
	for (Blob* b : allBlobs) {
		if (b->area() >= MIN_BLOB_AREA && b->width() >= MIN_BLOB_WIDTH)
			bigBlobs.push_back(b);
	}

	Mat res;
	// draw in red
	cvtColor(src, res, COLOR_GRAY2BGR);
	for (Blob *result : bigBlobs) {
		Point p1(result->minx, result->miny);
		Point p2(result->maxx, result->maxy);
		rectangle(res, p1, p2, Scalar(0, 0, 255));
	}
	//imshow("blobs", res);
	src = res; // delete later
	return bigBlobs;
}
