#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/xfeatures2d.hpp"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <unistd.h>
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
using namespace cv;
using namespace std;


#define WINDOW_NAME1 "src"			
					

void on_MouseHandle(int event, int x, int y, int flags, void* param);
void DrawRectangle( cv::Mat& img, cv::Rect box );
void showHistoCallback(int state, void* userData);

Rect g_rectangle;
bool g_bDrawingBox = false;
bool dst_exsit=false;
Mat srcImage,tempImage,ROIImage,dstImage;
Mat g_srcImage; 
Mat g_grayImage;
int g_nThresh = 33;
RNG g_rng(12345);
Mat drawing;
Mat g_cannyMat_output;
vector<vector<Point> > g_vContours;
vector<Vec4i> g_vHierarchy;


int main( int argc, char** argv )
{        g_rectangle = Rect(-1,-1,0,0);
	g_srcImage = imread( argv[1], 1 );
	if(!g_srcImage.data ) { printf("balabala \n"); return false; } 	
	cvtColor( g_srcImage, g_grayImage, COLOR_BGR2GRAY );
	blur( g_grayImage, g_grayImage, Size(3,3) );
	namedWindow( WINDOW_NAME1, WINDOW_AUTOSIZE );
	//imshow( WINDOW_NAME1, g_srcImage );
      Canny( g_grayImage, g_cannyMat_output, g_nThresh, g_nThresh*2, 3 );
	findContours( g_cannyMat_output, g_vContours, g_vHierarchy, RETR_TREE,           CHAIN_APPROX_SIMPLE, Point(0, 0) );
	 drawing = Mat::zeros( g_cannyMat_output.size(), CV_8UC3 );
	for( int i = 0; i< g_vContours.size(); i++ )
	{
		Scalar color = Scalar( g_rng.uniform(0, 255), g_rng.uniform(0,255), g_rng.uniform(0,255) );
		drawContours( drawing, g_vContours, i, color, 2, 8, g_vHierarchy, 0, Point() );
	}

        // imshow( WINDOW_NAME2, drawing );
//       drawing.copyTo(srcImage);
    g_cannyMat_output.copyTo(srcImage);      
        setMouseCallback(WINDOW_NAME1,on_MouseHandle,(void*)&srcImage);	
        // Create UI buttons
    createButton("Show histogram", showHistoCallback, NULL, QT_PUSH_BUTTON, 0);
   while(1){
	
		   srcImage.copyTo(tempImage);               
		if( g_bDrawingBox ) DrawRectangle( tempImage, g_rectangle );
		 imshow( WINDOW_NAME1, tempImage );
              if(dst_exsit)imshow("dst",dstImage);
		if( waitKey( 10 ) == 27 ) break;
}
	waitKey(0);
	return(0);
}

void on_MouseHandle(int event, int x, int y, int flags, void* param)
{

	Mat& image = *(cv::Mat*) param;
	switch( event)
	{
		
	case EVENT_MOUSEMOVE: 
		{
			if( g_bDrawingBox )
			{
				g_rectangle.width = x-g_rectangle.x;
				g_rectangle.height = y-g_rectangle.y;
			}
		}
		break;
        case EVENT_LBUTTONDBLCLK:
              {
                   g_bDrawingBox = false;
                  // drawing.copyTo(srcImage);
                    g_cannyMat_output.copyTo(srcImage); 
            }
		
	case EVENT_LBUTTONDOWN: 
		{
			g_bDrawingBox = true;
			g_rectangle =Rect( x, y, 0, 0 );
		}
		break;
        
		
	case EVENT_LBUTTONUP: 
		{
			g_bDrawingBox = false;
			if( g_rectangle.width < 0 )
			{
				g_rectangle.x += g_rectangle.width;
				g_rectangle.width *= -1;
			}

			if( g_rectangle.height < 0 ) 
			{
				g_rectangle.y += g_rectangle.height;
				g_rectangle.height *= -1;
			}
                       if(g_rectangle.width!=0&&g_rectangle.height!=0)
			{//ROIImage=image(g_rectangle);
                        
                        ROIImage=g_srcImage(g_rectangle);  
                        ROIImage.copyTo(dstImage);
                        imwrite("../dst.bmp",dstImage);
                        dst_exsit=true;


                             }
			DrawRectangle( image, g_rectangle );
                        
		}
		break;

	}
}

void DrawRectangle( cv::Mat& img, cv::Rect box )
{
	cv::rectangle(img,box.tl(),box.br(),cv::Scalar(g_rng.uniform(0, 255), g_rng.uniform(0,255), g_rng.uniform(0,255)));
}

void showHistoCallback(int state, void* userData)
{
    // if(dst_exsit){
int found=0;
cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
	Mat trainImage = imread("../dst.bmp"), trainImage_gray;
	//imshow("trained",trainImage);
	cvtColor(trainImage, trainImage_gray, CV_BGR2GRAY);
Mat dstImage1;

	vector<KeyPoint> train_keyPoint;
	Mat trainDescription;
	//SiftFeatureDetector featureDetector;
	f2d->detect(trainImage_gray, train_keyPoint);
	//SiftDescriptorExtractor featureExtractor;
	f2d->compute(trainImage_gray, train_keyPoint, trainDescription);
 
	BFMatcher matcher;
	vector<Mat> train_desc_collection(1, trainDescription);
	matcher.add(train_desc_collection);
	matcher.train();
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
     if((dp = opendir("../images")) == NULL) {
        fprintf(stderr,"cannot open directory:\n");
      
    }
    chdir("../images");
    while((entry = readdir(dp)) != NULL) {
       if(strcmp(".",entry->d_name) == 0 || 
                strcmp("..",entry->d_name) == 0)
                continue;

     //  printf("%s\n",entry->d_name);
        Mat captureImage, captureImage_gray;
		
		captureImage=imread(entry->d_name);
               cvtColor(captureImage, captureImage_gray, CV_BGR2GRAY);

		
		vector<KeyPoint> test_keyPoint;
		Mat testDescriptor;
		f2d->detect(captureImage_gray, test_keyPoint);
		f2d->compute(captureImage_gray, test_keyPoint, testDescriptor);
                 
		vector<vector<DMatch> > matches;
		matcher.knnMatch(testDescriptor, matches, 2);

		
		vector<DMatch> goodMatches;
		for(unsigned int i = 0; i < matches.size(); i++)
		{
			if(matches[i][0].distance < 0.6 * matches[i][1].distance)
				goodMatches.push_back(matches[i][0]);
		}
                 
                int a=(int)goodMatches.size();
                int b=(int)test_keyPoint.size();
                float c=(float)a/(float)b;
                 if(c>0.50){
		drawMatches(captureImage, test_keyPoint, trainImage, train_keyPoint, goodMatches, dstImage1);    
             //  printf("%d \n",(int)goodMatches.size());
              //  printf("%d \n",(int)test_keyPoint.size());
                printf("识别对应图库：%s\n",entry->d_name);
                printf("匹配度：%0.2f \n",c);
		imshow("output", dstImage1);
             found=1;
              //  imshow("dst1", trainDescription);
            break;

}


}
if(found==0){
printf("并未找到匹配图片\n");
}

}
