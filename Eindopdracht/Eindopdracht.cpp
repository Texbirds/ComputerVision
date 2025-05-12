#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct CursorSettings {
	Point position;
	string shape;
	int colorIndex;
	int size;
};

/////////////////  Project 1 - Virtual Painter //////////////////////

Mat img;
VideoCapture cap(0);
vector<CursorSettings> drawnPoints;

// Globale variabelen voor actieve cursorvorm
string currentShape = "circle";  // default
int currentSize = 20;

/////////////////////  COLOR VALUES ////////////////////////////////
// hmin, smin, vmin, hmax, smax, vmax
vector<vector<int>> myColors{
	{0,80,146,88,255,255},          // Red
	{100, 100, 50, 130, 255, 255},  // Blue
	{82,26,43,106,255,255}          // Green
};
vector<Scalar> myColorValues{
	{0,0,255},    // Red
	{255,0,0},    // Blue
	{0,255,0}     // Green
};
////////////////////////////////////////////////////////////////////

Point getContours(Mat image) {
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);
	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++) {
		int area = contourArea(contours[i]);

		string objectType;

		if (area > 1000) {
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			boundRect[i] = boundingRect(conPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;

			//drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			//rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
		}
	}
	return myPoint;
}

void printDetectedShape(Mat input) {
	Mat gray, blurImg, edge;
	cvtColor(input, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, blurImg, Size(5, 5), 3);
	Canny(blurImg, edge, 50, 150);

	vector<vector<Point>> contours;
	findContours(edge, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area > 1000) {
			vector<Point> approx;
			approxPolyDP(contours[i], approx, 0.02 * arcLength(contours[i], true), true);

			string shape;
			if (approx.size() == 3) shape = "triangle";
			else if (approx.size() == 4) shape = "square";
			else shape = "circle";

			cout << "Camera ziet vorm: " << shape << " met " << approx.size() << " hoeken." << endl;
			break;
		}
	}
}

// Detecteer de vorm van een voorgehouden object (3/4/>5 hoeken)
void detectTemplateShape(Mat input) {
	Mat gray, blurImg, edge;
	cvtColor(input, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, blurImg, Size(5, 5), 3);
	Canny(blurImg, edge, 50, 150);

	vector<vector<Point>> contours;
	findContours(edge, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area > 1000) {
			vector<Point> approx;
			approxPolyDP(contours[i], approx, 0.02 * arcLength(contours[i], true), true);
			Rect box = boundingRect(approx);

			if (approx.size() == 3) currentShape = "triangle";
			else if (approx.size() == 4) currentShape = "square";
			else currentShape = "circle";

			currentSize = 20;
			cout << "Geselecteerde vorm: " << currentShape << ", grootte: " << currentSize << endl;
			break;
		}
	}
}

// Detecteer kleur en sla punt met huidige vorm op
void findColor(Mat img) {
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++) {
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		Point myPoint = getContours(mask);
		if (myPoint.x != 0) {
			CursorSettings cs;
			cs.position = myPoint;
			cs.shape = currentShape;
			cs.colorIndex = i;
			cs.size = currentSize;
			drawnPoints.push_back(cs);
		}
	}
}

void drawOnCanvas(const vector<CursorSettings>& points, const vector<Scalar>& myColorValues) {
	for (const auto& cursor : points) {
		Scalar color = myColorValues[cursor.colorIndex];

		if (cursor.shape == "circle") {
			circle(img, cursor.position, cursor.size / 2, color, FILLED);
		}
		else if (cursor.shape == "square") {
			Rect square(cursor.position.x - cursor.size / 2,
				cursor.position.y - cursor.size / 2,
				cursor.size, cursor.size);
			rectangle(img, square, color, FILLED);
		}
		else if (cursor.shape == "triangle") {
			Point pt1(cursor.position.x, cursor.position.y - cursor.size / 2);
			Point pt2(cursor.position.x - cursor.size / 2, cursor.position.y + cursor.size / 2);
			Point pt3(cursor.position.x + cursor.size / 2, cursor.position.y + cursor.size / 2);
			vector<Point> triangle{ pt1, pt2, pt3 };
			fillConvexPoly(img, triangle, color);
		}
	}
}

int main() {
	while (true) {
		cap.read(img);
		printDetectedShape(img);

		// Druk op 's' om vorm te detecteren
		char key = waitKey(1);
		if (key == 's') {
			detectTemplateShape(img);
		}

		findColor(img);
		drawOnCanvas(drawnPoints, myColorValues);

		imshow("Image", img);
	}
	return 0;
}
