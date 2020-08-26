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

#include "rosbag_mock_drivers/MockCommsDriver.h"

namespace mock_drivers{

    bool MockCommsDriver::driverDiscovery(){
        cav_msgs::DriverStatus discovery_msg;
        
        discovery_msg.name = "MockCommsDriver";
        discovery_msg.status = 1;

        discovery_msg.can = false;
        discovery_msg.radar = false;
        discovery_msg.gnss = false;
        discovery_msg.lidar = false;
        discovery_msg.roadway_sensor = false;
        discovery_msg.comms = true;
        discovery_msg.controller = false;
        discovery_msg.camera = false;
        discovery_msg.imu = false;
        discovery_msg.trailer_angle_sensor = false;
        discovery_msg.lightbar = false;

        mock_driver_node_.publishDataNoHeader<cav_msgs::DriverStatus>("/hardware_interface/driver_discovery", discovery_msg);

        return true;
    }

    void MockCommsDriver::parserCB(const carma_simulation_msgs::BagData::ConstPtr& msg){
        // generate messages from bag data
        if(msg->inbound_binary_msg_bool.data){
            cav_msgs::ByteArray inbound_binary_msg = msg->inbound_binary_msg;
            // update time stamps
            inbound_binary_msg.header.stamp = ros::Time::now();
            // publish the data
            mock_driver_node_.publishData<cav_msgs::ByteArray>("/hardware_interface/comms/inbound_binary_msg", inbound_binary_msg);  
        }
    }

    
    void MockCommsDriver::outboundCallback(const cav_msgs::ByteArray::ConstPtr& msg){
        // TODO: add outbound callback (if necessary).
    };

    MockCommsDriver::MockCommsDriver(bool dummy){

        mock_driver_node_ = MockDriverNode(dummy);

        inbound_pub_ptr_ = boost::make_shared<ROSComms<cav_msgs::ByteArray>>(ROSComms<cav_msgs::ByteArray>(CommTypes::pub, false, 10, "inbound_binary_msg"));
        
        std::function<void(const cav_msgs::ByteArray::ConstPtr&)> outbound_ptr = std::bind(&MockCommsDriver::outboundCallback, this, std::placeholders::_1);
        outbound_sub_ptr_ = boost::make_shared<ROSComms<const cav_msgs::ByteArray::ConstPtr&>>(ROSComms<const cav_msgs::ByteArray::ConstPtr&>(outbound_ptr, CommTypes::sub, false, 10, "outbound_binary_msg"));
    }

    int MockCommsDriver::run(){

        mock_driver_node_.init();

        // bag parser subscriber
        mock_driver_node_.addSub<boost::shared_ptr<ROSComms<const carma_simulation_msgs::BagData::ConstPtr&>>>(bag_parser_sub_ptr_);

        // driver publisher and subscriber
        mock_driver_node_.addPub<boost::shared_ptr<ROSComms<cav_msgs::ByteArray>>>(inbound_pub_ptr_);
        mock_driver_node_.addSub<boost::shared_ptr<ROSComms<const cav_msgs::ByteArray::ConstPtr&>>>(outbound_sub_ptr_);

        // driver discovery publisher
        mock_driver_node_.addPub<boost::shared_ptr<ROSComms<cav_msgs::DriverStatus>>>(driver_discovery_pub_ptr_);
        mock_driver_node_.setSpinCallback(std::bind(&MockCommsDriver::driverDiscovery, this));

        mock_driver_node_.spin(1);

        return 0;
    }

}