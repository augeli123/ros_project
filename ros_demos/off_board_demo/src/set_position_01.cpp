#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <sensor_msgs/NavSatFix.h>
void  pose_setting(geometry_msgs::PoseStamped &pose , const double &pose_x, const double &pose_y, const double &pose_z);
void  point_getting(geometry_msgs::Point &position, const double & position_x, const double & position_y, const double & position_z);

//dss
geometry_msgs::PoseStamped home_pose, current_pose, mission_pose;
geometry_msgs::Point gps_home, gps_current, gps_mission;
mavros_msgs::State current_state;

int timecount=0;


//callback
void state_cb(const mavros_msgs::State::ConstPtr& msg)
{
  current_state = *msg;
}

void gpsCallBack(const sensor_msgs::NavSatFixConstPtr& gps)
{
  //	double northing, easting;
  //  	char zone;
  //	//LLtoUTM(gps->latitude, gps->longitude,  northing, easting , &zone);

  //get gps location
  point_getting(gps_current, gps->latitude, gps->longitude, gps->altitude);
  std::cout<<"curr.x:"<<gps_current.x<<"  curr.y:"<<gps_current.y<<"  curr.z"<<gps_current.z<<std::endl;
  std::cout<<"home.x:"<<gps_home.x<<"  home.y:"<<gps_home.y<<"  home.z"<<gps_home.z<<std::endl;

}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "offboard_node");
  ros::NodeHandle nh;

  ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);
  ros::Subscriber gps_sub = nh.subscribe("mavros/global_position/raw/fix", 10, gpsCallBack);
  ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
      ("mavros/setpoint_position/local", 10);
  ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
      ("mavros/cmd/arming");
  ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
      ("mavros/set_mode");

  //the setpoint publishing rate MUST be faster than 2Hz
  // 20Hz, which is 5ms
  ros::Rate rate(20.0);

  // wait for FCU connection
  while(ros::ok() && current_state.connected)
  {
    ros::spinOnce();
    rate.sleep();
  }

  pose_setting(mission_pose, 0, 0, 5);
  //send a few setpoints before starting
  for(int i = 100; ros::ok() && i > 0; --i)
  {
    // todo: send mission pose step by step
    local_pos_pub.publish(mission_pose);
    ros::spinOnce();
    rate.sleep();
  }

  mavros_msgs::SetMode offb_set_mode;
  offb_set_mode.request.custom_mode = "OFFBOARD";

  mavros_msgs::CommandBool arm_cmd;
  arm_cmd.request.value = true;

  ros::Time last_request = ros::Time::now();
  //TODO::get gps and set pose
  gps_home = gps_current;
  pose_setting(home_pose, gps_home.x, gps_home.y, gps_home.z);

  while(ros::ok() && (timecount<2500)) //add timecout to get out odinary setting
  {
    //set offboard
    if(current_state.mode != "OFFBOARD" && (ros::Time::now() - last_request > ros::Duration(5.0)))
    {
      if( set_mode_client.call(offb_set_mode) && offb_set_mode.response.mode_sent)
      {
        ROS_INFO("Offboard enabled");
      }
      last_request = ros::Time::now();
    }
    else
    {
      //arming
      if(!current_state.armed && (ros::Time::now() - last_request > ros::Duration(5.0)))
      {
        if(arming_client.call(arm_cmd) && arm_cmd.response.success)
        {
          ROS_INFO("Vehicle armed");
        }
        last_request = ros::Time::now();
      }
    }


    //setpoint and go there
    local_pos_pub.publish(mission_pose);
    timecount++;
    ros::spinOnce();
    rate.sleep();

    //check position if get there went back
    while(timecount>1500 && timecount<2000)
    {
      //todo: send target pose step by step
      pose_setting(mission_pose, 0, 5, 5);
      local_pos_pub.publish(mission_pose);
      ros::spinOnce();
      rate.sleep();
      timecount++;
    }

    //if neccessary, change into manual
    //MANUAL
    while(timecount>2000)
    {
      offb_set_mode.request.custom_mode = "MANUAL";
      if( current_state.mode != "MANUAL" && (ros::Time::now() - last_request > ros::Duration(5.0)))
      {
        if( set_mode_client.call(offb_set_mode) && offb_set_mode.response.mode_sent)
        {
          ROS_INFO("Offboard disabled");
        }
        last_request = ros::Time::now();
      }
    } // end of while
  }
  timecount=0;

  return 0;
}

void  point_getting(geometry_msgs::Point &position,
                    const double & position_x, const double & position_y, const double & position_z)
{
  //TODO::need conversion
  position.x = position_x;
  position.y = position_y;
  position.z = position_z;

}
void  pose_setting(geometry_msgs::PoseStamped &pose, const double & pose_x, const double & pose_y, const double & pose_z)
{
  pose.pose.position.x = pose_x;
  pose.pose.position.y = pose_y;
  pose.pose.position.z = pose_z;
}

