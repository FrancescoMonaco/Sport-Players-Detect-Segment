#ifndef WRITE_RESULTS_H
#define WRITE_RESULTS_H

#include "header.h"


void write_segmentation_results(cv::Mat segmented_image,cv::Mat saved_bin,std::string save);

#endif // !WRITE_RESULTS_H