#ifdef _DEBUG
#pragma comment(lib, "opencv_world4110d.lib")
#else
#pragma comment(lib, "opencv_world4110.lib")
#endif

#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // Laad originele afbeelding
    cv::Mat img = cv::imread("../images/landscape.jpg");

    // Resize (bijvoorbeeld verkleinen naar 50%)
    cv::Mat imgResized;
    cv::resize(img, imgResized, cv::Size(), 0.1, 0.1); // 10% in x en y richting

    // Crop een deel van de afbeelding
    int cropX = 100, cropY = 100, width = 200, height = 200;
    cv::Rect roi(cropX, cropY, width, height);
    cv::Mat imgCropped = img(roi);

    // Toon alle drie de resultaten
    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::imshow("Original Image", img);
    cv::imshow("Resized Image", imgResized);
    cv::imshow("Cropped Image", imgCropped);

    cv::waitKey(0);
    return 0;
}
