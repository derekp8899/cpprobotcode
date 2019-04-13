#define Phoenix_No_WPI // remove WPI dependencies
#include "ctre/Phoenix.h"
#include "ctre/phoenix/platform/Platform.h"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include <unistd.h>
#include "server.h"
#include <stdio.h>
#include <ctime>

#define MAX .3

using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;
using namespace std;

/* make some talons for drive train */
TalonSRX talLF(1);
TalonSRX talRF(2);
TalonSRX talLR(3);
TalonSRX talRR(4);

void drive(double fwd, double turn)
{
	double left = fwd - turn;
	double rght = fwd + turn; /* positive turn means turn robot LEFT */

	talLF.Set(ControlMode::PercentOutput, left);
	talLR.Set(ControlMode::PercentOutput, left);	
	talRF.Set(ControlMode::PercentOutput, rght*(-1));//reverse right side
	talRR.Set(ControlMode::PercentOutput, rght*(-1));

}
/** simple wrapper for code cleanup */
void sleepApp(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {

  //init the can interface to send the messages to
  /* don't bother prompting, just use can0 */
  //std::cout << "Please input the name of your can interface: ";
  std::string interface;
  //std::cin >> interface;
  interface = "can0";
  ctre::phoenix::platform::can::SetCANInterface(interface.c_str());
  int x = 0;
  //init the server stuff
  double rampTime = 0;
  double speedMult = .1;
  double maxTime = 1.2;
  int sockfd, newsockfd, num = 1, num2 = 0;
  float distance = 10.0;
  char buffer[256];
  float contIn[14];
  for(int i = 0; i < 14; i++){//init to zero
    contIn[i] = 0;
  }
  string retBuf;
  char* rBuf = (char*)malloc(255);
  connect(&sockfd, &newsockfd);
  clientConnect(&sockfd, &newsockfd);
  
  //zero out the drive signals
  drive(0,0);
  sleep(5);//wait for all inits to finish
  printf("attempt recv\n");
  fflush(stdout);
  //  sleep(1);
  num = recieve(sockfd, newsockfd, buffer);
  //  sleep(1);
  printf("testing recv: %s\n",buffer);
  fflush(stdout);
  sprintf(rBuf,"%f %f %f %f %f",10.0,10.0,10.0,10.0,10.0);
  send(newsockfd,rBuf);
  printf("Current control values\n");
  clock_t clockT = clock();
  float leftSpd;
  float rightSpd;
  while(true){//loop for on/off 1 second
    ctre::phoenix::unmanaged::FeedEnable(100);
    if(!(num = recieve(sockfd, newsockfd, buffer))){
      sscanf(buffer,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f", &contIn[0], &contIn[1], &contIn[2], &contIn[3], &contIn[4], &contIn[5], &contIn[6], &contIn[7], &contIn[8], &contIn[9], &contIn[10], &contIn[11], &contIn[12], &contIn[13]);
      printf("\rY: %f, X: %f",contIn[1],contIn[0]);
      rampTime = (clock()-clockT)/CLOCKS_PER_SEC;
      if(contIn[1] > .05 || contIn[1] < -.05 ){
	if(rampTime >= maxTime){
	  contIn[1]=contIn[1]*MAX;
	  contIn[0]=contIn[0]*MAX;
	  drive(contIn[1],contIn[0]);
	  leftSpd=contIn[1]-contIn[0];
	  rightSpd=contIn[1]+contIn[0];
	}
	else{
	  contIn[1]=contIn[1]*speedMult;
	  contIn[0]=contIn[0]*speedMult;
	  drive(contIn[1],contIn[0]);
	  leftSpd=contIn[1]-contIn[0];
	  rightSpd=contIn[1]+contIn[0];
	  speedMult=.1+(.16* (clock()-clockT)/(double)CLOCKS_PER_SEC);
	}
      }
      else{
	rampTime = 0;
	clockT = clock();
	drive(0,0);
	leftSpd=0;
	rightSpd=0;
      }
      /*
      if(contIn[1] > MAX)
      	drive(MAX,0);
      else if(contIn[1] < -MAX)
	drive(-MAX,0);
      else
	drive(contIn[1],0);
      */
      sprintf(rBuf,"%f %f %f %f %f",leftSpd,-rightSpd,leftSpd,-rightSpd,10.0);
      send(newsockfd,rBuf);
      retBuf="";
    }
    else{
      cout <<"exiting"<<endl;// "Disconnected from station attempting reconnect" << endl;
      // connect(&sockfd, &newsockfd);
      //clientConnect(&sockfd, &newsockfd);
      drive(0,0);
    }
  }     	
  return 0;
}
