#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;


#define WINDOW_NAME1 "src"			
//#define WINDOW_NAME2 "dst"					

void on_MouseHandle(int event, int x, int y, int flags, void* param);
void DrawRectangle( cv::Mat& img, cv::Rect box );


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
