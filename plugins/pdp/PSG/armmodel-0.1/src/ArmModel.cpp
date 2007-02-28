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
 
/** Kinematic funcitons to be used with the arm driver
  *
  * @author Sarah Gerster
  * @date 16.06.2005
  * @addtogroup iris_models Models 
  * @{ */

/** 
 * @defgroup player_model_arm arm
 
The @p arm model simulates a two-linked robot arm with a gripper as end
effector.

@htmlinclude arm_view.html

@par Provides
- @ref gazebo_interface_position3d : Interface to the position3d
 
@par Requires
-  @ref gazebo_interface_position3d : Interface to the position3d
-  @ref gazebo_interface_gripper : Interface to gripper
 
@par World file options:
- arm on ground or on robot (int)
  - Default: 0 (arm on ground)
  - 1 or 0
- gripper or no gripper (int)
  - Default: 1 (there is a gripper)
  - 1 or 0
- id (string)
- skin for base:
  - skinBase (filename)
  - skinBaseScale (3 doubles)
  - skinBaseXyz (3 doubles)
  - skinBaseRpy (3doubles)
  - Default: no skin if no skinfile is specified
- skin for links:
  - skinLink0 (filename)
  - skinLink0Scale (3 doubles)
  - skinLink0Xyz (3 doubles)
  - skinLink0Rpy (3 doubles)
  - skinLink1 (filename)
  - skinLink1Scale (3 doubles)
  - skinLink1Xyz (3 doubles)
  - skinLink1Rpy (3 doubles)
  - Default: no skin if no skinfile is specified
- skin for gripper:
  - skinLeftGripper (filename)
  - skinLeftGripperScale (3 doubles)
  - skinLeftGripperXyz (3 doubles)
  - skinLeftGripperRpy (3 doubles)
  - skinRightGripper (3 doubles)
  - skinRightGripperScale (3 doubles)
  - skinRightGripperXyz (3 doubles)
  - skinRightGripperRpy (3 doubles)
  - Default: no skin if no skinfile is specified
  
@par Example: Worldfile
The following file illustrates the use of the worldfile:
 
@include ground.world


@par Notes

@par Default configuration:
You can choose if the arm is on the ground or on a robot. You can also say if
the arm should have a gripper or not. You can set those   /// preferences in the
worldfile. They are read in the function ODELoad. If you don't specify
anything, or give unexpected values, the default configuration will be
used (arm has a gripper, it stands on the ground).

@par Gravity
I turned off the gravity for my simulation. It is of no interest here, and it
makes the simulation behave very strangely. Objects that are too small start
shaking, joints move even if htey are stopped,...

@par Coordinates
I get all the commands in jointspace coordinates and update also all the
data in jointspace coordinates. This has the advantage of causing less errors
because of conversions between coordinate spaces. The client can send a command
either in jointspace or in taskspace. The driver changes everything in
jointspace, changes position commands to velocity commands if needed, and then
sends commands in jointspace coordinates which can directly be passed on to the
joints to the model. 
 
@todo
- Figure out why the gripper is shaking when it is closed. The shaking stopps
after ~33 simulation time?!?
- Max force for gripping is set very low, otherwise gripper shakes even more.
The problem now is that it can only pick up objects with very very small mass.
How can this be fixed?
- Can't use too many cameras at a time, or put too many skins on the model,
oterwise it gets very slow and moves jerkily. Is there anything that can be
done to change that?
- Test how well simulation agrees with real arm.
- Test the configuration where the arm is on a robot. This seems not to work
very well... 
*/

/** @} */
#include "ArmModel.h"

/////////////////////////////////////////////////////////////////////////////
// Register the model
GZ_REGISTER_PLUGIN("ArmModel", ArmModel)


//////////////////////////////////////////////////////////////////////////////
// Constructor
ArmModel::ArmModel(World *world)
    : Model(world)
{ 
  // declare and initialise all the geometric values
  /* warning: If you change those values, you have to check if you need if
   * you also need to change it in the driver (gz_arm_position3d).
   */  
  // Base parameters
  this->mBasePlateLength = 0.12;
  this->mBasePlateWidth  = 0.12;
  this->mBasePlateHeight = 0.01;
  this->mBasePlateMass   = 0.05;
  this->mBaseLength    = 0.075;
  this->mBaseWidth     = 0.06;
  this->mBaseHeight    = 0.16;
  this->mBaseMass      = 0.50;
  
  // Link parameters
  this->mLinkLength[0]   = 0.215;
  this->mLinkWidth[0]    = 0.04;
  this->mLinkHeight[0]   = 0.0145;
  this->mLinkMass[0]     = 0.20;
  this->mLinkLimit[0]    = M_PI/2;
  this->mLinkLength[1]   = 0.16;
  this->mLinkWidth[1]    = 0.04;
  this->mLinkHeight[1]   = 0.009;
  this->mLinkMass[1]     = 0.10;
  this->mLinkLimit[1]    = 7*M_PI/10;
  this->mLinkFixation[0] = 0.04;
  this->mLinkFixation[1] = 0.02;
  this->mGripperHoldLength = 0.023;
  this->mGripperHoldWidth = 0.04;
  //Only Length where gripper can slide
  this->mGripperHoldHeight = 0.08;
  
  // Help to get axis of rotation of links to the right spot
  this->mWeightLength[0] = 0.04;
  this->mWeightWidth[0]  = 0.04;
  this->mWeightHeight[0] = 0.002;
  this->mWeightMass[0]   = 1;
  this->mWeightLength[1] = 0.04;
  this->mWeightWidth[1]  = 0.04;
  this->mWeightHeight[1] = 0.002;
  this->mWeightMass[1]   = 1;
  
  // Gripper parameters
  this->mGripperLength = 0.015;
  this->mGripperWidth = 0.001;
  this->mGripperHeight = 0.055;
  this->mGripperDistance = 0.019;
  this->mGripperHelpHeight = 0.005;
  this->mGripperMass = 0.1;
  this->mGripperBaseLength = 0.03;
  this->mGripperBaseWidth = 0.04;  
  this->mGripperBaseHeight = 0.035;
  this->mGripperBaseMass = 0.1;
  this->mGripperSliderHeight = 0.008;
  this->mGripperSliderLength = 0.003;
  this->mGripperSliderWidth = 0.04;
  this->mGripperSliderMass = 0.1;
  this->mLiftHeight = 0.07;
  this->mLiftMaxForce = 1;
  this->mGripMaxForce = 0.01;
  this->mGripMaxSpeed = 1;
  this->mLiftMaxSpeed = 0.02;
  this->mGripperLimitLow[0] = -(5*M_PI/180);
  this->mGripperLimitHigh[0] = (3*M_PI/180);
  this->mGripperLimitLow[1] = -(3*M_PI/180);
  this->mGripperLimitHigh[1] = (5*M_PI/180); 
  this->mLiftMinPosLimit = 0.;
  this->mLiftMaxPosLimit = this->mLiftHeight;
  
  // Joint speeds to give the commands
  this->mJointSpeed[0] = 0.0;
  this->mJointSpeed[1] = 0.0;
  this->mJointSpeed[2] = 0.0;
  
  // update period
  this->updatePeriod = 0.1;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
ArmModel::~ArmModel()
{
  return;
}


//////////////////////////////////////////////////////////////////////////////
int ArmModel::Load(WorldFile *file, WorldFileNode *node)
{

  // Create the ODE objects
  if (this->OdeLoad(file, node) != 0)
    return -1;
   
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load ODE objects
int ArmModel::OdeLoad(WorldFile *file, WorldFileNode *node)
{
  /* 0 = arm standing alone; 1 = arm mounted on robot
   * If another value is given in the config file, the simulation uses the
   * default value => arm on ground
   */
  mRobot = node->GetDouble("robot", 0);
  if (!((mRobot == 0) || (mRobot == 1)))
  {
    printf("Use default value... => robot on ground");
    mRobot = 0;
  }
  
  /* Using gripper? 0 = no, 1 = yes;
   * If another value is given in the config file, the simulation uses the
   * default value => arm with gripper
   */
  mGripper = node->GetDouble("gripper", 1);
  if (!((mGripper == 0) || (mGripper == 1)))
  {
    printf("Use default value... => there is a gripper");
    mGripper = 1;
  }
  
  // Variable to draw objects
  Geom *geom;
 
  // Draw Base   
  this->DrawBase(geom, node);
  
  // Draw links
  this->DrawLinks(geom, node);  
  this->SetLinksPos();
  this->SetLinksJoints();

  // Check if a gripper should be drawn
  if (mGripper)
  {  
    // Draw gripper
    this->DrawGripper(geom, node);
    this->SetGripperPos();
    this->SetGripperJoints();
    
    // Initialize gripper state to being closed and lift up
    this->CloseGrip();
    this->LiftUp();
    
    // Set the variables since initialization doesn't always work right
    this->mGripperOpen = false;
    this->mGripperStopped = true;
    this->mGripperUp = true;
    this->mGripperMoving = false;
  
  }

  return 0;
}


////////////////////////////////////////////////////////////////////////////////
int ArmModel::DrawLinks(Geom *geom, WorldFileNode *node)
{
  const char *skin_file; // variable to save skin filename
  
  // Create the links
  for (uint16_t i = 0; i < 2; i++)
  {
    // Create a real body
    this->links[i] = new Body(this->world, NULL);

    // Draw the linksweigths (neede for rotation around joint)
    geom = new BoxGeom(this->links[i], this->modelSpaceId, mWeightLength[i],
                        mWeightWidth[i], mWeightHeight[i]);
    geom->SetRelativePosition(GzVectorSet(0, 0, 0));
    geom->SetMass(mWeightMass[i]);         
    
    // Each link gets a different color
    if(i==0) // first link
    {
      // Apply a skin
      // Check if a skin filename is given in the worldfile
      skin_file = node->SearchFilename("skinLink0", NULL, NULL);
      if (skin_file)
      {
        GzVector pos;
        GzQuatern rot;
        GzVector scale;
        
        // set oject to be transparent and turn off rendering
        geom->SetTransparency(true);
        geom->SetColor(GzColor(0.0, 0.9, 0.0, 0));
        geom->SetSkinNull(skin_file != NULL);
        
        // set the skin on the object
        scale = node->GetPosition("skinLink0Scale", GzVectorSet(1,1,1));
        pos = node->GetPosition("skinLink0Xyz", GzVectorZero());
        rot = node->GetRotation("skinLink0Rpy", GzQuaternIdent());
        this->SetSkin(geom, skin_file, pos, rot, scale);
      } 
      else
      {
        // Draw the box
        geom->SetTransparency(false);
        geom->SetColor(GzColor(0.0, 0.9,0.0));
      }
    }
    else // second link
    {
      // Apply a skin
      skin_file = node->SearchFilename("skinLink1", NULL, NULL);
      if (skin_file)
      {
        GzVector pos;
        GzQuatern rot;
        GzVector scale;  
        
        // set oject to be transparent and turn off rendering      
        geom->SetTransparency(true);
        geom->SetColor(GzColor(0.0, 0.0, 0.9, 0));
        geom->SetSkinNull(skin_file != NULL);
        
        scale = node->GetPosition("skinLink1Scale", GzVectorSet(1,1,1));
        pos = node->GetPosition("skinLink1Xyz", GzVectorZero());
        rot = node->GetRotation("skinLink1Rpy", GzQuaternIdent());
        this->SetSkin(geom, skin_file, pos, rot, scale);
      } 
      else
      {
        geom->SetTransparency(false);
        geom->SetColor(GzColor(0.0, 0.0, 0.9));
      } 
    }
    
    // Draw the links
    geom = new BoxGeom(this->links[i], this->modelSpaceId, mLinkLength[i],
                        mLinkWidth[i], mLinkHeight[i]);
    geom->SetRelativePosition(GzVectorSet(mLinkLength[i]/2.0 -
                               mWeightLength[i]/2.0, 0., mWeightHeight[i]/2 +
                               mLinkHeight[i]/2));
    geom->SetMass(mLinkMass[i]);
    
    // set a different color for each link
    if(i==0) // first link
    {
      if(skin_file)
      {
        // make object transparent and turn off rendering 
        geom->SetTransparency(true);
        geom->SetColor(GzColor(0, 1, 0, 0));
        geom->SetSkinNull(skin_file != NULL);
      }
      else
      {
        geom->SetTransparency(false);
        geom->SetColor(GzColor(0,1,0));
      }
    }
    else // second link
    {
      if(skin_file)
      {
        // make object transparent and turn off rendering 
        geom->SetTransparency(true);
        geom->SetColor(GzColor(0, 0, 1, 0));
        geom->SetSkinNull(skin_file != NULL);
      }
      else
      {
        geom->SetTransparency(false);
        geom->SetColor(GzColor(0,0,1));
      }
      // Second link needs a additional part ot hold the gripper
      geom = new BoxGeom(this->links[i], this->modelSpaceId,
                          mGripperHoldLength, mGripperHoldWidth,
                          mGripperHoldHeight);
      geom->SetRelativePosition(GzVectorSet(mLinkLength[i] -
                                 mWeightLength[i]/2.0 - mGripperHoldLength/2.0,
                                 0., mWeightHeight[i]/2.0 -
                                 mGripperHoldHeight/2.0));
      geom->SetMass(mLinkMass[i]);
      if(skin_file)
      {
        // make object transparent and turn off rendering 
        geom->SetTransparency(true);
        geom->SetColor(GzColor(0, 0, 1, 0));
        geom->SetSkinNull(skin_file != NULL);
      }
      else
      {
        geom->SetTransparency(false);
        geom->SetColor(GzColor(0,0,1));
      }
      
    }
    
    // add new features to simulation
    this->AddBody(this->links[i]);
  }
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
int ArmModel::DrawBase(Geom *geom, WorldFileNode *node)
{
  const char *skin_file;
  skin_file = node->SearchFilename("skinBase", NULL, NULL);
  
  // Different building of base depending if arm is on robot or not
  if (mRobot)
  {
    // Arm on robot
    this->base = new Body(this->world);
    
    // Create base plate
    geom = new BoxGeom(this->base, this->modelSpaceId, mBasePlateLength,
                      mBasePlateWidth, mBasePlateHeight);
    geom->SetRelativePosition(GzVectorSet(0, 0, 0));
    geom->SetMass(mBasePlateMass);
    geom->SetColor(GzColor(1, 1, 1));
  
    if (skin_file)
    {
      // This seems not to work right yet.... :(
      // make object transparent and turn off rendering
      geom->SetTransparency(true);
      geom->SetColor(GzColor(0.9, 0.9, 0.0, 0));
      geom->SetSkinNull(skin_file != NULL);
    }
    else
    {
      geom->SetTransparency(false);
      geom->SetColor(GzColor(0.9, 0.9, 0.0));
    }
    
     // Create the base 
    geom = new BoxGeom(this->base, this->modelSpaceId, mBaseLength,
                      mBaseWidth, mBaseHeight);
    geom->SetRelativePosition(GzVectorSet(0, 0, mBasePlateHeight +
                                         mBaseHeight/2.0));
    geom->SetMass(mBaseMass);
    geom->SetColor(GzColor(1, 0, 0));

    this->AddBody(this->base,true);
    
    // Apply a skin
    if (skin_file)
    {
      GzVector pos;
      GzQuatern rot;
      GzVector scale;
      
      scale = node->GetPosition("skinBaseScale", GzVectorSet(1,1,1)); 
      pos = node->GetPosition("skinBaseXyz", GzVectorZero());
      rot = node->GetRotation("skinBaseRpy", GzQuaternIdent());
      geom->SetSkinNull(skin_file != NULL);
      this->SetSkin(geom, skin_file, pos, rot, scale);
    } 
                              
  }
  else
  {
    // Arm on ground
    // Make a dummy body to attach the groudplane and the base to it
    this->base = new Body(this->world , NULL, true);
 
    // The plane parameters
    GzVector normal = GzVectorSet(0, 0, 1);
    double altitude = 0;

    // Create a ground plane
    this->planeGeom = new PlaneGeom(this->base, this->modelSpaceId, altitude,
                                     normal);
    // Set color
    this->planeGeom->SetColor(GzColor(0.31, 0.3, 0.3));

    // Surface hardness
    this->planeGeom->SetHardness(dInfinity, 0);

    // Surface friction
    this->planeGeom->SetFriction(1.0, 1.0);

    // This is a fixed model, and should not collide with other fixed models
    dGeomSetCategoryBits((dGeomID) this->modelSpaceId, GZ_FIXED_COLLIDE);
    dGeomSetCollideBits((dGeomID) this->modelSpaceId, ~GZ_FIXED_COLLIDE);
    
    // Create the base
    geom = new BoxGeom(this->base, this->modelSpaceId, mBaseLength,
                        mBaseWidth, mBaseHeight);
    geom->SetRelativePosition(GzVectorSet(0, 0, mBasePlateHeight +
                                           mBaseHeight/2.0));
    geom->SetMass(mBaseMass);
    
    // Decide if model has to be transparent or not
    skin_file = node->SearchFilename("skinBase", NULL, NULL);
    if (skin_file)
    {
      // make object transparent and turn off rendering
      geom->SetTransparency(true);
      geom->SetColor(GzColor(0.9, 0.9, 0.0, 0));
      geom->SetSkinNull(skin_file != NULL);
    }
    else
    {
      geom->SetTransparency(false);
      geom->SetColor(GzColor(0.9, 0.9, 0.0));
    }

    // Create base plate
    geom = new BoxGeom(this->base, this->modelSpaceId, mBasePlateLength,
                        mBasePlateWidth, mBasePlateHeight);
    geom->SetRelativePosition(GzVectorSet(0, 0, mBasePlateHeight/2.0));
    geom->SetMass(mBasePlateMass);
    geom->SetColor(GzColor(0.9, 0.9, 0.0));
    geom->SetSkinNull(skin_file != NULL);   

    this->AddBody(this->base,true);
    
    // Apply a skin
    if (skin_file)
    {
      GzVector pos;
      GzQuatern rot;
      GzVector scale;
      
      scale = node->GetPosition("skinBaseScale", GzVectorSet(1,1,1)); 
      pos = node->GetPosition("skinBaseXyz", GzVectorZero());
      rot = node->GetRotation("skinBaseRpy", GzQuaternIdent());
      geom->SetSkinNull(skin_file != NULL);
      this->SetSkin(geom, skin_file, pos, rot, scale);
    } 
                             
  }
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
int ArmModel::DrawGripper(Geom *geom, WorldFileNode *node)
{
  const char *skin_file;
    
  // Create a real body
  this->gripper = new Body(this->world, NULL);

  // Draw the gripperbase
  geom = new BoxGeom(this->gripper, this->modelSpaceId,
                     mGripperSliderLength, mGripperSliderWidth,
                     mGripperSliderHeight);
  geom->SetRelativePosition(GzVectorSet(0.,0.,0.));
  geom->SetMass(mGripperSliderMass);                   
                     
  geom = new BoxGeom(this->gripper, this->modelSpaceId,
                     mGripperBaseLength, mGripperBaseWidth, mGripperBaseHeight);
  geom->SetRelativePosition(GzVectorSet(mGripperSliderLength +
                                        mGripperBaseLength/2.0, 0.,
                                        mGripperSliderHeight/2.0 -
                                        2.*mGripperBaseHeight/6.0));
  geom->SetMass(mGripperBaseMass);
  
  // Apply a skin
  skin_file = node->SearchFilename("skinGripperBase", NULL, NULL);
  if (skin_file)
  {
    GzVector pos;
    GzQuatern rot;
    GzVector scale;
    
    // make object transparent and turn off rendering
    geom->SetTransparency(true);
    geom->SetColor(GzColor(1.0, 1.0, 1.0, 0));
    geom->SetSkinNull(skin_file != NULL);
      
    scale = node->GetPosition("skinGripperBaseScale", GzVectorSet(1,1,1));
    pos = node->GetPosition("skinGripperBaseXyz", GzVectorZero());
    rot = node->GetRotation("skinGripperBaseRpy", GzQuaternIdent());
    this->SetSkin(geom, skin_file, pos, rot, scale);
  } 
  else
  {   
    geom->SetTransparency(false);
    geom->SetColor(GzColor(1.0, 1.0,1.0));
  }  
  // Add gripperbase to simulation
  this->AddBody(this->gripper);
  // reinitialize Skinfile to be able to use it for other gripper parts
  skin_file = NULL;
 
  // Setup the left gripper finger
  this->leftGripper = new Body(this->world, NULL); 
  // Draw the help pieces for left gripper
  geom = new BoxGeom(this->leftGripper, this->modelSpaceId,
                      mGripperLength, mGripperWidth, mGripperHelpHeight);
  geom->SetRelativePosition(GzVectorSet(0, 0, 0));
  geom->SetMass(mGripperMass);         
    
  // Apply a skin
  skin_file = node->SearchFilename("skinLeftGripper", NULL, NULL);
  if (skin_file)
  {
    GzVector pos;
    GzQuatern rot;
    GzVector scale;
    
    // make object transparent and turn off rendering
    geom->SetTransparency(true);
    geom->SetColor(GzColor(1.0, 0.0, 0.0, 0));
    geom->SetSkinNull(skin_file != NULL);
      
    scale = node->GetPosition("skinLeftGripperScale", GzVectorSet(1,1,1));
    pos = node->GetPosition("skinLeftGripperXyz", GzVectorZero());
    rot = node->GetRotation("skinLeftGripperRpy", GzQuaternIdent());
    this->SetSkin(geom, skin_file, pos, rot, scale);
  } 
  else
  {
    geom->SetTransparency(false);
    geom->SetColor(GzColor(1.0, 0.0,0.0));
  }  
  // Draw left gripper finger
  geom = new BoxGeom(this->leftGripper, this->modelSpaceId,
                     mGripperLength, mGripperWidth, mGripperHeight);
  geom->SetMass(mGripperMass);
  geom->SetColor(GzColor(1, 1, 1));
  geom->SetRelativePosition(GzVectorSet(0, 0, -mGripperHeight/2 -
                            mGripperHelpHeight/2)); 
  geom->SetFriction(dInfinity,dInfinity);
  if (skin_file)
  {
    // make object transparent and turn off rendering
    geom->SetTransparency(true);
    geom->SetColor(GzColor(1.0, 1.0, 1.0, 0));
    geom->SetSkinNull(skin_file != NULL);
  } 
  else
  {
    geom->SetTransparency(false);
    geom->SetColor(GzColor(1.0, 1.0,1.0));
  }
  // Add left gripper to simulation
  this->AddBody(this->leftGripper); 
  // reinitialize Skinfile to be able to use it for other gripper parts
  skin_file = NULL;
  
  // Setup the right gripper finger
  this->rightGripper = new Body(this->world, NULL);
  // Draw the help pieces for right gripper
  geom = new BoxGeom(this->rightGripper, this->modelSpaceId,
                     mGripperLength, mGripperWidth, mGripperHelpHeight);
  geom->SetRelativePosition(GzVectorSet(0, 0, 0));
  geom->SetMass(mGripperMass);         
    
  // Apply a skin
  skin_file = node->SearchFilename("skinRightGripper", NULL, NULL);
  if (skin_file)
  {
    GzVector pos;
    GzQuatern rot;
    GzVector scale;
    
    // make object transparent and turn off rendering
    geom->SetTransparency(true);
    geom->SetColor(GzColor(1.0, 0.0, 0.0, 0));
    geom->SetSkinNull(skin_file != NULL);
      
    scale = node->GetPosition("skinRightGripperScale", GzVectorSet(1,1,1));
    pos = node->GetPosition("skinRightGripperXyz", GzVectorZero());
    rot = node->GetRotation("skinRightGripperRpy", GzQuaternIdent());
    this->SetSkin(geom, skin_file, pos, rot, scale);
  } 
  else
  {
    geom->SetTransparency(false);
    geom->SetColor(GzColor(1.0, 0.0,0.0));
  }  
  // Draw right gripper finger
  geom = new BoxGeom(this->rightGripper, this->modelSpaceId,
                      mGripperLength, mGripperWidth, mGripperHeight);
  geom->SetMass(mGripperMass);
  geom->SetRelativePosition(GzVectorSet(0, 0, -mGripperHeight/2
                             - mGripperHelpHeight/2)); 
  geom->SetFriction(dInfinity,dInfinity);
  if (skin_file)
  {
    // make object transparent and turn off rendering
    geom->SetTransparency(true);
    geom->SetColor(GzColor(1.0, 1.0, 1.0, 0));
    geom->SetSkinNull(skin_file != NULL);
  } 
  else
  {
    geom->SetTransparency(false);
    geom->SetColor(GzColor(1.0, 1.0,1.0));
  }  
  // Add right gripper to simulation
  this->AddBody(this->rightGripper);
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
int ArmModel::SetLinksPos()
{
  // To attach the links to the base, we have to say where they are, how
  // they are arranged, and what their current motion is.
  
  // The origin of the model is in the centre of the base, on the floor.
  // => centre of mass of base is at (0,0,height/2).
  double xpos;
  double ypos = 0.;
  double zpos;
  
  // first link
  // Compute position of centre of first link with respect to origin
  xpos = mBaseLength/2. - mLinkFixation[0];
  zpos = mBasePlateHeight + mBaseHeight + mWeightHeight[0]/2.0; 
  this->links[0]->SetPosition(GzVectorSet(xpos, ypos, zpos));
  // No initial angular velocity
  this->links[0]->SetAngularVel(GzVectorSet(0.0, 0.0, 0.0)); 
  // No initial linear velocity
  this->links[0]->SetLinearVel(GzVectorSet(0.0, 0.0, 0.0)); 
  /* rotation can only be about z-Axis. Default position to start the arm is
   * when its looking in positive x-direction => no initial angle.
   */
  this->links[0]->SetRotation(GzQuaternFromAxis(0, 0, 1, 0));
  // Turn off gravity to get rid of some of the strange "shaking artifacts"
  this->links[0]->SetGravityMode(false);
  
  // second link
  // Compute position of centre of first link with respect to origin
  xpos = mBaseLength/2. - mLinkFixation[0] + mLinkLength[0] -
         mWeightLength[0]/2. - mLinkFixation[1];
  zpos = mBasePlateHeight + mBaseHeight + mWeightHeight[0] + mLinkHeight[0] +
         mWeightHeight[1]/2.0;
  this->links[1]->SetPosition(GzVectorSet(xpos, ypos, zpos));
  // No initial angular velocity
  this->links[1]->SetAngularVel(GzVectorSet(0.0, 0.0, 0.0)); 
  // No initial linear velocity
  this->links[1]->SetLinearVel(GzVectorSet(0.0, 0.0, 0.0)); 
  /* rotation can only be about z-Axis. Default position to start the arm is
   * when its looking in positive x-direction => no initial angle.
   */
  this->links[1]->SetRotation(GzQuaternFromAxis(0, 0, 1, 0));
  // Turn off gravity to get rid of some of the strange "shaking artifacts"
  this->links[1]->SetGravityMode(false);
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
int ArmModel::SetGripperPos()
{
  // Help variables to avoid redundant placement statements
  double gripper_xpos;
  double gripper_ypos;
  double gripper_zpos;
  double leftpos[3];
  double rightpos[3];
  
  // compute gripper position with respect to the origin of the model
  gripper_xpos = mBaseLength/2. - mLinkFixation[0] + mLinkLength[0] -
                 mWeightLength[0]/2. - mLinkFixation[1] - mWeightLength[1]/2. +
                 mLinkLength[1] + mGripperSliderLength/2.;
  gripper_ypos = 0.0;
  gripper_zpos = mBasePlateHeight + mBaseHeight + mWeightHeight[0] +
                 mLinkHeight[0] + mWeightHeight[1] - mGripperHoldHeight +
                 mLiftHeight - mGripperSliderHeight/2.0; 
  this->gripper->SetPosition(GzVectorSet(gripper_xpos, gripper_ypos,
                                         gripper_zpos));
  // No initial angular velocity                                         
  this->gripper->SetAngularVel(GzVectorSet(0.0, 0.0, 0.0)); 
  // No initial linear velocity
  this->gripper->SetLinearVel(GzVectorSet(0.0, 0.0, 0.0)); 
  // leave angle at 0
  this->gripper->SetRotation(GzQuaternFromAxis(0, 0, 1, 0));
  // Turn off gravity to get rid of some of the strange "shaking artifacts"
  this->gripper->SetGravityMode(false);
   
  // compute left finger position with respect to the origin of the model
  leftpos[0] = gripper_xpos + mGripperSliderLength/2. + mGripperBaseLength/2.;
  leftpos[1] = gripper_ypos + mGripperDistance/2.;
  leftpos[2] = gripper_zpos - mGripperHelpHeight/2. - 2.*mGripperBaseHeight/3.;
  this->leftGripper->SetPosition(GzVectorSet(leftpos[0], leftpos[1],
                                             leftpos[2]));
  // No initial angular velocity                                           
  this->leftGripper->SetAngularVel(GzVectorSet(0.0, 0.0, 0.0)); 
  // No initial linear velocity
  this->leftGripper->SetLinearVel(GzVectorSet(0.0, 0.0, 0.0)); 
  // leave angle at 0, then set it when simulation is started
  this->leftGripper->SetRotation(GzQuaternFromAxis(1, 0, 0, 0));
  // Turn off gravity to get rid of some of the strange "shaking artifacts"
  this->leftGripper->SetGravityMode(false); 
  
  // compute left finger position with respect to the origin of the model
  rightpos[0] = gripper_xpos + mGripperSliderLength/2. + mGripperBaseLength/2.;
  rightpos[1] = gripper_ypos - mGripperDistance/2.;
  rightpos[2] = gripper_zpos - mGripperHelpHeight/2. - 2.*mGripperBaseHeight/3.;
  this->rightGripper->SetPosition(GzVectorSet(rightpos[0], rightpos[1],
                                              rightpos[2]));
  // No initial angular velocity                                              
  this->rightGripper->SetAngularVel(GzVectorSet(0.0, 0.0, 0.0)); 
  // No initial linear velocity
  this->rightGripper->SetLinearVel(GzVectorSet(0.0, 0.0, 0.0)); 
  // leave angle at 0, then set it when simulation is started
  this->rightGripper->SetRotation(GzQuaternFromAxis(1, 0, 0, 0));
  // Turn off gravity to get rid of some of the strange "shaking artifacts"
  this->rightGripper->SetGravityMode(false);
 
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
int ArmModel::SetLinksJoints()
{
  // attach first link to base
  this->linkJoints[0] = new HingeJoint(this->world->worldId);
  this->linkJoints[0]->Attach(this->links[0], this->base);
  // Set an anchor for the joint
  GzVector a = links[0]->GetPosition();    
  this->linkJoints[0]->SetAnchor(a.x, a.y, a.z);   
  // Set axis of rotation                    
  this->linkJoints[0]->SetAxis(0, 0, 1);
  // Turn off the bouncing
  this->linkJoints[0]->SetParam(dParamBounce, 0);
  // Set joint limits
  this->linkJoints[0]->SetParam(dParamLoStop, -mLinkLimit[0]);
  this->linkJoints[0]->SetParam(dParamHiStop,  mLinkLimit[0]);
  // Maximal force that can act on joint
  this->linkJoints[0]->SetParam(dParamFMax, 1.0);
    
  
  // attach second link to first link  
  this->linkJoints[1] = new HingeJoint(this->world->worldId);
  this->linkJoints[1]->Attach(this->links[1], this->links[0]);
  // Set an anchor for the joint
  GzVector b = links[1]->GetPosition();    
  this->linkJoints[1]->SetAnchor(b.x, b.y, b.z);   
  // Set axis of rotation                         
  this->linkJoints[1]->SetAxis(0, 0, 1);
  // Turn off the bouncing
  this->linkJoints[1]->SetParam(dParamBounce, 0);
  // Set joint limits
  this->linkJoints[1]->SetParam(dParamLoStop, -mLinkLimit[1]);
  this->linkJoints[1]->SetParam(dParamHiStop,  mLinkLimit[1]);
  // Maximal force that can act on joint
  this->linkJoints[1]->SetParam(dParamFMax, 1.0);
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
int ArmModel::SetGripperJoints()
{
  // This joint allows the gripper to lift objects  
  // Attach gripper to second link
  this->gripperSliderJoint = new SliderJoint(this->world->worldId);
  this->gripperSliderJoint->Attach(this->gripper, this->links[1]);
  // Set Axis
  this->gripperSliderJoint->SetAxis(0, 0, 1);
  // Set joint limits
  this->gripperSliderJoint->SetParam(dParamLoStop, -mLiftHeight);
  this->gripperSliderJoint->SetParam(dParamHiStop, 0);
  /* Set max force to move gripper. If it is too high, the joint limits are not
   * respected. If it is too low, it can't lift up the objects... 
   */
  this->gripperSliderJoint->SetParam(dParamFMax, this->mLiftMaxForce);
  // Turn off the bouncing
  this->gripperSliderJoint->SetParam(dParamBounce, 0); 

  // Left gripper joint
  // Attach left finger to gripper base
  this->leftGripperJoint = new HingeJoint(this->world->worldId);
  this->leftGripperJoint->Attach(this->leftGripper, this->gripper);
  // Set anchor
  GzVector d = leftGripper->GetPosition();    
  this->leftGripperJoint->SetAnchor(d.x, d.y, d.z);
  // Set axis of rotation
  this->leftGripperJoint->SetAxis(-1, 0, 0);
  // Turn off the bouncing
  this->leftGripperJoint->SetParam(dParamBounce, 0);
  // Set joint limits
  this->leftGripperJoint->SetParam(dParamLoStop, mGripperLimitLow[0]);
  this->leftGripperJoint->SetParam(dParamHiStop, mGripperLimitHigh[0]);

  // Right gripper joint
  // Attach right finger to gripper base
  this->rightGripperJoint = new HingeJoint(this->world->worldId);
  this->rightGripperJoint->Attach(this->rightGripper, this->gripper);
  // Set anchor
  GzVector e = rightGripper->GetPosition();    
  this->rightGripperJoint->SetAnchor(e.x, e.y, e.z);
  // Set axis of rotation
  this->rightGripperJoint->SetAxis(1, 0, 0);
  // Turn off the bouncing
  this->rightGripperJoint->SetParam(dParamBounce, 0);
  // Set joint limits
  this->rightGripperJoint->SetParam(dParamLoStop, mGripperLimitLow[1]);
  this->rightGripperJoint->SetParam(dParamHiStop, mGripperLimitHigh[1]);

  return 0;
}



////////////////////////////////////////////////////////////////////////////////
int ArmModel::SetSkin(Geom *geom, const char *skinFile, GzVector pos,
                      GzQuatern rot, GzVector scale)
{ 
  // Apply a skin
  PRINT_MSG1(1, "loading skin file [%s]", skinFile);  
  if (geom->SetSkinFile(skinFile) != 0)
  {
    PRINT_ERR("unable to load skin file");
    return -1;
  }
  
  // Skin geometry
  geom->SetSkinPose(GzPoseSet(pos, rot));
  geom->SetSkinScale(scale);                                       
                                          
  return 0;                         
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the sensor
int ArmModel::Init(WorldFile *file, WorldFileNode *node)
{
  // Initialize position(3d) interface
  this->position_iface = gz_position_alloc();
  assert(this->position_iface);
  if (gz_position_create(this->position_iface, this->world->gz_server,
                         this->GetId(), "ArmModel", (int)this,
                         (int)this->parent) != 0)
  {
    return -1;
  }
  
  // Initialization of gripper interface
  this->gripper_iface = gz_gripper_alloc();
  assert(this->gripper_iface);
  if (gz_gripper_create(this->gripper_iface, this->world->gz_server,
                        this->GetId(), "ArmModel", (int)this, 
                        (int)this->parent) != 0)
  {                        
    return -1;
  }
     
  // Reset odometric pose
  this->odomPose[0] = 0.0;
  this->odomPose[1] = 0.0;
  this->odomPose[2] = 0.0;
  this->odomPose[3] = 0.0;
  this->odomPose[4] = 0.0;
  this->odomPose[5] = 0.0;
 
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the sensor
int ArmModel::Fini()
{
  // Close position interface
  gz_position_destroy(this->position_iface);
  gz_position_free(this->position_iface);
  this->position_iface = NULL;
  
  // Close gripper interface
  gz_gripper_destroy( this->gripper_iface );
  gz_gripper_free( this->gripper_iface );
  this->gripper_iface = NULL;
  
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update the sensor information; returns non-zero if sensor data has
// been updated
void ArmModel::Update(double step)
{  
  // Do nothing if paused
  if (step == 0)
  {
    return;
  }
  
  // Update the odometry (do this always for better accuracy)
  this->UpdateOdometry(step);
  
  // If enough time passed since last update...
  if ((this->world->GetSimTime() - this->updateTime) > this->updatePeriod)
  {
    // update time
    this->updateTime = this->world->GetSimTime();
   
    // Get commands from the external interface
    this->GetPositionCmd();
           
    // Update the interfaces
    this->PutPositionData();
  }
  
  return;
}


////////////////////////////////////////////////////////////////////////////////
void ArmModel::UpdateOdometry(double step)
{
  double theta_speed[3];
  theta_speed[0] = linkJoints[0]->GetAngleRate();
  theta_speed[1] = linkJoints[1]->GetAngleRate();
  theta_speed[2] = 0;

  double theta[3];

  theta[0] = linkJoints[0]->GetAngle();
  theta[1] = linkJoints[1]->GetAngle();
  theta[2] = 0;
  
  /// @note I save the data in joint space => odomPose[3] = angle of joint 0,
  /// odomPose[4] = angle of joint 1. Same holds for the speeds...
  /* I don't care about the z coordinate here. The gripper can only be up or
   * down. This information is taken from the booleans that describe the current
   * state of the gripper
   */
  this->odomPose[0] = 0;
  this->odomPose[1] = 0;
  this->odomPose[2] = 0;
  this->odomPose[3] = theta[0];
  this->odomPose[4] = theta[1];
  this->odomPose[5] = 0;
  
  this->odomVel[0] = 0;
  this->odomVel[1] = 0;
  this->odomVel[2] = 0;
  this->odomVel[3] = theta_speed[0];
  this->odomVel[4] = theta_speed[1];
  this->odomVel[5] = 0;
 
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get commands from the external interface
void ArmModel::GetPositionCmd()
{
  // Call other functions to make code a little easier to read...
  this->GetXyPositionCmd();
  
  if (mGripper)
  {
    this->GetGripperPositionCmd();
  }
   
  return;
}

    
//////////////////////////////////////////////////////////////////////////////
// Update external interfaces
void ArmModel::PutPositionData()
{
  // This doesn't send out commands, it just updates the data
  gz_position_lock(this->position_iface, 1);
    
  // Data timestamp
  this->position_iface->data->time = this->world->GetSimTime();
  
  // position data from link0 and link1
  // everything is 0, except odomPose[3]/[4] and odomVel[3]/[4]
  this->position_iface->data->pos[0] = this->odomPose[0];
  this->position_iface->data->pos[1] = this->odomPose[1];
  this->position_iface->data->pos[2] = this->odomPose[2];
  this->position_iface->data->rot[0] = this->odomPose[3];
  this->position_iface->data->rot[1] = this->odomPose[4];
  this->position_iface->data->rot[2] = this->odomPose[5];
  
  this->position_iface->data->vel_pos[0] = this->odomVel[0];
  this->position_iface->data->vel_pos[1] = this->odomVel[1];
  this->position_iface->data->vel_pos[2] = this->odomVel[2]; 
  this->position_iface->data->vel_rot[0] = this->odomVel[3];
  this->position_iface->data->vel_rot[1] = this->odomVel[4];
  this->position_iface->data->vel_rot[2] = this->odomVel[5];
  
  gz_position_unlock(this->position_iface);
  
  
  // Gripper status
  gz_gripper_lock(this->gripper_iface, 1);
  
  // Data timestamp
  this->gripper_iface->data->time = this->world->GetSimTime();
  
  this->gripper_iface->data->paddles_opened = this->mGripperOpen;
  this->gripper_iface->data->paddles_closed = !(this->mGripperOpen);
  // Assumption: closing/opening of gripper is "instantaneous"
  this->gripper_iface->data->paddles_moving = false;
  this->gripper_iface->data->lift_up = 
                          (this->mGripperUp && !(this->mGripperMoving));
  this->gripper_iface->data->lift_down = 
                          (!(this->mGripperUp) && !(this->mGripperMoving));
  this->gripper_iface->data->lift_moving = this->mGripperMoving;
  
  gz_gripper_unlock(this->gripper_iface);
  
  return;
}

////////////////////////////////////////////////////////////////////////////////
void ArmModel::GetXyPositionCmd()
{
  double cmd_velocity[6];

  gz_position_lock(this->position_iface, 1);
  // those should always be 0 since commands come only in jointspace
  cmd_velocity[0] = this->position_iface->data->cmd_vel_pos[0];
  cmd_velocity[1] = this->position_iface->data->cmd_vel_pos[1];
  cmd_velocity[2] = 0.0;
 
  // I use those for rotation about first and second joint
  // => jointspace coordinates 
  cmd_velocity[3] = this->position_iface->data->cmd_vel_rot[0];
  cmd_velocity[4] = this->position_iface->data->cmd_vel_rot[1]; 
  cmd_velocity[5] = 0.0; 
  gz_position_unlock(this->position_iface);

/// @todo Should I leave this in here in case anyone wants to use it?      
  if((fabs(cmd_velocity[0]) > 1E-5)||(fabs(cmd_velocity[1]) > 1E-5))
  {
    // This should never be the case the way it is implemented now...
    printf("Commands given in taskspace coordinates! \n");
    double theta[3];
    double link_length[3]; // holds the effective link lengths for kinematics
    theta[0] = linkJoints[0]->GetAngle();
    theta[1] = linkJoints[1]->GetAngle();
    theta[2] = 0.0;
/// @todo: change length such that end point matches gripper...
    /// @note: I approximate the joint to being a point. This might have to be
    ///        changed if the gripper can't be positionned precisely enough
    link_length[0] = mLinkLength[0] - mWeightLength[0]/2 - mWeightLength[1]/2;
    link_length[1] = mLinkLength[1] - mWeightLength[1] + mGripperBaseLength;
    link_length[2] = 0.0;
    
    // Compute jointspeeds to get the desired x and y speeds
    SetTargetVelocity(cmd_velocity, this->mJointSpeed, link_length,
                      theta, this->mLinkLimit);
  }
  else // commands given in jointspace
  {
    this->mJointSpeed[0] = cmd_velocity[3];
    this->mJointSpeed[1] = cmd_velocity[4];
    this->mJointSpeed[2] = cmd_velocity[5];
  }
   
  // Set a maximum speed
  if(mJointSpeed[0] > 0.5) mJointSpeed[0] = 0.5;
  if(mJointSpeed[1] > 0.5) mJointSpeed[1] = 0.5;
  
  // Send the computed speeds to the first 2 joints
  this->linkJoints[0]->SetParam(dParamVel, this->mJointSpeed[0]);
  this->linkJoints[1]->SetParam(dParamVel, this->mJointSpeed[1]);
    
  return;
}   


////////////////////////////////////////////////////////////////////////////////
void ArmModel::GetGripperPositionCmd()
{
  // Receives command from external interface.

  // Commands are defined in playerclient.h in Player's client directory.
  // GRIPopen   1
  // GRIPclose  2
  // GRIPstop   3
  // LIFTup     4
  // LIFTdown   5
  // LIFTstop   6
  // GRIPstore  7 -- not supported
  // GRIPdeploy 8 -- not supported
  // GRIPhalt   15
  // GRIPpress  16 -- not supported
  // LIFTcarry  17 -- not supported 

 
  // Call different function to move gripper, depending on the command we
  // received
  switch (this->gripper_iface->data->cmd)
  {
    case 1 :
    {
      this->mGripperStopped = false;
      this->OpenGrip();
      break;
    } 
    case 2 :
    {
      this->mGripperStopped = false;
      this->CloseGrip();
      break;
    }
    case 3 :
    {
      this->StopGrip();
      break;
    }    
    case 4 :
    {
      this->LiftUp();
      break;
    }   
    case 5 :
    {
      this->LiftDown();
      break;
    }   
    case 6 :
    {
      this->StopLift();
      break;
    }
    case 15 :
    {
      this->StopGrip();
      this->StopLift();
      break;
    }   
    default :
    {
      break;
    }
  }

  return;
}
    
    
////////////////////////////////////////////////////////////////////////////////
void ArmModel::OpenGrip()
{
  // Create temp variables
  double left_grip_pos = this->leftGripperJoint->GetAngle();
  double right_grip_pos = this->rightGripperJoint->GetAngle();
  double tol = 8E-3; // Don't panic because of small errors...
  
  // Check if gripper is already open
  if ((left_grip_pos < (mGripperLimitLow[0] + tol)) || 
      (right_grip_pos < (mGripperLimitLow[1] + tol)))
  {
    this->StopGrip();
    this->mGripperOpen = true;
    this->mGripperMoving = false;
  }
  else // start/keep moving
  {
    this->leftGripperJoint->SetParam(dParamFMax, mGripMaxForce);
    this->rightGripperJoint->SetParam(dParamFMax, mGripMaxForce);
    this->leftGripperJoint->SetParam(dParamVel, -this->mGripMaxSpeed);      
    this->rightGripperJoint->SetParam(dParamVel, -this->mGripMaxSpeed);
    this->mGripperMoving = true;
  }
 
  return;
}
    
    
////////////////////////////////////////////////////////////////////////////////
void ArmModel::CloseGrip()
{
  // Create temp variables
  double left_grip_pos = this->leftGripperJoint->GetAngle();
  double right_grip_pos = this->rightGripperJoint->GetAngle();
  double tol = 1E-2; // Don't panic because of small errors...
  
  if (!mGripperStopped)
  {
    if ((left_grip_pos > (mGripperLimitHigh[0] - tol)) || 
        (right_grip_pos > (mGripperLimitHigh[1] - tol)))
    {
      this->StopGrip();
      this->mGripperOpen = false;
      this->mGripperMoving = false;
    }
    else // start/keep moving
    {
      this->leftGripperJoint->SetParam(dParamFMax, mGripMaxForce);
      this->rightGripperJoint->SetParam(dParamFMax, mGripMaxForce);
      this->leftGripperJoint->SetParam(dParamVel, this->mGripMaxSpeed);      
      this->rightGripperJoint->SetParam(dParamVel, this->mGripMaxSpeed);
      this->mGripperMoving = true;
    }
  }
  
  return;
}        
       
    
////////////////////////////////////////////////////////////////////////////////
void ArmModel::LiftUp()    
{
  // get current lift position
  double lift_pos = this->gripperSliderJoint->GetPosition();
  double tol = 1E-3; // Don't panic because of small errors...
  
  // check if gripper is already up
  if (lift_pos > (mLiftMinPosLimit - tol))
  {
    this->StopGrip();
    this->mGripperUp = true;
    this->mGripperMoving = false;
  }
  else // start/keep moving
  {
    this->gripperSliderJoint->SetParam(dParamVel, this->mLiftMaxSpeed);
    this->mGripperMoving = true;
  }
  
  return;
}    
    
////////////////////////////////////////////////////////////////////////////////
void ArmModel::LiftDown()    
{
  // get current lift position
  double lift_pos = this->gripperSliderJoint->GetPosition();
  double tol = 1E-3; // Don't panic because of small errors...
 
  // check if gripper is already down
  if (lift_pos < (-mLiftMaxPosLimit + tol))
  {
    this->StopGrip();
    this->mGripperUp = false;
    this->mGripperMoving = false;
  }
  else // start/keep moving
  { 
    this->gripperSliderJoint->SetParam(dParamVel, -this->mLiftMaxSpeed);
    mGripperMoving = true;
  }

  return;
}    


//////////////////////////////////////////////////////////////////////////////
// Stop the gripper
void ArmModel::StopGrip()
{
  // Set velocity of gripper fingers to 0
  this->leftGripperJoint->SetParam(dParamVel, 0.);
  this->rightGripperJoint->SetParam(dParamVel, 0.);
  // change gripper mode
  this->mGripperStopped = true;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Stop the lift
void ArmModel::StopLift()
{
  // Update velocity to be in stop state
  this->gripperSliderJoint->SetParam(dParamVel, 0);
  // Change lift mode
  this->mGripperMoving = false;
  
  return;
}
