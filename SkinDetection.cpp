#include "SkinDetection.h"
#include <iostream>

using namespace cv;
using namespace std;

SkinMap* initializeSkinMap()
{
	cout << "Initializing skin map..." << endl;
	SkinMap* map = new SkinMap();
	for (int y = 0; y < 256; y++)
	{
		cout << "y: " << (int) y << endl;
		for (int cr = 0; cr < 256; cr++)
		{
			for (int cb = 0; cb < 256; cb++)
			{
				bool is_skin = isSkin(Vec3b(y, cr, cb));
				map->set(y, cr, cb, is_skin);
			}
		}
	}
	cout << "Skin map has been initialized." << endl;
	return map;
}

int verticalDistanceToLine(Point l1, Point l2, Point p)
{
	double k = (l2.y - l1.y) / (double) (l2.x - l1.x);
	int expected_y = k * (p.x - l1.x) + l1.y;
	return expected_y - p.y;
}

bool insideYCR(uchar y, uchar cr)
{
	Point left(0, 135);
	Point right(250, 135);
	Point bottom(170, 180);
	Point p(y, cr);
	int a = verticalDistanceToLine(left, right, p);
	int b = verticalDistanceToLine(left, bottom, p);
	int c = verticalDistanceToLine(bottom, right, p);
	return a <= 0 && b >= 0 && c >= 0;
}

bool insideYCB(uchar y, uchar cb)
{
	Point left(0, 135);
	Point right(195, 135);
	Point top(190, 80);
	Point p(y, cb);
	int a = verticalDistanceToLine(left, right, p);
	int b = verticalDistanceToLine(left, top, p);
	int c = verticalDistanceToLine(top, right, p);
	return a >= 0 && b <= 0 && c <= 0;
}

bool insideCRCB(uchar cr, uchar cb)
{
	Point center(135, 120);
	Point bottom(165, 120);
	Point top(150, 90);
	Point p(cr, cb);
	int a = verticalDistanceToLine(center, bottom, p);
	int b = verticalDistanceToLine(center, top, p);
	int c = verticalDistanceToLine(top, bottom, p);
	return a >= 0 && b <= 0 && c <= 0;
}

bool isSkin(Vec3b pixel)
{
	uchar y = pixel[0];
	uchar cr = pixel[1];
	uchar cb = pixel[2];
	return insideYCR(y, cr) && insideYCB(y, cb) && insideCRCB(cr, cb);
}

Mat detectSkin(Mat& source) {
	Mat ycrcb;
	cvtColor(source, ycrcb, COLOR_BGR2YCrCb);
	Mat skin = Mat::zeros(source.rows, source.cols, CV_8U);
	for (int r = 0; r < source.rows; r++) {
		for (int c = 0; c < source.cols; c++) {
			if (isSkin(ycrcb.at<Vec3b>(r, c)))
				skin.at<uchar>(r, c) = 255;
		}
	}
	return skin;
}

Mat detectSkin(Mat& source, SkinMap* skin_map) {
	Mat ycrcb;
	cvtColor(source, ycrcb, COLOR_BGR2YCrCb);
	Mat skin = Mat::zeros(source.rows, source.cols, CV_8U);
	for (int r = 0; r < source.rows; r++) {
		for (int c = 0; c < source.cols; c++) {
			Vec3b pixel = source.at<Vec3b>(r, c);
			if (skin_map->get(pixel[0], pixel[1], pixel[2]))
				skin.at<uchar>(r, c) = 255;
		}
	}
	return skin;
}