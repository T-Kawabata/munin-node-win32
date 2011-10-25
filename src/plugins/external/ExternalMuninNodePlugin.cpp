/* This file is part of munin-node-win32
 * Copyright (C) 2006-2008 Jory Stone (jcsston@jory.info)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "StdAfx.h"
#include "ExternalMuninNodePlugin.h"

#include "../../core/Service.h"


void removeAllNL (std::string &str)
{
    std::string temp;
    for (unsigned int i = 0; i < str.length(); i++)
        if ((str[i] != '\r') && (str[i] != '\n')) temp += str[i];
    str = temp;
}

void removeAllLF (std::string &str)
{
    std::string temp;
    for (unsigned int i = 0; i < str.length(); i++)
        if ((str[i] != '\r')) temp += str[i];
    str = temp;
}



ExternalMuninNodePlugin::ExternalMuninNodePlugin(const std::string &externalPlugin, int timeout)
  : m_ExternalPlugin(externalPlugin)
{
	m_TimeoutSec = timeout;
  m_Name = Run("name");
  removeAllNL(m_Name);
  // Check that the name is valid
  if (m_Name.find(" ") != m_Name.npos) {
	  _Module.LogError("Failed to load External plugin: %s", m_Name.c_str());
	  m_Name = "";
  }
}

ExternalMuninNodePlugin::~ExternalMuninNodePlugin()
{

}

int ExternalMuninNodePlugin::GetConfig(char *buffer, int len) 
{
  std::string output = Run("config");
  if (output.length() > 0) {
    strncpy(buffer, output.c_str(), len);
    return 0;
  } 
  return -1;
}

int ExternalMuninNodePlugin::GetValues(char *buffer, int len) 
{
  std::string output = Run("");
  if (output.length() > 0) {
	  removeAllLF(output);
	  strncpy(buffer, output.c_str(), len);
    return 0;
  } 
  return -1;
}

std::string ExternalMuninNodePlugin::Run(const char *command)
{
  // Build the command-line
  char cmdLine[MAX_PATH];
  _snprintf(cmdLine, MAX_PATH, "\"%s\" %s", m_ExternalPlugin.c_str(), command);

  PluginPipe pipe;
  if (pipe.Execute(A2TConvert(cmdLine).c_str()) == CPEXEC_OK) {
	  if (m_TimeoutSec == 0) {
			// Wait for the command to complete
			while (pipe.IsChildRunning())
			  Sleep(100); // don't do a very tight loop -- that reduces CPU usage
			return T2AConvert(pipe.GetOutput());
	  } else {
		  long loops = m_TimeoutSec * 10;
		  while (pipe.IsChildRunning() && loops > 0) {
			  Sleep(100); // don't do a very tight loop -- that reduces CPU usage
			  loops--;
		  }

		  if (pipe.IsChildRunning()) {
			  _Module.LogError("External plugin timeout: %s", m_Name.c_str());
			  pipe.Break();
			  return "";
		  }

		  return T2AConvert(pipe.GetOutput());
	  }
  }
  // Command failed, empty string
  return "";
}

ExternalMuninNodePlugin::PluginPipe::PluginPipe()
  : CConsolePipe(CPF_NOCONVERTOEM|CPF_CAPTURESTDOUT|CPF_NOAUTODELETE) 
{

}

ExternalMuninNodePlugin::PluginPipe::~PluginPipe()
{

}

void ExternalMuninNodePlugin::PluginPipe::OnReceivedOutput(LPCTSTR pszText) 
{
  if (pszText != NULL) {
    JCAutoLockCritSec lock(&m_BufferCritSec);
    m_Buffer += pszText;
  }
}

TString ExternalMuninNodePlugin::PluginPipe::GetOutput() 
{
  JCAutoLockCritSec lock(&m_BufferCritSec);
  return m_Buffer;
}
