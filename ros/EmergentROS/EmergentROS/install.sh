sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu quantal main" > /etc/apt/sources.list.d/ros-latest.list'
wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
sudo apt-get update
sudo apt-get install ros-hydro-desktop-full
sudo rosdep init
rosdep update
echo "source /opt/ros/hydro/setup.bash" >> ~/.bashrc
sudo apt-get install python-rosinstall
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
catkin_init_workspace
cd ~/catkin_ws/
catkin_make
sudo apt-get install ros-hydro-rosbridge-suite
echo "source ~/catkin_ws/devel/setup.bash" >> ~/.bashrc
echo "source /usr/share/gazebo-1.9/setup.sh" >> ~/.bashrc

