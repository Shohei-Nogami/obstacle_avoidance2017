#include"ros/ros.h"
#include<sensor_msgs/image_encodings.h>
#include<image_transport/image_transport.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<cv_bridge/cv_bridge.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<fstream>
#include <std_msgs/Empty.h>
#include"obst_avoid/point3d.h"
#include"obst_avoid/line_point3d.h"
#include"obst_avoid/sqr_point3d.h"
#include <ros/callback_queue.h>

class culc_ave
{

	ros::NodeHandle nh,nh1,nh2;
	image_transport::ImageTransport it;
//subscriber
	ros::Subscriber sub_depth;//DepthImage
	ros::Subscriber sub_empty;
	ros::CallbackQueue depth_queue;
	ros::CallbackQueue empty_queue;
//subscribe options
	ros::SubscribeOptions depth_option;
	ros::SubscribeOptions empty_option;
	ros::Publisher pub_dpt;
	image_transport::Publisher pub_dpt2;
	ros::Time start_time;
public:	
	cv_bridge::CvImagePtr depthimg;
	cv_bridge::CvImagePtr pubdepthimg;
	cv::Mat depth_img;
	cv::Mat view_depth_img;
	::obst_avoid::point3d p3d;
	::obst_avoid::line_point3d lp3d;
	::obst_avoid::sqr_point3d sp3d;
	static const int cn=12;
	static const int cnw=cn;
	static const int cnh=cn*2;
	const double f=350;

	const int width=672;
	const int height=376;
	bool rf=false;
	double prev_time=0;	//
	double new_time=0;	//
	double dt;
	double ave_depth[cnh][cnw];
	double prev_ave_depth[cnh][cnw];
	culc_ave()
		:it(nh)
		{

		view_depth_img=cv::Mat::zeros(cv::Size(width,height),CV_32FC1);
	 	pub_dpt=nh.advertise<obst_avoid::sqr_point3d>("ave_p3d",1);
	 	pub_dpt2=it.advertise("depth_img",1);
		nh1.setCallbackQueue(&depth_queue);
		nh2.setCallbackQueue(&empty_queue);
		sub_depth=nh1.subscribe("/zed/depth/depth_registered",1,&culc_ave::depth_callback,this);
//		sub_empty=nh2.subscribe("/empty_msg",1,&culc_ave::empty_callback,this);
		lp3d.line_p3d.reserve(cnw);
		sp3d.sqr_p3d.reserve(cnh);
	
	}
	~culc_ave(){
		
	}
	void depth_callback(const sensor_msgs::ImageConstPtr& msg)
	{
		lp3d.line_p3d.clear();
		sp3d.sqr_p3d.clear();
			try{
		depthimg= cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
		depth_img=depthimg->image;
			}
			catch (cv_bridge::Exception& e) {
		ROS_ERROR("Could not convert from '%s' to 'TYPE_32FC1'.",
		msg->encoding.c_str());
		return ;
			}
	}
	void culc_prc(void){
//		int cn=12;
//		cnw=cn*2;
//		cnh=cn;

		int nan_count,count;
		long double sum_depth;
		for(int i=0;i<cnh;i++){
			for(int j=0;j<cnw;j++){
				count=0;
				nan_count=0;
				sum_depth=0;
				for(int h=(int)(i*height/cnh);h<(int)((i+1)*height/cnh);h++){
					for(int w=(int)(j*width/cnw);w<(int)((j+1)*width/cnw);w++){
						double depth=depth_img.at<float>(h,w);
						if(!std::isnan(depth))
							sum_depth+=depth;
						else
							nan_count++;
						count++;
					}
				}
				ave_depth[i][j]=sum_depth/(count-nan_count);
				p3d.x=( -((double)j*width/cnw+(double)width/cnw/2)+(double)width/2 )*ave_depth[i][j]/f;
				p3d.y=( -((double)i*height/cnh+(double)height/cnh/2)+(double)height/2 )*ave_depth[i][j]/f;
				p3d.z=ave_depth[i][j];
				lp3d.line_p3d.push_back(p3d);
			}
			sp3d.sqr_p3d.push_back(lp3d);
			lp3d.line_p3d.clear();
		}
	}
	void conv_view_depth(void){
		float T;
		if(isprev_time()){
			T=dt*5;
			
			for(int i=0;i<cnh;i++){
				for(int j=0;j<cnw;j++){
					ave_depth[i][j]=(ave_depth[i][j]*dt+prev_ave_depth[i][j]*T)/(T+dt);
				
					for(int u=j*width/cnw;u<(j+1)*width/cnw;u++){
						for(int v=i*height/cnh;v<(i+1)*height/cnh;v++){
							view_depth_img.at<float>(i,j)=ave_depth[i][j];
						}
					}
				}
			}
		}
		for(int i=0;i<cnh;i++){
			for(int j=0;j<cnw;j++){
				prev_ave_depth[i][j]=ave_depth[i][j];
			}
		}
		
	}
	void empty_callback(const std_msgs::Empty& msg){
		rf=true;
	}
	void depthcall(void){
			depth_queue.callOne(ros::WallDuration(1));
	}
	bool isdepth(void){
		if(depth_img.empty())
			return false;
		else
			return true;
	}
	void initrf(void){
		rf=false;
	}
	bool isrf(void){
		if(rf)
			return true;
		else
			return false;
	}
	void pub3d(void){
		pub_dpt.publish(sp3d);	
	}
	void emptycall(void){
		empty_queue.callOne(ros::WallDuration(1));
	}
	void setstarttime(void){
		start_time= ros::Time::now();
	}
	void gettime(void){
		ros::Duration time = ros::Time::now()-start_time;
		new_time=time.toSec();
	}
	void getprevtime(void){
		prev_time=new_time;
	}
	void culcdt(void){
		dt=new_time-prev_time;
	}
	void print_dt(void){
		ROS_INFO("(dt,fps):(%f,%f)",dt,1/dt);
	}
	bool isnew_time(void){
		if(new_time==0)
			return false;
		else
			return true;
	}
	bool isprev_time(void){
		if(prev_time==0)
			return false;
		else 
			return true;
	}
	void pub_img(void){
		cv_bridge::CvImagePtr pubdepthimg(new cv_bridge::CvImage);
		pubdepthimg->encoding=sensor_msgs::image_encodings::TYPE_32FC1;
		pubdepthimg->image=view_depth_img.clone();
		pub_dpt2.publish(pubdepthimg->toImageMsg());
	}	
};

	int main(int argc,char **argv){
		ros::init(argc,argv,"improve_depth");
		
		culc_ave prc;
//		image_transport::ImageTransport it(nh);
		prc.setstarttime();
		while(ros::ok()){
			ROS_INFO("prc.depthcall();");			
			prc.depthcall();
//			ROS_INFO("prc.depthcall();");
			if(!prc.isdepth())
				continue;
			//
			std::cout<<"prc.culc_prc();\n";
			prc.culc_prc();
//			prc.initrf();
//			ROS_INFO("prc.pub3d()");
//			prc.pub3d();
			std::cout<<"prc.conv_view_depth();\n";
			prc.conv_view_depth();
//			ROS_INFO("prc.pub3d(),prc.emptycall();");
//			while(ros::ok()&&!prc.isrf()){
//				prc.emptycall();
	//		}
			std::cout<<"if(prc.isnew_time()){\n";
//			ROS_INFO("prc.emptycall();");
			if(prc.isnew_time()){
				prc.getprevtime();
				prc.gettime();
				prc.culcdt();
//				prc.print_dt();
			}
			else 
				prc.gettime();
			//publish 
			prc.pub_img();
		}
		return 0;
	}

