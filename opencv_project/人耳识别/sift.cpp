#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/xfeatures2d.hpp"
#include <opencv2/xfeatures2d/nonfree.hpp>
using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;



int main(int argc,char** argv)
{
       cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
	Mat trainImage = imread(argv[2]), trainImage_gray;
	imshow("trained",trainImage);
	cvtColor(trainImage, trainImage_gray, CV_BGR2GRAY);


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

		Mat captureImage, captureImage_gray;
		
		captureImage=imread(argv[1]);
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

		
		Mat dstImage;
		drawMatches(captureImage, test_keyPoint, trainImage, train_keyPoint, goodMatches, dstImage);      
                int a=(int)goodMatches.size();
                int b=(int)test_keyPoint.size();
                float c=(float)a/(float)b;
                printf("%d \n",(int)goodMatches.size());
                printf("%d \n",(int)test_keyPoint.size());
                printf("%0.2f \n",c);
		imshow("dst", dstImage);
                imshow("dst1", trainDescription);
waitKey(0);
	

	return 0;
}
