#pragma once
#include "../../core/MuninNodePlugin.h"
#include "SpeedFanNodePlugin.h"

class SpeedFanNodePluginFan : public MuninNodePlugin
{
public:
  SpeedFanNodePluginFan(SpeedFanNodePlugin* pSpeedfan);
  virtual ~SpeedFanNodePluginFan();

  virtual const char *GetName() { return "speedfan1"; }; 
  virtual int GetConfig(char *buffer, int len);
  virtual int GetValues(char *buffer, int len);
  virtual bool IsLoaded() { return true; };
private:
  SpeedFanNodePlugin* m_Speedfan;
};
