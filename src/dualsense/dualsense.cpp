extern "C" {
#include "dualsense.h"
}

#include <hidapi.h>
#include <memory>

#include "GCore/Interfaces/IPlatformHardware.h"
#include "GImplementations/Libraries/DualSense/DualSenseLibrary.h"
#include "linux_hidapi_platform.h"

struct JSDualSense {
  FDualSenseLibrary Lib;
};

namespace {

void EnsurePlatformReady() {
  static bool Ready = false;
  if (Ready) return;
  hid_init();
  IPlatformHardware::SetInstance(std::make_unique<LinuxHidapiPlatform>());
  Ready = true;
}

} // namespace

JSDualSense *js_dualsense_create(void) {
  EnsurePlatformReady();

  std::vector<FDeviceContext> Found;
  IPlatformHardware::Get().Detect(Found);

  for (FDeviceContext &Context : Found) {
    if (!IPlatformHardware::Get().CreateHandle(&Context)) continue;

    JSDualSense *Device = new JSDualSense();
    Device->Lib.Initialize(Context);
    Device->Lib.EnableMotionSensor(true);
    Device->Lib.EnableTouch(true);
    return Device;
  }
  return nullptr;
}

void js_dualsense_destroy(JSDualSense *Device) {
  if (!Device) return;
  Device->Lib.ShutdownLibrary();
  delete Device;
}

bool js_dualsense_update(JSDualSense *Device, float Delta, JSDualSenseState *OutState) {
  if (!Device || !OutState) return false;

  Device->Lib.UpdateInput(Delta);
  OutState->connected = Device->Lib.IsConnected();
  if (!OutState->connected) return false;

  FInputContext *Input = Device->Lib.GetMutableDeviceContext()->GetInputState();

  OutState->gyro[0] = Input->Gyroscope.X;
  OutState->gyro[1] = Input->Gyroscope.Y;
  OutState->gyro[2] = Input->Gyroscope.Z;

  OutState->accel[0] = Input->Accelerometer.X;
  OutState->accel[1] = Input->Accelerometer.Y;
  OutState->accel[2] = Input->Accelerometer.Z;

  OutState->touch.active = Input->bIsTouching;
  OutState->touch.finger_count = Input->TouchFingerCount;
  OutState->touch.x = Input->TouchRadius.X != 0.0f ? Input->TouchPosition.X / Input->TouchRadius.X : 0.0f;
  OutState->touch.y = Input->TouchRadius.Y != 0.0f ? Input->TouchPosition.Y / Input->TouchRadius.Y : 0.0f;

  return true;
}

void js_dualsense_set_lightbar(JSDualSense *Device, uint8_t r, uint8_t g, uint8_t b) {
  if (!Device) return;
  if (IGamepadLightbar *Lightbar = Device->Lib.GetIGamepadLightbar()) {
    Lightbar->SetLightbar({r, g, b});
  }
  Device->Lib.UpdateOutput();
}

void js_dualsense_set_player_leds(JSDualSense *Device, uint8_t leds) {
  if (!Device) return;
  if (IGamepadLightbar *Lightbar = Device->Lib.GetIGamepadLightbar()) {
    Lightbar->SetPlayerLed(static_cast<EDSPlayer>(leds), 255);
  }
  Device->Lib.UpdateOutput();
}
