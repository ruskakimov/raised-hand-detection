#pragma once
#include <opencv2\opencv.hpp>
#include <vector>

struct SkinMap {
public:
	SkinMap()
	{
		long size = pow(256, 3);
		map = std::vector<bool>(size);
	}
	uchar get(uchar y, uchar cr, uchar cb)
	{
		return map[getID(y, cr, cb)];
	}
	void set(uchar y, uchar cr, uchar cb, bool is_skin)
	{
		map[getID(y, cr, cb)] = is_skin;
	}
private:
	long getID(uchar y, uchar cr, uchar cb)
	{
		return y * pow(256, 2) + cr * 256 + cb;
	}
	std::vector<bool> map;
};

SkinMap* initializeSkinMap();

int verticalDistanceToLine(cv::Point l0, cv::Point l1, cv::Point x);

bool insideYCR(uchar y, uchar cr);

bool insideYCB(uchar y, uchar cb);

bool insideCRCB(uchar cr, uchar cb);

bool isSkin(cv::Vec3b pixel);

cv::Mat detectSkin(cv::Mat& source);
cv::Mat detectSkin(cv::Mat& source, SkinMap* skin_map);