#pragma once
#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_

#include "opencv2/imgproc/imgproc.hpp"
#include <GL/glew.h>
// #include <GLFW/glfw3.h>

// #include "GLSL.h"

int detectThirds(cv::Mat src);

cv::Mat ocvImgFromGlTex(GLuint glTexID);

#endif