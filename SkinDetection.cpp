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
	Point t_left(1, 123);
	Point t_right(207, 159);
	Point b_left(1, 134);
	Point b_right(81, 165);
	Point p(y, cr);
	int a = verticalDistanceToLine(t_left, t_right, p);
	int b = verticalDistanceToLine(b_left, b_right, p);
	int c = verticalDistanceToLine(b_right, t_right, p);
	return a <= 0 && b >= 0 && c >= 0;
}

bool insideYCB(uchar y, uchar cb)
{
	Point t_left(1, 126);
	Point t_mid(64, 108);
	Point t_right(188, 95);
	Point b_left(1, 131);
	Point b_right(183, 110);
	Point p(y, cb);
	int a = verticalDistanceToLine(t_left, t_mid, p);
	int b = verticalDistanceToLine(t_mid, t_right, p);
	int c = verticalDistanceToLine(t_right, b_right, p);
	int d = verticalDistanceToLine(b_left, b_right, p);
	return a <= 0 && b <= 0 && c >= 0 && d >= 0;
}

bool insideCRCB(uchar cr, uchar cb)
{
	Point t_left(126, 126);
	Point t_right(159, 92);
	Point b_left(125, 135);
	Point b_right(169, 106);
	Point p(cr, cb);
	int a = verticalDistanceToLine(t_left, t_right, p);
	int b = verticalDistanceToLine(b_left, b_right, p);
	int c = verticalDistanceToLine(t_right, b_right, p);
	int d = verticalDistanceToLine(t_left, b_left, p);
	return a <= 0 && b >= 0 && c <= 0 && d <= 0;
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