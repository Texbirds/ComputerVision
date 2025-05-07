#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void assignmentC();
void assignmentD();

void main() {
	/*assignmentC();*/
	assignmentD();
}

void assignmentC() {
	string path = "../../images/bars.bmp";
	Mat img = imread(path);

	Mat imgOpen;
	Mat kernel = getStructuringElement(MORPH_CROSS, Size(5, 5));
	erode(img, imgOpen, kernel);
	dilate(imgOpen, imgOpen, kernel);
	imshow("Original Image", img);
	imshow("Open Image", imgOpen);
	waitKey(0);
}

void assignmentD() {
    string path = "../../images/ballstripes.bmp";
    Mat img2 = imread(path);

    // Grijswaarden (voor zekerheid)
    Mat gray;
    cvtColor(img2, gray, COLOR_BGR2GRAY);

    // Binair maken
    Mat binary;
    threshold(gray, binary, 128, 255, THRESH_BINARY_INV); 

    Mat imgOpen2;
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
    morphologyEx(binary, imgOpen2, MORPH_OPEN, kernel); 
    Mat result;
    bitwise_not(imgOpen2, result);
    imshow("Original Image", img2);
    imshow("Stripes removed", result);
    waitKey(0);
}