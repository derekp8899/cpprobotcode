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

using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;

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
	/* don't bother prompting, just use can0 */
	//std::cout << "Please input the name of your can interface: ";
	std::string interface;
	//std::cin >> interface;
	interface = "can0";
	ctre::phoenix::platform::can::SetCANInterface(interface.c_str());
	int x = 0;
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
