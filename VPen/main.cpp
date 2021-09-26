#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img;
VideoCapture capture(0);
vector<vector<int>> newPoints;  // to store all points

vector<vector<int>> colors{ {62,65,42,117,255,255} };
	
vector<Scalar> colorValues{ {255,0,0} };
	
Point getContours(Mat image) {


	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	
	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point point(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			point.x = boundRect[i].x + boundRect[i].width / 2;
			point.y = boundRect[i].y;
		}
	}
	return point;
}

vector<vector<int>> findColor(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < colors.size(); i++)
	{
		Scalar lower(colors[i][0], colors[i][1], colors[i][2]);
		Scalar upper(colors[i][3], colors[i][4], colors[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		
		Point myPoint = getContours(mask);
		if (myPoint.x != 0) {
			newPoints.push_back({ myPoint.x,myPoint.y,i });
		}
	}
	return newPoints;
}


void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues){
	for (int i = 0; i < newPoints.size(); i++)	
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 
			10, myColorValues[newPoints[i][2]], FILLED);	
}

void main() {
	
	while (true) {
		capture.read(img);
		newPoints = findColor(img);
		drawOnCanvas(newPoints, colorValues);
		flip(img, img, 1);
		imshow("VPen", img);
		if (waitKey(1) == 'q')
			break;
	}
}