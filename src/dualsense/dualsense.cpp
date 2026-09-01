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

int js_dualsense_detect_all(JSDualSense *OutDevices[], int MaxDevices) {
  EnsurePlatformReady();

  std::vector<FDeviceContext> Found;
  IPlatformHardware::Get().Detect(Found);

  int Count = 0;
  for (FDeviceContext &Context : Found) {
    if (Count >= MaxDevices) break;
    if (!IPlatformHardware::Get().CreateHandle(&Context)) continue;

    JSDualSense *Device = new JSDualSense();
    Device->Lib.Initialize(Context);
    Device->Lib.EnableMotionSensor(true);
    Device->Lib.EnableTouch(true);
    OutDevices[Count++] = Device;
  }
  return Count;
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

  OutState->touchpad_pressed = Input->bTouch;

  OutState->touch[0].active = Input->bIsTouching;
  OutState->touch[0].x = Input->TouchRadius.X != 0.0f ? Input->TouchPosition.X / Input->TouchRadius.X : 0.0f;
  OutState->touch[0].y = Input->TouchRadius.Y != 0.0f ? Input->TouchPosition.Y / Input->TouchRadius.Y : 0.0f;

  // Upstream's ProcessTouchDualSense() has a byte-offset bug in its second
  // touch point decode (swaps bytes 0x26/0x27 relative to the pattern it
  // correctly uses for the first point), producing garbage x/y. Parsed here
  // directly from the raw report instead, per the layout confirmed against
  // linux/drivers/hid/hid-playstation.c's dualsense_touch_point[2].
  FDeviceContext *RawContext = Device->Lib.GetMutableDeviceContext();
  size_t Padding = RawContext->ConnectionType == EDSDeviceConnection::Bluetooth ? 2 : 1;
  const unsigned char *HidInput = &RawContext->Buffer[Padding];

  bool Point1Active = (HidInput[0x24] & 0x80) == 0;
  OutState->touch[1].active = Point1Active;
  if (Point1Active) {
    float RawX = static_cast<float>(((HidInput[0x26] & 0x0F) << 8) | HidInput[0x25]);
    float RawY = static_cast<float>((HidInput[0x27] << 4) | ((HidInput[0x26] & 0xF0) >> 4));
    OutState->touch[1].x = Input->TouchRadius.X != 0.0f ? RawX / Input->TouchRadius.X : 0.0f;
    OutState->touch[1].y = Input->TouchRadius.Y != 0.0f ? RawY / Input->TouchRadius.Y : 0.0f;
  }

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
