#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
    // Load image
    Mat img = imread("../images/guy.jpg");  

    // resize, should be around 1080p
    Size targetSize(1920, 1080);
    double aspectRatio = img.cols / (double)img.rows;
    int newWidth, newHeight;
    if (1920.0 / 1080.0 > aspectRatio) {
        newHeight = 1080;
        newWidth = static_cast<int>(aspectRatio * 1080);
    }
    else {
        newWidth = 1920;
        newHeight = static_cast<int>(1920 / aspectRatio);
    }
    resize(img, img, Size(newWidth, newHeight));

    // grayscale
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

	// bunch of random erode/dilate operations
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat morph = gray.clone();
    erode(morph, morph, kernel, Point(-1, -1), 24);  
    dilate(morph, morph, kernel, Point(-1, -1), 6);   
    erode(morph, morph, kernel, Point(-1, -1), 3);    
    dilate(morph, morph, kernel, Point(-1, -1), 6);   
    erode(morph, morph, kernel, Point(-1, -1), 6);    

    // darken
    Mat dark;
    morph.convertTo(dark, -1, 0.5, 0);  

	imshow("Original", img);
    imshow("Result", dark);
    waitKey(0);
    return 0;
}
