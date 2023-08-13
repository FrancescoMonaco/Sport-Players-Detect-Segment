// CV_Project.cpp : Francesco Pio Monaco

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include <string>
#include <iostream>
#include <vector>


#include "Utility.h"

//Constants, later to be moved to a config file
const std::string num_path = "D:/Download/process_data/Rec_templates";
const std::string part_path = "D:/Download/process_data/Body_templates";

const double color_variation_threshold = 10.0;
int orange_lower_bound = 120;
int orange_upper_bound = 255;

int main()
{
    //Put all images in a vector using glob
    std::vector<cv::Mat> images;
    std::vector<cv::Mat> numbers;
   // std::vector<cv::Mat> parts;

    std::string path = "D:/Download/Sport_scene_dataset/Images/*.jpg"; //select only jpg
    std::vector<cv::String> fn;
    std::vector<cv::String> fn2;
    std::vector<cv::String> fn3;
    cv::glob(path, fn, true); // recurse
    cv::glob(num_path, fn2, true); // recurse
    //cv::glob(part_path, fn3, true); // recurse

    for (size_t k = 0; k < fn.size(); ++k)
    {
        cv::Mat im = cv::imread(fn[k]);
        if (im.empty()) continue; //only proceed if successful
        images.push_back(im);
    }

    for (size_t k = 0; k < fn2.size(); ++k)
    {
        cv::Mat im = cv::imread(fn2[k], cv::IMREAD_GRAYSCALE);
        if (im.empty()) continue; //only proceed if successful
        numbers.push_back(im);

    }

    //for (size_t k = 0; k < fn3.size(); ++k) {
    //    cv::Mat im = cv::imread(fn3[k], cv::IMREAD_GRAYSCALE);
    //    if (im.empty()) continue;
    //    parts.push_back(im);
    //}

    for (auto& test : images)
    {
        //Create a copy of the image to work on
        cv::Mat test_copy = test.clone();
        //Detect edges
        cv::Mat edges;
        //Do a strong blur before canny
        cv::GaussianBlur(test, test, cv::Size(5, 5), 0);
        cv::Canny(test, edges, 100, 200);
        cv::imshow("edges", edges);
           //Detect contours
           std::vector<std::vector<cv::Point>> contours;
           cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
           bool doEqualization = hasOrangeColor(test, orange_lower_bound, orange_upper_bound);
        //Find the numbers in the image using template matching, there may be the same number multiple times
           for (auto& number : numbers)
           {
               //Create a grayscale version of test for template matching
               cv::Mat test_gray;
               cv::cvtColor(test, test_gray, cv::COLOR_BGR2GRAY);
               // equaize the gray image only if the original has orange in it
               if (doEqualization)
               {
				   cv::equalizeHist(test_gray, test_gray);
			   }
               cv::Mat result;
               //if the template is bigger than the image, skip it
               if (number.cols > test_gray.cols || number.rows > test_gray.rows)
               {
				   continue;
			   }
			   cv::matchTemplate(test_gray, number, result, cv::TM_CCORR_NORMED);
			   cv::threshold(result, result, 0.50, 1.0, cv::THRESH_TOZERO);
                //Put rectangles around the found numbers
               while (true)
               {
				   double minVal, maxVal;
				   cv::Point minLoc, maxLoc;
				   cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

                   // Calculate color variation within the detected rectangle
                   cv::Mat detected_region = test(cv::Rect(maxLoc.x, maxLoc.y, number.cols, number.rows));
                   cv::Scalar mean_color, stddev_color;
                   cv::meanStdDev(detected_region, mean_color, stddev_color);
                   double color_variation = stddev_color[0]; // Assuming grayscale image

                   if (maxVal >= 0.85 && color_variation > color_variation_threshold)
                   {
                       //Put the rectangle on the image only if it doesn't contain a semi uniform color

					   cv::rectangle(test, maxLoc, cv::Point(maxLoc.x + number.cols, maxLoc.y + number.rows), cv::Scalar(0, 0, 255), 2);
					   cv::floodFill(result, maxLoc, cv::Scalar(0), 0, cv::Scalar(0.1), cv::Scalar(1.0));
				   }
                   else
                   {
					   break;
				   }
			   }
           }
       //    cv::imshow("test", test);





        // Blur the image to reduce noise using a bilateral filter
  //      cv::Mat blurred;
  //      cv::bilateralFilter(test, blurred, 9, 75, 75);
  //      cv::imshow("blurred", blurred);
  //      //Detect blobs, we want to detect human bodies
  //      cv::SimpleBlobDetector::Params params;
  //      params.filterByArea = true;
  //      params.minArea = 100;
  //      params.maxArea = 1000;
  //      params.filterByCircularity = true;
  //      params.maxCircularity = 0.4;
  //      params.minCircularity = 0.1;
  //      params.filterByConvexity = false;
  //      params.filterByInertia = true;
  //      params.minInertiaRatio = 0.01;
  //      params.maxInertiaRatio = 0.3;
  //      params.filterByColor = false;
  //      params.blobColor = 255;
  //      cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
  //      // Detect blobs at different scales using default parameters
  //      std::vector<cv::KeyPoint> keypoints;

  //      for (double scale = 1.0; scale < 2.0; scale += 1.0)
  //      {
		//	cv::Mat resized;
		//	cv::resize(blurred, resized, cv::Size(), scale, scale);
  //          std::vector<cv::KeyPoint> Scaledkeypoints;
		//	detector->detect(resized, Scaledkeypoints);
  //          //Convert keypoints into original scale
  //          for (auto& keypoint : Scaledkeypoints)
  //          {
  //              keypoint.pt *= 1.0 / scale;
  //              keypoints.push_back(keypoint);
  //          }
		//}
  //      //Draw detected blobs as red circles.
  //      //DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
  //      cv::Mat result_keypoints;
  //      cv::drawKeypoints(test, keypoints, result_keypoints, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  //      cv::imshow("result_keypoints", result_keypoints);
        //Use HOGDescriptor to detect people
        cv::HOGDescriptor hog;
        hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
        std::vector<cv::Rect> found, found_filtered;
        hog.detectMultiScale(test_copy, found, 0, cv::Size(8, 8), cv::Size(32, 32), 1.05, 2);
        size_t i, j;
        for (i = 0; i < found.size(); i++)
        {
			cv::Rect r = found[i];
			for (j = 0; j < found.size(); j++)
				if (j != i && (r & found[j]) == r)
					break;
			if (j == found.size())
				found_filtered.push_back(r);
		}
        //Filter the rectangles based on std deviation of color and mean color
        //for (i = 0; i < found_filtered.size(); i++)
        //{
        //    cv::Rect r = found_filtered[i];
        //    //Calculate color variation within the detected rectangle
        //    cv::Mat detected_region = test(cv::Rect(r.x, r.y, r.width, r.height));
        //    cv::Scalar mean_color, stddev_color;
        //    cv::meanStdDev(detected_region, mean_color, stddev_color);
        //    double color_variation = stddev_color[0]; // Assuming grayscale image
        //    if (color_variation > color_variation_threshold)
        //    {
        //        //Put the rectangle in the vector only if it doesn't contain a semi uniform color
        //        found_filtered[i] = r;
        //    }
        //    else
        //    {
        //        found_filtered.erase(found_filtered.begin() + i);
        //        i--;
        //    }
        //}
        ////If the rectangles are overlapping, merge them
        //for (i = 0; i < found_filtered.size(); i++)
        //{
        //    cv::Rect r = found_filtered[i];
        //    for (j = 0; j < found_filtered.size(); j++)
        //        if (j != i && (r & found_filtered[j]) == r)
        //            break;
        //    if (j != found_filtered.size())
        //    {
        //        found_filtered.erase(found_filtered.begin() + i);
        //        i--;
        //    }
        //}



        for (i = 0; i < found_filtered.size(); i++)
        {

            cv::Rect r = found_filtered[i];
            //The HOG detector returns slightly larger rectangles than the real objects.
            //So we slightly shrink the rectangles to get a nicer output.
            r.x += cvRound(r.width * 0.1);
            r.width = cvRound(r.width * 0.8);
            r.y += cvRound(r.height * 0.07);
            r.height = cvRound(r.height * 0.8);

            cv::rectangle(test, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
        }


        cv::imshow("test", test);


        cv::waitKey(0);
    }
}
