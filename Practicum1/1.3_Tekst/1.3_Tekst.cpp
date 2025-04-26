#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::Mat image = cv::Mat::zeros(400, 800, CV_8UC3);
    cv::circle(image, cv::Point(200, 200), 50, cv::Scalar(0, 255, 0), -1);
    cv::rectangle(image, cv::Point(400, 150), cv::Point(600, 250), cv::Scalar(255, 0, 0), -1);
    cv::line(image, cv::Point(0, 0), cv::Point(800, 400), cv::Scalar(0, 0, 255), 5);
    cv::putText(image, "Hello OpenCV", cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::imshow("Image", image);
    cv::waitKey(0);
}
