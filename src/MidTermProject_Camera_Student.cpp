/* INCLUDES FOR THIS PROJECT */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "dataStructures.h"
#include "matching2D.hpp"

// to work with directories
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
// #include <boost/filesystem.hpp>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

/* MAIN PROGRAM */
int main(int argc, const char **argv)
{

    bool bStepsOp = false;
    bool bWaitKey = false;
    bool bProcessAll = false;
    bool bSaveImg = false;
    string detectorType = "HARRIS";               // SHITOMASI, HARRIS, FAST, BRISK, ORB, AKAZE, SIFT
    string descriptorType = "SIFT";               // BRISK, BRIEF, ORB, FREAK, AKAZE, SIFT
    string matcherType = "MAT_FLANN";             // MAT_BF, MAT_FLANN
    string descriptorTypeMatching = "DES_BINARY"; // DES_BINARY, DES_HOG
    string selectorType = "SEL_KNN";              // SEL_NN, SEL_KNN

    vector<string> detectors;
    vector<string> descriptors;
    if (bProcessAll)
    {
        detectors = {"SHITOMASI", "HARRIS", "FAST", "BRISK", "ORB", "AKAZE", "SIFT"};
        descriptors = {"BRISK", "BRIEF", "ORB", "FREAK", "AKAZE", "SIFT"};
    }
    else
    {
        detectors = {detectorType};
        descriptors = {descriptorType};
    }
// output the detector and descriptors that will be executed
    // 
    int idx = 1;
    for (string det : detectors)
    {
        // cout << det << " ";

        for (string desc : descriptors)
        {
            cout << idx << "\t";
            cout << det << "\t";
            cout << desc << "\t";
            cout << endl;
            idx++;
        }
    }
    cout << endl;

    /// WAS USED TO CHECK COMMAND LINE ARGUMENTS BUT NOT USED NOW
    // cout << detectorType << ", " << descriptorType << ": ";
    // parse arguements to set the keypoint algorithms

    // cout << "arguments: " << argc << endl;
    // int numOfArg = 3; // additional arguement is passed by default for pwd

    // if (argc < 2)
    // {
    //     cout << "No arguments passed, using default algorithms" << endl;
    // }
    // else if (argc < numOfArg)
    // {
    //     cout << "less than " << numOfArg << " arguments passed, using default algorithms" << endl;
    // }
    // else if (argc == numOfArg)
    // {
    //     cout << "valid number of arguements in order to set algorithms" << endl;
    // }
    // for (int i = 0; i < argc; i++)
    // {
    //     cout << "arg #" << i << "\targv: " << argv[i] << endl;
    // }

    // std::vector <std::string> arguments;
    // // std::string destination;
    // for (int i = 1; i < argc; ++i) { // Remember argv[0] is the path to the program, we want from argv[1] onwards
    //     if (i + 1 < argc)
    //         sources.push_back(argv[i]); // Add all but the last argument to the vector.
    //     else
    //         destination = argv[i];
    // }

    /* INIT VARIABLES AND DATA STRUCTURES */
    cout << "det: " << endl;
    idx = 1;


    for (string det : detectors)
    {

        int detectorPassIdx = 0; // how many times this detector has been run.
        // cout << det << " ";
        detectorType = det;
        for (string desc : descriptors)
        {
            // only do AKAZE descriptor with AKAZE detector
            if ((desc == "AKAZE") && (det != "AKAZE"))
            {
                continue;
            }
            // https://knowledge.udacity.com/questions/105392
            if ((desc == "ORB") && (det == "SIFT"))
            {
                continue;
            }
            if (desc == "SIFT")
            {
                descriptorTypeMatching = "DES_HOG";
            }
            descriptorTypeMatching = "DES_BIN";

            descriptorType = desc;
            cout << idx << "\t";
            cout << det << "\t";
            cout << desc << "\t";
            cout << endl;
            idx++;

            // data location
            string dataPath = "../";

            // camera
            string imgBasePath = dataPath + "images/";
            string imgPrefix = "KITTI/2011_09_26/image_00/data/000000"; // left camera, color
            string imgFileType = ".png";
            int imgStartIndex = 0; // first file index to load (assumes Lidar and camera names have identical naming convention)
            int imgEndIndex = 9;   // last file index to load
            int imgFillWidth = 4;  // no. of digits which make up the file index (e.g. img-0001.png)

            // misc
            int dataBufferSize = 2;       // no. of images which are held in memory (ring buffer) at the same time
            vector<DataFrame> dataBuffer; // list of data frames which are held in memory at the same time
            bool bVis = false;            // visualize results



            // iterate over detectors and descriptors

            /* MAIN LOOP OVER ALL IMAGES */

            for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
            {
                /* LOAD IMAGE INTO BUFFER */
                // cout << "----------- IMAGE " << imgIndex << " -----------" << endl;
                cout << "IMAGE " << imgIndex << "\t";// << endl;
                // assemble filenames for current index
                ostringstream imgNumber;
                imgNumber << setfill('0') << setw(imgFillWidth) << imgStartIndex + imgIndex;
                string imgFullFilename = imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

                // load image from file and convert to grayscale
                cv::Mat img, imgGray;
                img = cv::imread(imgFullFilename);
                cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

                //// STUDENT ASSIGNMENT
                //// TASK MP.1 -> replace the following code with ring buffer of size dataBufferSize

                // push image into data frame buffer
                DataFrame frame;
                frame.cameraImg = imgGray;
                    dataBuffer.push_back(frame);
                    if (dataBuffer.size() > dataBufferSize)
                        dataBuffer.erase(dataBuffer.begin());


                //// EOF STUDENT ASSIGNMENT
                if (bStepsOp)
                    cout << "#1 : LOAD IMAGE INTO BUFFER done" << endl;

                /* DETECT IMAGE KEYPOINTS */

                // extract 2D keypoints from current image
                vector<cv::KeyPoint> keypoints; // create empty feature list for current image
                // string detectorType = "SHITOMASI";
                // string detectorType = "HARRIS";

                //// STUDENT ASSIGNMENT
                //// TASK MP.2 -> add the following keypoint detectors in file matching2D.cpp and enable string-based selection based on detectorType
                //// -> HARRIS, FAST, BRISK, ORB, AKAZE, SIFT


                // RUN THE DETECTORS
                if (detectorType.compare("SHITOMASI") == 0)
                {
                    detKeypointsShiTomasi(keypoints, imgGray, false);
                }
                else if (detectorType.compare("HARRIS") == 0)
                {
                    detKeypointsHarris(keypoints, imgGray, false);
                }
                else // anything else, call the mdern implementations
                {
                    // cout << "anything else" << endl;
                    detKeypointsModern(keypoints, imgGray, detectorType, false);
                }


                //// EOF STUDENT ASSIGNMENT

                //// STUDENT ASSIGNMENT
                //// TASK MP.3 -> only keep keypoints on the preceding vehicle

                // only keep keypoints on the preceding vehicle

                bool bFocusOnVehicle = true;
                cv::Rect vehicleRect(535, 180, 180, 150); // topleft x, topleft y, width, height

                if (bFocusOnVehicle)
                {
                    // to help debugging
                    int eraseCnt = 0;
                    int keepCnt = 0;
                    for (auto it = keypoints.begin(); it < keypoints.end();)
                    {
                        int thispointx = it->pt.x;
                        int thispointy = it->pt.y;
                        if ((thispointx < vehicleRect.x) || (thispointx > (vehicleRect.x + vehicleRect.width)) || (thispointy < vehicleRect.y) || (thispointy > (vehicleRect.y + vehicleRect.height)))
                        {
                            // erase returns pointer to next element
                            it = keypoints.erase(it);
                            eraseCnt++;
                        }
                        else
                        {
                            keepCnt++;
                            it++;
                        }
                    }
                    cout << "vehicleKeypoints: " << keypoints.size() << ", ";// << endl;
                }

                bool visRect = false;
                if (visRect)
                {
                    cout << "\finished tKp: " << keypoints.size() << endl;
                    cv::Mat visImage = imgGray.clone();
                    cv::drawKeypoints(visImage, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
                    cv::rectangle(visImage, vehicleRect, cv::Scalar(255, 255, 255));
                    cv::imshow("rectangle", visImage);
                    cv::waitKey(0);
                }


                // save the images for viewing (detected keypoints)
                if (bSaveImg && (detectorPassIdx == 0) )
                {
                    string dirname = "../output/"+ detectorType;
                    string filename = "";

                    if (imgIndex == 0)
                    {
                        const char *pzPath = (char *)dirname.c_str();
                        // cout << "dir name: " << pzPath << endl;
                        // if (pzPath == NULL)
                        //     cout << "no directory set";
                        DIR *pDir;
                        bool bExists = false;
                        pDir = opendir(pzPath);
                        // cout << "dir found :" << pDir << endl;
                        if (pDir != 0)
                        {
                            bExists = true;
                            (void)closedir(pDir);
                        }
                        else
                        {
                            int status;
                            // cout << "going to make a dir" << endl;
                            status = mkdir(pzPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                        }
                    }
                    cv::Mat visImage = imgGray.clone();
                    cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
                    imwrite(dirname + "/" + detectorType + "_detected_kpts_img_" + std::to_string(imgIndex) + ".jpg", visImage);
                    cv::Rect croppingRect(500, 145, 250, 220); // topleft x, topleft y, width, height
                    // crop image for reporting...
                    cv::Mat croppedImage = visImage(croppingRect);
                    imwrite(dirname + "/" + detectorType + "_detected_kpts_img_crop_" + std::to_string(imgIndex) + ".jpg", croppedImage);
                }


                //// EOF STUDENT ASSIGNMENT

                // optional : limit number of keypoints (helpful for debugging and learning)
                bool bLimitKpts = false;
                if (bLimitKpts)
                {
                    int maxKeypoints = 50;

                    if (detectorType.compare("SHITOMASI") == 0)
                    { // there is no response info, so keep the first 50 as they are sorted in descending quality order
                        keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
                    }
                    cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
                    cout << " NOTE: Keypoints have been limited!" << endl;
                }

                // push keypoints and descriptor for current frame to end of data buffer
                (dataBuffer.end() - 1)->keypoints = keypoints;
                if (bStepsOp)
                    cout << "#2 : DETECT KEYPOINTS done" << endl;

                /* EXTRACT KEYPOINT DESCRIPTORS */

                //// STUDENT ASSIGNMENT
                //// TASK MP.4 -> add the following descriptors in file matching2D.cpp and enable string-based selection based on descriptorType
                //// -> BRIEF, ORB, FREAK, AKAZE, SIFT

                cv::Mat descriptors;
                // string descriptorType = "BRISK"; // BRIEF, ORB, FREAK, AKAZE, SIFT
                descKeypoints((dataBuffer.end() - 1)->keypoints, (dataBuffer.end() - 1)->cameraImg, descriptors, descriptorType);
                //// EOF STUDENT ASSIGNMENT

                // push descriptors for current frame to end of data buffer
                (dataBuffer.end() - 1)->descriptors = descriptors;
                if (bStepsOp)
                    cout << "#3 : EXTRACT DESCRIPTORS done" << endl;

                if (dataBuffer.size() > 1) // wait until at least two images have been processed
                {

                    /* MATCH KEYPOINT DESCRIPTORS */

                    vector<cv::DMatch> matches;


                    //// STUDENT ASSIGNMENT
                    //// TASK MP.5 -> add FLANN matching in file matching2D.cpp
                    //// TASK MP.6 -> add KNN match selection and perform descriptor distance ratio filtering with t=0.8 in file matching2D.cpp

                    matchDescriptors((dataBuffer.end() - 2)->keypoints, (dataBuffer.end() - 1)->keypoints,
                                     (dataBuffer.end() - 2)->descriptors, (dataBuffer.end() - 1)->descriptors,
                                     matches, descriptorTypeMatching, matcherType, selectorType);

                    //// EOF STUDENT ASSIGNMENT

                    // store matches in current data frame
                    (dataBuffer.end() - 1)->kptMatches = matches;
                    if (bStepsOp)
                        cout << "#4 : MATCH KEYPOINT DESCRIPTORS done" << endl;

                    // visualize matches between current and previous image
                    bVis = true;
                    if (bVis)
                    {
                        cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
                        cv::drawMatches((dataBuffer.end() - 2)->cameraImg, (dataBuffer.end() - 2)->keypoints,
                                        (dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints,
                                        matches, matchImg,
                                        cv::Scalar::all(-1), cv::Scalar::all(-1),
                                        vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                        string windowName = "Matching keypoints between two camera images";
                        cv::namedWindow(windowName, 7);
                        cv::imshow(windowName, matchImg);
                        if (bWaitKey)
                        {
                            cout << "Press key to continue to next image" << endl;
                            cv::waitKey(0); // wait for key to be pressed
                        }
                    }
                    bVis = false;
                    // cout << endl;
                }

                // TODO: could do some summarising her. the number of keypoints and matches were all returned.
                // only the time isnt.
            cout << endl;
            } // eof loop over all images
            detectorPassIdx ++; // increment the number of times that the detector has run, to allow us to only save files on single pass
            // cout << "dpidx: " << detectorPassIdx << endl;
        }
    }
    // eof loop over all descriptors and detectors

    return 0;
}
