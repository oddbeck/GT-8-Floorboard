#############################################################################
##
## Copyright (C) 2007~2017 Colin Willcocks.
## Copyright (C) 2005~2007 Uco Mesdag. 
## All rights reserved.
##
## This file is part of "GT-8 Fx FloorBoard".
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##
#############################################################################

TEMPLATE = app
CONFIG += static
CONFIG += embed_manifest_exe
#CONFIG += debug
CONFIG += c++11
TARGET = "GT-8FxFloorBoard"
DESTDIR = ./packager
	OBJECTS_DIR += release
	UI_DIR += ./generatedfiles
	MOC_DIR += ./generatedfiles/release
	INCLUDEPATH += ./generatedfiles \
    ./generatedfiles/release \
    .
TRANSLATIONS = language_fr.ts \
               language_dk.ts \
               language_ge.ts \
               language_ch.ts
               
CODECFORTR = UTF-8

DEPENDPATH += .
QT += xml
QT += widgets
QT += printsupport

#Platform dependent file(s)
win32 {
	exists("C:\Qt\gt-6b\windows\WinMM.Lib") {		# <-- Change the path to WinMM.Lib here!
                LIBS +=  C:\Qt\gt-6b\windows\WinMM.Lib		# <-- Change the path here also!
    } else { 
            LIBS += .\WinMM.Lib
            message("WINMM.LIB IS REQUIRED. IF NOT INSTALLED THEN")
            message("PLEASE DOWNLOAD AND INSTALL THE LATEST PLATFORM SDK")
            message("FROM MICROSOFT.COM AND AFTER INSTALLATION")
            message("CHANGE THE CORRECT (DOS) PATH TO WinMM.lib")
            message("IN THIS (GT-8FxFloorBoard.pro) FILE WHERE INDICATED")
        }
	 HEADERS += 
	 SOURCES += ./windows/RtMidi.cpp                        
	 INCLUDEPATH += ./windows
	message(Including Windows specific headers and sources...)
}
linux-g++-64 {
        LIBS += -lasound
        message("ALSA LIBRARIES SHOULD BE INSTALLED or ERROR will Occur")
        message("Please install the ALSA Audio System packages if not present")

         HEADERS +=
         INCLUDEPATH += ./linux
        message(Including Linux specific headers and sources...)
}
linux-g++ {
        LIBS += -lasound
	message("ALSA LIBRARIES SHOULD BE INSTALLED or ERROR will Occur") 
	message("Please install the ALSA Audio System packages if not present") 	
 
	 HEADERS += 
	 SOURCES += ./linux/RtMidi.cpp 
	 INCLUDEPATH += ./linux
	message(Including Linux specific headers and sources...)
}
macx {
	LIBS += -framework CoreMidi -framework CoreAudio -framework CoreFoundation
	message("X-Code LIBRARIES SHOULD BE INSTALLED or ERROR will Occur") 
	message("Please install the X-Code Audio System packages if not present") 
	 HEADERS += 
	 SOURCES += ./macosx/RtMidi.cpp 
	INCLUDEPATH += ./macosx
	ICON = GT-8FxFloorBoard.icns
	message(Including Mac OS X specific headers and sources...)
}


#Include file(s)
include(GT-8FxFloorBoard.pri)

#Windows resource file
win32:RC_FILE = GT-8FxFloorBoard.rc
