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

#ifndef PREFERENCESPAGES_H
#define PREFERENCESPAGES_H

#include <QWidget>
#include <QtWidgets>
#include <QSpinBox>
#include <QRadioButton>

class GeneralPage : public QWidget
{
	Q_OBJECT

public:
	GeneralPage(QWidget *parent = 0);
	QLineEdit* dirEdit;

public slots:
	void browseDir();
};

class MidiPage : public QWidget
{
public:
	MidiPage(QWidget *parent = 0);
	QComboBox* midiInCombo;
	QComboBox* midiOutCombo;
	QCheckBox* dBugCheckBox;
	QSpinBox* sysByteSpinBox1;
	QSpinBox* sysByteSpinBox2;
	QSpinBox* sysByteSpinBox3;
	QSpinBox* sysByteSpinBox4;
	QSpinBox* midiDelaySpinBox;
	int byte1;
	int byte2;
	int byte3;
	int byte4;
};

class WindowPage : public QWidget
{
public:
	WindowPage(QWidget *parent = 0);
	QCheckBox* windowCheckBox;
	QCheckBox* sidepanelCheckBox;
	QCheckBox* splashCheckBox;
};

class LanguagePage : public QWidget
{
public:
	LanguagePage(QWidget *parent = 0);
	QRadioButton* englishButton;
	QRadioButton* frenchButton;
	QRadioButton* germanButton;
	QRadioButton* chineseButton;
};

#endif // PREFERENCESPAGES_H

