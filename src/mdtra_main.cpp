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

// Purpose:
//	Create QApplication and main window
//	Implement OS-dependent "main" entry points

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_cpuid.h"
#include "mdtra_cuda.h"
#include <QtGui/QApplication>
#include <QtOpenGL/QtOpenGL>

#if defined(_DEBUG)
#define MDTRA_NO_SPLASH
#endif

QApplication *g_pApp = NULL;
QSplashScreen *g_pSplash = NULL;

int lcMain( int argc, char *argv[] )
{
	CheckCPU();
	MDTRA_CUDA_CheckSupport();

	QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);

	QLocale usLocale(QLocale::English, QLocale::UnitedStates);
	QLocale::setDefault(usLocale);

#if defined(MDTRA_RUSSIAN)
	QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
	QTextCodec::setCodecForLocale(codec);
#endif

	g_pApp = new QApplication( argc, argv );

#if !defined(MDTRA_NO_SPLASH)
	g_pSplash = new QSplashScreen(QPixmap(":/jpg/splash.jpg"));
	g_pSplash->setWindowFlags( g_pSplash->windowFlags() | Qt::WindowStaysOnTopHint );
	g_pSplash->show();
	g_pApp->processEvents();
#endif

	MDTRA_MainWindow mdtraMainWindow;
    mdtraMainWindow.show();
	mdtraMainWindow.raise();

#if !defined(MDTRA_NO_SPLASH)
	g_pSplash->finish( &mdtraMainWindow );
	delete g_pSplash;
	g_pSplash = NULL;
#endif

	//Load file in command line
	if (argc > 1 && strstr( argv[argc-1], "." )) {
		mdtraMainWindow.loadFile( argv[argc-1] );
	}

	return g_pApp->exec();
}

#if defined(WIN32)

#define MAX_ARGV	32

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	int argc;
	char *argv[MAX_ARGV];
	char filename[MAX_PATH];

	//Extract argc and argv
	argc = 1;
	memset(argv, 0, sizeof(argv));

	bool in_quote = false;

	while (*lpCmdLine && (argc < MAX_ARGV)) {
		while (*lpCmdLine && ((byte)(*lpCmdLine) <= 32))
			lpCmdLine++;

		if (*lpCmdLine) {
			if (*lpCmdLine == '"') {
				in_quote = !in_quote;
				lpCmdLine++;
			}

			argv[argc] = lpCmdLine;
			argc++;

			while (*lpCmdLine && (((byte)(*lpCmdLine) > 32) || in_quote)) {
				if (in_quote && (*lpCmdLine == '"')) {
					in_quote = !in_quote;
					break;
				}
				lpCmdLine++;
			}

			if (*lpCmdLine)	{
				*lpCmdLine = 0;
				lpCmdLine++;
			}
		}
	}

    GetModuleFileName(NULL, filename, MAX_PATH-1);
    argv[0] = filename;

	return lcMain( argc, argv );
}

#else //WIN32

int main( int argc, char *argv[] )
{
	return lcMain( argc, argv );
}

#endif //WIN32