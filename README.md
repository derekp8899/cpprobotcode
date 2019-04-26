# UND Mining Team Robot main code

For use on Jetson tx2 running ubuntu and CTRE TALONSRX motor controllers


#### Binaries
| Name        |  Notes |
| ------------- | ------------- |
| libCTRE_PhoenixPlatformLinuxSocketCan.a | Platform implementation to support USB-to-CANbus adapters in Linux (see parent folder for arch).|
| libCTRE_PhoenixCanutils.a | Implements CANComm_* routines so you don't have to.  But if you want to, remove this from the link list. |
| libCTRE_PhoenixCCI.a | Phoenix Common C Interface, leave this be. |
| libCTRE_Phoenix.a | Class library, this is built from Phoenix-frc-lib. |

For the latest version of binaries, see the CTRE maven repository here:  
http://devsite.ctr-electronics.com/maven/release/com/ctre/phoenix/

#### CAN USB Adapter
SocketCAN USB adapter used below.
- Firmware : https://github.com/HubertD/candleLight_fw
- Hardware : http://canable.io/



