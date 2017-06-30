#include "MouseCode.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

const char* MOUSE_IMAGE_PATH = "C:\\Users\\mattc\\Desktop\\temp\\mouse.jpg";
const int THRESH = 100;
const int BORDER_WIDTH = 100;
const int CENTER_X = 153;
const int CENTER_Y = 159;
const double ROW_0_RADIUS_PADDING_MULTIPLIER = 1.25;
const double ROW_1_RADIUS_PADDING_MULTIPLIER = 1.45;
const int ROW_0_DOTS = 10;
const int ROW_1_DOTS = 10;
const double RADIUS_RANGE = 180;
#define PI 3.14159265

MouseCode::MouseCode(Project project, int ticket_number)
{
	Mat mouse = imread(MOUSE_IMAGE_PATH);
	Mat out(mouse.rows + BORDER_WIDTH * 2, mouse.cols + BORDER_WIDTH * 2, mouse.depth());
	copyMakeBorder(mouse, out, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_CONSTANT, Scalar(255, 255, 255));

	const int mouse_radius = mouse.rows - CENTER_Y;
	//circle(out, point_for_angle(mouse_radius, 0), 5, Scalar(255, 0, 0), 2);
	
	// fill al dots, for testing
	for (int i = 0; i < ROW_0_DOTS + ROW_1_DOTS; i++)
	{
		int radius = mouse_radius * (i < ROW_0_DOTS ? ROW_0_RADIUS_PADDING_MULTIPLIER : ROW_1_RADIUS_PADDING_MULTIPLIER);
		double dot_angle_increment = RADIUS_RANGE / (i < ROW_0_DOTS ? ROW_0_DOTS - 1 : ROW_1_DOTS - 1);
		double position = (i < ROW_0_DOTS ? i - 0.25 : i - ROW_0_DOTS + 0.25);
		double angle = position * dot_angle_increment;
		Point p = point_for_angle(radius, angle);
		cout << angle << endl;
		circle(out, p, 10, Scalar(0, 0, 0), -1);
	}
	imwrite("C:\\Users\\mattc\\desktop\\temp\\mickey_dots.jpg", out);
	imshow("out", out);
}

Point MouseCode::point_for_angle(int radius, double angle)
{
	int x = radius * cos(angle * PI / 180);
	int y = radius * sin(angle * PI / 180);
	//return Point(BORDER_WIDTH + CENTER_X, BORDER_WIDTH + radius + CENTER_Y);
	return Point(BORDER_WIDTH + CENTER_X + x, BORDER_WIDTH + CENTER_Y + y);
}

MouseCode::~MouseCode()
{
}

vector<MouseCode> MouseCode::process_image(const char* source)
{
	vector<MouseCode> mouse_codes;
	RNG rng(12345);

	Mat src, src_gray;
	Mat mouse, mouse_gray;

	src = imread(source, 1);
	mouse = imread(MOUSE_IMAGE_PATH, 1);
	
	cvtColor(src, src_gray, CV_BGR2GRAY);
	cvtColor(mouse, mouse_gray, CV_BGR2GRAY);

	Mat canny_output;
	Mat mouse_canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Vec4i> mouse_hierarchy;
	vector<vector<Point> > mouse_contours;

	/// Detect edges using canny
	Canny(src_gray, canny_output, THRESH, THRESH * 2, 3);
	Canny(mouse_gray, mouse_canny_output, THRESH, THRESH * 2, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	findContours(mouse_canny_output, mouse_contours, mouse_hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		// Match contours
		double result = matchShapes(contours[i], mouse_contours[1], CV_CONTOURS_MATCH_I1, 0);
		//cout << "Match? " << result << endl;
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(src, contours, i, color, 2, 8, hierarchy, 0, Point());

		if (result < 0.04 && result > 0) {
			//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			//drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			//drawContours(src, contours, i, color, 2, 8, hierarchy, 0, Point());

			RotatedRect rRect = minAreaRect(contours[i]);
			Point2f vertices[4];
			rRect.points(vertices);
			for (int i = 0; i < 4; i++)
				line(src, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));

			Mat M, rotated, cropped;
			// get angle and size from the bounding box
			float angle = rRect.angle;
			Size rect_size = rRect.size;
			
			cout << "angle: " << angle << endl;

			if (rRect.angle < -45.) {
				angle += 90.0;
				swap(rect_size.width, rect_size.height);
			}

			// get the rotation matrix
			M = getRotationMatrix2D(rRect.center, angle, 1.0);
			// perform the affine transformation
			warpAffine(src, rotated, M, src.size(), INTER_CUBIC);
			// crop the resulting image
			rect_size.height += 150;
			rect_size.width += 150;
			getRectSubPix(rotated, rect_size, rRect.center, cropped);
			char buffer[255];

			sprintf(buffer, "cropped%i", i);
			imshow(buffer, cropped);
		}

	}
	//namedWindow("src", CV_WINDOW_NORMAL);
	//imshow("src", src);
	//resizeWindow("src", 1500, 2500);
	/*Mat drawing2 = Mat::zeros(mouse_canny_output.size(), CV_8UC3);
	for (int i = 0; i < mouse_contours.size(); i++)
	{
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	drawContours(drawing2, mouse_contours, i, color, 2, 8, mouse_hierarchy, 0, Point());
	}*/

	return mouse_codes;
}