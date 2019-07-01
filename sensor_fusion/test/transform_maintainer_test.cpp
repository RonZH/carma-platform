/*
 * Copyright (C) 2018-2019 LEIDOS.
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

#include "../src/transform_maintainer.h"

#include <array>
#include <gtest/gtest.h>
#include <iostream>

/**
 * Helper function calculates if two vectors are equal within some epsilon
 */
bool equal_vector3(tf2::Vector3 v1, tf2::Vector3 v2, double epsilon) {
  return 
    fabs(v1.getX() - v2.getX()) < epsilon &&
    fabs(v1.getY() - v2.getY()) < epsilon &&
    fabs(v1.getZ() - v2.getZ()) < epsilon;
}

/**
 * Helper function calculates if two quaternions are equal within some epsilon
 */
bool equal_quaternion(tf2::Quaternion v1, tf2::Quaternion v2, double epsilon) {
  return 
    equal_vector3(v1.getAxis(), v2.getAxis(), epsilon) &&
    fabs(v1.getW() - v2.getW()) < epsilon;
}   
/**
 * Tests the handling calculation of map->odom transform updates
 */
TEST(MapToOdomUpdateTest, test1)
{

  // Set the transform from base_link to position sensor
  tf2::Transform base_to_global_pos_sensor = tf2::Transform::getIdentity();

  // Location at prime meridian and equator with 90 deg initial heading
  wgs84_utils::wgs84_coordinate host_veh_coord;
  host_veh_coord.lat = 0.0 * wgs84_utils::DEG2RAD;
  host_veh_coord.lon = 0.0 * wgs84_utils::DEG2RAD;
  host_veh_coord.elevation = 0.0;
  host_veh_coord.heading = 90.0 * wgs84_utils::DEG2RAD;

  // an NED at lat = 0 lon = 0 is rotated -90 deg around the ecef y-axis
  tf2::Vector3 earth_to_map_trans(6378137.0, 0, 0);
  tf2::Vector3 y_axis(0,1,0);
  tf2::Quaternion earth_to_map_rot(y_axis, -90.0 * wgs84_utils::DEG2RAD);
  tf2::Transform earth_to_map(earth_to_map_rot, earth_to_map_trans);

  // Vehicle has not moved some transform from odom to base link should be identity
  tf2::Transform odom_to_base_link = tf2::Transform::getIdentity();

  tf2::Transform map_to_odom = TransformMaintainer::calculate_map_to_odom_tf(
    host_veh_coord,
    base_to_global_pos_sensor, 
    earth_to_map,
    odom_to_base_link
  );

  // The heading of 90 degrees means
  // the actual orientation of odom when there has been no odometry is with +x being due east
  tf2::Vector3 solution_trans(0, 0, 0);
  tf2::Vector3 sol_rot_axis(0,0,1);
  tf2::Quaternion solution_rot = tf2::Quaternion(sol_rot_axis, 90 * wgs84_utils::DEG2RAD);

  EXPECT_EQ(true, equal_vector3(map_to_odom.getOrigin(), solution_trans, 0.00001));
  EXPECT_EQ(true, equal_quaternion(map_to_odom.getRotation(), solution_rot, 0.0001));

  // Odometry moves the vehicle 10 meters along north axis
  tf2::Vector3 odom_to_base_link_trans(10,0,0);
  odom_to_base_link.setOrigin(odom_to_base_link_trans);
  odom_to_base_link.setRotation(tf2::Quaternion::getIdentity());

  // Actual location should be at (1,10,0) in the map
  host_veh_coord.lat = 9.043694770492556e-6 * wgs84_utils::DEG2RAD;
  host_veh_coord.lon = 8.983152841187856e-5 * wgs84_utils::DEG2RAD;
  host_veh_coord.elevation = 7.917173206806183e-6;
  // Call tf update
  map_to_odom = TransformMaintainer::calculate_map_to_odom_tf(
    host_veh_coord,
    base_to_global_pos_sensor, 
    earth_to_map,
    odom_to_base_link
  );

  // Compare new map_to_odom
  // The odom frame should be moved +1 along the map's +x axis
  solution_trans = tf2::Vector3(1, 0, 0);
  sol_rot_axis = tf2::Vector3(0,0,1);
  solution_rot = tf2::Quaternion(sol_rot_axis, 90 * wgs84_utils::DEG2RAD);
  
  EXPECT_EQ(true, equal_vector3(map_to_odom.getOrigin(), solution_trans, 0.00001));
  EXPECT_EQ(true, equal_quaternion(map_to_odom.getRotation(), solution_rot, 0.0001));
}

TEST(MapToECEFTransformTest, example1)
{
  
  // This is not an actual test case, but a block of code to find out the transform between
  // MAP frame and ECEF frame given its GPS output lat/lon/elevation and its location in map:
  // T_m_b * (T_e_n * T_n_b)^-1
  // => T_m_b * (T_e_b)^-1
  // => T_m_b * T_b_e
  // => T_m_e
  wgs84_utils::wgs84_coordinate host_veh_coord;
  host_veh_coord.lat = 38.956450218 * wgs84_utils::DEG2RAD;
  host_veh_coord.lon = -77.1502303932 * wgs84_utils::DEG2RAD;
  host_veh_coord.elevation = 73.2292646887;
  host_veh_coord.heading = 0.0 * wgs84_utils::DEG2RAD;
  tf2::Transform tf_ecef_to_ned = wgs84_utils::ecef_to_ned_from_loc(host_veh_coord);

  tf2::Vector3 ned_to_baselink_translation(0, 0, 0);
  tf2::Vector3 x_axis(1,0,0);
  tf2::Quaternion ned_to_baselink_rot(x_axis, 180.0 * wgs84_utils::DEG2RAD);
  tf2::Transform tf_ned_to_baselink(ned_to_baselink_rot, ned_to_baselink_translation);

  tf2::Vector3 map_to_baselink_translation(0.540367901325, 1.03276705742, -37.4717445374);
  tf2::Quaternion map_to_baselink_rot(-0.0120230214115,0.0028066100802,-0.224863943407, 0.974311950482);
  tf2::Transform tf_map_to_baselink(map_to_baselink_rot, map_to_baselink_translation);

  tf2::Transform tf_map_to_ecef = (tf_map_to_baselink * ((tf_ecef_to_ned * tf_ned_to_baselink).inverse())).inverse();
  std::cerr << std::setprecision(20) << tf_map_to_ecef.getOrigin().getX() << std::endl;
  std::cerr << std::setprecision(20) << tf_map_to_ecef.getOrigin().getY() << std::endl;
  std::cerr << std::setprecision(20) << tf_map_to_ecef.getOrigin().getZ() << std::endl;
  std::cerr << std::setprecision(20) << tf_map_to_ecef.getRotation().getX() << std::endl;
  std::cerr << std::setprecision(20) << tf_map_to_ecef.getRotation().getY() << std::endl;
  std::cerr << std::setprecision(20) << tf_map_to_ecef.getRotation().getZ() << std::endl;
  std::cerr << std::setprecision(20) << tf_map_to_ecef.getRotation().getW() << std::endl;
  
}

int main(int argc, char**argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
