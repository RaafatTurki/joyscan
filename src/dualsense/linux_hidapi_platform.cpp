#include "linux_hidapi_platform.h"

#include <hidapi.h>

namespace {

constexpr unsigned short SONY_VENDOR_ID = 0x054C;
constexpr unsigned short DUALSENSE_PRODUCT_ID = 0x0CE6;
constexpr unsigned short DUALSENSE_EDGE_PRODUCT_ID = 0x0DF2;

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
  int BytesRead = hid_read(Handle, Context->Buffer, sizeof(Context->Buffer));
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
