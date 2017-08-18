#include "FaceDetection.h"

using namespace cv;

std::vector<Rect> detectFaces(Mat &gray)
{
	CascadeClassifier face_cascade;
	std::vector<Rect> faces;
	if (!face_cascade.load(cascade_filepath)) {
		printf("Error loading cascade!\n");
		return faces;
	}
	face_cascade.detectMultiScale(gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	return faces;
}