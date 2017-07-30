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

#include "menuPage_midi.h"

menuPage_midi::menuPage_midi(QWidget *parent)
    : menuPage(parent)
{
    setImage(":/images/system_pushbutton.png");
    setEditPages();
}

void menuPage_midi::updateSignal()
{

}

void menuPage_midi::setEditPages()
{
    editDetails()->page()->addLabel(0, 1, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");
    //editDetails()->page()->newGroupBox("System Preamp 1");

    editDetails()->page()->newGroupBox("SYSTEM PreAmp");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "00", "System01");

    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "10", "00", "03", "System01");
    editDetails()->page()->addStackControl();

    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(2, 0, 1, 1, "10", "00", "02", "System01");
    editDetails()->page()->addStackControl();

    editDetails()->page()->addComboBox(3, 0, 1, 1, "03", "00", "04", "System01"); // preamp mode  (system/patch)
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    // TYPE SETTINGS
    editDetails()->page()->insertStackField(1, 2, 0, 1, 1);
    //    editDetails()->page()->insertStackField(2, 1, 0, 1, 1);

    editDetails()->page()->newStackField(1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1, Qt::AlignCenter);
    editDetails()->page()->newGroupBox("Ch.Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "05", "System01");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1, Qt::AlignCenter);
    editDetails()->page()->newGroupBox("Ch.Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "05", "System01");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1, Qt::AlignCenter);
    editDetails()->page()->newGroupBox("Dynamic");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "04", "System01");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    // CHANNEL A
    editDetails()->page()->insertStackField(0, 1, 1, 2, 1);
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Channel A");

    editDetails()->page()->newGroupBox("Pre Amp A");
    editDetails()->page()->newStackControl(2);
    editDetails()->page()->addComboBox(0, 0, 1, 5, "10", "00", "06", "System01");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addComboBox(0, 5, 1, 1, "10", "00", "0E", "System01");

    editDetails()->page()->addKnob(2, 0, 1, 1, "10", "00", "10", "System01");
    editDetails()->page()->addKnob(2, 1, 1, 1, "10", "00", "08", "System01");
    editDetails()->page()->addKnob(2, 2, 1, 1, "10", "00", "09", "System01");
    editDetails()->page()->addKnob(2, 3, 1, 1, "10", "00", "0A", "System01");
    editDetails()->page()->addKnob(2, 4, 1, 1, "10", "00", "0B", "System01");

    editDetails()->page()->insertStackField(2, 2, 5, 1, 1);

    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "0C", "System01");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "16", "System01");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Speaker/Cabinet A");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "10", "00", "11", "System01");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "10", "00", "13", "System01");
    editDetails()->page()->addComboBox(1, 0, 1, 2, "10", "00", "12", "System01");
    editDetails()->page()->addKnob(0, 2, 2, 1, "10", "00", "14", "System01");
    editDetails()->page()->addKnob(0, 3, 2, 1, "10", "00", "15", "System01");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Solo");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0F", "System01");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "10", "System01");
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    // CHANNEL B
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Channel B");

    editDetails()->page()->newGroupBox("Pre Amp B");
    editDetails()->page()->newStackControl(3);
    editDetails()->page()->addComboBox(0, 0, 1, 5, "10", "00", "17", "System01");
    editDetails()->page()->addStackControl();

    editDetails()->page()->addComboBox(0, 5, 1, 1, "10", "00", "1F", "System01");

    editDetails()->page()->addKnob(2, 0, 1, 1, "10", "00", "18", "System01");
    editDetails()->page()->addKnob(2, 1, 1, 1, "10", "00", "19", "System01");
    editDetails()->page()->addKnob(2, 2, 1, 1, "10", "00", "1A", "System01");
    editDetails()->page()->addKnob(2, 3, 1, 1, "10", "00", "1B", "System01");
    editDetails()->page()->addKnob(2, 4, 1, 1, "10", "00", "1C", "System01");

    editDetails()->page()->insertStackField(3, 2, 5, 1, 1);

    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "1D", "System01");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "27", "System01");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Speaker/Cabinet B");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "10", "00", "22", "System01");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "10", "00", "24", "System01");
    editDetails()->page()->addComboBox(1, 0, 1, 2, "10", "00", "23", "System01");
    editDetails()->page()->addKnob(0, 2, 2, 1, "10", "00", "25", "System01");
    editDetails()->page()->addKnob(0, 3, 2, 1, "10", "00", "26", "System01");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Solo");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "20", "System01");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "21", "System01");
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "0D", "System01");
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "1E", "System01");
    editDetails()->page()->addStackField();

    editDetails()->addPage("21", "00", "02", "00");


    editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");
    editDetails()->page()->newGroupBox("Custom Preamp 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "00", "System03"); // preamp type
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "01", "System03"); // bottom
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "02", "System03"); // edge
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "00", "03", "System03"); // bass freq
    editDetails()->page()->addKnob(0, 4, 1, 1, "02", "00", "04", "System03"); // treb freq
    editDetails()->page()->addKnob(0, 5, 1, 1, "02", "00", "05", "System03"); // pre low
    editDetails()->page()->addKnob(0, 6, 1, 1, "02", "00", "06", "System03"); // pre high
    editDetails()->page()->addGroupBox(1, 0, 1, 2);

    editDetails()->page()->newGroupBox("Custom Preamp 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "01", "00", "System03"); // preamp type
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "01", "01", "System03"); // bottom
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "01", "02", "System03"); // edge
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "01", "03", "System03"); // bass freq
    editDetails()->page()->addKnob(0, 4, 1, 1, "02", "01", "04", "System03"); // treb freq
    editDetails()->page()->addKnob(0, 5, 1, 1, "02", "01", "05", "System03"); // pre low
    editDetails()->page()->addKnob(0, 6, 1, 1, "02", "01", "06", "System03"); // pre high
    editDetails()->page()->addGroupBox(2, 0, 1, 2);

    editDetails()->page()->newGroupBox("Custom Preamp 3");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "02", "00", "System03"); // preamp type
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "02", "01", "System03"); // bottom
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "02", "02", "System03"); // edge
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "02", "03", "System03"); // bass freq
    editDetails()->page()->addKnob(0, 4, 1, 1, "02", "02", "04", "System03"); // treb freq
    editDetails()->page()->addKnob(0, 5, 1, 1, "02", "02", "05", "System03"); // pre low
    editDetails()->page()->addKnob(0, 6, 1, 1, "02", "02", "06", "System03"); // pre high
    editDetails()->page()->addGroupBox(3, 0, 1, 2);

    editDetails()->addPage("21", "00", "02", "01");


    editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");
    editDetails()->page()->newGroupBox("Custom Speaker 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "00", "System03"); // speaker type
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "01", "System03"); // colour low
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "02", "System03"); // colour high
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "03", "System03"); // number of cones
    editDetails()->page()->addKnob(0, 4, 1, 1, "03", "00", "04", "System03"); // cab type
    editDetails()->page()->addGroupBox(1, 0, 1, 2);

    editDetails()->page()->newGroupBox("Custom Speaker 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "01", "00", "System03"); // speaker type
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "01", "01", "System03"); // colour low
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "01", "02", "System03"); // colour high
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "01", "03", "System03"); // number of cones
    editDetails()->page()->addKnob(0, 4, 1, 1, "03", "01", "04", "System03"); // cab type
    editDetails()->page()->addGroupBox(2, 0, 1, 2);
    editDetails()->addPage("21", "00", "02", "02");


    editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");
    editDetails()->page()->newGroupBox("Custom Distortion 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "00", "System03"); // dist type
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "01", "System03"); // bottom
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "02", "System03"); // top
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "00", "03", "System03"); // low
    editDetails()->page()->addKnob(0, 4, 1, 1, "04", "00", "04", "System03"); // high
    editDetails()->page()->addGroupBox(1, 0, 1, 2);

    editDetails()->page()->newGroupBox("Custom Distortion 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "01", "00", "System03"); // dist type
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "01", "01", "System03"); // bottom
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "01", "02", "System03"); // top
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "01", "03", "System03"); // low
    editDetails()->page()->addKnob(0, 4, 1, 1, "04", "01", "04", "System03"); // high
    editDetails()->page()->addGroupBox(2, 0, 1, 2);

    editDetails()->page()->newGroupBox("Custom Distortion 3");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "02", "00", "System03"); // dist type
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "02", "01", "System03"); // bottom
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "02", "02", "System03"); // top
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "02", "03", "System03"); // low
    editDetails()->page()->addKnob(0, 4, 1, 1, "04", "02", "04", "System03"); // high
    editDetails()->page()->addGroupBox(3, 0, 1, 2);

    editDetails()->addPage("21", "00", "02", "03");


    editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");
    editDetails()->page()->newGroupBox("Custom Wah 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "05", "00", "00", "System03"); // wah type
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "01", "System03"); // Q
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "00", "02", "System03"); // range low
    editDetails()->page()->addKnob(0, 3, 1, 1, "05", "00", "03", "System03"); // range high
    editDetails()->page()->addKnob(0, 4, 1, 1, "05", "00", "04", "System03"); // presence
    editDetails()->page()->addGroupBox(1, 0, 1, 2);

    editDetails()->page()->newGroupBox("Custom Wah 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "05", "01", "00", "System03"); // wah type
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "01", "01", "System03"); // Q
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "01", "02", "System03"); // range low
    editDetails()->page()->addKnob(0, 3, 1, 1, "05", "01", "03", "System03"); // range high
    editDetails()->page()->addKnob(0, 4, 1, 1, "05", "01", "04", "System03"); // presence
    editDetails()->page()->addGroupBox(2, 0, 1, 2);

    editDetails()->page()->newGroupBox("Custom Wah 3");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "05", "02", "00", "System03"); // wah type
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "02", "01", "System03"); // Q
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "02", "02", "System03"); // range low
    editDetails()->page()->addKnob(0, 3, 1, 1, "05", "02", "03", "System03"); // range high
    editDetails()->page()->addKnob(0, 4, 1, 1, "05", "02", "04", "System03"); // presence
    editDetails()->page()->addGroupBox(3, 0, 1, 2);

    editDetails()->addPage("21", "00", "02", "04");
}
