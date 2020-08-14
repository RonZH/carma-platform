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

#include <gmock/gmock.h>
#include "cpp_mock_drivers/MockCANDriver.h"
#include <cav_msgs/BagData.h>


namespace mock_drivers{

    TEST(MockCANDriver, Constructor){
        MockCANDriver d1;
        MockCANDriver d2(false);
        MockCANDriver d3(true);

        ASSERT_EQ(d1.getMockDriverNode().isDummy(), false);
        ASSERT_EQ(d2.getMockDriverNode().isDummy(), false);
        ASSERT_EQ(d3.getMockDriverNode().isDummy(), true);
    }

    TEST(MockCANDriver, pubCallbacks){
        MockCANDriver d(true);

        cav_msgs::BagData::ConstPtr test_msg_ptr(new cav_msgs::BagData());
        ASSERT_TRUE((*test_msg_ptr).header.stamp.isZero());

        d.parserCB(test_msg_ptr);

        std::vector<std::string> test_str_vector = d.getMockDriverNode().getTopics();
        std::vector<ros::Time> test_time_vector = d.getMockDriverNode().getTimeStamps();

        ASSERT_EQ(test_str_vector[0], "acc_engaged");
        ASSERT_EQ(test_str_vector[1], "acceleration");
        ASSERT_EQ(test_str_vector[2], "antilock_brakes_active");
        ASSERT_EQ(test_str_vector[3], "brake_lights");
        ASSERT_EQ(test_str_vector[4], "brake_position");
        ASSERT_EQ(test_str_vector[5], "engine_speed");
        ASSERT_EQ(test_str_vector[6], "fuel_flow");
        ASSERT_EQ(test_str_vector[7], "odometer");
        ASSERT_EQ(test_str_vector[8], "parking_brake");
        ASSERT_EQ(test_str_vector[9], "speed");
        ASSERT_EQ(test_str_vector[10], "stability_ctrl_active");
        ASSERT_EQ(test_str_vector[11], "stability_ctrl_enabled");
        ASSERT_EQ(test_str_vector[12], "steering_wheel_angle");
        ASSERT_EQ(test_str_vector[13], "throttle_position");
        ASSERT_EQ(test_str_vector[14], "traction_ctrl_active");
        ASSERT_EQ(test_str_vector[15], "traction_ctrl_enabled");
        ASSERT_EQ(test_str_vector[16], "transmission_state");
        ASSERT_EQ(test_str_vector[17], "turn_signal_state");
        ASSERT_EQ(test_str_vector[18], "vehicle/twist");
        ASSERT_EQ(test_str_vector[19], "vehicle_status");
        ASSERT_EQ(test_str_vector[20], "velocity_accel");

        // Give a range because the nanoseconds go too fast for the test to pass if its assert equal
        ros::Duration range(0.0001);
        
        EXPECT_TRUE((test_time_vector[0] > ros::Time::now() - range) && (test_time_vector[0] < ros::Time::now() + range));
        EXPECT_TRUE((test_time_vector[1] > ros::Time::now() - range) && (test_time_vector[1] < ros::Time::now() + range));
        EXPECT_TRUE((test_time_vector[2] > ros::Time::now() - range) && (test_time_vector[2] < ros::Time::now() + range));
    }

    TEST(MockCANDriver, run){
        MockCANDriver d(true);

        ASSERT_EQ(d.run(), 0);
    }
}