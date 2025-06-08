#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct CursorSettings {
	Point position;
	string shape;
	Scalar color;
	int size;
};

Mat img;
VideoCapture cap(0);
vector<CursorSettings> drawnPoints;

// === CURRENT CURSOR SETTINGS (after shape detection) ===
string savedShape = "circle";
int savedSize = 20;
Scalar savedColor = Scalar(0, 0, 255);  // default red

// === Detect if a shape is being held up ===
bool isCursorShapeVisible(Mat input) {
	Mat hsv, mask;
	cvtColor(input, hsv, COLOR_BGR2HSV);
	inRange(hsv, Scalar(0, 50, 50), Scalar(180, 255, 255), mask);

	vector<vector<Point>> contours;
	findContours(mask, contours, noArray(), RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (size_t i = 0; i < contours.size(); ++i) {
		double area = contourArea(contours[i]);
		if (area > 1000) {
			vector<Point> approx;
			approxPolyDP(contours[i], approx, 0.02 * arcLength(contours[i], true), true);

			cout << "[SHAPE DETECTED] Contour #" << i
				<< " | Area: " << area
				<< " | Vertices: " << approx.size() << endl;

			return true;
		}
	}
	return false;
}


void clearCanvas() {
	drawnPoints.clear();
	img = Mat::zeros(img.size(), CV_8UC3); // black canvas
	cout << "[SCREEN CLEARED]\n";
}

// === Find largest contour and return its center + shape ===
Point getContours(Mat image, vector<Point>& contourOut) {
	vector<vector<Point>> contours;
	findContours(image, contours, noArray(), RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Point myPoint(0, 0);

	for (auto& contour : contours) {
		if (contourArea(contour) > 1000) {
			vector<Point> approx;
			approxPolyDP(contour, approx, 0.02 * arcLength(contour, true), true);
			Rect box = boundingRect(approx);
			myPoint = Point(box.x + box.width / 2, box.y + box.height / 2);
			contourOut = contour;
			break;
		}
	}
	return myPoint;
}

// === Detect shape + color + size of colored object ===
void detectCursorSettings(Mat input) {
	Mat hsv, mask, result;
	cvtColor(input, hsv, COLOR_BGR2HSV);

	// Invert white background → keep only colored parts
	// Keep pixels with saturation and value above a threshold
	inRange(hsv, Scalar(0, 50, 50), Scalar(180, 255, 255), mask);

	vector<vector<Point>> contours;
	findContours(mask, contours, noArray(), RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	if (contours.empty()) {
		cout << "[!] Geen contour gevonden bij shape-scan.\n";
		return;
	}

	// Use largest contour
	int maxIndex = 0;
	double maxArea = 0;
	for (int i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area > maxArea) {
			maxArea = area;
			maxIndex = i;
		}
	}

	vector<Point> approx;
	approxPolyDP(contours[maxIndex], approx, 0.02 * arcLength(contours[maxIndex], true), true);
	Rect box = boundingRect(approx);

	// Shape classification
	string shape;
	if (approx.size() == 3) shape = "triangle";
	else if (approx.size() == 4) shape = "square";
	else shape = "circle";

	int size = max(box.width, box.height) / 10;

	// Mean color inside shape
	Mat maskContour = Mat::zeros(input.size(), CV_8UC1);
	drawContours(maskContour, contours, maxIndex, Scalar(255), FILLED);
	Scalar meanColor = mean(input, maskContour);

	savedShape = shape;
	savedSize = size;
	savedColor = meanColor;

	cout << "[SHAPE SELECTED] Vorm: " << savedShape
		<< " | Grootte: " << savedSize
		<< " | Kleur: (B,G,R) = (" << savedColor[0] << ", " << savedColor[1] << ", " << savedColor[2] << ")\n";
}



// === Track black sock/marker (only if no shape is visible) ===
void trackPen(Mat frame, bool cursorInFrame) {

	Mat imgHSV, mask;
	cvtColor(frame, imgHSV, COLOR_BGR2HSV);

	Scalar lower_black(0, 0, 0);
	Scalar upper_black(180, 255, 60);
	inRange(imgHSV, lower_black, upper_black, mask);

	vector<vector<Point>> contours;
	findContours(mask, contours, noArray(), RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (size_t i = 0; i < contours.size(); ++i) {
		double area = contourArea(contours[i]);
		if (area > 300 && area < 3000) {
			Rect box = boundingRect(contours[i]);
			float aspect = (float)box.width / box.height;
			if (aspect > 0.3 && aspect < 2.0) {
				Point center(box.x + box.width / 2, box.y + box.height / 2);
				drawnPoints.push_back({ center, savedShape, savedColor, savedSize });
				break;
			}
		}
	}
}


// === Render shapes ===
void drawOnCanvas(const vector<CursorSettings>& points) {
	for (const auto& c : points) {
		if (c.shape == "circle") {
			circle(img, c.position, c.size / 2, c.color, FILLED);
		}
		else if (c.shape == "square") {
			Rect r(c.position.x - c.size / 2, c.position.y - c.size / 2, c.size, c.size);
			rectangle(img, r, c.color, FILLED);
		}
		else if (c.shape == "triangle") {
			Point pt1(c.position.x, c.position.y - c.size / 2);
			Point pt2(c.position.x - c.size / 2, c.position.y + c.size / 2);
			Point pt3(c.position.x + c.size / 2, c.position.y + c.size / 2);
			vector<Point> triangle{ pt1, pt2, pt3 };
			fillConvexPoly(img, triangle, c.color);
		}
	}
}

int main() {
	while (true) {
		cap.read(img);
		if (img.empty()) break;

		bool shapeVisible = isCursorShapeVisible(img);

		char key = waitKey(1);
		if (key == 's') {
			detectCursorSettings(img);
		}
		else if (key == 'z') {
			clearCanvas();
		}

		trackPen(img, shapeVisible);
		drawOnCanvas(drawnPoints);

		imshow("Virtual Painter", img);
	}
	return 0;
}
