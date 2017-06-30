

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "MouseCode.h"

using namespace cv;
using namespace std;

/// Function header
void thresh_callback(int, void*);

/** @function main */
int main(int argc, char** argv)
{
	MouseCode::process_image("C:\\Users\\mattc\\Desktop\\temp\\board6.jpg");
	//MouseCode x = MouseCode(MouseCode::PROJECT_ONEID, 10);
	waitKey(0);
	return(0);
}
