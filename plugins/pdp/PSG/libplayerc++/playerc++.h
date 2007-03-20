/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2000-2003
 *     Brian Gerkey, Kasper Stoy, Richard Vaughan, & Andrew Howard
 *
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

/***************************************************************************
 * Desc: Player v2.0 C++ client
 * Authors: Brad Kratochvil, Toby Collett
 *
 * Date: 23 Sep 2005
 # CVS: $Id: playerc++.h,v 1.54.2.6 2006/09/22 23:58:34 gerkey Exp $
 **************************************************************************/


#ifndef PLAYERCC_H
#define PLAYERCC_H

#include <cmath>
#include <string>
#include <list>

#include "playerc.h"
#include "utility.h"
#include "playerc++config.h"
#include "playerclient.h"
#include "playererror.h"
#include "clientproxy.h"

namespace PlayerCc
{

// /**
// * The @p SomethingProxy class is a template for adding new subclasses of
// * ClientProxy.  You need to have at least all of the following:
// */
// class SomethingProxy : public ClientProxy
// {
//
//   private:
//
//     // Subscribe
//     void Subscribe(uint aIndex);
//     // Unsubscribe
//     void Unsubscribe();
//
//     // libplayerc data structure
//     playerc_something_t *mDevice;
//
//   public:
//     // Constructor
//     SomethingProxy(PlayerClient *aPc, uint aIndex=0);
//     // Destructor
//     ~SomethingProxy();
//
// };

/** @ingroup player_clientlib_cplusplus
 * @addtogroup player_clientlib_cplusplus_proxies Proxies
 * @brief A proxy class is associated with each kind of device

  The proxies all inherit from @p ClientProxy and implement the functions
  from @ref player_clientlib_libplayerc.

 @{

 */

// ==============================================================
//
// These are alphabetized, please keep them that way!!!
//
// ==============================================================

/**
The @p CameraProxy class can be used to get images from a @ref
interface_camera device. */
class CameraProxy : public ClientProxy
{

  private:

    virtual void Subscribe(uint aIndex);
    virtual void Unsubscribe();

    // libplayerc data structure
    playerc_camera_t *mDevice;

    std::string mPrefix;
    int mFrameNo;

  public:

    /// Constructor
    CameraProxy (PlayerClient *aPc, uint aIndex=0);

    virtual ~CameraProxy();

    /// Save the frame
    /// @arg aPrefix is the string prefix to name the image.
    /// @arg aWidth is the number of 0s to pad the image numbering with.
    void SaveFrame(const std::string aPrefix, uint aWidth=4);

    /// decompress the image
    void Decompress();

    /// Image color depth
    uint GetDepth() const { return GetVar(mDevice->bpp); };

    /// Image dimensions (pixels)
    uint GetWidth() const { return GetVar(mDevice->width); };

    /// Image dimensions (pixels)
    uint GetHeight() const { return GetVar(mDevice->height); };

    /// @brief Image format
    /// Possible values include
    /// - @ref PLAYER_CAMERA_FORMAT_MONO8
    /// - @ref PLAYER_CAMERA_FORMAT_MONO16
    /// - @ref PLAYER_CAMERA_FORMAT_RGB565
    /// - @ref PLAYER_CAMERA_FORMAT_RGB888
    uint GetFormat() const { return GetVar(mDevice->format); };

    /// Size of the image (bytes)
    uint GetImageSize() const { return GetVar(mDevice->image_count); };

    /// @brief Image data
    /// This function copies the image data into the data buffer aImage.
    /// The buffer should be allocated according to the width, height, and
    /// depth of the image.  The size can be found by calling @ref GetImageSize().
    void GetImage(uint8_t* aImage) const
      {
        return GetVarByRef(mDevice->image,
                           mDevice->image+GetVar(mDevice->image_count),
                           aImage);
      };

    /// @brief What is the compression type?
    /// Currently supported compression types are:
    /// - @ref PLAYER_CAMERA_COMPRESS_RAW
    /// - @ref PLAYER_CAMERA_COMPRESS_JPEG
    uint GetCompression() const { return GetVar(mDevice->compression); };

};
/**
 * The @p Graphics2dProxy class is used to draw simple graphics into a
 * rendering device provided by Player using the graphics2d
 * interface. For example, the Stage plugin implements this interface
 * so you can draw into the Stage window. This is very useful to
 * visualize what's going on in your controller.
 */
class Graphics2dProxy : public ClientProxy
{

  private:

    // Subscribe
    void Subscribe(uint aIndex);
    // Unsubscribe
    void Unsubscribe();

    // libplayerc data structure
    playerc_graphics2d_t *mDevice;

  public:
    // Constructor
    Graphics2dProxy(PlayerClient *aPc, uint aIndex=0);
    // Destructor
    ~Graphics2dProxy();

    /// Set the current pen color
    void Color(player_color_t col);

    /// Set the current pen color
    void Color(uint8_t red,  uint8_t green,  uint8_t blue,  uint8_t alpha);

    /// Clear the drawing area
    void Clear(void);

    /// Draw a set of points
    void DrawPoints(player_point_2d_t pts[], int count);

    /// Draw a polygon defined by a set of points
    void DrawPolygon(player_point_2d_t pts[],
                     int count,
                     bool filled,
                     player_color_t fill_color);

    /// Draw a line connecting  set of points
    void DrawPolyline(player_point_2d_t pts[], int count);
};

/**
 * The @p Graphics3dProxy class is used to draw simple graphics into a
 * rendering device provided by Player using the graphics3d
 * interface.
 */
class Graphics3dProxy : public ClientProxy
{

  private:

    // Subscribe
    void Subscribe(uint aIndex);
    // Unsubscribe
    void Unsubscribe();

    // libplayerc data structure
    playerc_graphics3d_t *mDevice;

  public:
    // Constructor
    Graphics3dProxy(PlayerClient *aPc, uint aIndex=0);
    // Destructor
    ~Graphics3dProxy();

    /// Set the current pen color
    void Color(player_color_t col);

    /// Set the current pen color
    void Color(uint8_t red,  uint8_t green,  uint8_t blue,  uint8_t alpha);

    /// Clear the drawing area
    void Clear(void);

    /// Draw a set of verticies
    void Draw(player_graphics3d_draw_mode_t mode, player_point_3d_t pts[], int count);

};

/**
The @p GripperProxy class is used to control a @ref
interface_gripper device.  The latest gripper data held in a
handful of class attributes.  A single method provides user control.
*/
class GripperProxy : public ClientProxy
{

  private:

    void Subscribe(uint aIndex);
    void Unsubscribe();

    // libplayerc data structure
    playerc_gripper_t *mDevice;

  public:

    /// constructor
    GripperProxy(PlayerClient *aPc, uint aIndex=0);
    /// destructor
    ~GripperProxy();

    ///
    uint GetState() const { return GetVar(mDevice->state); };
    ///
    uint GetBeams() const { return GetVar(mDevice->beams); };
    ///
    uint GetOuterBreakBeam() const
      { return GetVar(mDevice->outer_break_beam); };
    ///
    uint GetInnerBreakBeam() const
      { return GetVar(mDevice->inner_break_beam); };
    ///
    uint GetPaddlesOpen() const
      { return GetVar(mDevice->paddles_open); };
    ///
    uint GetPaddlesClosed() const
      { return GetVar(mDevice->paddles_closed); };
    ///
    uint GetPaddlesMoving() const
      { return GetVar(mDevice->paddles_moving); };
    ///
    uint GetGripperError() const
      { return GetVar(mDevice->gripper_error); };
    ///
    uint GetLiftUp() const { return GetVar(mDevice->lift_up); };
    ///
    uint GetLiftDown() const { return GetVar(mDevice->lift_down); };
    ///
    uint GetLiftMoving() const { return GetVar(mDevice->lift_moving); };
    ///
    uint GetLiftError() const { return GetVar(mDevice->lift_error); };

    /// Send a gripper command.  Look in the Player user manual for details
    /// on the command and argument.
    void SetGrip(uint8_t aCmd, uint8_t aArg=0);
};
/**
The @p LaserProxy class is used to control a @ref interface_laser
device.  The latest scan data is held in two arrays: @p ranges and @p
intensity.  The laser scan range, resolution and so on can be configured
using the Configure() method.  */
class LaserProxy : public ClientProxy
{
  private:

    void Subscribe(uint aIndex);
    void Unsubscribe();

    // libplayerc data structure
    playerc_laser_t *mDevice;

    double aMinLeft;
    double aMinRight;

    // local storage of config
    double min_angle, max_angle, scan_res, range_res;
    bool intensity;

  public:

    /// constructor
    LaserProxy(PlayerClient *aPc, uint aIndex=0);
    /// destructor
    ~LaserProxy();

    /// Number of points in scan
    uint GetCount() const { return GetVar(mDevice->scan_count); };

    /// Angular resolution of scan (radians)
    double GetScanRes() const { return GetVar(mDevice->scan_res); };

    /// Range resolution of scan (mm)
    double GetRangeRes() const { return GetVar(mDevice->range_res); };


    /// Scan range for the latest set of data (radians)
    double GetMinAngle() const { return GetVar(mDevice->scan_start); };
    /// Scan range for the latest set of data (radians)
    double GetMaxAngle() const
    {
      scoped_lock_t lock(mPc->mMutex);
      return mDevice->scan_start + mDevice->scan_count*mDevice->scan_res;
    };

//    /// Whether or not reflectance (i.e., intensity) values are being returned.
//    bool IsIntensity() const { return GetVar(mDevice->intensity); };

//    /// Scan data (polar): range (m) and bearing (radians)
//    double GetScan(uint aIndex) const
//      { return GetVar(mDevice->scan[aIndex]); };

    /// Scan data (Cartesian): x,y (m)
    player_point_2d_t GetPoint(uint aIndex) const
      { return GetVar(mDevice->point[aIndex]); };


    /// get the range
    double GetRange(uint aIndex) const
      { return GetVar(mDevice->ranges[aIndex]); };

    /// get the bearing
    double GetBearing(uint aIndex) const
      { return GetVar(mDevice->scan[aIndex][1]); };


    /// get the intensity
    int GetIntensity(uint aIndex) const
      { return GetVar(mDevice->intensity[aIndex]); };

    /// Configure the laser scan pattern.  Angles @p min_angle and
    /// @p max_angle are measured in radians.
    /// @p scan_res is measured in units of 0.01 degrees;
    /// valid values are: 25 (0.25 deg), 50 (0.5 deg) and
    /// 100 (1 deg).  @p range_res is measured in mm; valid values
    /// are: 1, 10, 100.  Set @p intensity to @p true to
    /// enable intensity measurements, or @p false to disable.
    void Configure(double aMinAngle,
                   double aMaxAngle,
                   uint aScanRes,
                   uint aRangeRes,
                   bool aIntensity);

    /// Get the current laser configuration; it is read into the
    /// relevant class attributes.
    void RequestConfigure();

    /// Get the laser's geometry; it is read into the
    /// relevant class attributes.
    void RequestGeom();

    /// Accessor for the pose (fill it in by calling RequestGeom)
    player_pose_t GetPose()
    {
      player_pose_t p;
      scoped_lock_t lock(mPc->mMutex);

      p.px = mDevice->pose[0];
      p.py = mDevice->pose[1];
      p.pa = mDevice->pose[2];
      return(p);
    }

    /// Accessor for the size (fill it in by calling RequestGeom)
    player_bbox_t GetSize()
    {
      player_bbox_t b;
      scoped_lock_t lock(mPc->mMutex);

      b.sl = mDevice->size[0];
      b.sw = mDevice->size[1];
      return(b);
    }

    /// Min left
    double MinLeft () { return aMinLeft; }
    /// Min right
    double MinRight () { return aMinRight; }

    /// Range access operator.  This operator provides an alternate
    /// way of access the range data.  For example, given an @p
    /// LaserProxy named @p lp, the following expressions are
    /// equivalent: @p lp.ranges[0], @p lp.Ranges(0),
    /// and @p lp[0].
    double operator [] (uint index) const
      { return GetRange(index);}

};


/**
The @p LimbProxy class is used to control a @ref interface_limb
device.
 */
class LimbProxy : public ClientProxy
{
  private:

    void Subscribe(uint aIndex);
    void Unsubscribe();

   // libplayerc data structure
    playerc_limb_t *mDevice;

  public:

    LimbProxy(PlayerClient *aPc, uint aIndex=0);
    ~LimbProxy();

    /// Geometry request - call before getting the
    /// geometry of a joint through the accessor method
    void RequestGeometry(void);

    /// Power control
    void SetPowerConfig(bool aVal);
    /// Brakes control
    void SetBrakesConfig(bool aVal);
    /// Speed control
    void SetSpeedConfig(float aSpeed);

    /// Move the limb to the home position
    void MoveHome(void);
    /// Stop the limb immediately
    void Stop(void);
    /// Move the end effector to a given pose
    void SetPose(float aPX, float aPY, float aPZ,
                 float aAX, float aAY, float aAZ,
                 float aOX, float aOY, float aOZ);
    /// Move the end effector to a given position, ignoring orientation
    void SetPosition(float aX, float aY, float aZ);
    /// Move the end effector along a vector of given length,
    /// maintaining current orientation
    void VectorMove(float aX, float aY, float aZ, float aLength);

    /// Accessor method for getting the limb's data
    player_limb_data_t GetData(void) const;
    /// Same again for getting the limb's geometry
    player_limb_geom_req_t GetGeom(void) const;
};
/**
The @p Position2dProxy class is used to control a @ref
interface_position2d device.  The latest position data is contained
in the attributes xpos, ypos, etc.  */
class Position2dProxy : public ClientProxy
{

  private:

    void Subscribe(uint aIndex);
    void Unsubscribe();

    // libplayerc data structure
    playerc_position2d_t *mDevice;

  public:

    /// constructor
    Position2dProxy(PlayerClient *aPc, uint aIndex=0);
    /// destructor
    ~Position2dProxy();

    /// Send a motor command for velocity control mode.
    /// Specify the forward, sideways, and angular speeds in m/sec, m/sec,
    /// and radians/sec, respectively.
    void SetSpeed(double aXSpeed, double aYSpeed, double aYawSpeed);

    /// Same as the previous SetSpeed(), but doesn't take the yspeed speed
    /// (so use this one for non-holonomic robots).
    void SetSpeed(double aXSpeed, double aYawSpeed)
        { return SetSpeed(aXSpeed, 0, aYawSpeed);}

    /// Overloaded SetSpeed that takes player_pose_t as an argument
    void SetSpeed(player_pose_t vel)
        { return SetSpeed(vel.px, vel.py, vel.pa);}

    /// Send a motor command for position control mode.  Specify the
    /// desired pose of the robot as a player_pose_t.
    /// desired motion speed  as a player_pose_t.
    void GoTo(player_pose_t pos, player_pose_t vel);

    /// Same as the previous GoTo(), but doesn't take speed
    void GoTo(player_pose_t pos)
      {GoTo(pos,(player_pose_t) {0,0,0}); }

    /// Same as the previous GoTo(), but only takes position arguments,
    /// no motion speed setting
    void GoTo(double aX, double aY, double aYaw)
      {GoTo((player_pose_t) {aX,aY,aYaw},(player_pose_t) {0,0,0}); }

    /// Sets command for carlike robot
    void SetCarlike(double aXSpeed, double aDriveAngle);

    /// Get the device's geometry; it is read into the
    /// relevant class attributes.
    void RequestGeom();

    /// Accessor for the pose (fill it in by calling RequestGeom)
    player_pose_t GetPose()
    {
      player_pose_t p;
      scoped_lock_t lock(mPc->mMutex);
      p.px = mDevice->pose[0];
      p.py = mDevice->pose[1];
      p.pa = mDevice->pose[2];
      return(p);
    }

    /// Accessor for the size (fill it in by calling RequestGeom)
    player_bbox_t GetSize()
    {
      player_bbox_t b;
      scoped_lock_t lock(mPc->mMutex);
      b.sl = mDevice->size[0];
      b.sw = mDevice->size[1];
      return(b);
    }

    /// Enable/disable the motors.
    /// Set @p state to 0 to disable or 1 to enable.
    /// Be VERY careful with this method!  Your robot is likely to run across the
    /// room with the charger still attached.
    void SetMotorEnable(bool enable);

    // Select velocity control mode.
    //
    // For the the p2os_position driver, set @p mode to 0 for direct wheel
    // velocity control (default), or 1 for separate translational and
    // rotational control.
    //
    // For the reb_position driver: 0 is direct velocity control, 1 is for
    // velocity-based heading PD controller (uses DoDesiredHeading()).
    //void SelectVelocityControl(unsigned char mode);

    /// Reset odometry to (0,0,0).
    void ResetOdometry();

    /// Select position mode
    /// Set @p mode for 0 for velocity mode, 1 for position mode.
    //void SelectPositionMode(unsigned char mode);

    /// Sets the odometry to the pose @p (x, y, yaw).
    /// Note that @p x and @p y are in m and @p yaw is in radians.
    void SetOdometry(double aX, double aY, double aYaw);

    /// Set PID terms
    //void SetSpeedPID(double kp, double ki, double kd);

    /// Set PID terms
    //void SetPositionPID(double kp, double ki, double kd);

    /// Set speed ramping profile
    /// spd rad/s, acc rad/s/s
    //void SetPositionSpeedProfile(double spd, double acc);

    //
    // void DoStraightLine(double m);

    //
    //void DoRotation(double yawspeed);

    //
    //void DoDesiredHeading(double yaw, double xspeed, double yawspeed);

    //
    //void SetStatus(uint8_t cmd, uint16_t value);

    //
    //void PlatformShutdown();

    /// Accessor method
    double  GetXPos() const { return GetVar(mDevice->px); };

    /// Accessor method
    double  GetYPos() const { return GetVar(mDevice->py); };

    /// Accessor method
    double GetYaw() const { return GetVar(mDevice->pa); };

    /// Accessor method
    double  GetXSpeed() const { return GetVar(mDevice->vx); };

    /// Accessor method
    double  GetYSpeed() const { return GetVar(mDevice->vy); };

    /// Accessor method
    double  GetYawSpeed() const { return GetVar(mDevice->va); };

    /// Accessor method
    bool GetStall() const { return GetVar(mDevice->stall); };

};

/**

The @p Position3dProxy class is used to control
a interface_position3d device.  The latest position data is
contained in the attributes xpos, ypos, etc.
*/
class Position3dProxy : public ClientProxy
{

  private:

    void Subscribe(uint aIndex);
    void Unsubscribe();

    // libplayerc data structure
    playerc_position3d_t *mDevice;

  public:

    /// constructor
    Position3dProxy(PlayerClient *aPc, uint aIndex=0);
    /// destructor
    ~Position3dProxy();

    /// Send a motor command for a planar robot.
    /// Specify the forward, sideways, and angular speeds in m/s, m/s, m/s,
    /// rad/s, rad/s, and rad/s, respectively.
    void SetSpeed(double aXSpeed, double aYSpeed, double aZSpeed,
                  double aRollSpeed, double aPitchSpeed, double aYawSpeed);

    /// Send a motor command for a planar robot.
    /// Specify the forward, sideways, and angular speeds in m/s, m/s,
    /// and rad/s, respectively.
    void SetSpeed(double aXSpeed, double aYSpeed,
                  double aZSpeed, double aYawSpeed)
      { SetSpeed(aXSpeed,aYSpeed,aZSpeed,0,0,aYawSpeed); }

    /// simplified version of SetSpeed
    void SetSpeed(double aXSpeed, double aYSpeed, double aYawSpeed)
      { SetSpeed(aXSpeed, aYSpeed, 0, 0, 0, aYawSpeed); }

    /// Same as the previous SetSpeed(), but doesn't take the sideways speed
    /// (so use this one for non-holonomic robots).
    void SetSpeed(double aXSpeed, double aYawSpeed)
      { SetSpeed(aXSpeed,0,0,0,0,aYawSpeed);}

    /// Overloaded SetSpeed that takes player_pose3d_t as input
    void SetSpeed(player_pose3d_t vel)
      { SetSpeed(vel.px,vel.py,vel.pz,vel.proll,vel.ppitch,vel.pyaw);}


    /// Send a motor command for position control mode.  Specify the
    /// desired pose of the robot as a player_pose3d_t structure
    /// desired motion speed as a player_pose3d_t structure
    void GoTo(player_pose3d_t aPos, player_pose3d_t aVel);

    /// Same as the previous GoTo(), but does'n take vel argument
    void GoTo(player_pose3d_t aPos)
      { GoTo(aPos, (player_pose3d_t) {0,0,0,0,0,0}); }


    /// Same as the previous GoTo(), but only takes position arguments,
    /// no motion speed setting
    void GoTo(double aX, double aY, double aZ,
              double aRoll, double aPitch, double aYaw)
      { GoTo((player_pose3d_t) {aX,aY,aZ,aRoll,aPitch,aYaw},
              (player_pose3d_t) {0,0,0,0,0,0});
      }

    /// Enable/disable the motors.
    /// Set @p state to 0 to disable or 1 to enable.
    /// @attention Be VERY careful with this method!  Your robot is likely
    /// to run across the room with the charger still attached.
    void SetMotorEnable(bool aEnable);

    /// Select velocity control mode.
    /// This is driver dependent.
    void SelectVelocityControl(int aMode);

    /// Reset odometry to (0,0,0,0,0,0).
    void ResetOdometry();

    /// Sets the odometry to the pose @p (x, y, z, roll, pitch, yaw).
    /// Note that @p x, @p y, and @p z are in m and @p roll,
    /// @p pitch, and @p yaw are in radians.
    void SetOdometry(double aX, double aY, double aZ,
                     double aRoll, double aPitch, double aYaw);

    // Select position mode
    // Set @p mode for 0 for velocity mode, 1 for position mode.
    //void SelectPositionMode(unsigned char mode);

    //
    //void SetSpeedPID(double kp, double ki, double kd);

    //
    //void SetPositionPID(double kp, double ki, double kd);

    // Sets the ramp profile for position based control
    // spd rad/s, acc rad/s/s
    //void SetPositionSpeedProfile(double spd, double acc);

    /// Accessor method
    double  GetXPos() const { return GetVar(mDevice->pos_x); };

    /// Accessor method
    double  GetYPos() const { return GetVar(mDevice->pos_y); };

    /// Accessor method
    double  GetZPos() const { return GetVar(mDevice->pos_z); };

    /// Accessor method
    double  GetRoll() const { return GetVar(mDevice->pos_roll); };

    /// Accessor method
    double  GetPitch() const { return GetVar(mDevice->pos_pitch); };

    /// Accessor method
    double  GetYaw() const { return GetVar(mDevice->pos_yaw); };

    /// Accessor method
    double  GetXSpeed() const { return GetVar(mDevice->vel_x); };

    /// Accessor method
    double  GetYSpeed() const { return GetVar(mDevice->vel_y); };

    /// Accessor method
    double  GetZSpeed() const { return GetVar(mDevice->vel_z); };

    /// Accessor method
    double  GetRollSpeed() const { return GetVar(mDevice->vel_roll); };

    /// Accessor method
    double  GetPitchSpeed() const { return GetVar(mDevice->vel_pitch); };

    /// Accessor method
    double  GetYawSpeed() const { return GetVar(mDevice->vel_yaw); };

    /// Accessor method
    bool GetStall () const { return GetVar(mDevice->stall); };
};

/**
The @p SimulationProxy proxy provides access to a
@ref interface_simulation device.
*/
class SimulationProxy : public ClientProxy
{
  private:

    void Subscribe(uint aIndex);
    void Unsubscribe();

    // libplayerc data structure
    playerc_simulation_t *mDevice;

  public:
    /// constructor
    SimulationProxy(PlayerClient *aPc, uint aIndex=0);
    /// destructor
    ~SimulationProxy();

    /// set the 2D pose of an object in the simulator, identified by the
    /// std::string. Returns 0 on success, else a non-zero error code.
    void SetPose2d(char* identifier, double x, double y, double a);

    /// get the pose of an object in the simulator, identified by the
    /// std::string Returns 0 on success, else a non-zero error code.
    void GetPose2d(char* identifier, double& x, double& y, double& a);
};

/** @} (proxies)*/
}

/**
The @p SonarProxy class is used to control a @ref interface_sonar
device.  The most recent sonar range measuremts can be read from the
range attribute, or using the the [] operator.
*/
/* class SonarProxy : public ClientProxy */
/* { */
/*   private: */

/*     void Subscribe(uint aIndex); */
/*     void Unsubscribe(); */

/*     // libplayerc data structure */
/*     playerc_sonar_t *mDevice; */

/*   public: */
/*     /// constructor */
/*     SonarProxy(PlayerClient *aPc, uint aIndex=0); */
/*     /// destructor */
/*     ~SonarProxy(); */

/*     /// return the sensor count */
/*     uint GetCount() const { return GetVar(mDevice->scan_count); }; */

/*     /// return a particular scan value */
/*     double GetScan(uint aIndex) const */
/*       { return GetVar(mDevice->scan[aIndex]); }; */
/*     /// This operator provides an alternate way of access the scan data. */
/*     /// For example, SonarProxy[0] == SonarProxy.GetRange(0) */
/*     double operator [] (uint aIndex) const { return GetScan(aIndex); } */

/*     /// Number of valid sonar poses */
/*     uint GetPoseCount() const { return GetVar(mDevice->pose_count); }; */

/*     /// Sonar poses (m,m,radians) */
/*     player_pose_t GetPose(uint aIndex) const */
/*       { return GetVar(mDevice->poses[aIndex]); }; */

/*     // Enable/disable the sonars. */
/*     // Set @p state to 1 to enable, 0 to disable. */
/*     // Note that when sonars are disabled the client will still receive sonar */
/*     // data, but the ranges will always be the last value read from the sonars */
/*     // before they were disabled. */
/*     //void SetEnable(bool aEnable); */

/*     /// Request the sonar geometry. */
/*     void RequestGeom(); */
/* }; */


namespace std
{
  std::ostream& operator << (std::ostream& os, const player_point_2d_t& c);
  std::ostream& operator << (std::ostream& os, const player_pose_t& c);
  std::ostream& operator << (std::ostream& os, const player_pose3d_t& c);
  std::ostream& operator << (std::ostream& os, const player_bbox_t& c);
  std::ostream& operator << (std::ostream& os, const player_segment_t& c);
  std::ostream& operator << (std::ostream& os, const playerc_device_info_t& c);

  std::ostream& operator << (std::ostream& os, const PlayerCc::ClientProxy& c);
  std::ostream& operator << (std::ostream& os, const PlayerCc::CameraProxy& c);
  std::ostream& operator << (std::ostream& os, const PlayerCc::GripperProxy& c);
  std::ostream& operator << (std::ostream& os, const PlayerCc::LaserProxy& c);
  std::ostream& operator << (std::ostream& os, const PlayerCc::LimbProxy& c);
  std::ostream& operator << (std::ostream& os, const PlayerCc::Position2dProxy& c);
  std::ostream& operator << (std::ostream& os, const PlayerCc::Position3dProxy& c);
  std::ostream& operator << (std::ostream& os, const PlayerCc::SimulationProxy& c);
  //  std::ostream& operator << (std::ostream& os, const PlayerCc::SonarProxy& c);
}

#endif

