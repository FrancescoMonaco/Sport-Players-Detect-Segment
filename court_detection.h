#pragma once
#include "header.h"

void box_elimination(cv::Mat image,cv::Mat img_out,std::string str );

void fill_image(cv::Mat& image);

void color_quantization(cv::Mat image,cv::Mat& clustered);

void field_distinction(cv::Mat image_box,cv::Mat clustered,cv::Mat& segmented_field);

void merge_clusters(cv::Mat& labels, cv::Mat& centers, float merge_threshold);

//actually not used 
void lines_detector(cv::Mat image);

//actually not used
void court_localization(cv::Mat image, cv::Mat& edges);

double standard_deviation(cv::Mat box_image, cv::Mat mask);