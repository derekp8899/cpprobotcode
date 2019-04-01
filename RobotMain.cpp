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
	talRF.Set(ControlMode::PercentOutput, left*(-1));//reverse right side
	talRR.Set(ControlMode::PercentOutput, left*(-1));

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
  //inti the server stuff
  
  int sockfd, newsockfd, num = 1, num2 = 0;
  float distance = 10.0;
  char buffer[256];
  float contIn[14];
  for(int i = 0; i < 14; i++){//init to zero
    contIn[i] = 0;
  }
  //test the scanf from buffer;
  sprintf(buffer,"1 1 1 1 1 1 1 1 1 1 1 1 1 1");
  cout << buffer << endl;
  sscanf(buffer,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f", &contIn[0], &contIn[1], &contIn[2], &contIn[3], &contIn[4], &contIn[5], &contIn[6], &contIn[7], &contIn[8], &contIn[9], &contIn[10], &contIn[11], &contIn[12], &contIn[13]);
  //scanf();
  cout << contIn[0] << contIn[1] << contIn[2] << contIn[3] << contIn[4] << contIn[5] << contIn[6] << contIn[7] << contIn[8] << contIn[9] << contIn[10] << contIn[11] << contIn[12] << contIn[13] << endl;
     //this is the test bed for the socket connection
  connect(&sockfd, &newsockfd);
  clientConnect(&sockfd, &newsockfd);
  num = recieve(sockfd, newsockfd, buffer);
  if(!num)
    cout<< "recieved something" << num <<endl;
    sscanf(buffer,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f", &contIn[0], &contIn[1], &contIn[2], &contIn[3], &contIn[4], &contIn[5], &contIn[6], &contIn[7], &contIn[8], &contIn[9], &contIn[10], &contIn[11], &contIn[12], &contIn[13]);
    cout << contIn[1] << endl;

  
  //zero out the drive signals
  drive(0,0);
  while(true){//loop for on/off 1 second
    ctre::phoenix::unmanaged::FeedEnable(100);
    x++;
    if((x%1000)==0){
      drive(.2,0);
      for(int x = 0; x < 1000; x++){
	ctre::phoenix::unmanaged::FeedEnable(100);
	drive(.2,0);
	sleepApp(1);
      }
      x = 0;
    }
    else{
      drive(0,0);
    }
    sleepApp(1);
  }     	
  return 0;
}
