#pragma once
#include <opencv2\opencv.hpp>
#include <vector>

const std::string cascade_filepath = "./haarcascade/haarcascade_frontalface_alt.xml";

std::vector<cv::Rect> detectFaces(cv::Mat &gray);