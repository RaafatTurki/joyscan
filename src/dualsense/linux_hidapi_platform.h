#pragma once

#include "GCore/Interfaces/IPlatformHardware.h"

class LinuxHidapiPlatform : public IPlatformHardware {
public:
  void Read(FDeviceContext *Context) override;
  void Write(FDeviceContext *Context) override;
  void Detect(std::vector<FDeviceContext> &Devices) override;
  bool CreateHandle(FDeviceContext *Context) override;
  void InvalidateHandle(FDeviceContext *Context) override;
  void ProcessAudioHaptic(FDeviceContext *Context) override;
};
