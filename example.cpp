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
TalonSRX talL1(1);
TalonSRX talL2(2);
TalonSRX talR1(3);
TalonSRX talR2(4);

void drive(double fwd, double turn)
{
	double left = fwd - turn;
	double rght = fwd + turn; /* positive turn means turn robot LEFT */

	talL1.Set(ControlMode::PercentOutput, left);
	talL2.Set(ControlMode::PercentOutput, left);	
	talR2.Set(ControlMode::PercentOutput, left);
	talR1.Set(ControlMode::PercentOutput, left);

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

	drive(-.2,0);
	while(true){
	  ctre::phoenix::unmanaged::FeedEnable(100);
	  
	}     	
	return 0;
}
