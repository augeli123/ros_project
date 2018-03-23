#include"ros/ros.h"
#include"ros_demo/demo_srv.h"
#include<iostream>
#include<sstream>
using namespace std;
bool demo_service_callback(ros_demo::demo_srv::Request &req,
ros_demo::demo_srv::Response &res)
{
    std::stringstream ss;
    ss<<"Recieved Here";
    res.out=ss.str();
    ROS_INFO("frome client [%s]",req.in.c_str());
    return true;
}
int main(int argc,char **argv)
{
    ros::init(argc,argv,"demo_service_server");
    ros::NodeHandle n;
    ros::ServiceServer service =n.advertiseService("demo_service",demo_service_callback);
    ros::spin();
    return 0;
}