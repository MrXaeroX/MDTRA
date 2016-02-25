/***************************************************************************
* Copyright (C) 2011-2016 Alexander V. Popov.
* 
* This file is part of Molecular Dynamics Trajectory 
* Reader & Analyzer (MDTRA) source code.
* 
* MDTRA source code is free software; you can redistribute it and/or 
* modify it under the terms of the GNU General Public License as 
* published by the Free Software Foundation; either version 2 of 
* the License, or (at your option) any later version.
* 
* MDTRA source code is distributed in the hope that it will be 
* useful, but WITHOUT ANY WARRANTY; without even the implied 
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
* See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software 
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
***************************************************************************/
#ifndef MDTRA_SAVERESTORE_H
#define MDTRA_SAVERESTORE_H

#include <QtCore/QByteArray>
#include <QtCore/QFile>

template<typename T>
class MDTRA_SaveRestore
{
public:
	MDTRA_SaveRestore( T *pWidget ) : m_pWidget(pWidget)
	{
		assert(m_pWidget != NULL);
	}
	bool save( QFile &file )
	{
		ushort geo_size;
		ushort layout_size;
		QByteArray geo_data = m_pWidget->saveGeometry();
		QByteArray layout_data = m_pWidget->saveState();

		geo_size = geo_data.size();
		layout_size = layout_data.size();

		if (!file.putChar(geo_size & 0xFF)) return false;
		if (!file.putChar((geo_size >> 8) & 0xFF)) return false;
		if (file.write(geo_data) != geo_size) return false;

		if (!file.putChar(layout_size & 0xFF)) return false;
		if (!file.putChar((layout_size >> 8) & 0xFF)) return false;
		if (file.write(layout_data) != layout_size) return false;

		return true;
	}
	bool restore( QFile &file )
	{
		char c1, c2;
		ushort geo_size;
		ushort layout_size;
		QByteArray geo_data;
		QByteArray layout_data;

		if (!file.getChar( &c1 )) return false;
		if (!file.getChar( &c2 )) return false;
		geo_size = ((uchar)c2 << 8) | (uchar)c1;
		if (geo_size <= 0) return false;
        geo_data = file.read(geo_size);
        if (geo_data.size() != geo_size) return false;

		if (!file.getChar( &c1 )) return false;
		if (!file.getChar( &c2 )) return false;
		layout_size = ((uchar)c2 << 8) | (uchar)c1;
		if (layout_size <= 0) return false;
        layout_data = file.read(layout_size);
        if (layout_data.size() != layout_size) return false;
		
		if (!m_pWidget->restoreGeometry(geo_data)) return false;
		if (!m_pWidget->restoreState(layout_data)) return false;

		return true;
	}
private:
	T *m_pWidget;
};

#endif //MDTRA_SAVERESTORE_H