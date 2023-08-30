// CV_Project.cpp : Francesco Pio Monaco

#include "../CV_Project/headers/Utility.h" 
#include "../CV_Project/headers/header.h"
#include "../CV_Project/headers/court_detection.h"
#include "../CV_Project/headers/player_detection.h"
#include "../CV_Project/headers/write_results.h"

// ***STRING CONSTANTS***
const std::string rel_path = "D:/Download/Sport_scene_dataset";
const std::string partial = "/ProcessedBoxes/";
const std::string complete = "/Predictions/";


// COSE DA FARE
// 1) linee nel watershed
// 2) 

// ***MAIN***
int main()
{

    std::string path = rel_path + "/Images/*.jpg"; //select only jpg

    // Load images
    std::vector<cv::Mat> images;
    std::vector<cv::String> fn;
    cv::glob(path, fn, true);

    for (size_t k = 0; k < fn.size(); ++k)
    {
        cv::Mat im = cv::imread(fn[k]);
        if (im.empty()) continue; //only proceed if successful
        images.push_back(im);
    }

    // BEGIN OF THE PROCESSING PIPELINE
    int savenum= 0;
    for (auto& test : images)
    {
        //Create a copy of the image to work on
        cv::Mat test_copy = test.clone();



    }
    std::vector<BoundingBox> processedData = loadBoundingBoxData(rel_path + "/Masks", true);
    //Reorganize the vector into a vector of vectors of BoundingBoxes
    std::vector<std::vector<cv::Rect>> processedData2 = reshapeBB(processedData);

    // for each image, keep also the relative fn during the loop
    for (size_t k = 0; k < images.size(); k++) {
        //pick the fn of the image
         cv::String fn2 = fn[k];
         int num = extractNumber(fn2);
         std::cout << num << std::endl;
         std::cout << "Boxes before cleaning: " << processedData2[num-1].size() << std::endl;
         //cleanRectangles(processedData2[k], images[k]);
         //std::cout << "Boxes after cleaning: " << processedData2[k].size() << std::endl;
         //show the image with the boxes
   //      for (auto& r : processedData2[k]) {
			// cv::rectangle(images[k], r, cv::Scalar(0, 255, 0), 2);
		 //}

         std::string boxes = rel_path + "Images/im" + std::to_string(num);

         //string to take the image
         std::string img_path = boxes + ".jpg";

         //string to save segmentation ground truth mask
         std::string bin = boxes + "_bin.png";

         //string to save the same mask, with a different color coding
         std::string img_save = boxes + "_color.jpg";

         //bounding boxes for player detection
         boxes = boxes + "_bb.txt";

         //test settings
         boxes = rel_path + "/Masks/im" + std::to_string(num);
         boxes = boxes + "_bb.txt";

         cv::Mat  image_box = images[k].clone();


         cv::Mat seg_image(image_box.size(), CV_8UC1);
         //player_segmentation(image_box, seg_image, boxes);

         cv::Mat mask, clustered;
         //player_elimination(image_box, seg_image, mask);

         //clustering(seg_image, clustered);

         //put image_box in and with the mask and show it
         //cv::Mat final_image;
         //image_box.copyTo(final_image, mask);
         //cv::imshow("Final image", mask);
         //cv::waitKey(0);
         cv::Vec2f line;
         bool val = line_refinement(image_box, line);
         std::cout << "Line: " << val << std::endl;

         //Create a copy of the image to work on

        // cv::Mat  image_box = images[k].clone();


         //eliminate boxes inside the image to have a better field detection 
        // box_elimination(image_box, image_box, boxes);

         //cover the holes left by the remotion of the boxes
         //fill_image(image_box);

         //clusterize the image in order to detect the field and the background  
         //cv::Mat clustered;
        // color_quantization(image_box, clustered);

         //assign specific label for the test task
         cv::Mat segmented_field = clustered.clone();
         //field_distinction(clustered, clustered, segmented_field);


         //start players segmentation
         //player_segmentation(images[k], image_box, boxes);

         //unire cose di player segmentation and field 

         //save bin
        // cv::Mat save_bin (segmented_field.rows, segmented_field.cols,CV_8UC1);
        // write_segmentation_results(segmented_field,save_bin,bin);


   //      std::vector<int> labels = classify(images[k], processedData2[num-1]);

   //      //for each box draw it on the image using different color for the labels
   //      for (size_t i = 0; i < processedData2[num-1].size(); i++) {
			// cv::rectangle(images[k], processedData2[num-1][i], cv::Scalar(0, 255, 0), 2);
			// cv::putText(images[k], std::to_string(labels[i]), cv::Point(processedData2[num-1][i].x, processedData2[num-1][i].y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
		 //}


         cv::imshow("Image", images[k]);
		 cv::waitKey(0);
    }


    /*
    //Evaluation Part
    //std::vector<BoundingBox> resultData = loadBoundingBoxData("D:/Download/Sport_scene_dataset/Masks");
    //std::vector<BoundingBox> predData = loadBoundingBoxData("D:/Download/Sport_scene_dataset/Masks");
    float result = processBoxPreds(resultData, resultData);
    std::cout << "mAP: " << result << std::endl;

    std::vector<cv::Mat> resultData2 = loadSemanticSegmentationData("D:/Download/Sport_scene_dataset/Masks");
    float result2 = processSemanticSegmentation(resultData2, resultData2);
    std::cout << "IoU: " << result2 << std::endl;

    //Show results, uncomment to show
    showResults("D:/Download/Sport_scene_dataset/Images", "D:/Download/Sport_scene_dataset/Masks");
    */
}