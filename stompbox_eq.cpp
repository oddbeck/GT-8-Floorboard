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

#include "stompbox_eq.h"                 

stompbox_eq::stompbox_eq(QWidget *parent)
    : stompBox(parent)
{
	/* EQ */
	setImage(":/images/eq.png");
	setLSB("08", "00");
	setSlider1("08", "00", "03");
	setSlider2("08", "00", "06");
	setSlider3("08", "00", "09");
	setSlider4("08", "00", "0A");
	setSlider5("08", "00", "0C");
	setButton("08", "00", "00");
	editDetails()->patchPos(572, 26, "08", "00");
	setEditPages();
};

void stompbox_eq::updateSignal()
{
	updateSlider1("08", "00", "03");
	updateSlider2("08", "00", "06");
	updateSlider3("08", "00", "09");
	updateSlider4("08", "00", "0A");
	updateSlider5("08", "00", "0C");
	updateButton("08", "00", "00");
};

void stompbox_eq::setEditPages()
{
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
	editDetails()->page()->addSwitch(0, 0, 1, 1, "08", "00", "00", "middle", Qt::AlignCenter);
	editDetails()->page()->addGroupBox(0, 0, 1, 1);

	editDetails()->page()->newGroupBox("Equalizer");
	editDetails()->page()->newGroupBox("Low");
	editDetails()->page()->addKnob(0, 0, 1, 1, "08", "00", "02");
	editDetails()->page()->addKnob(0, 1, 1, 1, "08", "00", "03");
	editDetails()->page()->addGroupBox(0, 0, 1, 1);

	editDetails()->page()->newGroupBox("Low-Middle");
	editDetails()->page()->addKnob(0, 0, 1, 1, "08", "00", "06");
	editDetails()->page()->addKnob(1, 0, 1, 1, "08", "00", "04");
	editDetails()->page()->addKnob(2, 0, 1, 1, "08", "00", "05");
	editDetails()->page()->addGroupBox(0, 1, 1, 1);

	editDetails()->page()->newGroupBox("High-Middle");
	editDetails()->page()->addKnob(0, 0, 1, 1, "08", "00", "09");
	editDetails()->page()->addKnob(1, 0, 1, 1, "08", "00", "07");
	editDetails()->page()->addKnob(2, 0, 1, 1, "08", "00", "08");
	editDetails()->page()->addGroupBox(0, 2, 1, 1);

	editDetails()->page()->newGroupBox("High");
	editDetails()->page()->addKnob(0, 0, 1, 1, "08", "00", "0A");
	editDetails()->page()->addKnob(0, 1, 1, 1, "08", "00", "0B");
	editDetails()->page()->addGroupBox(0, 3, 1, 1);
	editDetails()->page()->addGroupBox(0, 1, 1, 1);

	editDetails()->page()->newGroupBox("Level");
	editDetails()->page()->addKnob(0, 0, 1, 1, "08", "00", "0C");
	editDetails()->page()->addGroupBox(0, 2, 1, 1);

	editDetails()->addPage();	
};
