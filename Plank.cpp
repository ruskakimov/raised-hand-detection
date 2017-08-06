#include "Plank.h"
#include <iostream>

using namespace cv;
using namespace std;

//void Plank::updatePlank(Mat &img) {
//	int start_y = max(y - activeDistance, 0);
//	int end_y = min(y + activeDistance, img.rows - 1);
//	for (int r = start_y; r <= end_y; r++) {
//		for (int c = left_x; c <= right_x; c++) {
//			uchar *color = &img.at<uchar>(r, c);
//			if (*color == 255) {
//				this->y = r;
//				return;
//			}
//		}
//	}
//}

void Plank::drawOn(Mat &img, Scalar color) {
	Point p1(left_x, y);
	Point p2(right_x, y);
	line(img, p1, p2, color, 3);
	p1 = * new Point(left_x, y - activeDistance);
	p2 = * new Point(right_x, y + activeDistance);
	rectangle(img, p1, p2, color);
}

void Plank::debug() {
	cout << "Left: " << left_x << endl;
	cout << "Right: " << right_x << endl;
	cout << "Y: " << y << endl;
	cout << "active: " << activeDistance << endl;
	cout << endl;
}