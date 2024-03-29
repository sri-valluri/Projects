#include <string>
#include <iostream>
#include <stdio.h>
#include <iostream>
#include <math.h>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


void withChessBoard()
{
	string name = "";
	cout << "Name of video: ";
	getline(cin, name);
	int x = 0;
	VideoCapture cap(name + ".mp4");
	cout << "\n" << "Starting video" << "\n";
	//VideoCapture cap("withChessBoard.mp4");
	while (1) {
		Mat frame;
		cap >> frame;
		if (frame.empty())
			break;

		Mat img;
		cvtColor(frame, img, CV_BGR2GRAY); //Grayscale
		Size patternsize(7, 7);
		int numSquares = 49;
		vector<vector<Point3f>> object_points;
		vector<vector<Point2f>> image_points;
		vector<Point2f> corners;
		vector<Point3f> board_points;
		vector<Point3d> framePoints;
		vector<Point2d> cube;

		//Cube
		int shift = 2;
		framePoints.push_back(Point3d(0.0 + shift, 0.0 + shift, 0.0));
		framePoints.push_back(Point3d(2.0 + shift, 0.0 + shift, 0.0));

		framePoints.push_back(Point3d(0.0 + shift, 2.0 + shift, 0.0));
		framePoints.push_back(Point3d(0.0 + shift, 0.0 + shift, 2.0));
		framePoints.push_back(Point3d(2.0 + shift, 2.0 + shift, 0.0));
		framePoints.push_back(Point3d(0.0 + shift, 2.0 + shift, 2.0));
		framePoints.push_back(Point3d(2.0 + shift, 0.0 + shift, 2.0));
		framePoints.push_back(Point3d(2.0 + shift, 2.0 + shift, 2.0));

		for (int j = 0; j < numSquares; j++)
			board_points.push_back(Point3f(j / 7, j % 7, 0.0f));

		bool patternfound = findChessboardCorners(img, patternsize, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
		bool storedValues = false;

		if (patternfound)
		{
			cornerSubPix(img, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			//drawChessboardCorners(img, patternsize, corners, patternfound);
			image_points.push_back(corners);
			object_points.push_back(board_points);

			Mat distCoeffs;
			vector<Mat> rvec, tvec;
			Mat cameraMatrix = Mat(3, 3, CV_32FC1);
			cameraMatrix.ptr<float>(0)[0] = 1;
			cameraMatrix.ptr<float>(1)[1] = 1;

			if (!storedValues)
			{
				calibrateCamera(object_points, image_points, frame.size(), cameraMatrix, distCoeffs, rvec, tvec);
				storedValues = true;
			}

			int lenObj = object_points.size() / 2;
			int lenImage = image_points.size() / 2;

			solvePnP(object_points[lenObj], image_points[lenImage], cameraMatrix, distCoeffs, rvec.front(), tvec.front());
			projectPoints(framePoints, rvec.front(), tvec.front(), cameraMatrix, distCoeffs, cube);

			line(frame, cube[0], cube[1], CV_RGB(0, 255, 0), 2); //X
			line(frame, cube[0], cube[2], CV_RGB(0, 255, 0), 2); //Y
			line(frame, cube[0], cube[3], CV_RGB(0, 255, 0), 2); //Z
			line(frame, cube[1], cube[6], CV_RGB(0, 255, 0), 2);
			line(frame, cube[3], cube[6], CV_RGB(0, 255, 0), 2);
			line(frame, cube[1], cube[4], CV_RGB(0, 255, 0), 2);
			line(frame, cube[2], cube[4], CV_RGB(0, 255, 0), 2);
			line(frame, cube[4], cube[7], CV_RGB(0, 255, 0), 2);
			line(frame, cube[6], cube[7], CV_RGB(0, 255, 0), 2);
			line(frame, cube[3], cube[5], CV_RGB(0, 255, 0), 2);
			line(frame, cube[7], cube[5], CV_RGB(0, 255, 0), 2);
			line(frame, cube[2], cube[5], CV_RGB(0, 255, 0), 2);
		}

		imshow("Chess Board", frame);			// Display the resulting frame
		char c = (char)waitKey(25); 			// Press  ESC on keyboard to exit
		if (c == 27)
			break;
		x++;
	}

	cap.release(); 		// When everything done, release the video capture object
	destroyAllWindows(); 		// Closes all the frames
	return;
}

void noChessBoard()
{
	Mat img_keypoints_1; Mat img_keypoints_2;
	Mat img_1; Mat img2; 
	Mat descriptors_1, descriptors_2;
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	int first = 0;
	VideoCapture cap("noChessBoard.mp4");
	while (1) {
		Mat frame;
		cap >> frame;
		if (frame.empty())
			break;
		cvtColor(frame, frame, CV_BGR2GRAY); //Grayscale
		
		if (first == 0)
		{ 
			Mat img_1 = frame;
			Ptr<ORB> detector = ORB::create();
			detector->detectAndCompute(img_1, Mat(), keypoints_1, descriptors_1);
			//drawKeypoints(img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
			first = 1;
			//cout << "Detected" << "\n";
			cout << img_1.empty() << "\n";
		}

		else
		{
			Mat img_2 = frame;
			Ptr<ORB> detector = ORB::create();
			detector->detectAndCompute(img_2, Mat(), keypoints_2, descriptors_2);
			//drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
			//cout << "Done w / second" << "\n";
			descriptors_1.convertTo(descriptors_1, CV_32F); 
			descriptors_2.convertTo(descriptors_2, CV_32F);


			FlannBasedMatcher matcher;
			std::vector< DMatch > matches;
			matcher.match(descriptors_1, descriptors_2, matches);
		
			double max_dist = 0; double min_dist = 100;
			//-- Quick calculation of max and min distances between keypoints
			for (int i = 0; i < descriptors_1.rows; i++)
			{
				double dist = matches[i].distance;
				if (dist < min_dist) min_dist = dist;
				if (dist > max_dist) max_dist = dist;
			}
			printf("-- Max dist : %f \n", max_dist);
			printf("-- Min dist : %f \n", min_dist);
			std::vector< DMatch > good_matches;
			for (int i = 0; i < descriptors_1.rows; i++)
			{
				if (matches[i].distance <= max(2 * min_dist, 0.02))
				{
					good_matches.push_back(matches[i]);
				}
			}

			cout << "Done Matching" << "\n";

			vector< Point2f > obj;
			vector< Point2f > scene;


			for (int i = 0; i < good_matches.size(); i++)
			{
				//-- Get the keypoints from the good matches
				obj.push_back(keypoints_1[good_matches[i].queryIdx].pt);
				scene.push_back(keypoints_2[good_matches[i].trainIdx].pt);
			}

			// Find the Homography Matrix
			Mat H = findHomography(obj, scene, CV_RANSAC);
			cout << H;
			
		}
		
		//-- Step 1: Detect the keypoints using ORB Detector
		
		//detector->detect(img_2, keypoints_2);

		//-- Draw keypoints
		//drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
		
		//-- Show detected (drawn) keypoints
		//imshow("Keypoints 1", img_keypoints_1);
		//imshow("Keypoints 2", img_keypoints_2);
	
		imshow("Chess Board", frame);			// Display the resulting frame
		char c = (char)waitKey(25); 			// Press  ESC on keyboard to exit
		if (c == 27)
			break;
	}

	cap.release(); 		// When everything done, release the video capture object
	destroyAllWindows(); 
	// Closes all the frames
	return;
	
}

int main()
{
	withChessBoard();
}
