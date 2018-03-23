#include"ros/ros.h"
#include"std_msgs/Int32.h"
#include<iostream>
#include"ros_demo/demo_srv.h"
#include<sstream>
using namespace std;
int main(int argc,char **argv)
{
    ros::init(argc,argv,"demo_service_client");
    ros::NodeHandle n;
    ros::Rate loop_rate(10);
    ros::ServiceClient client=n.serviceClient<ros_demo::demo_srv>("demo_service");
    while (ros::ok())
    {
        ros_demo::demo_srv srv;
        std::stringstream ss;
        ss<<"Sending Here";
        srv.request.in=ss.str();
        if(client.call(srv))
        { 
            ROS_INFO(" Server says [%s]",srv.response.out.c_str());
        }
        else{
            ROS_ERROR("Failed to call service");
	         return 1;
        }
    ros::spinOnce();
	//Setting the loop rate
	loop_rate.sleep();

    }
   return 0;
}
