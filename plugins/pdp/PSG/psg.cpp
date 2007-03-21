#include "psg.h"
#include <QtPlugin>
#include <iostream>

const taVersion PSGPlugin::version(1,0,0,0);

PSGPlugin::PSGPlugin(QObject*){}

int PSGPlugin::InitializeTypes() {
  ta_Init_psg();  // call the maketa-generated type initialization routine
  return 0;
}

int PSGPlugin::InitializePlugin() {
  return 0;
}

const char* PSGPlugin::url() {
  return "http://grey.colorado.edu/cgi-bin/trac.cgi";
}

Q_EXPORT_PLUGIN2(psg, PSGPlugin);

////////////////////////////////////////
//  	PSG Base

using namespace PlayerCc;

void PSGBase::Initialize() {
  client = NULL;
}

void PSGBase::Destroy() {
  if (client) {delete client; client = NULL;}
}

bool PSGBase::AttachToPlayer() {
  if(TestError(!(client = new PlayerClient("localhost")), "AttachToPlayer()",
	       "could not attach!"))
    return false;
  return true;
}

void PSGDemo::Initialize() {
  sonar_proxy = NULL;
  position_proxy = NULL;

  rate_per_second = 20;
  collision_threshold = .5;
  run_speed = .1;
}

void PSGDemo::Destroy() {
  if (position_proxy) {delete position_proxy; position_proxy = NULL;}
  if (sonar_proxy) {delete sonar_proxy; sonar_proxy = NULL;}
}

bool PSGDemo::RunDemo(int n_iterations) {
  if(TestError(!client, "RunDemo", "need to attach to player first")) return false;

  if(TestError(!(sonar_proxy = new SonarProxy(client,0)), "RunDemo",
	       "could not get sonor proxy")) return false;
  if(TestError(!(position_proxy = new Position2dProxy(client,0)), "RunDemo",
	       "could not get pos proxy")) return false;

  for(int i=0;i<n_iterations;i++) {
    double turnrate, speed;

    // read from the proxies
    client->Read();

    // print out sonars for fun
    std::cout << *sonar_proxy << std::endl;

    // do simple collision avoidance
    if(((*sonar_proxy)[0] + (*sonar_proxy)[1]) < ((*sonar_proxy)[6] + (*sonar_proxy)[7]))
      turnrate = dtor(-rate_per_second); // turn 20 degrees per second
    else
      turnrate = dtor(rate_per_second);

    if((*sonar_proxy)[3] < collision_threshold)
      speed = 0;
    else
      speed = run_speed;

    // command the motors
    position_proxy->SetSpeed(speed, turnrate);
  }
}
