/****************************************************************************
**  
** Copyright (C) 2007~2017 Colin Willcocks. 
** Copyright (C) 2005~2007 Uco Mesdag.
** All rights reserved.
** This file is part of "GT-8 Fx FloorBoard".
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#include "stompbox_fv.h"

stompbox_fv::stompbox_fv(QWidget *parent)
    : stompBox(parent)
{
	/* VOLUME */
	setImage(":/images/fv.png");
	setLSB("15", "00");
	editDetails()->patchPos(1266, 38, "0E", "00");
	//editDetails()->patchPos(1442, 66, "13", "00");
	setEditPages();
}

void stompbox_fv::updateSignal()
{
	//updateButton("15", "00", "00");
}

void stompbox_fv::setEditPages()
{
  
  editDetails()->page()->newGroupBox("Foot Volume");
	editDetails()->page()->addComboBox(0, 0, 1, 1, "0E", "00", "08", "bottom", Qt::AlignTop);
	editDetails()->page()->addKnob(0, 1, 1, 1, "0E", "00", "07");
	//editDetails()->page()->addComboBox(0, 2, 1, 1, "0E", "00", "09", "bottom", Qt::AlignTop);
	editDetails()->page()->addGroupBox(0, 0, 1, 1);
	
	editDetails()->page()->newGroupBox("EXP Pedal");
	editDetails()->page()->addComboBox(0, 0, 1, 1, "15", "00", "00", "bottom", Qt::AlignTop);   // on/off
	editDetails()->page()->addKnob(0, 1, 1, 1, "15", "00", "01");    //min
	editDetails()->page()->addKnob(0, 2, 1, 1, "15", "00", "02");      // max
	editDetails()->page()->addGroupBox(1, 0, 1, 1);
	
	editDetails()->page()->newGroupBox("EXP Pedal Switch");
	editDetails()->page()->addComboBox(0, 0, 1, 1, "14", "00", "00", "bottom", Qt::AlignTop);   // on/off
	editDetails()->page()->addComboBox(0, 2, 1, 1, "14", "00", "01", "bottom", Qt::AlignTop);  //function
	editDetails()->page()->addGroupBox(0, 1, 1, 1);
	
  editDetails()->page()->newGroupBox("CTL Pedal");
	editDetails()->page()->addComboBox(0, 0, 1, 1, "13", "00", "00", "bottom", Qt::AlignTop);   // on/off
	editDetails()->page()->addComboBox(0, 2, 1, 1, "13", "00", "01", "bottom", Qt::AlignTop);  //function
	editDetails()->page()->addGroupBox(1, 1, 1, 1);
	
	editDetails()->page()->newGroupBox("Master");
	editDetails()->page()->addKnob(0, 0, 1, 1, "0E", "00", "04");    //master level
	editDetails()->page()->addKnob(0, 1, 1, 1, "0E", "00", "05");      // master BPM
	editDetails()->page()->addGroupBox(2, 0, 1, 1);
	
	editDetails()->page()->newGroupBox("Amp CTL Jack");
	editDetails()->page()->addComboBox(0, 0, 1, 1, "0F", "00", "00", "bottom", Qt::AlignTop);   // on/off
	editDetails()->page()->addGroupBox(2, 1, 1, 1);

	editDetails()->addPage();
}
