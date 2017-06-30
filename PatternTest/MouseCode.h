#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>

class MouseCode
{
public:
	enum Project { PROJECT_DIDFE = 0, PROJECT_ONEID = 1 };
	MouseCode(Project project, int ticket_number);
	~MouseCode();
	static std::vector<MouseCode> process_image(const char* source);
private:
	static cv::Point point_for_angle(int radius, double angle);
};

