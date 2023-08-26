#include"../headers/court_detection.h"
#include "../headers/segmentation.h"

void box_elimination(cv::Mat image, cv::Mat img_out, std::string str)
{	
	//file with boxes 
	std::ifstream file(str);

	if (file.is_open()) {
		
		std::string line;
		
		//take the string with the position of bounding box
		while (std::getline(file, line)) {
			int x,y,w,h;

			std::istringstream iss(line);
			
			iss >> x >> y >> w >> h;
			/*
			std::cout << x << std::endl;
			std::cout << y << std::endl;
			std::cout << w << std::endl;
			std::cout << h << std::endl;
			*/

			//delete the box 
			for (int i = x; i <= x+w; i++) {
				for (int j = y; j <= y+h; j++) {
					
					img_out.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 0, 0);

				}
			}
		}

	}else {
		std::cout << "error path\n";
	}

	//visualize the image
	cv::imshow("image w/out boxes", img_out);
	cv::waitKey(0);
	
}

void fill_image(cv::Mat image){
	//start filling the image
	
	
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			//i fill the holes left by the remove of boxes
			if (image.at<cv::Vec3b>(i, j)==cv::Vec3b(0,0,0)) {
				cv::Vec3b color = image.at<cv::Vec3b>(i , j-1);
				image.at<cv::Vec3b>(i, j) = color;
			}
		}
	}

	//visualize the image
	cv::imshow("image w/out boxes", image);
	cv::waitKey(0);

}




void color_quantization(cv::Mat image,cv::Mat& img_out) {


	//segmentation(image, image);


	int numClusters = 2; // Number of desired colors after quantization
	cv::Mat labels, centers;
	cv::TermCriteria criteria=	cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.1);
	

	cv::Mat floatImage, clustered ;
	image.convertTo(floatImage, CV_32FC3, 1.0 / 255.0);

	cv::kmeans(floatImage.reshape(1, floatImage.rows * floatImage.cols), numClusters, labels, criteria, 10, cv::KMEANS_PP_CENTERS,centers);

	//replace colors 
	
	

	// Define replacement colors
	cv::Vec3b colors[2];

	colors[0] = cv::Vec3b(255, 0, 0); // Red
	colors[1] = cv::Vec3b(0, 0, 255); // Blue
	clustered = cv::Mat(image.rows, image.cols, CV_8UC3);
	
	for (int i = 0; i < image.cols * image.rows; i++) {
		
		int el=labels.at<int>(i,0);
		//std::cout << el<<std::endl;
		clustered.at<cv::Vec3b>(i / image.cols, i % image.cols)= colors[el] ;
	
	}

	//cv::Mat converted;
	//clustered.convertTo(converted, CV_8U);
	//clustered.convertTo(clustered, CV_8U);
	//cv::imshow("Original Image", image);
	cv::imshow("Quantized Image", clustered);
	cv::waitKey(0);
	img_out = clustered.clone();

}

//it is used to find lines on 
void field_distinction(cv::Mat clustered,cv::Mat& segmented_field) {
	
	//colour used to distict field and background
	cv::Vec3b green(0,255,0);
	
	cv::Mat mask1,mask2;
	cv::inRange(clustered, cv::Vec3b(255, 0, 0), cv::Vec3b(255, 0, 0),mask1);
	
	cv::inRange(clustered, cv::Vec3b(0, 0,255), cv::Vec3b(0, 0, 255), mask2);

	int pixel_count1 = cv::countNonZero(mask1);
	int pixel_count2 = cv::countNonZero(mask2);

	
	if (pixel_count2 > pixel_count1) {
		
		segmented_field.setTo(green, mask2);
		segmented_field.setTo(cv::Vec3b(0,0,0), mask1);
	}
	else {

		segmented_field.setTo(green, mask1);
		segmented_field.setTo(cv::Vec3b(0, 0, 0), mask2);

	}
	cv::imshow("segmented field", segmented_field);
	cv::waitKey(0);
}


//localize the field lines by using hough transform not used 
void lines_detector(cv::Mat image) {


	cv::Mat edge;

	court_localization(image,edge);

	cv::Mat grey_edge;
	// Copy edges to the images that will display the results in BGR
	cvtColor(edge, grey_edge, cv::COLOR_GRAY2BGR);

	// Standard Hough Line Transform
	std::vector<cv::Vec2f> lines; // will hold the results of the detection
	HoughLines(edge, lines, 1, CV_PI / 180, 180, 0, 0); // runs the actual detection

	// Draw the lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(image, pt1, pt2, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
	}

	imshow("Detected Lines (in red) - Standard Hough Line Transform", image);
	cv::waitKey(0);
}


//localizza il contorno non usato
void court_localization(cv::Mat image, cv::Mat& edge) {


	//start by using canny to localize the lines 
	cv::Mat blur_img;

	cv::GaussianBlur(image, blur_img, cv::Size(9, 9), 0.5, 0.5);

	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y, test_grad;

	cv::Sobel(blur_img, grad_x, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
	cv::Sobel(blur_img, grad_y, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(grad_x, abs_grad_x);
	cv::convertScaleAbs(grad_y, abs_grad_y);
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, test_grad);
	//Compute the median of the gradient magnitude
	cv::Scalar mean, stddev;
	cv::meanStdDev(test_grad, mean, stddev);
	double median = mean[0];
	int canny_c = 7;
	std::cout << "Median: " << median << std::endl;
	//cv::Mat edges;


	cv::Canny(image, edge, (canny_c * median) / 2, canny_c * median);

	cv::imshow("edges", edge);
	cv::waitKey(0);
}