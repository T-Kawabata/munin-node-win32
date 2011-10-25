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
#include "SMARTMuninNodePlugin.h"

SMARTMuninNodePlugin::SMARTMuninNodePlugin(CSmartReader* reader ,UCHAR ucDriveIndex)
{
    int printCount;
	
	m_pSmartReader = reader;
	m_ucDriveIndex = ucDriveIndex;

	m_pSmartReader->AddRef();
	m_pSmartReader->UpdateSMART();

	_snprintf(m_sModel ,sizeof(m_sModel) ,"%s" ,(PCHAR)m_pSmartReader->m_stDrivesInfo[m_ucDriveIndex].m_stInfo.sModelNumber);

	PCHAR buffer = m_sDriveMap;
	int   len    = sizeof(m_sDriveMap);

	for(int i=0;i < 32 ;i++)
	{
		if(m_pSmartReader->m_stDiskExtents[i].NumberOfDiskExtents > 0 && m_pSmartReader->m_stDiskExtents[i].Extents[0].DiskNumber == ucDriveIndex)
		{
			printCount = _snprintf(buffer ,len ,"%c: " ,'A' + i);

			len    -= printCount;
			buffer += printCount;
		}
	}

	_snprintf(m_sName ,sizeof(m_sName),"smart%d" ,m_ucDriveIndex);
}

SMARTMuninNodePlugin::~SMARTMuninNodePlugin() 
{
	if(	m_pSmartReader->RemoveRef() == 0)
	{
		delete m_pSmartReader;
	}
}

int SMARTMuninNodePlugin::GetConfig(char *buffer, int len) 
{
	int printCount;

	printCount = _snprintf(buffer, len, "graph_title S.M.A.R.T values for %s ( %s)\n" ,m_sModel,m_sDriveMap);
	len -= printCount;
	buffer += printCount;

	printCount = _snprintf(buffer, len, "graph_args --base 1000 --lower-limit 0\n"
										"graph_vlabel Attribute S.M.A.R.T value\n"
										"graph_category disk\n"
										"graph_info This graph shows the value of S.M.A.R.T attributes of drive.\n");
	len -= printCount;
	buffer += printCount;

	JCAutoLockCritSec lock(&m_pSmartReader->m_BlockCritSec);
	m_pSmartReader->UpdateSMART();

    int rows = 0;
    ST_DRIVE_INFO *pDriveInfo = m_pSmartReader->GetDriveInfo(m_ucDriveIndex);
    if (pDriveInfo != NULL)
	{
		ST_SMART_INFO *pSmartInfo;

		pSmartInfo = m_pSmartReader->GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMART_ATTRIB_START_REALLOCATION_SECTOR_COUNT);
		if (pSmartInfo != NULL)
		{
            printCount = _snprintf(buffer, len, "smart_05.label Reallocated Sectors Count\n"
				                                "smart_05.draw LINE2\n"
 												"smart_05.warning 040:\n"
 												"smart_05.critical 036:\n");
// 												"smart_05.critical %d:\n" ,pSmartInfo->m_dwThreshold);
			len       -= printCount;
			buffer    += printCount;
			rows++;
		}

		pSmartInfo = m_pSmartReader->GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMART_ATTRIB_TEMPERATURE);
		if (pSmartInfo != NULL)
		{
            printCount = _snprintf(buffer, len, "smart_C2.label Temperature\n"
				                                "smart_C2.draw LINE2\n");
			len       -= printCount;
			buffer    += printCount;
			rows++;
		}

		pSmartInfo = m_pSmartReader->GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMART_ATTRIB_CURRENT_PENDING_SECTOR_COUNT);
		if (pSmartInfo != NULL)
		{
            printCount = _snprintf(buffer, len, "smart_C5.label Current Pending Sector Count\n"
				                                "smart_C5.draw LINE2\n"
 												"smart_C5.warning 010:\n"
 												"smart_C5.critical 000:\n");
// 												"smart_C5.critical %d:\n" ,pSmartInfo->m_dwThreshold);
			len       -= printCount;
			buffer    += printCount;
			rows++;
		}

		pSmartInfo = m_pSmartReader->GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMART_ATTRIB_UNCORRECTABLE_SECTOR_COUNT);
		if (pSmartInfo != NULL)
		{
            printCount = _snprintf(buffer, len, "smart_C6.label Uncorrectable Sector Count\n"
				                                "smart_C6.draw LINE2\n"
 												"smart_C6.warning 010:\n"
 												"smart_C6.critical 000:\n");
// 												"smart_C6.critical %d:\n" ,pSmartInfo->m_dwThreshold);
			len       -= printCount;
			buffer    += printCount;
			rows++;
		}
		strncat(buffer, ".\n", len);
	}

	if(rows > 0)
	{
		return 0;
	}

	return -1;
}

int SMARTMuninNodePlugin::GetValues(char *buffer, int len) 
{ 
	int printCount;

	JCAutoLockCritSec lock(&m_pSmartReader->m_BlockCritSec);
	m_pSmartReader->UpdateSMART();

    int rows = 0;
    ST_DRIVE_INFO *pDriveInfo = m_pSmartReader->GetDriveInfo(m_ucDriveIndex);
    if (pDriveInfo != NULL)
	{
		ST_SMART_INFO *pSmartInfo;
		pSmartInfo = m_pSmartReader->GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMART_ATTRIB_START_REALLOCATION_SECTOR_COUNT);
		if (pSmartInfo != NULL)
		{
			printCount = _snprintf(buffer, len, "smart_05.value %d\n" ,pSmartInfo->m_ucValue);
			len       -= printCount;
			buffer    += printCount;
			rows++;
		}

		pSmartInfo = m_pSmartReader->GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMART_ATTRIB_TEMPERATURE);
		if (pSmartInfo != NULL)
		{
			printCount = _snprintf(buffer, len, "smart_C2.value %d\n" ,pSmartInfo->m_bRawValue[0]);
			len       -= printCount;
			buffer    += printCount;
			rows++;
		}

		pSmartInfo = m_pSmartReader->GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMART_ATTRIB_CURRENT_PENDING_SECTOR_COUNT);
		if (pSmartInfo != NULL)
		{
			printCount = _snprintf(buffer, len, "smart_C5.value %d\n" ,pSmartInfo->m_ucValue);
			len       -= printCount;
			buffer    += printCount;
			rows++;
		}

		pSmartInfo = m_pSmartReader->GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMART_ATTRIB_UNCORRECTABLE_SECTOR_COUNT);
		if (pSmartInfo != NULL)
		{
			printCount = _snprintf(buffer, len, "smart_C6.value %d\n" ,pSmartInfo->m_ucValue);
			len       -= printCount;
			buffer    += printCount;
			rows++;
		}
		strncat(buffer, ".\n", len);
	}

	if(rows > 0)
	{
		return 0;
	}

	return -1;
}
