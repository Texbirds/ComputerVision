#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;

float w = 83 * 6, h = 63 * 6;
Mat matrix, imgWarp;

int main()
{
    cv::Mat img = cv::imread("../../images/notwarpedyet.jpg");
    cv::Mat imgResized;
    float scale = 0.5; 

    cv::resize(img, imgResized, cv::Size(), scale, scale);

    Point2f src[4] = {
        Point2f(708 * scale, 653 * scale), // left top
        Point2f(2077 * scale, 1033 * scale), // right top
        Point2f(748 * scale, 2156 * scale), // left bottom
        Point2f(2117 * scale, 2115 * scale) // right bottom
    };

    Point2f dst[4] = {
        Point2f(0.0f, 0.0f), // left top
        Point2f(w, 0.0f),    // right top
        Point2f(0.0f, h),    // left bottom
        Point2f(w, h)        // right bottom
    };

    matrix = getPerspectiveTransform(src, dst);
    warpPerspective(imgResized, imgWarp, matrix, Point(w, h));

    for (int i = 0; i < 4; i++)
    {
        circle(imgResized, src[i], 10, Scalar(0, 0, 255), FILLED);
        circle(imgWarp, dst[i], 10, Scalar(0, 0, 255), -1);
    }

    imshow("Original Image", imgResized);
    imshow("Warped Image", imgWarp);

    cv::waitKey(0);
}
