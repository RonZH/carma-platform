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

#pragma once

#include "rosbag_mock_drivers/MockDriver.h"
#include <radar_msgs/RadarStatus.h>
#include <radar_msgs/RadarTrackArray.h>

namespace mock_drivers
{
class MockRadarDriver : public MockDriver
{
private:
  const std::string radar_status_topic_ = "radar/status";
  const std::string radar_tracks_raw_topic_ = "radar/tracks_raw";

public:
  MockRadarDriver(bool dummy = false);
  int run();
  bool driverDiscovery();
};

}  // namespace mock_drivers