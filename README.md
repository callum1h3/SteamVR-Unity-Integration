# SteamVR-Unity-Integration

## About
This project is basically a system that allows a unity project to write pointer addresses to a SteamVR driver. This can be usefully because:
- You can bypass Anti-Cheat on games because Anti-Cheat does not protect SteamVR itself.
- You can abuse the position of your controllers. For example in gorilla tag you can set the position of the controller 100 units infront of you then tag people across the map
- You can play VR games with mouse and keyboard and I have already demostrated this with Gorilla Tag and Pavlov VR.
- You can create a kinda speed hack by setting the position of the camera.
This might be good for some people if you don't want to worry about having to bypass the games Anti-Cheat. As for me I have no idea how to game hack so it was very helpful.
## Installation
1. In order to install the drivers you will need to either build your version of the driver or download the driver from the releases tab in github. If your building your driver remember to rename the driver to driver_null
2. Then with the driver navigate to steamVR and go to the null driver, this is where my null driver is located: C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers\null
3. Replace the original driver in null/bin/win64 with the driver you have downloaded or built.
4. Go back to null/resources/settings and open the file in that folder with a text editor.
5. Change the line "Enable" to 'true' to enable the device.
6. Load up SteamVR and it should be installed.
7. Open the sample project in unity and run it. The sample project has code already for Gorilla Tag to test in.
## How to Code the Unity Project
The main file in the unity project is the VRManager. Without that the whole project can't send information to the driver so have the VRManager Mono Script loaded at all times in the scene.

You can use PointerNetwork.SetPointer<T>( string name, T val ) to manually set the pointers in the VR driver. As a example:
```c#
  PointerNetwork.SetPointer<float>( "LHANDTRIGGER", 1 );
```

You can easily move your jointstick around in VR to make your character move with this code:
```c#
  PointerNetwork.SetJointStick( "LHAND", new Vector2(Input.GetAxisRaw("Horizontal"), Input.GetAxisRaw("Vertical")) );
```

You can edit pointers in the basics.h file in the driver project but if you want a list of all the default pointers here:
```
HMDPositionX - float
HMDPositionY - float
HMDPositionZ - float

HMDRotationW - float
HMDRotationX - float
HMDRotationY - float
HMDRotationZ - float

LHANDPositionX - float
LHANDPositionY - float
LHANDPositionZ - float

LHANDRotationW - float
LHANDRotationX - float
LHANDRotationY - float
LHANDRotationZ - float

LHANDAPP - int
LHANDGRIP - int
LHANDSYSTEM - int
LHANDTRACKPAD - int
LHANDTrackpadX - float
LHANDTrackpadY - float
LHANDTRIGGER - float

RHANDPositionX - float
RHANDPositionY - float
RHANDPositionZ - float

RHANDRotationW - float
RHANDRotationX - float
RHANDRotationY - float
RHANDRotationZ - float

RHANDAPP - int
RHANDGRIP - int
RHANDSYSTEM - int
RHANDTRACKPAD - int
RHANDTrackpadX - float
RHANDTrackpadY - float
RHANDTRIGGER - float
```
If you want to edit the rotation of the device it is important to use the unity quaternion object and pass all of the data from the object. However you can put the UnityToVR Mono Script on a gameobject and set the deviceID to one of the following: 'HMD', 'LHAND', 'RHAND'.

## Common Issues
- SteamVR is booting up with a error message: This is because theres a error in the driver. What I used to debug is a log system to write to a file but you will have to manually go around deleting your code to find the issue.

## Extra
- This project is half taken from https://github.com/r57zone/OpenVR-driver-for-DIY. Please see his project for the original project files. All what I did is modify the driver and interface it into Unity.
- If you have any issues about the project just leave a issue in github, I will be waiting :)

## To Do
- Complete Rewrite of the driver!
