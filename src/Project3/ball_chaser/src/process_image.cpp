#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service DriveToTarget");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    float lin_x = 0.0;
    float ang_z = 0.0;

    for (int i = 0; i < img.height * img.width; i++) {
 
       if (img.data[3*i] == white_pixel && img.data[3*i+1] == white_pixel && img.data[3*i+2] == white_pixel) {
          
          if (i % img.width < img.width / 3) {
             // drive left
             //ROS_INFO("Left");
             lin_x = 0.5;
             ang_z = 1.0;
          }
          else if (i % img.width < 2 * img.width / 3) {
             // drive straight
             //ROS_INFO("Straight");
             lin_x = 0.5;
             ang_z = 0.0;
          }
          else {
             // drive right
	     //ROS_INFO("Right");
             lin_x = 0.5;
             ang_z = -1.0; 
          }
          break;
       }
    }
    
    drive_robot(lin_x, ang_z);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
