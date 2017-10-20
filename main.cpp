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

// Pa stands for Person area
// Pas stands for Person areas
void updatePaQueue(deque<PersonArea*> &raised_queue, unordered_set<PersonArea*> &have_raised, PersonArea* &pa);
vector<PersonArea*> preparePas(vector<string> &names, vector<Rect> &faces, Size &size);
void updatePasPositions(vector<PersonArea*> &person_areas, deque<PersonArea*> &raised_queue, unordered_set<PersonArea*> &have_raised);
void displayTotalCount(Mat &img, int count);
Mat generateQueueDisplay(deque<PersonArea*> &person_areas, Size &size);
void sortAreas(vector<Rect> &areas);
Mat rgbOverlap(Mat &motion, Mat &skin);

int main() {
	// hardcoded names (left to right)
	// yeah I know this sucks
	// for demonstration purposes only
	string video = "three.mp4";

	vector<string> names;
	if (video == "two.mp4")
	{
		names = vector<string>{ "Abdullah", "Nelson" };
	}
	else if (video == "three.mp4")
	{
		names = vector<string>{ "Henry", "Ching", "Lee" };
	}
	else if (video == "five.mp4")
	{
		names = vector<string>{ "Abdullah", "Logan", "Umar", "Angel", "Rickveen" };
	}
	else if (video == "one.mp4")
	{
		names = vector<string>{ "Hamid" };
	}

	VideoCapture cam("./videos/" + video); // read sample video from video folder
	VideoWriter res; // video result with interface
	VideoWriter res2; // video result with motion detection and graph for demostration and debugging
	if (!cam.isOpened())
		return -1;
	namedWindow("result", WINDOW_AUTOSIZE);

	Mat previousFrame; // previous frame for motion detection
	cam >> previousFrame;
	cvtColor(previousFrame, previousFrame, CV_BGR2GRAY);

	vector<Rect> faces = detectFaces(previousFrame);

	sortAreas(faces);
	if (faces.empty()) {
		cout << "No faces found!" << endl;
		return -1;
	}
	else {
		cout << faces.size() << endl;
	}
	// person area is an object with
	// a rect area around an individual
	// 5 highest motion points from last 5 frames
	// methods for identifying if the hand has been raised (strictly ascending)
	// or lowered (strictly descending)
	vector<PersonArea*> person_areas = preparePas(names, faces, previousFrame.size());

	// queue of raised hands
	unordered_set<PersonArea*> have_raised; // set for fast checking (possibly a bad pattern)
	deque<PersonArea*> raised_queue; // actual queue

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

		Mat queueDisplay = generateQueueDisplay(raised_queue, img.size());
		hconcat(img, queueDisplay, img);

		cvtColor(motion, motion, COLOR_GRAY2BGR);

		for (auto pa : person_areas) {
			pa->drawOn(img);
			pa->drawOn(motion);
			pa->drawGraphOn(motion);
		}
		displayTotalCount(img, raised_queue.size());

		if (res.isOpened()) {
			res << img;
		}
		else {
			res = *new VideoWriter("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, img.size());
		}

		if (res2.isOpened()) {
			res2 << motion;
		}
		else {
			res2 = *new VideoWriter("motion_result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, motion.size());
		}

		imshow("result", img);
		imshow("motion", motion);
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


vector<PersonArea*> preparePas(vector<string> &names, vector<Rect> &faces, Size &size)
{
	vector<PersonArea*> person_areas;
	int face_width_sum = 0;
	int i = 0;
	for (Rect face : faces) {
		face_width_sum += face.width;
		person_areas.push_back(new PersonArea(names[i], face, QUEUE_LEN, size));
		i++;
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


Mat generateQueueDisplay(deque<PersonArea*> &person_areas, Size &size)
{
	Mat display = Mat::zeros(Size(300, size.height), CV_8UC3);

	Scalar white(255, 255, 255);
	Scalar green(0, 255, 0);

	Point text_start(30, 100);
	putText(display, "Queue:", text_start, FONT_HERSHEY_PLAIN, 4, white, 2);
	text_start.y += 30;

	for (int i = 0; i < person_areas.size(); i++)
	{
		text_start.y += 35;
		Scalar color = white;
		if (i == 0) color = green;
		string name = person_areas[i]->getName();
		string txt = to_string(i + 1) + ". " + name;
		putText(display, txt, text_start, FONT_HERSHEY_PLAIN, 2, color, 2);
	}

	return display;
}

bool compareFn(Rect &l, Rect &r) {
	return l.x < r.x;
}

void sortAreas(vector<Rect> &areas)
{
	sort(areas.begin(), areas.end(), compareFn);
}

Mat rgbOverlap(Mat &motion, Mat &skin)
{
	Mat rgb(motion.size(), CV_8UC3, Scalar(0, 0, 0));
	for (int r = 0; r < rgb.rows; r++)
	{
		for (int c = 0; c < rgb.cols; c++)
		{
			rgb.at<Vec3b>(r, c)[0] = motion.at<uchar>(r, c);
			rgb.at<Vec3b>(r, c)[2] = skin.at<uchar>(r, c);
		}
	}
	return rgb;
}