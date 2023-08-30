#ifndef COURT_DETECTION_H
#define COURT_DETECTION_H
#include "header.h"

void player_elimination(cv::Mat image, cv::Mat& img_out, cv::Mat mask);

void fill_image(cv::Mat& image);

void color_quantization(cv::Mat image, cv::Mat& clustered);

void field_distinction(cv::Mat image_box, cv::Mat clustered, cv::Mat& segmented_field);

void merge_clusters(cv::Mat& labels, cv::Mat& centers, float merge_threshold);

//actually not used 
void lines_detector(cv::Mat image);

//actually not used
void court_localization(cv::Mat image, cv::Mat& edges);

/// @brief Finds the line that divides the playing field from the background
/// @param image where to find the line
/// @param longest_line the line that divides the playing field from the background if found
/// @return true if the line is found, false otherwise
bool line_refinement(cv::Mat& image, cv::Vec2f& longest_line);

#endif // !COURT_DETECTION_H
