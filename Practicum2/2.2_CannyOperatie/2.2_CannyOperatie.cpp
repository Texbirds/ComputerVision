#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void main() {

	string path = "../../images/test.png";
	Mat img = imread(path);
	Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

	int threshold1 = 50;
	int threshold2 = 150;
	namedWindow("Trackbars", (640, 200));
	createTrackbar("Threshold1", "Trackbars", &threshold1, 255);
	createTrackbar("Threshold2", "Trackbars", &threshold2, 255);

	while (true) {
		while (true)
		{
			cvtColor(img, imgGray, COLOR_BGR2GRAY);
			GaussianBlur(imgGray, imgBlur, Size(7, 7), 5, 0);
			Canny(imgBlur, imgCanny, threshold1, threshold2);

			Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
			dilate(imgCanny, imgDil, kernel);
			erode(imgDil, imgErode, kernel);

			imshow("Original Image", img);
			imshow("Gray Image", imgGray);
			imshow("Blurred Image", imgBlur);
			imshow("Canny Edge Detection", imgCanny);
			imshow("Dilated Image", imgDil);
			imshow("Eroded Image", imgErode);

			waitKey(1);
		}
	}
}