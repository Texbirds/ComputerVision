#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap(0); 
    if (!cap.isOpened()) {
        cout << "Error: Webcam not accessible!" << endl;
        return -1;
    }

    CascadeClassifier faceCascade;
    faceCascade.load("../../images/haarcascade_frontalface_default.xml");
    Mat img;
    vector<Rect> faces;

    while (true)
    {
        cap.read(img); 
        faceCascade.detectMultiScale(img, faces, 1.1, 10);

        for (int i = 0; i < faces.size(); i++)
        {
            rectangle(img, faces[i].tl(), faces[i].br(), Scalar(255, 0, 255), 3);
        }

        imshow("Webcam Face Detection", img);

        if (waitKey(1) == 'q') {
            break;
        }
    }

    cap.release(); 
    destroyAllWindows(); 
}
