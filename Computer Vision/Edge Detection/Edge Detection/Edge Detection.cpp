#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace cv;

Mat grayscale(Mat image, String imageName)
{
	//Display Regular Image
	//namedWindow( imageName, CV_WINDOW_NORMAL );
	//imshow( imageName, image );
	//waitKey(0);

	Mat gray_image;

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			Vec3b pixel = image.at<Vec3b>(i, j);
			int blue = (pixel.val[0]);
			int green = pixel.val[1];
			int red = pixel.val[2];

			double avg = (pixel[0] + pixel[1] + pixel[2]) / 3;
			//printf ("%d\n", avg);s

			pixel[0] = avg;
			pixel[1] = avg;
			pixel[2] = avg;
			image.at<Vec3b>(i, j) = pixel;
		}
	}

	//Display Gray Scale
	//namedWindow( imageName, CV_WINDOW_NORMAL );
	//imshow( imageName, image);
	//waitKey(0);
	return image;
}

Mat gaus(Mat orig, String imageName, int len)
{
	Mat image = orig.clone();

	double kernel[3][3] = 
	{
		{2,4,2},
		{4,8,4},
		{2,4,2}
	};
	double sum = 0;
	for (int r = 0; r < len; r++)
	{
		for (int i = 1; i < image.rows - 1; i++)
		{
			for (int j = 1; j < image.cols - 1; j++)
			{
				for (int x = -1; x <= 1; x++)
				{
					for (int y = -1; y <= 1; y++)
					{
						Vec3b pixel = orig.at<Vec3b>(i + x, j + y);
						sum += ((pixel.val[0]) * (kernel[x + 1][y + 1]));
					}
				}
				Vec3b center = image.at<Vec3b>(i, j);
				sum = (sum / 32.0);
				center[0] = sum;
				center[1] = sum;
				center[2] = sum;
				sum = 0;
				image.at<Vec3b>(i, j) = center;
			}
		}
		orig = image.clone();
		printf("Gaus: %d\n", r);
	}

	//namedWindow(imageName, CV_WINDOW_NORMAL);
	//imshow(imageName, image);
	//waitKey(0);
	return image;
}

void sobel(Mat orig, String imageName)
{
	//namedWindow("Orig", CV_WINDOW_NORMAL );
	//imshow("Orig", image);
	printf("Starting sobel\n");
	Mat image = orig.clone();

	double kernelx[3][3] =
	{
		{ 1,0,-1},
		{ 2,0,-2},
		{ 1,0,-1 }
	};

	double kernely[3][3] = 
	{
		{ 1,2,1 },
		{ 0,0,0 },
		{ -1,-2,-1}
	};

	double sumx = 0;
	double sumy = 0;
	//Sobel
	for (int i = 1; i < image.rows - 1; i++)
		{
			for (int j = 1; j < image.cols - 1; j++)
			{
				for (int x = -1; x <= 1; x++)
				{
					for (int y = -1; y <= 1; y++)
					{
						Vec3b pixel = orig.at<Vec3b>(i + x, j + y);
						//sumx += double((pixel[0]) * (kernelx[x + 1][y + 1]));
						sumy += double((pixel[0]) * (kernely[x + 1][y + 1]));
					}
				}
				Vec3b center = image.at<Vec3b>(i, j);
				double sum = sqrt(pow(sumx,2) + pow(sumy,2));
				double t = atan2(sumy, sumx);
				center[0] = sum;
				center[1] = t;
				center[2] = sum;
				image.at<Vec3b>(i, j) = center;
				sumx = 0;
				sumy = 0;
			}
		}
	
	printf("Starting canny\n");
	Mat sobel = image.clone();

	double upperThreshold = 250;
	double minThreshold = 250;

	//Canny
	for (int i = 1; i < image.rows - 1; i++)
		{
			for (int j = 1; j < image.cols - 1; j++)
			{
				Vec3b topLeft = image.at<Vec3b>(i-1, j+1);
				Vec3b top = image.at<Vec3b>(i, j+1);
				Vec3b topRight = image.at<Vec3b>(i+1, j+1);
				Vec3b left = image.at<Vec3b>(i-1, j);
				Vec3b center = image.at<Vec3b>(i, j);
				Vec3b right = image.at<Vec3b>(i+1, j);
				Vec3b botLeft = image.at<Vec3b>(i-1, j-1);
				Vec3b bot = image.at<Vec3b>(i, j-1);
				Vec3b botRight = image.at<Vec3b>(i+1, j-1);

				double length = center[0];
				double angle = center[1];

				
				if (angle > 22.5 && angle < 67.5)
				{
					if (length > topLeft[0] && length > topRight[0])
					{
						center[0] = 255;
						center[1] = 255;
						center[2] = 255;
						sobel.at<Vec3b>(i, j) = center;
					}
					else
					{
						center[0] = 0;
						center[1] = 0;
						center[2] = 0;
						sobel.at<Vec3b>(i, j) = center;
					}
				}

				/*
				else if (angle > 67.5 && angle < 112.5)
				{
					if (length > top[0] && length > bot[0])
					{
						center[0] = 255;
						center[1] = 255;
						center[2] = 255;
						sobel.at<Vec3b>(i, j) = center;
					}
					else
					{
						center[0] = 0;
						center[1] = 0;
						center[2] = 0;
						sobel.at<Vec3b>(i, j) = center;
					}

				}

				else if (angle > 112.5 && angle < 157.5)
				{
					if (length > topRight[0] && length > botLeft[0])
					{
						center[0] = 255;
						center[1] = 255;
						center[2] = 255;
						sobel.at<Vec3b>(i, j) = center;
					}
					else
					{
						center[0] = 0;
						center[1] = 0;
						center[2] = 0;
						sobel.at<Vec3b>(i, j) = center;
					}
				}
				
				else if ((angle > 0 && angle <22.5) || (angle > 157.5 && angle < 180))
				{
					if (length > left[0] && length > right[0])
					{
						center[0] = 255;
						center[1] = 255;
						center[2] = 255;
						sobel.at<Vec3b>(i, j) = center;
					}
					else
					{
						center[0] = 0;
						center[1] = 0;
						center[2] = 0;
						sobel.at<Vec3b>(i, j) = center;
					}
				}
				
				*/
				if (center[0] > upperThreshold)
				{
					center[0] = 255;
					center[1] = 255;
					center[2] = 255;
					sobel.at<Vec3b>(i, j) = center;
				}
				else if (center[0] < minThreshold)
				{
					center[0] = 0;
					center[1] = 0;
					center[2] = 0;
					sobel.at<Vec3b>(i, j) = center;
				}
			}
		}
	
	namedWindow(imageName, CV_WINDOW_NORMAL);
	imshow(imageName, sobel);
	waitKey(0);
}

int main()
{
	Mat image;
	image = imread("easy.jpg");

	if (!image.data)
	{
		printf("No image data \n");
		return -1;
	}
	print("Started\n");
	Mat gray = grayscale(image.clone(), "Hello");
	printf("Done grayscaling\n");
	Mat blur = gaus(gray, "Gaus", 5);
	//printf("Done blurring\n");
	//sobel(blur, "my dude");
	return 0;
}