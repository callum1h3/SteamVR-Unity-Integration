#include "csampledevicedriver.h"
#include "csamplecontrollerdriver.h"
#include "basics.h"
#include <math.h>
#include <Windows.h>
#include <iostream>

using namespace vr;
using namespace std;

//Head tracking vars
static double yaw = 0, pitch = 0, roll = 0;
static double pX = 0, pY = 0, pZ = 0;
static double t0, t1, t2, t3, t4, t5;

CSampleDeviceDriver::CSampleDeviceDriver()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;

    //DriverLog( "Using settings values\n" );
    m_flIPD = vr::VRSettings()->GetFloat(k_pch_SteamVR_Section, k_pch_SteamVR_IPD_Float);

    char buf[1024];
    vr::VRSettings()->GetString(k_pch_Sample_Section, k_pch_Sample_SerialNumber_String, buf, sizeof(buf));
    m_sSerialNumber = buf;

    vr::VRSettings()->GetString(k_pch_Sample_Section, k_pch_Sample_ModelNumber_String, buf, sizeof(buf));
    m_sModelNumber = buf;

    m_nWindowX = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowX_Int32);
    m_nWindowY = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowY_Int32);
    m_nWindowWidth = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowWidth_Int32);
    m_nWindowHeight = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowHeight_Int32);
    m_nRenderWidth = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_RenderWidth_Int32);
    m_nRenderHeight = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_RenderHeight_Int32);
    m_flSecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_SecondsFromVsyncToPhotons_Float);
    m_flDisplayFrequency = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_DisplayFrequency_Float);

    /*DriverLog( "driver_null: Serial Number: %s\n", m_sSerialNumber.c_str() );
        DriverLog( "driver_null: Model Number: %s\n", m_sModelNumber.c_str() );
        DriverLog( "driver_null: Window: %d %d %d %d\n", m_nWindowX, m_nWindowY, m_nWindowWidth, m_nWindowHeight );
        DriverLog( "driver_null: Render Target: %d %d\n", m_nRenderWidth, m_nRenderHeight );
        DriverLog( "driver_null: Seconds from Vsync to Photons: %f\n", m_flSecondsFromVsyncToPhotons );
        DriverLog( "driver_null: Display Frequency: %f\n", m_flDisplayFrequency );
        DriverLog( "driver_null: IPD: %f\n", m_flIPD );*/
}

CSampleDeviceDriver::~CSampleDeviceDriver()
{
}

EVRInitError CSampleDeviceDriver::Activate(TrackedDeviceIndex_t unObjectId)
{
    m_unObjectId = unObjectId;
    m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str());
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str());
    vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserIpdMeters_Float, m_flIPD);
    vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserHeadToEyeDepthMeters_Float, 0.f);
    vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_DisplayFrequency_Float, m_flDisplayFrequency);
    vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_SecondsFromVsyncToPhotons_Float, m_flSecondsFromVsyncToPhotons);

    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);

    // avoid "not fullscreen" warnings from vrmonitor
    vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false);

    //Debug mode activate Windowed Mode (borderless fullscreen), locked to 30 FPS, for testing
    vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DisplayDebugMode_Bool, true);

    // Icons can be configured in code or automatically configured by an external file "drivername\resources\driver.vrresources".
    // Icon properties NOT configured in code (post Activate) are then auto-configured by the optional presence of a driver's "drivername\resources\driver.vrresources".
    // In this manner a driver can configure their icons in a flexible data driven fashion by using an external file.
    //
    // The structure of the driver.vrresources file allows a driver to specialize their icons based on their HW.
    // Keys matching the value in "Prop_ModelNumber_String" are considered first, since the driver may have model specific icons.
    // An absence of a matching "Prop_ModelNumber_String" then considers the ETrackedDeviceClass ("HMD", "Controller", "GenericTracker", "TrackingReference")
    // since the driver may have specialized icons based on those device class names.
    //
    // An absence of either then falls back to the "system.vrresources" where generic device class icons are then supplied.
    //
    // Please refer to "bin\drivers\sample\resources\driver.vrresources" which contains this sample configuration.
    //
    // "Alias" is a reserved key and specifies chaining to another json block.
    //
    // In this sample configuration file (overly complex FOR EXAMPLE PURPOSES ONLY)....
    //
    // "Model-v2.0" chains through the alias to "Model-v1.0" which chains through the alias to "Model-v Defaults".
    //
    // Keys NOT found in "Model-v2.0" would then chase through the "Alias" to be resolved in "Model-v1.0" and either resolve their or continue through the alias.
    // Thus "Prop_NamedIconPathDeviceAlertLow_String" in each model's block represent a specialization specific for that "model".
    // Keys in "Model-v Defaults" are an example of mapping to the same states, and here all map to "Prop_NamedIconPathDeviceOff_String".
    //
    bool bSetupIconUsingExternalResourceFile = true;
    if (!bSetupIconUsingExternalResourceFile) {
        // Setup properties directly in code.
        // Path values are of the form {drivername}\icons\some_icon_filename.png
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceOff_String, "{null}/icons/headset_sample_status_off.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, "{null}/icons/headset_sample_status_searching.gif");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{null}/icons/headset_sample_status_searching_alert.gif");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReady_String, "{null}/icons/headset_sample_status_ready.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{null}/icons/headset_sample_status_ready_alert.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, "{null}/icons/headset_sample_status_error.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, "{null}/icons/headset_sample_status_standby.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, "{null}/icons/headset_sample_status_ready_low.png");
    }

    return VRInitError_None;
}

void CSampleDeviceDriver::Deactivate()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

void CSampleDeviceDriver::EnterStandby()
{
}

void *CSampleDeviceDriver::GetComponent(const char *pchComponentNameAndVersion)
{
    if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version)) {
        return (vr::IVRDisplayComponent *)this;
    }

    // override this to add a component to a driver
    return NULL;
}

void CSampleDeviceDriver::PowerOff()
{
}

void CSampleDeviceDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1) {
        pchResponseBuffer[0] = 0;
    }
}

void CSampleDeviceDriver::GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight)
{
    *pnX = m_nWindowX;
    *pnY = m_nWindowY;
    *pnWidth = m_nWindowWidth;
    *pnHeight = m_nWindowHeight;
}

bool CSampleDeviceDriver::IsDisplayOnDesktop()
{
    return true;
}

bool CSampleDeviceDriver::IsDisplayRealDisplay()
{
    return false;
}

void CSampleDeviceDriver::GetRecommendedRenderTargetSize(uint32_t *pnWidth, uint32_t *pnHeight)
{
    *pnWidth = m_nRenderWidth;
    *pnHeight = m_nRenderHeight;
}

void CSampleDeviceDriver::GetEyeOutputViewport(EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight)
{
    *pnY = 0;
    *pnWidth = m_nWindowWidth / 2;
    *pnHeight = m_nWindowHeight;

    if (eEye == Eye_Left) {
        *pnX = 0;
    } else {
        *pnX = m_nWindowWidth / 2;
    }
}

void CSampleDeviceDriver::GetProjectionRaw(EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom)
{
    *pfLeft = -1.0;
    *pfRight = 1.0;
    *pfTop = -1.0;
    *pfBottom = 1.0;
}

DistortionCoordinates_t CSampleDeviceDriver::ComputeDistortion(EVREye eEye, float fU, float fV)
{
    DistortionCoordinates_t coordinates;
    coordinates.rfBlue[0] = fU;
    coordinates.rfBlue[1] = fV;
    coordinates.rfGreen[0] = fU;
    coordinates.rfGreen[1] = fV;
    coordinates.rfRed[0] = fU;
    coordinates.rfRed[1] = fV;
    return coordinates;
}

DeviceData DeviceToClass( int device )
{
    if (device == 1)
        return VRData.HMD;
    else if (device == 2)
        return VRData.leftController;
    else
        return VRData.rightController;
}

void SetDevicePos(int device, Vector3 pos)
{
    if (device == 1)
        VRData.HMD.transform.position = pos;
    else if (device == 2)
    {
        log_write(std::to_string(pos.y));
        VRData.leftController.transform.position = pos;
    }
    else
        VRData.rightController.transform.position = pos;
}

void SetDeviceAngle(int device, Quaternion angle)
{
    if (device == 1)
        VRData.HMD.transform.rotation = angle;
    else if (device == 2)
        VRData.leftController.transform.rotation = angle;
    else
        VRData.rightController.transform.rotation = angle;
}

float degreeToRad = 1.57079632679;

vr::DriverPose_t CSampleDeviceDriver::GetPose()
{
    vr::DriverPose_t pose = { 0 };
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
    pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

    pose.vecPosition[0] = VRData.HMD.transform.position.x;
    pose.vecPosition[1] = VRData.HMD.transform.position.y;
    pose.vecPosition[2] = VRData.HMD.transform.position.z;

    //Set controller rotation
    pose.qRotation.w = VRData.HMD.transform.rotation.w;
    pose.qRotation.x = VRData.HMD.transform.rotation.x;
    pose.qRotation.y = VRData.HMD.transform.rotation.y;
    pose.qRotation.z = VRData.HMD.transform.rotation.z;

    return pose;
}

void CSampleDeviceDriver::RunFrame()
{
    // In a real driver, this should happen from some pose tracking thread.
    // The RunFrame interval is unspecified and can be very irregular if some other
    // driver blocks it for some periodic task.
    if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
    }
}

CSampleControllerDriver::CSampleControllerDriver()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
}

void CSampleControllerDriver::SetControllerIndex(int32_t CtrlIndex)
{
    ControllerIndex = CtrlIndex;
}

CSampleControllerDriver::~CSampleControllerDriver()
{
}

vr::EVRInitError CSampleControllerDriver::Activate(vr::TrackedDeviceIndex_t unObjectId)
{
    m_unObjectId = unObjectId;
    m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ControllerType_String, "vive_controller");
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_LegacyInputProfile_String, "vive_controller");

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ModelNumber_String, "ViveMV");
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ManufacturerName_String, "HTC");
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_RenderModelName_String, "vr_controller_vive_1_5");

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_TrackingSystemName_String, "VR Controller");
    vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_DeviceClass_Int32, TrackedDeviceClass_Controller);

    switch (ControllerIndex) {
    case 1:
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, "CTRL1Serial");
        break;
    case 2:
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, "CTRL2Serial");
        break;
    }

    uint64_t supportedButtons = 0xFFFFFFFFFFFFFFFFULL;
    vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, vr::Prop_SupportedButtons_Uint64, supportedButtons);

    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    //vr::VRProperties()->SetUint64Property( m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2 );

    // avoid "not fullscreen" warnings from vrmonitor
    //vr::VRProperties()->SetBoolProperty( m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false );

    // our sample device isn't actually tracked, so set this property to avoid having the icon blink in the status window
    //vr::VRProperties()->SetBoolProperty( m_ulPropertyContainer, Prop_NeverTracked_Bool, false );

    // even though we won't ever track we want to pretend to be the right hand so binding will work as expected

    switch (ControllerIndex) {
    case 1:
        vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_LeftHand);
        break;
    case 2:
        vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_RightHand);
        break;
    }

    // this file tells the UI what to show the user for binding this controller as well as what default bindings should
    // be for legacy or other apps
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_InputProfilePath_String, "{null}/input/mycontroller_profile.json");

    //  Buttons handles
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/application_menu/click", &HButtons[0]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/grip/click", &HButtons[1]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/system/click", &HButtons[2]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trackpad/click", &HButtons[3]);

    // Analog handles
    vr::VRDriverInput()->CreateScalarComponent(
        m_ulPropertyContainer, "/input/trackpad/x", &HAnalog[0],
        vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided
    );
    vr::VRDriverInput()->CreateScalarComponent(
        m_ulPropertyContainer, "/input/trackpad/y", &HAnalog[1],
        vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided
    );
    vr::VRDriverInput()->CreateScalarComponent(
        m_ulPropertyContainer, "/input/trigger/value", &HAnalog[2],
        vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided
    );

    vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);

    // create our haptic component
    vr::VRDriverInput()->CreateHapticComponent(m_ulPropertyContainer, "/output/haptic", &m_compHaptic);

    return VRInitError_None;
}

void CSampleControllerDriver::Deactivate()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

void CSampleControllerDriver::EnterStandby()
{
}

void* CSampleControllerDriver::GetComponent(const char* pchComponentNameAndVersion)
{
    // override this to add a component to a driver
    return NULL;
}

void CSampleControllerDriver::PowerOff()
{
}

void CSampleControllerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1) {
        pchResponseBuffer[0] = 0;
    }
}

vr::DriverPose_t CSampleControllerDriver::GetPose()
{
    DriverPose_t pose = { 0 };
    //pose.poseIsValid = false;
    pose.poseIsValid = true;
    //pose.result = TrackingResult_Calibrating_OutOfRange;
    pose.result = TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
    pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

    if (ControllerIndex == 1) {
        //log_write(std::to_string(VRData.leftController.transform.position.x));
        pose.vecPosition[0] = VRData.leftController.transform.position.x;
        pose.vecPosition[1] = VRData.leftController.transform.position.y;
        pose.vecPosition[2] = VRData.leftController.transform.position.z;

        //Set controller rotation
        pose.qRotation.w = VRData.leftController.transform.rotation.w;
        pose.qRotation.x = VRData.leftController.transform.rotation.x;
        pose.qRotation.y = VRData.leftController.transform.rotation.y;
        pose.qRotation.z = VRData.leftController.transform.rotation.z;
        return pose;
    }
    else {
        pose.vecPosition[0] = VRData.rightController.transform.position.x;
        pose.vecPosition[1] = VRData.rightController.transform.position.y;
        pose.vecPosition[2] = VRData.rightController.transform.position.z;

        //Set controller rotation
        pose.qRotation.w = VRData.rightController.transform.rotation.w;
        pose.qRotation.x = VRData.rightController.transform.rotation.x;
        pose.qRotation.y = VRData.rightController.transform.rotation.y;
        pose.qRotation.z = VRData.rightController.transform.rotation.z;
        return pose;
    }
}

void CSampleControllerDriver::RunFrame()
{

    // Your driver would read whatever hardware state is associated with its input components and pass that
    // in to UpdateBooleanComponent. This could happen in RunFrame or on a thread of your own that's reading USB
    // state. There's no need to update input state unless it changes, but it doesn't do any harm to do so.

    if (ControllerIndex == 1) {
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[0], VRData.leftController.buttonData.ApplicationMenu, 0); //Application Menu
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[1], VRData.leftController.buttonData.Grip, 0); //Grip
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[2], VRData.leftController.buttonData.System, 0); //System
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[3], VRData.leftController.buttonData.Trackpad, 0); //Trackpad

        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], VRData.leftController.buttonData.TrackpadXY.x, 0); //Trackpad x
        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], VRData.leftController.buttonData.TrackpadXY.y, 0); //Trackpad y

        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], VRData.leftController.buttonData.Trigger, 0); // Trigger
    }
    else {
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[0], VRData.rightController.buttonData.ApplicationMenu, 0); //Application Menu
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[1], VRData.rightController.buttonData.Grip, 0); //Grip
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[2], VRData.rightController.buttonData.System, 0); //System
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[3], VRData.rightController.buttonData.Trackpad, 0); //Trackpad

        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], VRData.rightController.buttonData.TrackpadXY.x, 0); //Trackpad x
        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], VRData.rightController.buttonData.TrackpadXY.y, 0); //Trackpad y

        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], VRData.rightController.buttonData.Trigger, 0); // Trigger
    }

    //if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
    //}

}

void CSampleControllerDriver::ProcessEvent(const vr::VREvent_t& vrEvent)
{
    switch (vrEvent.eventType) {
    case vr::VREvent_Input_HapticVibration:
        if (vrEvent.data.hapticVibration.componentHandle == m_compHaptic) {
            // This is where you would send a signal to your hardware to trigger actual haptic feedback
            //DriverLog( "BUZZ!\n" );
        }
        break;
    }
}

std::string CSampleControllerDriver::GetSerialNumber() const
{
    switch (ControllerIndex) {
    case 1:
        return "CTRL1Serial";
        break;
    case 2:
        return "CTRL2Serial";
        break;
    }
}


