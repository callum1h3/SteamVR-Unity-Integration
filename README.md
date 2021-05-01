# SteamVR-Unity-Integration

## About
This project is basically a system that allows a unity project to write pointer addresses to a SteamVR driver. This can be usefully because:
- You can bypass Anti-Cheat on games because Anti-Cheat does not protect SteamVR itself.
- You can abuse the position of your controllers. For example in gorilla tag you can set the position of the controller 100 units infront of you then tag people across the map
- You can play VR games with mouse and keyboard and I have already demostrated this with Gorilla Tag and Pavlov VR.
- You can create a kinda speed hack by setting the position of the camera.
This might be good for some people if you don't want to worry about having to bypass the games Anti-Cheat. As for me I have no idea how to game hack so it was very helpful.
## Installation
1. In order to install the drivers you will need to either build your version of the driver or download the driver from the releases tab in github.
2. Then with the driver navigate to steamVR and go to the null driver, this is where my null driver is located: C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers\null
3. Replace the original driver in null/bin/win64 with the driver you have downloaded or built.
4. Go back to null/resources/settings and open the file in that folder with a text editor.
5. Change the line "Enable" to 'true' to enable the device.
6. Load up SteamVR and it should be installed.
7. Open the sample project in unity and run it. The sample project has code already for Gorilla Tag to test in.
## How to Code the Unity Project
- The main file in the unity project is the VRManager. Without that the whole project can't send information to the driver so have the VRManager Mono Script loaded at all times in the scene.
- You can use PointerNetwork.SetPointer<T>( string name, T val ) to manually set the pointers in the VR driver.
-