#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>
#include <unordered_set>

#include "MotionDetection.h"
#include "FaceDetection.h"
#include "PersonArea.h"

#define QUEUE_LEN 5

using namespace cv;
using namespace std;

void updatePaQueue(deque<PersonArea*> &raised_queue, unordered_set<PersonArea*> &have_raised, PersonArea* &pa);
vector<PersonArea*> preparePas(vector<Rect> &faces, Size &size);
void updatePasPositions(vector<PersonArea*> &person_areas, deque<PersonArea*> &raised_queue, unordered_set<PersonArea*> &have_raised);
void displayTotalCount(Mat &img, int count);

int main() {
	VideoCapture cam("two.mp4");
	VideoWriter res;
	if (!cam.isOpened())
		return -1;
	namedWindow("result", WINDOW_FREERATIO);

	Mat previousFrame; // previous frame for motion detection
	cam >> previousFrame;
	cvtColor(previousFrame, previousFrame, CV_BGR2GRAY);

	vector<Rect> faces = detectFaces(previousFrame);
	if (faces.empty()) {
		cout << "No faces found!" << endl;
		return -1;
	}
	vector<PersonArea*> person_areas = preparePas(faces, previousFrame.size());

	// queue of raised hands
	unordered_set<PersonArea*> have_raised;
	deque<PersonArea*> raised_queue;

	while (true) {
		Mat img;
		cam >> img;
		if (img.empty())
			break;

		Mat motion = detectMotion(img, previousFrame);

		for (auto pa : person_areas) {
			pa->update(motion);
			updatePaQueue(raised_queue, have_raised, pa);
		}
		updatePasPositions(person_areas, raised_queue, have_raised);

		Mat result;
		cvtColor(motion, result, COLOR_GRAY2BGR);

		for (auto pa : person_areas) {
			pa->drawOn(img);
			//pa->drawGraphOn(img);
		}
		displayTotalCount(img, raised_queue.size());

		if (res.isOpened())
			res << img;
		else
			res = *new VideoWriter("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 50, result.size());
		imshow("result", img);
		int keyCode = waitKey(5);
		if (keyCode == 27)
			break;
	}
}


void displayTotalCount(Mat &img, int count)
{
	Point text_start(30, 100);
	string txt = "Total: " + to_string(count);
	putText(img, txt, text_start, FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 0), 5);
	putText(img, txt, text_start, FONT_HERSHEY_PLAIN, 4, Scalar(0, 255, 0), 2);
}


void updatePasPositions(vector<PersonArea*> &person_areas, deque<PersonArea*> &raised_queue, unordered_set<PersonArea*> &have_raised)
{
	for (PersonArea* pa : person_areas)
	{
		if (have_raised.find(pa) == have_raised.end())
		{
			pa->setPos(-1);
		}
		else
		{
			int pos = 0;
			for (auto r : raised_queue)
			{
				pos++;
				if (pa == r) break;
			}
			pa->setPos(pos);
		}
	}
}


void updatePaQueue(deque<PersonArea*> &raised_queue, unordered_set<PersonArea*> &have_raised, PersonArea* &pa)
{
	if (pa->isRaised() && have_raised.find(pa) == have_raised.end())
	{
		// new raised hand
		raised_queue.push_back(pa);
		have_raised.insert(pa);
	}
	else if (!pa->isRaised() && have_raised.find(pa) != have_raised.end())
	{
		// lowered hand
		for (auto i = raised_queue.begin(); i != raised_queue.end(); i++)
		{
			if (*i == pa)
			{
				raised_queue.erase(i);
				break;
			}
		}
		have_raised.erase(pa);
	}
}


vector<PersonArea*> preparePas(vector<Rect> &faces, Size &size)
{
	vector<PersonArea*> person_areas;
	int face_width_sum = 0;
	for (Rect face : faces) {
		face_width_sum += face.width;
		person_areas.push_back(new PersonArea(face, QUEUE_LEN, size));
	}
	// set min gap between areas as average face width
	int average_face_width = face_width_sum / faces.size();
	cout << "Average face width: " << average_face_width << endl;
	PersonArea::minGap = average_face_width;

	// remove overlaps
	for (int i = 0; i < person_areas.size(); i++) {
		for (int j = i + 1; j < person_areas.size(); j++) {
			person_areas[i]->removeOverlap(person_areas[j]);
		}
	}
	// increase gap
	for (int i = 0; i < person_areas.size(); i++) {
		for (int j = 0; j < person_areas.size(); j++) {
			if (i != j) person_areas[i]->increaseGap(person_areas[j]);
		}
	}
	return person_areas;
}