#pragma once
#include "../../core/MuninNodePlugin.h"
#include "SmartReader.h"

class SMARTMuninNodePlugin : public MuninNodePlugin {  
public:
  SMARTMuninNodePlugin(CSmartReader* reader ,UCHAR ucDriveIndex);
  virtual ~SMARTMuninNodePlugin();

  virtual const char *GetName() { return m_sName; };
  virtual int GetConfig(char *buffer, int len);
  virtual int GetValues(char *buffer, int len);
  virtual bool IsLoaded() { return true; };
private:
  CSmartReader* m_pSmartReader;
  UCHAR         m_ucDriveIndex;

  CHAR          m_sModel[40];
  CHAR          m_sDriveMap[256];
  CHAR          m_sName[40];
};
