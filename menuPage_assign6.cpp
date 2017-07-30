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

#include "menuPage_assign6.h"

menuPage_assign6::menuPage_assign6(QWidget *parent)
    : menuPage(parent)
{
  setLSB("1B", "00");
  editDetails()->patchPos(1834, 34, "1B", "00");    //192 sysx file offset and data length of chorus parameters x2,
        setEditPages();
}

void menuPage_assign6::updateSignal()
{

}

void menuPage_assign6::setEditPages()
{
    editDetails()->page()->newGroupBox(tr("Assign 6"));
    editDetails()->page()->newGroupBox(tr(""));
    editDetails()->page()->addSwitch(0, 0, 1, 1, "1B", "00", "00", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->newGroupBox("Assign 6 Target");
    editDetails()->page()->addTarget(0, 0, 1, 3, "1B", "00", "02", "target", "04", "06"); // Target
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->newGroupBox("Assign 6 Source");
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "1B", "00", "08");                    // Source
    editDetails()->page()->addStackControl();
    editDetails()->page()->addComboBox(0, 1, 1, 1, "1B", "00", "09");                    // Source Mode
    editDetails()->page()->addRange(0, 2, 1, 2, "1B", "00", "0A", "Structure");         // min/max range widget
    editDetails()->page()->addGroupBox(1, 1, 1, 1);
    editDetails()->page()->insertStackField(0, 2, 1, 1, 1);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->addPage();

    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Assign 6 Internal Pedal");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "1B", "00", "0C");                    // Internal Pedal Trigger
    editDetails()->page()->addKnob(0, 1, 1, 1, "1B", "00", "0D");                        // Internal Pedal Time
    editDetails()->page()->addComboBox(0, 2, 1, 1, "1B", "00", "0E");                    // Internal Pedal Curve
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Assign 6 Wave Pedal");
    editDetails()->page()->addKnob(1, 0, 1, 1, "1B", "00", "0F", "normal","right", 105);// Wave Form Rate
    editDetails()->page()->addComboBox(1, 1, 1, 1, "1B", "00", "10");                    // Wave Form
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Input");
    editDetails()->page()->addKnob(0, 0, 1, 1, "1E", "00", "00", "normal","right", 85);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();
}
