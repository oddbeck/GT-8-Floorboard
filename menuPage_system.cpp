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

#include "menuPage_system.h"

menuPage_system::menuPage_system(QWidget *parent)
    : menuPage(parent)
{
  setImage(":/images/system_pushbutton.png");
  //setLSB("30", "00");
        setEditPages();
}

void menuPage_system::updateSignal()
{

}

void menuPage_system::setEditPages()
{
  editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");
  editDetails()->page()->newGroupBox("SYSTEM");
  editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "00", "System01"); // lcd contrast
  editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "0A", "System01"); // knob mode
  editDetails()->page()->addComboBox(2, 0, 1, 1, "03", "00", "0B", "System01"); // num pdl sw
  editDetails()->page()->addComboBox(3, 0, 1, 1, "03", "00", "09", "System01"); // dial func
  editDetails()->page()->addGroupBox(1, 0, 1, 1);

  editDetails()->page()->newGroupBox("Internal Pedals");
  editDetails()->page()->addComboBox(0, 0, 1, 1, "03", "00", "0F", "System01"); // exp pdl func
  editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "0E", "System01"); // exp pdl sw func
  editDetails()->page()->addComboBox(2, 0, 2, 1, "03", "00", "08", "System01"); // exp pdl hold
  editDetails()->page()->addComboBox(4, 0, 1, 1, "03", "00", "0D", "System01"); // ctl pdl func
  editDetails()->page()->addGroupBox(1, 1, 1, 1);

  editDetails()->page()->newGroupBox("External Pedals");
  editDetails()->page()->addComboBox(0, 0, 2, 1, "03", "00", "10", "System01"); // sub exp func
  editDetails()->page()->addComboBox(2, 0, 1, 1, "03", "00", "11", "System01"); // sub ctl1 func
  editDetails()->page()->addComboBox(3, 0, 1, 1, "03", "00", "12", "System01"); // sub ctl2 func
  editDetails()->page()->addGroupBox(1, 2, 1, 1);

  editDetails()->page()->newGroupBox("LEVEL METER");
  editDetails()->page()->addComboBox(0, 0, 1, 1, "05", "00", "00", "System01"); // meter point
  editDetails()->page()->addGroupBox(1, 3, 1, 1);

  editDetails()->page()->newGroupBox("TUNER");
  editDetails()->page()->addComboBox(0, 0, 1, 1, "00", "00", "00", "System00"); // tuner pitch
  editDetails()->page()->addLabel(0, 1, 1, 1, "           ");
  editDetails()->page()->addComboBox(0, 2, 1, 1, "00", "00", "01", "System00"); //tuner bypass
  editDetails()->page()->addGroupBox(2, 0, 1, 2);

  editDetails()->page()->newGroupBox("PATCH CHANGE");
  editDetails()->page()->addComboBox(0, 0, 1, 1, "03", "00", "03", "System01"); // patch change mode
  editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "05", "System01");     // bank extent
  editDetails()->page()->addComboBox(0, 2, 1, 1, "03", "00", "07", "System01"); // bank change mode
  editDetails()->page()->addGroupBox(2, 2, 1, 2);
  editDetails()->addPage("21", "00", "01", "00");


  editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");

  editDetails()->page()->newGroupBox("Global NS and Reverb");
  editDetails()->page()->addKnob(0, 0, 1, 1, "00", "00", "05", "System01"); // NS threshold
  editDetails()->page()->addKnob(0, 1, 1, 1, "00", "00", "06", "System01"); // reverb level
  editDetails()->page()->addGroupBox(1, 0, 1, 1);

  editDetails()->page()->newGroupBox("DIGITAL OUT");
  editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "0C", "System01"); // DGT output level
  editDetails()->page()->addGroupBox(2, 0, 1, 2);

  editDetails()->addPage("21", "00", "01", "01");


  editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");

  editDetails()->page()->newGroupBox("INPUT");
  editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "01", "System01"); // input level
  editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "02", "System01"); // input presence
  editDetails()->page()->addGroupBox(0, 0, 1, 1);

  editDetails()->page()->newGroupBox("OUTPUT");
  editDetails()->page()->addComboBox(0, 0, 1, 1, "00", "00", "00", "System01"); // main output select
  editDetails()->page()->addKnob(0, 1, 1, 1, "00", "00", "01", "System01"); // eq low
  editDetails()->page()->addKnob(0, 2, 1, 1, "00", "00", "02", "System01"); // eq mid
  editDetails()->page()->addKnob(0, 3, 1, 1, "00", "00", "03", "System01"); // eq freq
  editDetails()->page()->addKnob(0, 4, 1, 1, "00", "00", "04", "System01"); // eq high
  editDetails()->page()->addGroupBox(2, 0, 1, 2);

  editDetails()->addPage("21", "00", "01", "02");


  editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");
  editDetails()->page()->newGroupBox("System Midi");
  editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "00", "System01"); // rx channel
  editDetails()->page()->addComboBox(1, 0, 1, 1, "04", "00", "01", "System01"); // omni mode
  editDetails()->page()->addComboBox(2, 0, 1, 1, "04", "00", "02", "System01"); // tx channel
  editDetails()->page()->addComboBox(3, 0, 1, 1, "04", "00", "03", "System01"); // device ID
  editDetails()->page()->addComboBox(4, 0, 1, 1, "04", "00", "04", "System01"); // sync clock
  editDetails()->page()->addComboBox(5, 0, 1, 1, "04", "00", "05", "System01"); //  pc out
  editDetails()->page()->addLabel(0, 1, 1, 1, "                             ");

  editDetails()->page()->addComboBox(0, 2, 1, 1, "04", "00", "06", "System01"); // exp out
  editDetails()->page()->addComboBox(1, 2, 1, 1, "04", "00", "07", "System01"); // exp sw out
  editDetails()->page()->addComboBox(2, 2, 1, 1, "04", "00", "08", "System01"); // ctl pdl out
  editDetails()->page()->addComboBox(3, 2, 1, 1, "04", "00", "09", "System01"); // sub ctl1 out
  editDetails()->page()->addComboBox(4, 2, 1, 1, "04", "00", "0A", "System01"); // sub ctl2 out
  editDetails()->page()->addComboBox(5, 2, 1, 1, "04", "00", "0B", "System01"); // map select
  editDetails()->page()->addGroupBox(1, 0, 1, 1);
  editDetails()->addPage("21", "00", "01", "03");

  editDetails()->page()->addLabel(0, 0, 1, 1, "***CAUTION*** Settings changes are automatically written to GT-8");
  editDetails()->page()->newGroupBox("MANUAL MODE settings");
  editDetails()->page()->addComboBox(0, 3, 1, 1, "00", "00", "04", "System02"); // manual bank up
  editDetails()->page()->addLabel(1, 0, 1, 1, "  ");
  editDetails()->page()->addComboBox(2, 3, 1, 1, "00", "00", "05", "System02"); // manual bank down
  editDetails()->page()->addLabel(3, 0, 1, 1, "  ");
  editDetails()->page()->addLabel(4, 0, 1, 1, "  ");
  editDetails()->page()->addComboBox(5, 0, 1, 1, "00", "00", "00", "System02"); // manual pedal 1
  editDetails()->page()->addComboBox(5, 1, 1, 1, "00", "00", "01", "System02"); // manual pedal 2
  editDetails()->page()->addComboBox(5, 2, 1, 1, "00", "00", "02", "System02"); // manual pedal 3
  editDetails()->page()->addComboBox(5, 3, 1, 1, "00", "00", "03", "System02"); // manual pedal 4
  editDetails()->page()->addGroupBox(1, 0, 1, 1);
  editDetails()->addPage("21", "00", "01", "04");

  editDetails()->addPage();
}




