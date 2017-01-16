/***************************************************************************
* Copyright (C) 2011-2017 Alexander V. Popov.
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
#ifndef MDTRA_CUSTOMTABLEWIDGETITEMS_H
#define MDTRA_CUSTOMTABLEWIDGETITEMS_H

#include <QtCore/QVariant>
#include <QtGui/QTableWidget>
#include <QtGui/QPushButton>

class QTableWidgetItem_SortByUserRole_i: public QObject, public QTableWidgetItem
{
	Q_OBJECT
public:
	QTableWidgetItem_SortByUserRole_i() : QTableWidgetItem() {}
	QTableWidgetItem_SortByUserRole_i( QString qs ) : QTableWidgetItem( qs ) {}
	bool operator< ( const QTableWidgetItem &other ) const
	{
		if( this->data( Qt::UserRole ).toInt() < other.data( Qt::UserRole ).toInt() )
			return true;
		else
			return false;
	}
};

class QTableWidgetItem_SortByUserRole_f: public QObject, public QTableWidgetItem
{
	Q_OBJECT
public:
	QTableWidgetItem_SortByUserRole_f() : QTableWidgetItem() {}
	QTableWidgetItem_SortByUserRole_f( QString qs ) : QTableWidgetItem( qs ) {}
	bool operator< ( const QTableWidgetItem &other ) const
	{
		if( this->data( Qt::UserRole ).toFloat() < other.data( Qt::UserRole ).toFloat() )
			return true;
		else
			return false;
	}
};

class QPushButtonWithTag: public QPushButton
{
	Q_OBJECT
public:
	QPushButtonWithTag() : QPushButton() {}
	QPushButtonWithTag( int tag, const QString& title, QWidget* parent ) : QPushButton( title, parent ) 
	{
		m_iTag = tag;
	}
	int getTag( void ) const { return m_iTag; }
private:
	int m_iTag;
};

#endif //MDTRA_CUSTOMTABLEWIDGETITEMS_H