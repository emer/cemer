/**
 * Copyright (c) 2014 Jilk Systems, Inc.
 * 
 * This file is part of Gazebo-ROSBridge Messages.
 *
 * Gazebo-ROSBridge Messages is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gazebo-ROSBridge Messages is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gazebo-ROSBridge Messages.  If not, see http://www.gnu.org/licenses/.
 * 
 */
package com.jilk.ros.gazebo.message;

import com.jilk.ros.message.Message;
import com.jilk.ros.message.MessageType;

@MessageType(string = "gazebo_msgs/GetModelStateResponse")
public class GetModelStateResponse extends Message {
    public GeometryPose pose;
    public GeometryTwist twist;
    public boolean success;
    public String status_message;
}
