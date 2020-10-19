#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_bot(float lin_x,float ang_z)
{
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x=lin_x;
	srv.request.angular_z=ang_z;
    
    client.call(srv);

}

void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255; 
    bool flag = false;

    for (int i = 0; i < img.height; i++){ //for each row
        int k = img.step*i; //to linearize the index
        for (int j = 0; j < img.step; j+=3){ //for each column
            if (img.data[k+j] == white_pixel && img.data[k+j+1] == white_pixel && img.data[k+j+2] == white_pixel){ //3 white pixels in a row
                flag = true;
                if (j/3 <= img.width/3){
                    drive_bot(0.5, 0.5); //go left
                } else if (j/3 > 2*img.width/3){
                    drive_bot(0.5, -0.5); //go right
                } else {
                    drive_bot(0.5, 0.0); //go straight
                }
                break;
            }
        }
        if(flag) break;
    }
    if (!flag){
        drive_bot(0.0, 0.0); //stop
    }
    return;

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