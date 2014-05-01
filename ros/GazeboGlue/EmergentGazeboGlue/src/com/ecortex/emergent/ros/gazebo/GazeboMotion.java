/*
 * Copyright (c) 2014 eCortex, Inc.  All Rights Reserved.
 * 
 * Author: David J. Jilk
 * 
 */
package com.ecortex.emergent.ros.gazebo;

import com.jilk.ros.ROSClient;
import com.jilk.ros.MessageHandler;
import com.jilk.ros.Topic;
import com.jilk.ros.Service;

import com.jilk.ros.gazebo.message.GetModelStateRequest;
import com.jilk.ros.gazebo.message.GetModelStateResponse;
import com.jilk.ros.gazebo.message.SetModelStateRequest;
import com.jilk.ros.gazebo.message.GazeboServiceResult;
import com.jilk.ros.gazebo.message.GazeboModelState;
import com.ecortex.emergent.ros.message.MotionCommand;
import com.jilk.ros.gazebo.message.GeometryPoint;
import com.jilk.ros.gazebo.message.GeometryPose;
import com.jilk.ros.gazebo.message.GeometryQuaternion;
import com.jilk.ros.gazebo.message.GeometryTwist;
import com.jilk.ros.gazebo.message.GeometryVector3;


public class GazeboMotion implements MessageHandler<MotionCommand> {
    private static final String modelName = "cranberry";
    private static final String referenceFrame = "world";
    private static final String motionTopicName = "/emergent/motion";
    private static final String getPositionServiceName = "/gazebo/get_model_state";
    private static final String setPositionServiceName = "/gazebo/set_model_state";
    private static final double unitRotation = -0.3927; // clockwise pi/8 or 22.5 degrees
    private static final double unitTranslation = 0.1; // 10 cm or 3.3"
    

    private Topic<MotionCommand> motionCommand;
    private Service<GetModelStateRequest, GetModelStateResponse> getPosition;
    private Service<SetModelStateRequest, GazeboServiceResult> setPosition;
    private boolean running;
    private GetModelStateRequest request;
    private GazeboCamera camera;
    
    public GazeboMotion(ROSClient ros, GazeboCamera camera) {
        motionCommand = new Topic<MotionCommand>(motionTopicName, MotionCommand.class, ros);
        getPosition = new Service<GetModelStateRequest,
                GetModelStateResponse>(getPositionServiceName,
                GetModelStateRequest.class, GetModelStateResponse.class, ros);
        setPosition = new Service<SetModelStateRequest,
                GazeboServiceResult>(setPositionServiceName,
                SetModelStateRequest.class, GazeboServiceResult.class, ros);        
        running = false;
        request = new GetModelStateRequest();
        request.model_name = modelName;
        request.relative_entity_name = referenceFrame;
        this.camera = camera;
    }
    
    public void start() {
        if (!running) {
            motionCommand.subscribe(this);
            running = true;
        }
    }
    
    public void stop() {
        if (running) {
            motionCommand.unsubscribe();
            running = false;
        }
    }
    
    public boolean isRunning() {
        return running;
    }
            
    @Override
    public void onMessage(MotionCommand motion) {
        try {
            SetModelStateRequest newPosition = getNewPosition(motion);
            if (newPosition != null)
                setPosition.callBlocking(newPosition);
            camera.publishNext();
        }
        catch (InterruptedException ex) {}
    }
    
    private SetModelStateRequest getNewPosition(MotionCommand motion) {
        SetModelStateRequest result = new SetModelStateRequest();
        result.model_state = newPosition(motion);
        if (result.model_state != null)
            return result;
        else return null;
    }    
    
    public GazeboModelState newPosition(MotionCommand motion) {
        //System.out.println("Pose Command: L=" + pose.linear_velocity + " A=" + pose.angular_velocity);
        GetModelStateResponse currentPosition = getCurrentPosition();
        if (currentPosition != null) {
            GazeboModelState newPosition = createModelStateMessage();
            newPosition.pose.copyFrom(currentPosition.pose);
            newPosition.twist.copyFrom(currentPosition.twist);

            if (motion.motion_type) {
                // This calculation was carefully derived.
                //   It would be better to have generic quaternion-to-angle/vector conversion routines
                double angle = 2 * Math.acos(newPosition.pose.orientation.w);  // only works because we are in xy-plane
                if (newPosition.pose.orientation.z > 0) 
                    angle = (2 * Math.PI) - angle;
                double delta_x = Math.cos(angle) * unitTranslation * motion.motion_units;
                double delta_y = -Math.sin(angle) * unitTranslation * motion.motion_units;

                newPosition.pose.position.x += delta_x;
                newPosition.pose.position.y += delta_y;        
                //System.out.println("qz=" + newPosition.pose.orientation.z + " qw=" + newPosition.pose.orientation.w + " angle=" + angle + " dx=" + delta_x + " dy=" + delta_y);
            }
            else {
                newPosition.pose.orientation = compose(newPosition.pose.orientation,
                        rotation(unitRotation * motion.motion_units));
            }
            return newPosition;
        }
        else return null;
    }
    
    private GazeboModelState createModelStateMessage() {
        GazeboModelState message = new GazeboModelState();
        message.model_name = modelName;
        message.pose = new GeometryPose();
        message.twist = new GeometryTwist();
        message.pose.position = new GeometryPoint();
        message.pose.orientation = new GeometryQuaternion();
        message.twist.linear = new GeometryVector3();
        message.twist.angular = new GeometryVector3();
        message.reference_frame = referenceFrame;
        return message;
    }
    
    private GetModelStateResponse getCurrentPosition() {
        GetModelStateResponse result = null;
        try {
            result = getPosition.callBlocking(request);
            getPosition.call(request);
            if (result != null && !result.success) {
                System.out.println("Error retrieving position: " + result.status_message);
                result = null;
            }
        }
        catch (InterruptedException ex) {
        }
        return result;
    }

    private GeometryQuaternion rotation(double angle) {
        GeometryQuaternion quaternion = new GeometryQuaternion();
        double s = Math.sin(angle / 2);
        quaternion.x = 0;
        quaternion.y = 0;
        quaternion.z = s;
        quaternion.w = Math.cos(angle / 2);
        return quaternion;
    }

   // Quaternion multiplication (compose two rotations)
    public GeometryQuaternion compose(GeometryQuaternion a, GeometryQuaternion b) {
        GeometryQuaternion result = new GeometryQuaternion();
        result.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
        result.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
        result.y = a.w*b.y + a.y*b.w + a.z*b.x - a.x*b.z;
        result.z = a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x;
        return result;
    }        
    
    
}

