 /*************************************************************************
 * Author: Abhinav Jain
 * Contact: abhinavjain241@gmail.com, abhinav.jain@heig-vd.ch
 * Date: 28/06/2016
 *
 * This file contains source code to the server node of the ROS package
 * comm_tcp developed at LaRA (Laboratory of Robotics and Automation)
 * as part of my project during an internship from May 2016 - July 2016.
 *
 * (C) All rights reserved. LaRA, HEIG-VD, 2016 (http://lara.populus.ch/)
 ***************************************************************************/
#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "std_msgs/String.h"

using namespace std;
std_msgs::Float32 steering_angle_encoder, speed;


void callbackSteeringAngle(const std_msgs::Float32& msg){

  	steering_angle_encoder.data = msg.data;

}

void callbackCurSpeed(const std_msgs::Float32& msg){

  	speed.data = msg.data;

}


void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main (int argc, char** argv)
{
  ros::init(argc, argv, "server_node");
  ros::NodeHandle nh;
  ros::Publisher server_pub = nh.advertise<std_msgs::String>("/server_messages/", 1000);
  ros::Subscriber sub_steering_ang_encd = nh.subscribe("/steering_angle", 1000, &callbackSteeringAngle);
  ros::Subscriber sub_cur_gSpeed = nh.subscribe("/cur_gSpeed", 1000, &callbackCurSpeed);
  ros::Rate loop_rate(25);
  //Testing package is working fine
  int sockfd, newsockfd, portno; //Socket file descriptors and port number
  socklen_t clilen; //object clilen of type socklen_t
  char buffer[256]; //buffer array of size 256
  struct sockaddr_in serv_addr, cli_addr; ///two objects to store client and server address
  std_msgs::String message;
  std::stringstream ss;
  int n;
  ros::Duration d(0.01); // 100Hz
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  portno = atoi(argv[1]);
  cout << "Hello there! This node is listening on port " << portno << " for incoming connections" << endl;
  cout<< "Test"<<endl;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
      error("ERROR opening socket");
  else
  	cout<<"Socket Opened: "<<sockfd<<endl;
  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
      error("setsockopt(SO_REUSEADDR) failed");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0)
            error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd,
              (struct sockaddr *) &cli_addr,
              &clilen);
  if (newsockfd < 0)
       error("ERROR on accept");
  
  while(ros::ok()) {
     ss.str(std::string()); //Clear contents of string stream
     bzero(buffer,256);
     //n = read(newsockfd,buffer,255);
     //if (n < 0) error("ERROR reading from socket");
     // printf("Here is the message: %s\n",buffer);
     //ss << buffer;
     //message.data = ss.str();
     //ROS_INFO("%s", message.data.c_str());
     //server_pub.publish(message);
     //n = write(newsockfd,"I got your message",18);
     string sendData = to_string(steering_angle_encoder.data);
     sendData = sendData+" ";
     sendData = sendData+to_string(speed.data);
     n = write(newsockfd,sendData.c_str(),sendData.length());
     //n = write(newsockfd,sendData.c_str(),10);
     //n = write(newsockfd,"I got your message",18);
     //ROS_INFO("Data sent: %f", steering_angle_encoder.data);
     ROS_INFO("Data sent: %s", sendData.c_str());
     if (n < 0) error("ERROR writing to socket");
     //close(newsockfd);
     //close(sockfd);
     ros::spinOnce();
     loop_rate.sleep();
     //d.sleep();
  }
  return 0;
}
