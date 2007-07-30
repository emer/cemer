/*
 *  Kinematics for two-linked robot arm
 *  Copyright (C) 2005
 *     Sarah Gerster
 *
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


#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/*! \file kinematics.h implements the basic kinematics used to transform
 * data and commands from taskspace to jointspace coordinates or vice versa.
 * Thoses functions are to be used with two-linked robot arm. The jointspace
 * data is always passed as an array of 3, because the arm has 3 motors. I'm
 * only using the first two motors for the positioning here. The third motor is
 * directly controlled through gripper commands which wants it to move up or
 * down a fixed height. This can be implemented through a variable parameter in
 * the configuration file and there is no need for a function to be convert
 * anything from taskspace coordinates to jointspace coordinates. The
 * taskcoordinates are always saved in arrays of 6. Even if I don't need roll
 * and pitch here, it is nice to have it this way, such that it can easily be
 * adapted if needed.
*/

/*! \note When working in jointspace, I also use an array of 6 to save the
 * angles. Then roll = angle of first motor, pitch = angle of second motor. The
 * other fields of the array are not used.
 */

////////////////////////////////////////////////////////////////////////////////

/*! \fn int32_t GetLinkCoordinates(double argPose[3], double argLink[6])
    \brief returns link position of motor0 as roll, of motor1 as pitch and
           of motor 2 as yaw. It sets x,y,z to 0.

    \param argPose The motor angles.
    \param argLink Array where joint angles are saved in spaces [3],[4] and [5].
*/                       
int32_t GetLinkCoordinates(double argPose[3], double argLink[6]);


////////////////////////////////////////////////////////////////////////////////

/*! \fn int32_t GetLinkVelocities(double argPose[3], double argLink[6])
    \brief returns link velocity of link0 as roll velocity, of link1 as
           pitch velocity and of link2 as yaw velocity. It sets x,y,z
           velocities to 0.

    \param argPose The motor velocities.
    \param argLink Array where jointspeeds are saved in spaces [3],[4] and [5].
*/ 
int32_t GetLinkVelocities(double argPose[3], double argLink[6]);


////////////////////////////////////////////////////////////////////////////////

/*! \fn void SetTargetPosition(double argPose[6], double argLink[3], 
        double linkLength[3], double positionPose[3])
    \brief computes the jointspace coordinates to put the end-effector at the
           desired (x,y)-position. Only angles for the first two motors are set 
           (xy-plane positioning)

    \param argPose Postion of end effector. Only the x and y components are used
    \param argLink Array where motor positions to be set are saved
    \param linkLength Array with link lengths
    \param positionPose Current position of motors. Used to tell arm to stay
           where it is, if it is asked to go to a place which it can't reach.
*/
void SetTargetPosition(double argPose[6], double argLink[3], 
                       double linkLength[3], double positionPose[3]);

                       
////////////////////////////////////////////////////////////////////////////////

/*! \fn void SetTargetVelocity(double argVelocity[6], double argLinkVelocity[3],
        double linkLength[3], double linkPose[3], double jointLimit[3])
    \brief computes the jointspace velocites to reach the desired tip velocity
           given in taskspace coordinates. Only the x and y components are
           used (end effector velocity in xy-plane).

    \param argVelocity desired velocity of end effector
    \param argLinkVelocity Array where motor speeds to be set are saved
    \param linkLength Array with link lengths
    \param linkPose Current position of motors
    \param jointLimit how far angles can turn       
*/
void SetTargetVelocity(double argVelocity[6], double argLinkVelocity[3],
                       double linkLength[3], double linkPose[3], 
                       double jointLimit[3]);

                       
////////////////////////////////////////////////////////////////////////////////
    
/*! \fn int32_t GetTaskCoordinates(double argPose[3], double argTask[6],
                                   double linkLength[3])
    \brief computes the task coordinates for given joint angles. x, y and angle
           of tip (saved as yaw) are computed. z, roll and pitch are set to 0.

    \param argPose Given joint angles. Only angles of joint0 and joint1 are used
    \param argTask Postion of end effector
    \param linkLength Array with link lengths
*/
int32_t GetTaskCoordinates(double argPose[3], double argTask[6],
                           double linkLength[3]);

                           
////////////////////////////////////////////////////////////////////////////////

/*! \fn int32_t GetTaskVelocities(double argPoseVelocity[3], 
        double argTaskVelocity[6], double linkLength[3], double positionPose[3])
    \brief computes the taskspace velocities for given joint speeds. xspeed,
           yspeed and anglespeed of tip (saved as yawspeed) are computed.
           zspeed, rollspeed and pitchspeed are set to 0.

    \param argPoseVelocity Given jointspeeds. Only speeds of joint0 and joint1
                           are used
    \param argTask Speed of end effector
    \param linkLength Array with link lengths
    \param positonPose current angles of motors
*/                           
int32_t GetTaskVelocities(double argPoseVelocity[3], 
                          double argTaskVelocity[6],
                          double linkLength[3],
                          double positionPose[3]);
         

////////////////////////////////////////////////////////////////////////////////
                                                                  
#endif

