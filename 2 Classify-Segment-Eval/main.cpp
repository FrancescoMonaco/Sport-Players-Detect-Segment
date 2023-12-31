// main.cpp : Francesco Pio Monaco

#include "headers/Utility.h" 
#include "headers/header.h"
#include "headers/court_detection.h"
#include "headers/player_detection.h"

// ***STRING CONSTANTS***
const std::string partial = "/ProcessedBoxes/";
const std::string complete = "/Predictions";
const std::string mask_path = "/Masks";

// ***MAIN***
int main(int argc, char** argv)
{
    // LOAD ZONE
        // Check if the number of arguments is correct
    if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <relative_path>" << std::endl;
		return -1;
	}
        // Take the rel path from argv
    std::string rel_path = argv[1];

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
    std::cout << "------\nStarting image processing pipeline" << std::endl;

        // Take the raw bounding boxes
    std::vector<BoundingBox> processedData = loadBoundingBoxData(rel_path + "/ProcessedBoxes", false);
        // Reorganize the vector into a vector of vectors of Rects
    std::vector<std::vector<cv::Rect>> processedData2 = reshapeBB(processedData);

    
        // For each test image
    for (size_t k = 0; k < images.size(); k++) {

        // Pick the fn of the image
         cv::String fn2 = fn[k];
         int num = extractNumber(fn2);
         
         // Clean the boxes
         std::cout << "*** Image " << num << " ***" << std::endl;
         std::cout << "Cleaning the boxes" << std::endl;
         cleanRectangles(processedData2[num-1], images[k]);


        // Classify the boxes
        std::cout << "Classifying" << std::endl;
        std::vector<int> labels_BB = classify(images[k], processedData2[num - 1]);

        // Write the boxes to a file
        writeBB(images[k], processedData2[num-1], labels_BB, rel_path + complete + "/im" + std::to_string(num) + "_bb.txt");


         // Strings to save the files
         std::string boxes = rel_path + "/Predictions/im" + std::to_string(num) + "_bb.txt";
         std::string seg_bin_file = rel_path + complete + "/im" + std::to_string(num) + "_bin.png";
         std::string seg_color_file = rel_path + complete + "/im" + std::to_string(num) + "_color.png";

         // Copy the image to work on it
         cv::Mat  image_box = images[k].clone();

         // Start player segmentation
         std::cout << "Segmenting the players" << std::endl;
         cv::Mat seg_image(image_box.size(), CV_8UC1);

         // ***Choose segmentation approach***
         //
         // if approach = true perform standard segmentation
         // else perform the more robust approach
         //
         bool approach = true;

         if (approach) {
             player_segmentation(image_box, seg_image, boxes);
         }
         else {
             player_segmentation_robust(image_box, seg_image, boxes);
         }

         // Variables for the field segmentation
         cv::Mat mask, clustered, centroid;

         // Eliminate boxes inside the image to have a better field detection 
         std::cout << "Segmenting the field" << std::endl;
          player_elimination(image_box, mask, seg_image);
         // Field segmentation
          color_quantization(image_box, clustered, centroid);
          cv::Mat segmentation = clustered.clone();
          field_distinction(image_box, clustered, segmentation);

          cv::Vec2f line;
          bool val = line_refinement(image_box, line);
          if (val) {
              court_segmentation_refinement(segmentation, line);
          }
          // Merge the two segmentations
          unifySegmentation(segmentation, seg_image, processedData2[num - 1], labels_BB);

          // Save the segmentation
          std::cout << "Saving the results" << std::endl;
          cv::Mat segmentation_bin = cv::Mat::zeros(segmentation.rows, segmentation.cols, CV_8UC1);
          createSegmentationPNG(segmentation, segmentation_bin);
          
          writeSEG(segmentation_bin, seg_bin_file);
          writeSEG(segmentation, seg_color_file);
    } 
    
   
   
    // EVALUATION PIPELINE
    std::cout << "------\nEvaluation Pipeline" << std::endl;
        // Bounding Box Evaluation
    std::vector<BoundingBox> resultData = loadBoundingBoxData(rel_path + mask_path);
    std::vector<BoundingBox> resultData_rev = loadBoundingBoxData(rel_path + mask_path, true, true);
    std::vector<BoundingBox> predData = loadBoundingBoxData(rel_path + complete);
    float result_bb = processBoxPreds(resultData, predData);
    float result_bb_rev = processBoxPreds(resultData_rev, predData);

        // AP for each image
    singleImageAP(resultData, resultData_rev, predData, images.size());

        // Semantic Segmentation Evaluation
    std::vector<cv::Mat> segmentationGOLD = loadSemanticSegmentationData(rel_path + mask_path);
    std::vector<cv::Mat> segmentationGOLD_REV = loadSemanticSegmentationData(rel_path + mask_path, true);
    std::vector<cv::Mat> segmentationPRED = loadSemanticSegmentationData(rel_path + complete);
    std::cout << "Semantic Segmentation Eval" << std::endl;
    float result_seg = processSemanticSegmentation(segmentationGOLD, segmentationPRED);
    std::cout << "Semantic Segmentation Reverse Eval" << std::endl;
    float result_seg_rev = processSemanticSegmentation(segmentationGOLD_REV, segmentationPRED);

    std::cout << "------\n";
    std::cout << "mAP: " << std::max(result_bb, result_bb_rev) << std::endl;
    std::cout << "IoU: " << std::max(result_seg, result_seg_rev) << std::endl;

    // ***Show results, uncomment to show***
    //showResults(rel_path + "/Images", rel_path + complete);
    
}