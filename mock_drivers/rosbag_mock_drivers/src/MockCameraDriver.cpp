/*
 * Copyright (C) 2019-2020 LEIDOS.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "rosbag_mock_drivers/MockCameraDriver.h"

namespace mock_drivers{

    bool MockCameraDriver::driverDiscovery(){
        cav_msgs::DriverStatus discovery_msg;
        
        discovery_msg.name = "MockCameraDriver";
        discovery_msg.status = 1;

        discovery_msg.can = false;
        discovery_msg.radar = false;
        discovery_msg.gnss = false;
        discovery_msg.lidar = false;
        discovery_msg.roadway_sensor = false;
        discovery_msg.comms = false;
        discovery_msg.controller = false;
        discovery_msg.camera = true;
        discovery_msg.imu = false;
        discovery_msg.trailer_angle_sensor = false;
        discovery_msg.lightbar = false;

        mock_driver_node_.publishDataNoHeader<cav_msgs::DriverStatus>("driver_discovery", discovery_msg);

        return true;
    }

    void MockCameraDriver::parserCB(const cav_simulation_msgs::BagData::ConstPtr& msg){
        
        ros::Time curr_time = ros::Time::now();
        
        // generate messages from bag data
       if(msg->camera_info_flag){
            sensor_msgs::CameraInfo camera_info = msg->camera_info;
            camera_info.header.stamp = curr_time;
            mock_driver_node_.publishData<sensor_msgs::CameraInfo>("camera/camera_info", camera_info);
        }

        if(msg->image_raw_flag){
            sensor_msgs::Image image_raw = msg->image_raw;
            image_raw.header.stamp = curr_time;
            mock_driver_node_.publishData<sensor_msgs::Image>("camera/image_raw", image_raw);
        }

        if(msg->image_rects_flag){
            sensor_msgs::Image image_rects = msg->image_rects;
            image_rects.header.stamp = curr_time;
            mock_driver_node_.publishData<sensor_msgs::Image>("camera/image_rects", image_rects);
        }

        if(msg->projection_matrix_flag){
            autoware_msgs::ProjectionMatrix projection_matrix = msg->projection_matrix;
            projection_matrix.header.stamp = curr_time;
            mock_driver_node_.publishData<autoware_msgs::ProjectionMatrix>("camera/projection_matrix", projection_matrix);
        }
    } 

    MockCameraDriver::MockCameraDriver(bool dummy){

        mock_driver_node_ = MockDriverNode(dummy);

        // camera_info_ptr_ = boost::make_shared<ROSComms<sensor_msgs::CameraInfo>>(ROSComms<sensor_msgs::CameraInfo>(CommTypes::pub, false, 10, "camera_info"));
        // image_raw_ptr_ = boost::make_shared<ROSComms<sensor_msgs::Image>>(ROSComms<sensor_msgs::Image>(CommTypes::pub, false, 10, "image_raw"));
        // image_rects_ptr_ = boost::make_shared<ROSComms<sensor_msgs::Image>>(ROSComms<sensor_msgs::Image>(CommTypes::pub, false, 10, "image_rects"));
        // projection_matrix_ptr_ = boost::make_shared<ROSComms<autoware_msgs::ProjectionMatrix>>(ROSComms<autoware_msgs::ProjectionMatrix>(CommTypes::pub, false, 10, "projection_matrix"));
        
    }

    int MockCameraDriver::run(){

        mock_driver_node_.init();

        // // bag_parser subscriber
        // mock_driver_node_.addSub<boost::shared_ptr<ROSComms<const cav_simulation_msgs::BagData::ConstPtr&>>>(bag_parser_sub_ptr_);

        // // data topic publishers
        // mock_driver_node_.addPub<boost::shared_ptr<ROSComms<sensor_msgs::CameraInfo>>>(camera_info_ptr_);
        // mock_driver_node_.addPub<boost::shared_ptr<ROSComms<sensor_msgs::Image>>>(image_raw_ptr_);
        // mock_driver_node_.addPub<boost::shared_ptr<ROSComms<sensor_msgs::Image>>>(image_rects_ptr_);
        // mock_driver_node_.addPub<boost::shared_ptr<ROSComms<autoware_msgs::ProjectionMatrix>>>(projection_matrix_ptr_);

        addPassthroughPub<sensor_msgs::CameraInfo>(bag_prefix_ + camera_info_topic_, camera_info_topic_, false, 10);
        addPassthroughPub<sensor_msgs::Image>(bag_prefix_ + image_raw_topic_, image_raw_topic_, false, 10);
        addPassthroughPub<sensor_msgs::Image>(bag_prefix_ + image_rects_topic_, image_rects_topic_, false, 10);
        addPassthroughPub<autoware_msgs::ProjectionMatrix>(bag_prefix_ + projection_matrix_topic_, projection_matrix_topic_, false, 10);

        // driver discovery publisher
        mock_driver_node_.addPub<boost::shared_ptr<ROSComms<cav_msgs::DriverStatus>>>(driver_discovery_pub_ptr_);
        mock_driver_node_.setSpinCallback(std::bind(&MockCameraDriver::driverDiscovery, this));

        mock_driver_node_.spin(100);

        return 0;
    }

}