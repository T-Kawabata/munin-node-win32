/* This file is part of munin-node-win32
 * Copyright (C) 2006-2007 Jory Stone (jcsston@jory.info)
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
#include "SpeedFanNodePlugin-Temp.h"

SpeedFanNodePluginTemp::SpeedFanNodePluginTemp(SpeedFanNodePlugin* pSpeedfan) 
{
  m_Speedfan = pSpeedfan;
  m_Speedfan->AddRef();
}

SpeedFanNodePluginTemp::~SpeedFanNodePluginTemp()
{
	if(m_Speedfan->RemoveRef() == 0)
	{
		delete m_Speedfan;
	}
}

int SpeedFanNodePluginTemp::GetConfig(char *buffer, int len) 
{
  int printCount;
  printCount = _snprintf(buffer, len, 
    "graph_title System temperature\n"
    "graph_args --base 1000 -l 0\n"
    "graph_vlabel temp in C\n"
    "graph_category sensors\n"
    "graph_info This graph shows the temperature in degrees Celsius.\n");
  len -= printCount;
  buffer += printCount;

  return m_Speedfan->GetTempConfig(buffer ,len);
}

int SpeedFanNodePluginTemp::GetValues(char *buffer, int len) 
{
	return m_Speedfan->GetTempValues(buffer ,len);
}
