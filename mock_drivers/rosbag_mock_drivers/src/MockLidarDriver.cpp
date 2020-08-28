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

#include "rosbag_mock_drivers/MockLidarDriver.h"

namespace mock_drivers{

    bool MockLidarDriver::driverDiscovery(){
        ROS_ERROR_STREAM("1");
        cav_msgs::DriverStatus discovery_msg;
        
        discovery_msg.name = "MockLidarDriver";
        discovery_msg.status = 1;

        discovery_msg.can = false;
        discovery_msg.radar = false;
        discovery_msg.gnss = false;
        discovery_msg.lidar = true;
        discovery_msg.roadway_sensor = false;
        discovery_msg.comms = false;
        discovery_msg.controller = false;
        discovery_msg.camera = false;
        discovery_msg.imu = false;
        discovery_msg.trailer_angle_sensor = false;
        discovery_msg.lightbar = false;
        ROS_ERROR_STREAM("2");
        mock_driver_node_.publishDataNoHeader<cav_msgs::DriverStatus>("driver_discovery", discovery_msg);
        ROS_ERROR_STREAM("3");
        return true;
    }
    
    void MockLidarDriver::parserCB(const cav_simulation_msgs::BagData::ConstPtr& msg){
        // generate messages from bag data
        ROS_ERROR_STREAM("4");
        if(msg->points_raw_flag){
            ROS_ERROR_STREAM("5");
            sensor_msgs::PointCloud2 updated_msg = msg->points_raw;
            // update time stamps
            ROS_ERROR_STREAM("6");
            updated_msg.header.stamp = ros::Time::now();
            ROS_ERROR_STREAM("7");
            // publish the data
            mock_driver_node_.publishData<sensor_msgs::PointCloud2>("lidar/points_raw", updated_msg);
            ROS_ERROR_STREAM("8");
        }
        
    }

    MockLidarDriver::MockLidarDriver(bool dummy){

        mock_driver_node_ = MockDriverNode(dummy);

        points_raw_ptr_ = boost::make_shared<ROSComms<sensor_msgs::PointCloud2>>(ROSComms<sensor_msgs::PointCloud2>(CommTypes::pub, false, 10, "lidar/points_raw"));
        
    }

    int MockLidarDriver::run(){

        mock_driver_node_.init();

        // bag parser subscriber
        mock_driver_node_.addSub<boost::shared_ptr<ROSComms<const cav_simulation_msgs::BagData::ConstPtr&>>>(bag_parser_sub_ptr_);

        // driver publisher and subscriber
        mock_driver_node_.addPub<boost::shared_ptr<ROSComms<sensor_msgs::PointCloud2>>>(points_raw_ptr_);

        // driver discovery publisher
        mock_driver_node_.addPub<boost::shared_ptr<ROSComms<cav_msgs::DriverStatus>>>(driver_discovery_pub_ptr_);
        mock_driver_node_.setSpinCallback(std::bind(&MockLidarDriver::driverDiscovery, this));

        mock_driver_node_.spin(100);
        return 0;
    }

}