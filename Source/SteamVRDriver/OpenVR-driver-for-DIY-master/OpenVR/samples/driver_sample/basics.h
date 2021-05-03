#ifndef BASICS_H
#define BASICS_H

#include <openvr_driver.h>
#include <fstream>
#include <string>
#include <iostream>

class Vector3
{
public:
	float x = 0;
	float y = 0;
	float z = 0;


	Vector3(float x1, float y1, float z1)
	{
		x = x1;
		y = y1;
		z = z1;
	};
};

class Vector2
{
public:
	float x = 0;
	float y = 0;

	Vector2(float x1, float y1)
	{
		x = x1;
		y = y1;
	};
};

class Quaternion
{
public:
	float w = 0;
	float x = 0;
	float y = 0;
	float z = 0;

	Quaternion(float x1, float y1, float z1, float w1)
	{
		w = w1;
		x = x1;
		y = y1;
		z = z1;
	};
};


class Transform
{
public:
	Vector3 position = Vector3(0, 0, 0);
	Quaternion rotation = Quaternion(0, 0, 0, 0);

	Transform()
	{
		position = Vector3(0, 0, 0);
		rotation = Quaternion(0, 0, 0, 0);
	}
};

class ButtonData
{
public:
	int ApplicationMenu = 0;
	int Grip = 0;
	int System = 0;
	int Trackpad = 0;

	Vector2 TrackpadXY = Vector2(0,0);
	float Trigger = 0;
};
class DeviceData
{
public:
	Transform transform = Transform();
};

class ControllerData : public DeviceData
{
public:
	ButtonData buttonData = ButtonData();
};


class DataHolder
{
public:
	DeviceData HMD = DeviceData();
	ControllerData leftController = ControllerData();
	ControllerData rightController = ControllerData();
};

static DataHolder VRData = DataHolder();

static DataHolder getData()
{
	return VRData;
}

static void setupData()
{
	VRData = DataHolder();
	VRData.HMD = DeviceData();
	VRData.HMD.transform = Transform();

	VRData.leftController = ControllerData();
	VRData.leftController.transform = Transform();

	VRData.rightController = ControllerData();
	VRData.rightController.transform = Transform();
}

static std::string log_data = "";
static void log_write(std::string text)
{
	log_data = log_data + (text + "\n");

	std::ofstream myfile("C:\\Users\\callu\\AppData\\Roaming\\VR\\log.txt");
	if (myfile.is_open())
	{
		myfile << log_data;
		myfile.close();
	}
}

#if defined(_WINDOWS)
#include <windows.h>
#else
#include <string.h>
#define _stricmp strcasecmp
int GetAsyncKeyState(int key);

#define VK_NUMPAD3 0x8001
#define VK_NUMPAD1 0x8002
#define VK_NUMPAD4 0x8003
#define VK_NUMPAD6 0x8004
#define VK_NUMPAD8 0x8005
#define VK_NUMPAD2 0x8006
#define VK_NUMPAD9 0x8007
#define VK_UP      0x8008
#define VK_DOWN    0x8009
#define VK_LEFT    0x80010
#define VK_RIGHT   0x80011
#define VK_PRIOR   0x80012
#define VK_NEXT    0x80013
#define VK_END     0x80014
#endif

// keys for use with the settings API
extern const char *const k_pch_Sample_Section;
extern const char *const k_pch_Sample_SerialNumber_String;
extern const char *const k_pch_Sample_ModelNumber_String;
extern const char *const k_pch_Sample_WindowX_Int32;
extern const char *const k_pch_Sample_WindowY_Int32;
extern const char *const k_pch_Sample_WindowWidth_Int32;
extern const char *const k_pch_Sample_WindowHeight_Int32;
extern const char *const k_pch_Sample_RenderWidth_Int32;
extern const char *const k_pch_Sample_RenderHeight_Int32;
extern const char *const k_pch_Sample_SecondsFromVsyncToPhotons_Float;
extern const char *const k_pch_Sample_DisplayFrequency_Float;

extern bool g_bExiting;

static bool vrepichasInit = false;
static void RunInitalize()
{
	log_write("VR Initalized!");

	std::ofstream myfile;

	char* appdata = getenv("APPDATA");
	strcat(appdata, "\\processInformation.txt");

	myfile.open(appdata);
	myfile << GetCurrentProcessId() << "\n";

	// HMD
	myfile << "HMDPositionX" << " " << &VRData.HMD.transform.position.x << "\n";
	myfile << "HMDPositionY" << " " << &VRData.HMD.transform.position.y << "\n";
	myfile << "HMDPositionZ" << " " << &VRData.HMD.transform.position.z << "\n";

	myfile << "HMDRotationW" << " " << &VRData.HMD.transform.rotation.w << "\n";
	myfile << "HMDRotationX" << " " << &VRData.HMD.transform.rotation.x << "\n";
	myfile << "HMDRotationY" << " " << &VRData.HMD.transform.rotation.y << "\n";
	myfile << "HMDRotationZ" << " " << &VRData.HMD.transform.rotation.z << "\n";

	// LEFTCONTROLLER
	myfile << "LHANDPositionX" << " " << &VRData.leftController.transform.position.x << "\n";
	myfile << "LHANDPositionY" << " " << &VRData.leftController.transform.position.y << "\n";
	myfile << "LHANDPositionZ" << " " << &VRData.leftController.transform.position.z << "\n";

	myfile << "LHANDRotationW" << " " << &VRData.leftController.transform.rotation.w << "\n";
	myfile << "LHANDRotationX" << " " << &VRData.leftController.transform.rotation.x << "\n";
	myfile << "LHANDRotationY" << " " << &VRData.leftController.transform.rotation.y << "\n";
	myfile << "LHANDRotationZ" << " " << &VRData.leftController.transform.rotation.z << "\n";

	// LEFTCONTROLLER BUTTONS
	myfile << "LHANDAPP" << " " << &VRData.leftController.buttonData.ApplicationMenu << "\n";
	myfile << "LHANDGRIP" << " " << &VRData.leftController.buttonData.Grip << "\n";
	myfile << "LHANDSYSTEM" << " " << &VRData.leftController.buttonData.System << "\n";
	myfile << "LHANDTRACKPAD" << " " << &VRData.leftController.buttonData.Trackpad << "\n";

	myfile << "LHANDTrackpadX" << " " << &VRData.leftController.buttonData.TrackpadXY.x << "\n";
	myfile << "LHANDTrackpadY" << " " << &VRData.leftController.buttonData.TrackpadXY.y << "\n";

	myfile << "LHANDTRIGGER" << " " << &VRData.leftController.buttonData.Trigger << "\n";

	// RIGHTCONTROLLER
	myfile << "RHANDPositionX" << " " << &VRData.rightController.transform.position.x << "\n";
	myfile << "RHANDPositionY" << " " << &VRData.rightController.transform.position.y << "\n";
	myfile << "RHANDPositionZ" << " " << &VRData.rightController.transform.position.z << "\n";

	myfile << "RHANDRotationW" << " " << &VRData.rightController.transform.rotation.w << "\n";
	myfile << "RHANDRotationX" << " " << &VRData.rightController.transform.rotation.x << "\n";
	myfile << "RHANDRotationY" << " " << &VRData.rightController.transform.rotation.y << "\n";
	myfile << "RHANDRotationZ" << " " << &VRData.rightController.transform.rotation.z << "\n";

	// RIGHTCONTROLLER BUTTONS
	myfile << "RHANDAPP" << " " << &VRData.rightController.buttonData.ApplicationMenu << "\n";
	myfile << "RHANDGRIP" << " " << &VRData.rightController.buttonData.Grip << "\n";
	myfile << "RHANDSYSTEM" << " " << &VRData.rightController.buttonData.System << "\n";
	myfile << "RHANDTRACKPAD" << " " << &VRData.rightController.buttonData.Trackpad << "\n";

	myfile << "RHANDTrackpadX" << " " << &VRData.rightController.buttonData.TrackpadXY.x << "\n";
	myfile << "RHANDTrackpadY" << " " << &VRData.rightController.buttonData.TrackpadXY.y << "\n";

	myfile << "RHANDTRIGGER" << " " << &VRData.rightController.buttonData.Trigger << "\n";

	myfile.close();

	vrepichasInit = true;
}


inline vr::HmdQuaternion_t HmdQuaternion_Init( double w, double x, double y, double z )
{
	if (!vrepichasInit)
		RunInitalize();

    vr::HmdQuaternion_t quat;
    quat.w = w;
    quat.x = x;
    quat.y = y;
    quat.z = z;
    return quat;
}

inline void HmdMatrix_SetIdentity(vr::HmdMatrix34_t *pMatrix)
{
    pMatrix->m[0][0] = 1.f;
    pMatrix->m[0][1] = 0.f;
    pMatrix->m[0][2] = 0.f;
    pMatrix->m[0][3] = 0.f;
    pMatrix->m[1][0] = 0.f;
    pMatrix->m[1][1] = 1.f;
    pMatrix->m[1][2] = 0.f;
    pMatrix->m[1][3] = 0.f;
    pMatrix->m[2][0] = 0.f;
    pMatrix->m[2][1] = 0.f;
    pMatrix->m[2][2] = 1.f;
    pMatrix->m[2][3] = 0.f;
}

#endif // BASICS_H
