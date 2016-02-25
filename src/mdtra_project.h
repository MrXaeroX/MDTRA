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
#ifndef MDTRA_PROJECT_H
#define MDTRA_PROJECT_H

#define MDTRA_PROJECT_FILE_MAGIC				0xDEFECEED
#define MDTRA_PROJECT_FILE_VERSION_OLD			108
#define MDTRA_PROJECT_FILE_VERSION				109

class MDTRA_MainWindow;
class MDTRA_PDB_File;
class QTextStream;

typedef struct stMDTRA_DataArg
{
	int atomIndex;
} MDTRA_DataArg;

typedef struct stMDTRA_Stream
{
	int				index;
	QString			name;
	QStringList		files;
	MDTRA_PDB_File* pdb;
	float			xscale;
	unsigned int	format_identifier;
	unsigned int	flags;
	char			reserved[28];
} MDTRA_Stream;

typedef struct stMDTRA_Selection
{
	QString			string;
	int				flags;
	int				size;
	int*			data;
} MDTRA_Selection;

typedef struct stMDTRA_Prog
{
	QString			sourceCode;
	byte*			byteCode;
	int				byteCodeSize;
} MDTRA_Prog;

#define DATASOURCE_USERFLAG_ANGLE_DEG	0x1
#define DATASOURCE_USERFLAG_ANGLE_RAD	0x2

typedef struct stMDTRA_DataSource
{
	int				index;
	int				streamIndex;
	QString			name;
	MDTRA_DataType	type;
	MDTRA_DataArg	arg[MAX_DATA_SOURCE_ARGS];
	MDTRA_Selection selection;
	MDTRA_Selection selection2;
	MDTRA_Prog		prog;
	QString			userdata;
	QString			temp;
	int				userFlags;
	char			reserved[28];
} MDTRA_DataSource;

#define PLOTLABEL_FLAG_SELECTED			0x1
#define PLOTLABEL_FLAG_TRANSPARENT		0x2
#define PLOTLABEL_FLAG_BORDER			0x4
#define PLOTLABEL_FLAG_CONNECTION		0x8
#define PLOTLABEL_FLAG_VERTLINE			0x10

typedef struct stMDTRA_Label
{
	QString				text;
	int					sourceNum;
	int					snapshotNum;
	int					x, y;
	int					wide, tall;
	int					flags;
} MDTRA_Label;

typedef struct stMDTRA_Result
{
	int					index;
	int					status;
	bool				buildCorrelation;
	QString				name;
	MDTRA_DataType		type;
	MDTRA_YScaleUnits	units;
	MDTRA_Layout		layout;
	QList<MDTRA_DSRef>	sourceList;
	QList<MDTRA_Label>	labelList;
	char				reserved[32];
} MDTRA_Result;

typedef struct stMDTRA_StreamWorkResult
{
	const MDTRA_DataSource*	pDataSource;
	MDTRA_Result*			pResult;
	MDTRA_DSRef*			pDSRef;
	MDTRA_PDB_File*			pRefPDB;
	void*					pProgInterpreter;
	bool					threadHasZero[MDTRA_MAX_THREADS];
	bool					threadAllPositive[MDTRA_MAX_THREADS];
	float					threadStat[MDTRA_TSP_MAX][MDTRA_MAX_THREADS];
	bool					threadStatInit[MDTRA_MAX_THREADS];
} MDTRA_StreamWorkResult;

typedef struct stMDTRA_StreamWork
{
	const MDTRA_Stream*		pStream;
	int						workBase;
	int						workCount;
	MDTRA_PDB_File*			tempPDB[MDTRA_MAX_THREADS];
	MDTRA_PDB_File*			averagePDB;
	QList<MDTRA_StreamWorkResult> pResults;
} MDTRA_StreamWork;

class MDTRA_Project
{
public:
    MDTRA_Project( MDTRA_MainWindow *pMainWindow );
	~MDTRA_Project();

	bool loadFile( const QString &projectPath, QDataStream *stream );
	bool saveFile( const QString &projectPath, QDataStream *stream );

	void clear( void );
	bool build( bool rebuildAll );

	bool checkUniqueStreamName( const QString &name );
	int getStreamCount( void ) const { return m_StreamList.count(); }
	int getStreamCountByFormatIdentifier( unsigned int format ) const;
	int getValidStreamCount( void ) const;
	int getValidStreamCount( int formatMask ) const;
	int registerStream( const QString &name, const QStringList &files, float xscale, unsigned int format_identifier, unsigned int flags );
	void unregisterStream( int index );
	void modifyStream( int index, const QString &name, const QStringList &files, float xscale, unsigned int format_identifier, unsigned int flags );
	MDTRA_Stream *fetchStreamByIndex( int streamIndex ) const;
	MDTRA_Stream *fetchStream( int index ) const;

	bool checkUniqueDataSourceName( const QString &name );
	int getDataSourceCount( void ) const { return m_DataSourceList.count(); }
	int getDataSourceCountByStreamIndex( int index );
	void invalidateDataSourceByStreamIndex( int index );
	void invalidateDataSourceByIndex( int index );
	int registerDataSource( const QString &name, int streamIndex, MDTRA_DataType dataType, const MDTRA_DataArg *pdataArgs, const MDTRA_Selection &selection1, const MDTRA_Selection &selection2, const MDTRA_Prog &prog, const QString& userdata, int userFlags, bool updateGUI );
	void unregisterDataSource( int index );
	void modifyDataSource( int index, const QString &name, int streamIndex, MDTRA_DataType dataType, const MDTRA_DataArg *pdataArgs, const MDTRA_Selection &selection1, const MDTRA_Selection &selection2, const MDTRA_Prog &prog, const QString& userdata, int userFlags );
	MDTRA_DataSource *fetchDataSourceByIndex( int index );
	MDTRA_DataSource *fetchDataSource( int index );

	bool checkUniqueResultName( const QString &name );
	int getResultCount( void ) const { return m_ResultList.count(); }
	int getResultCountByType( MDTRA_DataType type ) const;
	int getResultDataSourceCountByStreamIndex( int index );
	int getResultDataSourceCountByDataSourceIndex( int index );
	int registerResult( const QString &name, MDTRA_DataType type, MDTRA_YScaleUnits scaleUnits, MDTRA_Layout layout, const QList<MDTRA_DSRef> &dsref, bool updateGUI );
	void unregisterResult( int index );
	void modifyResult( int index, const QString &name, MDTRA_DataType type, MDTRA_YScaleUnits scaleUnits, MDTRA_Layout layout, const QList<MDTRA_DSRef> &dsref );
	MDTRA_Result *fetchResultByIndex( int index );
	MDTRA_Result *fetchResult( int index );
	void invalidateResult( int index, bool updateList );
	void exportResult( int index, const QString &fileSuffix, QTextStream *stream, int dataFilter );
	void exportStats( int index, const QString &fileSuffix, QTextStream *stream );
	bool addResultLabel( int index, const MDTRA_Label *pLabel );
	bool editResultLabel( int index, int labelIndex, const MDTRA_Label *pLabel );
	bool removeResultLabel( int index, int labelIndex );

	void updateStreamList( void );
	void updateDataSourceList( void );
	void updateResultList( void );

private:
	void profileStart( void );
	void profileEnd( void );

	float sampleData( const float *pDataPtr, int iSample, int iMaxSample, int iFilterSize, MDTRA_YScaleUnits ysu );
	void exportResultToTXT( const MDTRA_Result *pResult, QTextStream *stream, int dataFilter );
	void exportResultToCSV( const MDTRA_Result *pResult, QTextStream *stream, int dataFilter );
	void exportStatsToTXT( const MDTRA_Result *pResult, QTextStream *stream );
	void exportStatsToCSV( const MDTRA_Result *pResult, QTextStream *stream );

private:
	MDTRA_MainWindow* m_pMainWindow;
	QList<MDTRA_Stream> m_StreamList;
	QList<MDTRA_DataSource> m_DataSourceList;
	QList<MDTRA_Result> m_ResultList;
	dword m_profStart;
};

#endif //MDTRA_PROJECT_H