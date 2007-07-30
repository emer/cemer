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
/* Desc: Kinematics for two-linked robot arm
 * Author: Sarah Gerster
 */

 
/** Kinematic funcitons to be used with the arm driver
  *
  * @author Sarah Gerster
  * @date 16.06.2005
  * @addtogroup iris_drivers Drivers
  * @{ */

/**

@par Jointspace vs taskspace:
- In jointspace, the yaw, pitch and roll of the variables are used to store the
theta of motor0, motor1 and motor2. There are comments in the code at the
places where there could be some confusion.
- In taskspace, we are only able to rotate around the z-axis. Therefore, the
pitch and roll are always equal to 0. The value stored in yaw is the total
twist of the arm-tip.
 
@todo
- Test if all kinematic singularities are avoided with the current conditions
*/

/** @} */

#include "kinematics.h"

////////////////////////////////////////////////////////////////////////////////
/// This file implements the basic kinematics for the 2-linked robot arm
////////////////////////////////////////////////////////////////////////////////

int32_t GetLinkCoordinates(double argPose[3], double argLink[6])
{
  // Set x, y, z to 0 since they are of no interest in this case
  argLink[0] = 0.;
  argLink[1] = 0.;
  argLink[2] = 0.;
  
  // save the given angles given as roll, pitch and yaw
  argLink[3] = argPose[0];
  argLink[4] = argPose[1];
  argLink[5] = argPose[2];

  return 0;
}


////////////////////////////////////////////////////////////////////////////////

int32_t GetLinkVelocities(double argPoseVelocity[3], 
                          double argLinkVelocity[6])
{
  // set x, y and z speed to 0, since they are of no interest in this case
  argLinkVelocity[0] = 0.;
  argLinkVelocity[1] = 0.;
  argLinkVelocity[2] = 0.;
  
  // save the given jointspeeds as rollspeed, pitchspeed and yawspeed
  argLinkVelocity[3] = argPoseVelocity[0];
  argLinkVelocity[4] = argPoseVelocity[1];
  argLinkVelocity[5] = argPoseVelocity[2];
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////

void SetTargetPosition(double argPose[6], double argLink[3], 
                       double linkLength[3], double positionPose[3])
{
  // Alias to make code shorter and easier to read
  double *theta   = &(argLink[0]);
  double &x       = argPose[0];
  double &y       = argPose[1];
  double eps      = 1E-5; // to avoid a statement like x == l1+l2
  double delta1   = 5E-3;
  double delta2   = 5E-3;
  
  double cos_theta1, sin_theta1;
  double k0,k1; // help variables
  double l0 = linkLength[0];
  double l1 = linkLength[1];
  
  /* if the arm is completly streched out, there is a kinematic singularity and
   * the angle cannot be computed. Since this is the default position to start
   * the robot in, I check for this position before computing anything else.
   */
  if(((x > l0+l1-eps)||(x < l0+l1+eps)) && (y==0))
  {
    theta[0] = 0.0;
    theta[1] = 0.0;
  }
  // else apply kinematics
  else
  {
    cos_theta1 = (x*x + y*y - l0*l0 - l1*l1)/(2*l0*l1);
    // Check if the position can be reached
    if (fabs(cos_theta1) <= 1) 
    {
      // There exists a solution
      sin_theta1 = sqrt(1-cos_theta1*cos_theta1);
    
      theta[1] = atan2(sin_theta1, cos_theta1);

      k0 = l0 + l1*cos_theta1;
      k1 = l1*sin_theta1;

      theta[0] = -(atan2(k1,k0) - atan2(y,x));
    }
    else
    {
      // Posiiton cannot be reached
      printf("No solution can be found!\n");
      // Just leave arm where it is...
      theta[0] = positionPose[0];
      theta[1] = positionPose[1];
    }
  } 
  
  /* Check for kinematic singularities: arm should not be sent to a position
   * from where it can't move away anymore.
   */
  if(0 == theta[1])
  {
    // Don't set theta1 exactly to 0 to avoid kinematic singularity
    /*note: The value should be larger than the tolerance for the PID
     *      potision control that is set for the simulation!
     */
    theta[1] = 0.01; 
    printf("Avoided kinematic singularity!\n"); 
  } 

  // Third motor is controlled directly by gripper...
  theta[2] = 0;
  
  return;
} 


////////////////////////////////////////////////////////////////////////////////

void SetTargetVelocity(double argVelocity[6], 
                       double argLinkVelocity[3], double linkLength[3], 
                       double linkPose[3], double jointLimit[3])
{
  // Alias to make code shorter and easier to read
  double *thetaspeed = &(argLinkVelocity[0]);
  double &x          = argVelocity[0];
  double &y          = argVelocity[1];

  // Help variables to make code shorter
  double l0,l1;
  double theta0, theta1;
  double cos_01, sin_01, cos_0, sin_0, sin_1;
  double det_j;

  l0 = linkLength[0];
  l1 = linkLength[1];
  theta0 = linkPose[0];
  theta1 = linkPose[1];

  cos_01 = cos(theta0 + theta1);
  sin_01 = sin(theta0 + theta1);
  cos_0  = cos(theta0);
  sin_0  = sin(theta0);
  sin_1  = sin(theta1);
  
  // determinant of the jacobian
  det_j = l0*l1*sin_1;
  
  // Check if inverse kinematics can be applied
  if (0 != fabs(det_j))
  { 
    // Check for kinematic singularities
    if(1e-4 > fabs(det_j))
    {
      // We're closed to a kinematic singularity => don't go there!
      // Set velocities to 0
      thetaspeed[0] = 0.;
      thetaspeed[1] = 0.;
      printf("Avoided kinematic singularity!\n");
    }
    else
    {  
      // Check to make sure everything is physically possible
      if ((fabs(theta0)<jointLimit[0]) &&
          (fabs(theta1)<jointLimit[1]))
      {
        // We can find a solution
        thetaspeed[0] = l1*cos_01*x + l1*sin_01*y;
        thetaspeed[1] = -l0*cos_0*x-l1*cos_01*x-l0*sin_0*y-l1*sin_01*y;

        // Don't forget to divide by the det_j
        thetaspeed[0] /= det_j;
        thetaspeed[1] /= det_j;
      }
      else
      {
        // Joints can't be turned that far... 
        // Just set velocity to 0
        printf("Cannot turn joint that far! Set velocities to 0...\n");
        thetaspeed[0] = 0.;
        thetaspeed[1] = 0.;     
      }
    }
  }
  else
  {  
    // We cannot find a solution
    // Just set the velocities to 0....
    printf("Could not find a solution! Set velocities to 0...\n");
    thetaspeed[0] = 0.;
    thetaspeed[1] = 0.;
  }
  
  // Third motor is controlled directly by gripper... nothing is done here.  
  thetaspeed[2] = 0.;
  
  return;
} 


////////////////////////////////////////////////////////////////////////////////

int32_t GetTaskCoordinates(double argPose[3], double argTask[6],
                           double linkLength[3])
{
  // Alias to make code shorter and easier to read
  double &x         = argTask[0];
  double &y         = argTask[1];
  double &z         = argTask[2];
  double &roll      = argTask[3];
  double &pitch     = argTask[4];
  double &theta_tip = argTask[5];  // yaw
    
  // Help variables ot make code shorter
  double l0,l1,theta0,theta1;

  l0 = linkLength[0];
  l1 = linkLength[1];
  theta0 = argPose[0];
  theta1 = argPose[1];

  // compute x,y and thetatip
  x = l0*cos(theta0) + l1*cos(theta0+theta1);
  y = l0*sin(theta0) + l1*sin(theta0+theta1);
  theta_tip = theta0 + theta1;
  
  // set other values to 0
  z = 0;
  roll = pitch = 0;
    
  return 0;
}


////////////////////////////////////////////////////////////////////////////////

int32_t GetTaskVelocities(double argPoseVelocity[3], double argTaskVelocity[6], 
                          double linkLength[3], double positionPose[3])
{
  // Alias to make code shorter and easier to read
  double &xspeed         = argTaskVelocity[0];
  double &yspeed         = argTaskVelocity[1];
  double &zspeed         = argTaskVelocity[2];
  double &rollspeed      = argTaskVelocity[3];
  double &pitchspeed     = argTaskVelocity[4];
  double &theta_tipspeed = argTaskVelocity[5];  // yawspeed
  
  // Help varaibles to make code easier to read
  double l0,l1;
  double theta0,theta1,theta0_speed,theta1_speed;

  l0 = linkLength[0];
  l1 = linkLength[1];
  theta0 = positionPose[0];
  theta1 = positionPose[1];
  
  theta0_speed = argPoseVelocity[0];
  theta1_speed = argPoseVelocity[1];

  // Compute xspeed, yspeed and thetaspeed
  xspeed = -l0 * sin(theta0) * theta0_speed - l1 * sin(theta0+theta1) *
            (theta0_speed + theta1_speed);
  yspeed =  l0 * cos(theta0) * theta0_speed +
            l1 * cos(theta0+theta1) * (theta0_speed + theta1_speed);
  theta_tipspeed = theta0_speed + theta1_speed;

  // Set other speeds to 0
  zspeed = 0;
  rollspeed = 0;
  pitchspeed = 0;
  
 
  return 0;
} 



