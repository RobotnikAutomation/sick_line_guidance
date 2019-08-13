/*
 * FigureEightVelocityFSM implements a simple state machine, creating cmd_vel messages
 * to drive a TurtleBot in a figure of eight.
 *
 * Copyright (C) 2019 Ing.-Buero Dr. Michael Lehning, Hildesheim
 * Copyright (C) 2019 SICK AG, Waldkirch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of SICK AG nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission
 *     * Neither the name of Ing.-Buero Dr. Michael Lehning nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *      Authors:
 *         Michael Lehning <michael.lehning@lehning.de>
 *
 *  Copyright 2019 SICK AG
 *  Copyright 2019 Ing.-Buero Dr. Michael Lehning
 *
 */
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <ros/ros.h>

#include "sick_line_guidance_demo/figure_eight_fsm.h"
#include "sick_line_guidance_demo/time_format.h"

/*
 * class FigureEightVelocityFSM implements a simple state machine, creating cmd_vel messages
 * to drive a TurtleBot in a figure of eight.
 */

/*
 * Constructor
 */
sick_line_guidance_demo::FigureEightVelocityFSM::FigureEightVelocityFSM()
{
  // define the state machine
  m_state_cnt = 0;
  m_next_state_switch = ros::Time(0);
  VelocityState state;
  state.cmd_vel.linear.x = 0;
  state.cmd_vel.linear.y = 0;
  state.cmd_vel.linear.z = 0;
  state.cmd_vel.angular.x = 0;
  state.cmd_vel.angular.y = 0;
  state.cmd_vel.angular.z = 0;
  // 1. state: drive anti-clockwise half circle witch 180 degree turn in 5 seconds
  m_vec_vel_states.push_back(state);
  m_vec_vel_states.back().duration = ros::Duration(5.0);
  m_vec_vel_states.back().cmd_vel.linear.x = 0.01;
  m_vec_vel_states.back().cmd_vel.angular.z = M_PI / m_vec_vel_states.back().duration.toSec();
  // 2. state: stop for 2 seconds
  m_vec_vel_states.push_back(state);
  m_vec_vel_states.back().duration = ros::Duration(3.0);
  m_vec_vel_states.back().cmd_vel.linear.x = 0.00;
  m_vec_vel_states.back().cmd_vel.angular.z = 0.00;
  // 3. state: drive anti-clockwise half circle witch 180 degree turn in 5 seconds
  m_vec_vel_states.push_back(state);
  m_vec_vel_states.back().duration = ros::Duration(5.0);
  m_vec_vel_states.back().cmd_vel.linear.x = 0.01;
  m_vec_vel_states.back().cmd_vel.angular.z = M_PI / m_vec_vel_states.back().duration.toSec();
  // 4. state: stop for 2 seconds
  m_vec_vel_states.push_back(state);
  m_vec_vel_states.back().duration = ros::Duration(3.0);
  m_vec_vel_states.back().cmd_vel.linear.x = 0.00;
  m_vec_vel_states.back().cmd_vel.angular.z = 0.00;
  // 5. state: drive clockwise half circle witch 180 degree turn in 5 seconds
  m_vec_vel_states.push_back(state);
  m_vec_vel_states.back().duration = ros::Duration(5.0);
  m_vec_vel_states.back().cmd_vel.linear.x = 0.01;
  m_vec_vel_states.back().cmd_vel.angular.z = -M_PI / m_vec_vel_states.back().duration.toSec();
  // 6. state: stop for 2 seconds
  m_vec_vel_states.push_back(state);
  m_vec_vel_states.back().duration = ros::Duration(3.0);
  m_vec_vel_states.back().cmd_vel.linear.x = 0.00;
  m_vec_vel_states.back().cmd_vel.angular.z = 0.00;
  // 7. state: drive clockwise half circle witch 180 degree turn in 5 seconds
  m_vec_vel_states.push_back(state);
  m_vec_vel_states.back().duration = ros::Duration(5.0);
  m_vec_vel_states.back().cmd_vel.linear.x = 0.01;
  m_vec_vel_states.back().cmd_vel.angular.z = -M_PI / m_vec_vel_states.back().duration.toSec();
  // 8. state: stop for 2 seconds
  m_vec_vel_states.push_back(state);
  m_vec_vel_states.back().duration = ros::Duration(3.0);
  m_vec_vel_states.back().cmd_vel.linear.x = 0.00;
  m_vec_vel_states.back().cmd_vel.angular.z = 0.00;
}

/*
 * Destructor
 */
sick_line_guidance_demo::FigureEightVelocityFSM::~FigureEightVelocityFSM()
{
}

/*
 * Next cycle, internal state is updated, velocity message may switch to next movement
 */
void sick_line_guidance_demo::FigureEightVelocityFSM::update(void)
{
  ros::Time cur_time = ros::Time::now();
  if(!m_next_state_switch.isValid())
  {
    m_next_state_switch = cur_time + m_vec_vel_states[m_state_cnt].duration;
  }
  if(cur_time >= m_next_state_switch)
  {
    m_state_cnt = ((m_state_cnt + 1) % (m_vec_vel_states.size()));
    m_next_state_switch = cur_time + m_vec_vel_states[m_state_cnt].duration;
    ROS_INFO_STREAM(sick_line_guidance_demo::TimeFormat::formatDateTime() << "FigureEightVelocityFSM: state switched, cmd_vel: linear.x="
      << std::fixed << std::setprecision(3) << (m_vec_vel_states[m_state_cnt].cmd_vel.linear.x) << ", angular.z=" << (m_vec_vel_states[m_state_cnt].cmd_vel.angular.z/M_PI) << "*PI");
  }
}

/*
 * Returns the cmd_vel message for the current movement
 */
geometry_msgs::Twist sick_line_guidance_demo::FigureEightVelocityFSM::getVelocity(void)
{
  return m_vec_vel_states[m_state_cnt].cmd_vel;
}