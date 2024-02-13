 /*************************************************************************
 * Forked from: Abhinav Jain
 * Contact: abhinavjain241@gmail.com, abhinav.jain@heig-vd.ch
 * Date: 28/06/2016
 *
 *Modified at: Indian Institute of Technology, Bhubaneswar
 ***************************************************************************/
#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "std_msgs/String.h"
#include <bits/stdc++.h>  
#include <ctype.h>
using namespace std;

#define MESSAGE_FREQ 100

void error(const char *msg) {
    perror(msg);
    exit(0);
}

class Listener {
private:
    char topic_message[256] = { 0 };
public:
    void callback(const std_msgs::String::ConstPtr& msg);
    char* getMessageValue();
};

void Listener::callback(const std_msgs::String::ConstPtr& msg) {
    memset(topic_message, 0, 256);
    strcpy(topic_message, msg->data.c_str());
    ROS_INFO("I heard:[%s]", msg->data.c_str());
}

char* Listener::getMessageValue() {
    return topic_message;
}

bool solve( string s ) {
   for( int i = 1; i < s.length(); i++ ) {
      if( !isdigit( s[i] )) {
         return false;
      }
   }
   return true;
}


int main() {
	int argc = 4;
	//char *argv[] = {"client_node","14.139.195.5","27015","-e", NULL};
	char *argv[] = {"client_node","14.139.195.12","27015","-e", NULL};//omen-varcoe
	//char *argv[] = {"client_node","192.168.74.227","27015","-e", NULL};
	ros::init(argc, argv, "client_node");
	ROS_INFO_STREAM("Listening to G29 commands...");
	ros::NodeHandle nh;
	
	ros::Publisher pub_steering_G29 = nh.advertise<std_msgs::Float32> ("steering_command_G29",1000);
	ros::Publisher pub_throttle_G29 = nh.advertise<std_msgs::Float32> ("throttle_command_G29",1000);
	ros::Publisher pub_braking_G29 = nh.advertise<std_msgs::Float32> ("brake_command_G29",1000);
	ros::Publisher pub_reverse_G29 = nh.advertise<std_msgs::Float32> ("reverse_command_G29",1000);//here
	ros::spinOnce();
	
    ros::Rate loop_rate(MESSAGE_FREQ); // set the rate as defined in the macro MESSAGE_FREQ
	Listener listener;
        ros::Subscriber client_sub = nh.subscribe("/client_messages", 1, &Listener::callback, &listener);
    int sockfd, portno, n, choice = 1;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    bool echoMode = false;
    if (argc < 3) {
       fprintf(stderr,"Usage--: $ rosrun comm_tcp client_node <hostname> <port> --arguments\nArguments:\n -e : Echo mode\n");
       exit(0);
    }
    if (argc > 3)
		if (strcmp(argv[3], "-e") == 0)
			echoMode = true;
    portno = atoi(argv[2]);

    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) 
        error("ERROR opening socket");
        
    server = gethostbyname(argv[1]);
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    //std::cout << "How do you want the client to behave?:\n1. Be able to send messages manually\n2. Subscribe to /client_messages and send whatever's available there\nYour choice:";
    //std::cin >> choice;
    choice = 2;
    
	while(ros::ok()) {
	
	std_msgs::Float32 msg_steering_command;
    	std_msgs::Float32 msg_throttle_command;
    	std_msgs::Float32 msg_braking_command;
    	std_msgs::Float32 msg_reverse_command;//here
	
        bzero(buffer,256);
        if (choice == 1) {
            printf("Please enter the message: ");
            fgets(buffer,255,stdin);
        } else if (choice == 2) {
            strcpy(buffer, listener.getMessageValue());
            loop_rate.sleep();
        }
	    //n = write(sockfd,buffer,strlen(buffer));
	    if (n < 0) 
	         error("ERROR writing to socket");
	    if (echoMode) {
			bzero(buffer, 256);
		    n = read(sockfd,buffer,255);
		    
		    if (n < 0)
				error("ERROR reading reply");
				
		    // ss is an object of stringstream that references the S string.  
    		    stringstream ss(buffer); 
    		    
    		    string str;
    		    

            	    getline(ss, str, ' '); // skipping the first two fields
            	    getline(ss, str, ' ');
            	    
            	    if (solve(str)==true)
        	    	msg_reverse_command.data = stof(str);//here
        	    
            	    
            	    // Use while loop to check the getline() function condition.  
    		    getline(ss, str, ' '); // `str` is used to store the token string while ' ' whitespace is used as the delimiter.
    		    //std::cout << "Before conversion: " << str << endl;
    		    if (solve(str)==true)
    		    {
    		    	float steering_temp = float(stof(str));
    		    	float multiplier = 0.0137329;
    		    	msg_steering_command.data = steering_temp*multiplier;
    		    }
	
    		    	//msg_steering_command.data = stof(str);
        	    	//msg_steering_command.data = stof(str)*450.0/32760.0; // converting 32768 to 670 degrees	
        	    //std::cout << "After conversion: " << msg_steering_command.data << endl;
        	    
        	    getline(ss, str, ' '); // `str` is used to store the token string while ' ' whitespace is used as the delimiter.
        	    //std::cout << "Second part: " << str << endl;
        	    if (solve(str)==true)        	    	
        	    	msg_throttle_command.data = stof(str);
        	    //std::cout << "2*Second part: " << msg_throttle_command.data*2 << endl;
        	    
        	    getline(ss, str, ' '); // `str` is used to store the token string while ' ' whitespace is used as the delimiter.
        	    //std::cout << "Third part: " << str << endl;
        	    if (solve(str)==true)
        	    	msg_braking_command.data = stof(str);
        	       	       	  
    		    if(msg_steering_command.data>-400 && msg_steering_command.data < 400 && msg_steering_command.data!=0)
    		    	pub_steering_G29.publish(msg_steering_command);
    		    if (msg_throttle_command.data!=0)
    		    	pub_throttle_G29.publish(msg_throttle_command);
    		    if (msg_braking_command.data!=0)
    		    	pub_braking_G29.publish(msg_braking_command);
    		    if (msg_reverse_command.data==0 or msg_reverse_command.data==1)
    		    	pub_reverse_G29.publish(msg_reverse_command);//here
            	    	
		    //printf("%s\n", buffer);
	    }
	    ros::spinOnce();
    	}
	return 0;
}
