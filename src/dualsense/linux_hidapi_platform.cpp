#include "linux_hidapi_platform.h"

#include <hidapi.h>

namespace {

constexpr unsigned short SONY_VENDOR_ID = 0x054C;
constexpr unsigned short DUALSENSE_PRODUCT_ID = 0x0CE6;
constexpr unsigned short DUALSENSE_EDGE_PRODUCT_ID = 0x0DF2;

// The DualSense exposes more than one HID interface per physical unit (e.g. a
// vendor-defined one alongside the actual gamepad reports); hidapi's Linux
// hidraw backend returns one hid_device_info per interface, all sharing the
// same VID/PID. Only the interface that declares the standard Generic
// Desktop/Gamepad top-level collection carries the input reports this
// library parses — filtering on it keeps Detect() to exactly one entry per
// physical controller.
constexpr unsigned short GENERIC_DESKTOP_USAGE_PAGE = 0x01;
constexpr unsigned short GAMEPAD_USAGE = 0x05;

EDSDeviceType DeviceTypeForProductId(unsigned short ProductId) {
  if (ProductId == DUALSENSE_PRODUCT_ID) return EDSDeviceType::DualSense;
  if (ProductId == DUALSENSE_EDGE_PRODUCT_ID) return EDSDeviceType::DualSenseEdge;
  return EDSDeviceType::NotFound;
}

} // namespace

void LinuxHidapiPlatform::Detect(std::vector<FDeviceContext> &Devices) {
  hid_device_info *List = hid_enumerate(SONY_VENDOR_ID, 0);
  for (hid_device_info *Info = List; Info != nullptr; Info = Info->next) {
    EDSDeviceType DeviceType = DeviceTypeForProductId(Info->product_id);
    if (DeviceType == EDSDeviceType::NotFound) continue;
    if (Info->usage_page != GENERIC_DESKTOP_USAGE_PAGE || Info->usage != GAMEPAD_USAGE) continue;

    FDeviceContext Context;
    Context.Path = Info->path;
    Context.DeviceType = DeviceType;
    Context.ConnectionType = (Info->bus_type == HID_API_BUS_BLUETOOTH)
                                  ? EDSDeviceConnection::Bluetooth
                                  : EDSDeviceConnection::Usb;
    Devices.push_back(Context);
  }
  hid_free_enumeration(List);
}

bool LinuxHidapiPlatform::CreateHandle(FDeviceContext *Context) {
  hid_device *Handle = hid_open_path(Context->Path.c_str());
  if (!Handle) return false;

  hid_set_nonblocking(Handle, 1);
  Context->Handle = Handle;
  Context->IsConnected = true;
  return true;
}

void LinuxHidapiPlatform::InvalidateHandle(FDeviceContext *Context) {
  if (Context->Handle) {
    hid_close(static_cast<hid_device *>(Context->Handle));
    Context->Handle = nullptr;
  }
  Context->IsConnected = false;
}

void LinuxHidapiPlatform::Read(FDeviceContext *Context) {
  if (!Context->Handle) return;
  hid_device *Handle = static_cast<hid_device *>(Context->Handle);
  int BytesRead;
  while ((BytesRead = hid_read(Handle, Context->Buffer, sizeof(Context->Buffer))) > 0) {}
  if (BytesRead < 0) {
    Context->IsConnected = false;
  }
}

void LinuxHidapiPlatform::Write(FDeviceContext *Context) {
  if (!Context->Handle) return;
  hid_device *Handle = static_cast<hid_device *>(Context->Handle);
  hid_write(Handle, Context->GetRawOutputBuffer(), sizeof(Context->Buffer));
}

void LinuxHidapiPlatform::ProcessAudioHaptic(FDeviceContext *) {}
