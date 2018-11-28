# bluetooth-raspberry-android
Create a raspberry pi 3 bluetooth server sending data to an android device (or Linux) client.

__Details__ : 
On the raspberry pi _server_, a worker thread increments an integer every 5s. 
This value is send over Bluetooth to the connected clients. This part is only c++ based and is a Qt console application 
(no window). The _clients_ try to find the uuid bluetooth service to connect. Then it only listens to the server incoming value
and update the QML based window displaying the value.

## Requirements
* Qt built on raspberry pi 3 with the bluetooth libs (used : Qt 5.11)
* Qt for Android (used : armv7 compiler with ndk10)
* _optional_ : Qt for Desktop (used : Ubuntu 18.04)

## Build
After cloning the repo, open the two projects in QtCreator (.pro) and build each one with the right kits.
After deploying, launch the server first (on the pi), then the client. 
For the moment, the client has to be relaunched to restart discovery.

_NB_ : Devices were paired for test. I did'nt test without pairing.

_NB_ : You can change the uuid service for more security ! Make sure to use the same between server and clients

## Current issues
Clients work well on Ubuntu 18.04 but not on Android SDK 24 (LG G6). Any help would be appreciated.

## License
_GNU General Public License v3.0_
