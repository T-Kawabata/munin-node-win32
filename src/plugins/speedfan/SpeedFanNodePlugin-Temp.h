#pragma once
#include "../../core/MuninNodePlugin.h"
#include "SpeedFanNodePlugin.h"

class SpeedFanNodePluginTemp : public MuninNodePlugin
{
public:
  SpeedFanNodePluginTemp(SpeedFanNodePlugin* pSpeedFan);
  virtual ~SpeedFanNodePluginTemp();

  virtual const char *GetName() { return "speedfan2"; }; 
  virtual int GetConfig(char *buffer, int len);
  virtual int GetValues(char *buffer, int len);
  virtual bool IsLoaded() { return true; };

private:
  SpeedFanNodePlugin* m_Speedfan;
};
