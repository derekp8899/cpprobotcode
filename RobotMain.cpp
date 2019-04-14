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

#define MAX .3 //our max value for the drive system

//the common namespaces for the program
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

//function to do the driving
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

  
  //for the ramp clock
  double rampTime = 0;
  double speedMult = .1;
  double maxTime = 1.2;

  //for the server connections
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

  //container for storing the distance measure and sensor value
  float distRet;
  int sensorFlag;
  
  //zero out the drive signals
  drive(0,0);
  sleep(5);//wait for all inits to finish

  //test the connection with a single recv and send
  printf("attempt recv\n");
  fflush(stdout);
  num = recieve(sockfd, newsockfd, buffer);
  printf("testing recv: %s\n",buffer);
  fflush(stdout);
  sprintf(rBuf,"%f %f %f %f %f",10.0,10.0,10.0,10.0,10.0);
  send(newsockfd,rBuf);

  //print out the current control values
  printf("Current control values\n");

  //for more stuff for the ramping and for holding values for mining status
  clock_t clockT = clock();
  float leftSpd;
  float rightSpd;
  int extend=0;
  int mineOn=0;
  
  while(true){//the main control loop
    ctre::phoenix::unmanaged::FeedEnable(100);//enable the drive functions of the talons
    //if we can still recieve new control values
    if(!(num = recieve(sockfd, newsockfd, buffer))){
      sscanf(buffer,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f", &contIn[0], &contIn[1], &contIn[2], &contIn[3], &contIn[4], &contIn[5], &contIn[6], &contIn[7], &contIn[8], &contIn[9], &contIn[10], &contIn[11], &contIn[12], &contIn[13]);//parse the input string
      printf("\rY: %f, X: %f",contIn[1],contIn[0]);//print the recieved values
      rampTime = (clock()-clockT)/CLOCKS_PER_SEC;//start the ramptimer
      /* this is the logic for the ramp timer  */
      if(contIn[1] > .05 || contIn[1] < -.05 ){//if we have control values out of the 'deadzone'
	if(rampTime >= maxTime){//we we are over the max ramp time
	  contIn[1]=contIn[1]*MAX;
	  contIn[0]=contIn[0]*MAX;//set control values to max
	  drive(contIn[1],contIn[0]);//init the drive
	  leftSpd=contIn[1]-contIn[0];
	  rightSpd=contIn[1]+contIn[0];//set the return values for the ui
	}
	else{//else we are still ramping up the speed
	  contIn[1]=contIn[1]*speedMult;
	  contIn[0]=contIn[0]*speedMult;//set the control values based on surrent speed multiplier
	  drive(contIn[1],contIn[0]);//init the drive
	  leftSpd=contIn[1]-contIn[0];
	  rightSpd=contIn[1]+contIn[0];//set the return values for the ui
	  speedMult=.1+(.16* (clock()-clockT)/(double)CLOCKS_PER_SEC);//update the speed multiplier
	}
      }
      else{//else we set all the stuff to zero
	rampTime = 0;
	clockT = clock();
	drive(0,0);
	leftSpd=0;
	rightSpd=0;
      }
      /*--------------------------------------------------------*/
      /* THIS IS THE OUT DATED DRIVE FUNCTIONS */
      /*
      if(contIn[1] > MAX)
      	drive(MAX,0);
      else if(contIn[1] < -MAX)
	drive(-MAX,0);
      else
	drive(contIn[1],0);
      */
      /*------------------------------------------------------*/

      // generate the return string to the client for the ui and send
      sprintf(rBuf,"%f %f %f %f %f",leftSpd,-rightSpd,leftSpd,-rightSpd,10.0);//switch to using dist with cam on, known value of ten for now also add in the miner flags
      send(newsockfd,rBuf);
      retBuf="";
    }
    else{//set the drive to zero and exit the program if we lose the connection
      cout <<"exiting"<<endl;// "Disconnected from station attempting reconnect" << endl; //add the second string back in if reconnect is finished
      drive(0,0);
    }
  }     	
  return 0;
}
