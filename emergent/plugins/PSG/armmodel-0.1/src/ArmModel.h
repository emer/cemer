/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/* Desc: Model for two-linked robotarm
 * Author: Sarah Gerster
 * Date: June 2005
 */


#ifndef ARMMODEL_HH
#define ARMMODEL_HH


#include <gazebo.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
  
#include <gazebo/BoxGeom.hh>
#include <gazebo/CylinderGeom.hh>
#include <gazebo/ModelFactory.hh>
#include <gazebo/Body.hh>
#include <gazebo/Model.hh>
#include <gazebo/World.hh>
#include <gazebo/WorldFile.hh>
#include <gazebo/HingeJoint.hh>
#include <gazebo/SliderJoint.hh>
#include <gazebo/PlaneGeom.hh>
#include <gazebo/Error.hh>

#include "kinematics.h"




class ArmModel : public Model
{
  public: 
  
    // Constructor
    ArmModel( World *world );
  
    // Destructor
    virtual ~ArmModel();

    // Load stuff
    int Load( WorldFile *file, WorldFileNode *node );
  
    // Initialize stuff
    int Init( WorldFile *file, WorldFileNode *node );

    // Finalize stuff
    int Fini();
  
    // Update the model state
    virtual void Update( double step );
    
    // Grip command functions
    void OpenGrip();
    void CloseGrip();
    void StopGrip();
  
    // Lift command functions
    void StopLift();
    void LiftUp();
    void LiftDown();

  private:
  
    // Update the odometry
    void UpdateOdometry( double step );

    // Get commands from the external interface
    void GetPositionCmd();
    
    // Get commands for position of tip in x-y-plane
    void GetXyPositionCmd();
    
    // Get commands for gripper (up/down and open/closed)
    void GetGripperPositionCmd();
     
    // Update the data in the external interface
    // This doesn't send out commands, it just updates the data
    void PutPositionData();

    // Load ODE stuff
    int OdeLoad( WorldFile *file, WorldFileNode *node );
    
    // Draw the base
    int DrawBase(Geom *geom, WorldFileNode *node);
    
    // Draw the links
    int DrawLinks(Geom *geom, WorldFileNode *node);
    
    // Draw Gripper
    int DrawGripper(Geom *geom, WorldFileNode *node);
    
    // Set the pos of the links relative to the pos of the base
    int SetLinksPos();
    
    // Set the pos of the gripper relative to the pos of the base
    int SetGripperPos();
    
    // Set the joints for the links
    int SetLinksJoints();
    
    // Set the joints for the gripper
    int SetGripperJoints();
    
    // Put a skin on object
    int SetSkin(Geom *geom, const char *skinFile, GzVector pos, GzQuatern rot,
                GzVector scale);
    
    // Update parameters
    double updatePeriod, updateTime;
                    
    // External interfaces
    gz_position_t *position_iface;
    gz_gripper_t  *gripper_iface;
  
    // Odometric pose and speed estimates
    double odomPose[6];
    double odomVel[6];

    // ODE components
    Body *basePlate;
    Body *base;
    Body *links[3];
    Body *gripper;
    Body *leftGripper;
    Body *rightGripper;
    Body *baseHelp;
    Body *chessFigure;
    HingeJoint *baseJoint;
    HingeJoint *linkJoints[3];
    HingeJoint *leftGripperJoint;
    HingeJoint *rightGripperJoint;
    SliderJoint *gripperSliderJoint;
    PlaneGeom *planeGeom;
  
    // Geometry...
      // Base
      double mBasePlateLength;
      double mBasePlateWidth;
      double mBasePlateHeight;
      double mBasePlateMass;
      double mBaseLength;
      double mBaseWidth;
      double mBaseHeight;
      double mBaseMass;
    
      // Links
        // "real" link
        double mLinkLength[3];
        double mLinkWidth[3];
        double mLinkHeight[3];
        double mLinkMass[3];
        double mLinkFixation[3];
        double mLinkLimit[3];
        double mGripperHoldLength;
        double mGripperHoldWidth;
        double mGripperHoldHeight;
      
        // Help pieces to make link rotate around the joint 
        // instead of around its centre of mass
        double mWeightLength[3];
        double mWeightWidth[3];
        double mWeightHeight[3];
        double mWeightMass[3];
    
    
      //Gripper
        // "real" gripper
        // fingers
        double mGripperLength;
        double mGripperWidth;
        double mGripperHeight;
        double mGripperDistance; // distance between gripper fingers
        double mGripperMass;
        //base
        double mGripperBaseLength;
        double mGripperBaseWidth;
        double mGripperBaseHeight;
        double mGripperBaseMass;
        double mGripperSliderLength;
        double mGripperSliderWidth;
        double mGripperSliderHeight;
        double mGripperSliderMass;
      
        // help variables to get the rotations right
        double mGripperHelpHeight;
      
        // Joint limits, forces, speeds
        double mLiftHeight; // distance along which gripper can slide
        double mLiftMaxForce;
        double mGripMaxForce;
        double mGripMaxSpeed;
        double mLiftMaxSpeed;
        double mGripperLimitHigh[2];
        double mGripperLimitLow[2];
        double mLiftMinPosLimit;
        double mLiftMaxPosLimit;
      
    // Joint velocities
    double mJointSpeed[3];
    
    // mRobot = 0 if arm is on the ground, 1 if arm is on a robot
    double mRobot;
    
    // Using gripper? 0 = no, 1 = yes; 
    double mGripper;
    
    // Gripper's mode
    bool mGripperOpen;
    bool mGripperOpening;
    bool mGripperStopped;
   
    // Lift's mode
    bool mGripperUp;
    bool mGripperMoving;
};

#endif

