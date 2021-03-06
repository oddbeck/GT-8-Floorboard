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

#include "customKnob.h"
#include "MidiTable.h"
#include "SysxIO.h"

customKnob::customKnob(QWidget *parent, QString hex1, QString hex2, QString hex3, QString background, QString direction) : QWidget(parent)
{
	this->hex1 = hex1;
	this->hex2 = hex2;
	this->hex3 = hex3;
  this->area = direction;
	MidiTable *midiTable = MidiTable::Instance();
	if (area == "normal" || area == "turbo" || area.isEmpty()){this->area = "Structure";}
	int range = midiTable->getRange(this->area, hex1, hex2, hex3);
	int rangeMin = midiTable->getRangeMinimum(this->area, hex1, hex2, hex3);
	
	QPoint bgPos = QPoint(0, -3); // Correction needed y-3.
	QPoint knobPos = QPoint(5, 4); // Correction needed y+1 x-1.

	QLabel *newBackGround = new QLabel(this);
	if (background == "normal" || background != "Structure")
	{
		newBackGround->setPixmap(QPixmap(":/images/knobbgn.png"));
	}
	else if (background == "turbo")
	{
		newBackGround->setPixmap(QPixmap(":/images/knobbgt.png"));
	}
	else
	{
		newBackGround->setPixmap(QPixmap(":/images/knobbg.png"));
	}
	newBackGround->move(bgPos);

	QString imagePath(":/images/knob.png");
	unsigned int imageRange = 100;
	this->knob = new customDial(0, rangeMin, range, 1, 10, knobPos, this, hex1, hex2, hex3, imagePath, imageRange);
        this->setFixedSize(newBackGround->pixmap()->size() - QSize(0, 4)); // Correction needed h-4.

	QObject::connect(this, SIGNAL( updateSignal() ),
                this->parent(), SIGNAL( updateSignal() ));

	QObject::connect(this, SIGNAL( updateDisplay(QString) ),
                this->parent(), SIGNAL( updateDisplay(QString) ));
}

void customKnob::paintEvent(QPaintEvent *)
{
	/*DRAWS RED BACKGROUND FOR DEBUGGING PURPOSE */
	/*QPixmap image(":images/dragbar.png");
	
	QRectF target(0.0, 0.0, this->width(), this->height());
	QRectF source(0.0, 0.0, this->width(), this->height());

	QPainter painter(this);
	painter.drawPixmap(target, image, source);*/
}

void customKnob::setValue(int value)
{
	this->knob->setValue(value);
}

void customKnob::valueChanged(int value, QString hex1, QString hex2, QString hex3)
{
	MidiTable *midiTable = MidiTable::Instance();
	
	QString valueHex = QString::number(value, 16).toUpper();
	if(valueHex.length() < 2) valueHex.prepend("0");

	SysxIO *sysxIO = SysxIO::Instance(); bool ok;
	if(midiTable->isData(this->area, hex1, hex2, hex3))
	{	
		int maxRange = QString("7F").toInt(&ok, 16) + 1;
		int value = valueHex.toInt(&ok, 16);
		int dif = value/maxRange;
		QString valueHex1 = QString::number(dif, 16).toUpper();
		if(valueHex1.length() < 2) valueHex1.prepend("0");
		QString valueHex2 = QString::number(value - (dif * maxRange), 16).toUpper();
		if(valueHex2.length() < 2) valueHex2.prepend("0");
	
		sysxIO->setFileSource(this->area, hex1, hex2, hex3, valueHex1, valueHex2);		
	}
	else
	{
	 
		sysxIO->setFileSource(this->area, hex1, hex2, hex3, valueHex);
	}

	QString valueStr = midiTable->getValue(this->area, hex1, hex2, hex3, valueHex);

	emit updateDisplay(valueStr);
	emit updateSignal();
}
