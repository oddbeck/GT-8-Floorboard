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

#include <QChar>
#include "floorBoardDisplay.h"
#include "Preferences.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "midiIO.h"
#include "renameWidget.h"
#include "globalVariables.h"
#include "customRenameWidget.h"
#include "customControlListMenu.h"


// Platform-dependent sleep routines.
#ifdef Q_OS_WIN
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif


floorBoardDisplay::floorBoardDisplay(QWidget *parent, QPoint pos)
    : QWidget(parent)
{
    this->pos = pos;
    this->timer = new QTimer(this);
    this->patchLoadError = false;
    this->blinkCount = 0;

    int patchDisplayRowOffset = 5;
    int editButtonRowOffset = 45;
    int assignButtonRowOffset = 83;
    int tempRowOffset = 499;
    int bottomOffset = 544;
    this->patchNumDisplay = new customDisplay(QRect(25, patchDisplayRowOffset, 50, 34), this);
    this->patchNumDisplay->setLabelPosition(true);
    this->patchNumDisplay->setMainObjectName("bankMain");
    this->patchNumDisplay->setSubObjectName("bankSub");
    this->patchNumDisplay->setWhatsThis(tr("Patch Number Display.<br>displays the currently selected patch<br>and patch write memory location."));
    this->patchDisplay = new customDisplay(QRect(85, patchDisplayRowOffset, 150, 34), this);
    this->patchDisplay->setWhatsThis(tr("Patch Number Display.<br>displays the currently selected patch<br>and patch write memory location."));
    this->patchDisplay->setMainObjectName("nameMain");
    this->patchDisplay->setSubObjectName("nameSub");
    this->valueDisplay = new customDisplay(QRect(245, patchDisplayRowOffset, 150, 34), this);
    this->valueDisplay->setMainObjectName("valueMain");
    this->valueDisplay->setSubObjectName("valueSub");
    this->valueDisplay->setWhatsThis(tr("Patch Number Display.<br>displays the currently selected patch<br>and patch write memory location."));

    this->temp1Display = new customLabelDisplay(QRect(10, tempRowOffset+17, 170, 18), this);
    this->temp1Display->setLabelPosition(true);
    this->temp1Display->setMainObjectName("nameMain");
    this->temp1Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp1Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));
    this->temp2Display = new customLabelDisplay(QRect(190, tempRowOffset+17, 170, 18), this);
    this->temp2Display->setLabelPosition(true);
    this->temp2Display->setMainObjectName("nameMain");
    this->temp2Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp1Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));
    this->temp3Display = new customLabelDisplay(QRect(370, tempRowOffset+17, 170, 18), this);
    this->temp3Display->setLabelPosition(true);
    this->temp3Display->setMainObjectName("nameMain");
    this->temp3Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp1Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));
    this->temp4Display = new customLabelDisplay(QRect(550, tempRowOffset+17, 170, 18), this);
    this->temp4Display->setLabelPosition(true);
    this->temp4Display->setMainObjectName("nameMain");
    this->temp4Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp1Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));

    Preferences *preferences = Preferences::Instance();
    QString version = preferences->getPreferences("General", "Application", "version");
    this->patchDisplay->setMainText(deviceType + (tr(" Fx FloorBoard")));
    this->patchDisplay->setSubText(tr("version"), version);

    initPatch = new initPatchListMenu(QRect(405, patchDisplayRowOffset+19, 168, 15), this);
    //this->initPatch->setWhatsThis(tr("Clicking on this will load a patch from a predefined selection.<br>patches place in the Init Patches folder will appear in this list at the start of the next session."));
    renameWidget *nameEdit = new renameWidget(this);
    nameEdit->setGeometry(85, patchDisplayRowOffset, 150, 34);
    nameEdit->setWhatsThis(tr("Clicking on this will open<br>a text dialog window<br>allowing user text input."));
    customRenameWidget *userDialog = new customRenameWidget(this, "20", "00", "00", "Structure", "20");
    userDialog->setGeometry(500, editButtonRowOffset+5, 262, 25);
    userDialog->setWhatsThis(tr("Clicking on this will open<br>a text dialog window<br>allowing user text input."));
    customRenameWidget *patchDialog = new customRenameWidget(this, "1F", "00", "00", "Structure", "80");
    patchDialog->setGeometry(4, bottomOffset, 770, 25);
    patchDialog->setWhatsThis(tr("Clicking on this will open<br>a text dialog window<br>allowing user text input."));

    this->connectButton = new customButton(tr("Connect"), false, QPoint(405, patchDisplayRowOffset), this, ":/images/greenledbutton.png");
    this->connectButton->setWhatsThis(tr("Connect Button<br>used to establish a continuous midi connection<br>when lit green, the connection is valid"));
    this->writeButton = new customButton(tr("Write/Sync"), false, QPoint(494, patchDisplayRowOffset), this, ":/images/ledbutton.png");
    this->writeButton->setWhatsThis(tr("Write/Sync Button<br>if the patch number displays [temp buffer]<br>the current patch is sent to the GT temporary memory only<br>or else the patch will be written to the displayed patch memory location."));
    //this->assign_Button = new customButton(tr("Assigns"), false, QPoint(584, patchDisplayRowOffset), this, ":/images/pushbutton.png");
    //this->assign_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->system_midi_Button = new customButton(tr("Custom Settings"), false, QPoint(673, patchDisplayRowOffset), this, ":/images/pushbutton.png");
    this->system_midi_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->system_Button = new customButton(tr("System Settings"), false, QPoint(673, patchDisplayRowOffset+19), this, ":/images/pushbutton.png");
    this->system_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->ns2_Button = new customButton(tr("FV/Pedal"), false, QPoint(584, patchDisplayRowOffset+19), this, ":/images/pushbutton.png");
    this->ns2_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->preamp1_Button = new customButton(tr("PreAmp/Spkr"), false, QPoint(10, editButtonRowOffset), this, ":/images/pushbutton.png");
    this->preamp1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->compressor_Button = new customButton(tr("Compressor"), false, QPoint(10, editButtonRowOffset+18), this, ":/images/pushbutton.png");
    this->compressor_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->distortion_Button = new customButton(tr("Distortion"), false, QPoint(90,editButtonRowOffset), this, ":/images/pushbutton.png");
    this->distortion_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->ns1_Button = new customButton(tr("Noise.Sup."), false, QPoint(90, editButtonRowOffset+18), this, ":/images/pushbutton.png");
    this->ns1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->fx1_Button = new customButton(tr("FX 1"), false, QPoint(170, editButtonRowOffset), this, ":/images/pushbutton.png");
    this->fx1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->fx2_Button = new customButton(tr("FX 2"), false, QPoint(170, editButtonRowOffset+18), this, ":/images/pushbutton.png");
    this->fx2_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->reverb_Button = new customButton(tr("Reverb"), false, QPoint(250, editButtonRowOffset), this, ":/images/pushbutton.png");
    this->delay_Button = new customButton(tr("Delay"), false, QPoint(250, editButtonRowOffset+18), this, ":/images/pushbutton.png");
    this->delay_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->chorus_Button = new customButton(tr("Chorus"), false, QPoint(330, editButtonRowOffset), this, ":/images/pushbutton.png");
    this->sendreturn_Button = new customButton(tr("Loop"), false, QPoint(330, editButtonRowOffset+18), this, ":/images/pushbutton.png");
    this->sendreturn_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->eq_Button = new customButton(tr("Equaliser"), false, QPoint(410, editButtonRowOffset), this, ":/images/pushbutton.png");
    this->eq_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->pedal_Button = new customButton(tr("Wah"), false, QPoint(410, editButtonRowOffset+18), this, ":/images/pushbutton.png");
    this->pedal_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->assign1_Button = new customButton(tr("Assign 1"), false, QPoint(10, assignButtonRowOffset), this, ":/images/pushbutton.png");
    this->assign1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->assign2_Button = new customButton(tr("Assign 2"), false, QPoint(100, assignButtonRowOffset), this, ":/images/pushbutton.png");
    this->assign2_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->assign3_Button = new customButton(tr("Assign 3"), false, QPoint(190, assignButtonRowOffset), this, ":/images/pushbutton.png");
    this->assign3_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->assign4_Button = new customButton(tr("Assign 4"), false, QPoint(280, assignButtonRowOffset), this, ":/images/pushbutton.png");
    this->assign4_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->assign5_Button = new customButton(tr("Assign 5"), false, QPoint(370, assignButtonRowOffset), this, ":/images/pushbutton.png");
    this->assign5_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->assign6_Button = new customButton(tr("Assign 6"), false, QPoint(460, assignButtonRowOffset), this, ":/images/pushbutton.png");
    this->assign6_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->assign7_Button = new customButton(tr("Assign 7"), false, QPoint(550, assignButtonRowOffset), this, ":/images/pushbutton.png");
    this->assign7_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    this->assign8_Button = new customButton(tr("Assign 8"), false, QPoint(640, assignButtonRowOffset), this, ":/images/pushbutton.png");
    this->assign8_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->temp1_copy_Button = new customButton(tr("ClipBd-1 Copy"), false, QPoint(10, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp1_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp1_paste_Button = new customButton(tr("ClipBd-1 Paste"), false, QPoint(100, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp1_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));
    this->temp2_copy_Button = new customButton(tr("ClipBd-2 Copy"), false, QPoint(190, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp2_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp2_paste_Button = new customButton(tr("ClipBd-2 Paste"), false, QPoint(280, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp2_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));
    this->temp3_copy_Button = new customButton(tr("ClipBd-3 Copy"), false, QPoint(370, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp3_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp3_paste_Button = new customButton(tr("ClipBd-3 Paste"), false, QPoint(460, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp3_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));
    this->temp4_copy_Button = new customButton(tr("ClipBd-4 Copy"), false, QPoint(550, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp4_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp4_paste_Button = new customButton(tr("ClipBd-4 Paste"), false, QPoint(640, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp4_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));

    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(this, SIGNAL(setStatusSymbol(int)), sysxIO, SIGNAL(setStatusSymbol(int)));
    QObject::connect(this, SIGNAL(setStatusProgress(int)), sysxIO, SIGNAL(setStatusProgress(int)));
    QObject::connect(this, SIGNAL(setStatusMessage(QString)), sysxIO, SIGNAL(setStatusMessage(QString)));

    QObject::connect(sysxIO, SIGNAL(notConnectedSignal()), this, SLOT(notConnected()));
    QObject::connect(this, SIGNAL(notConnectedSignal()), this, SLOT(notConnected()));
    QObject::connect(this->parent(), SIGNAL(updateSignal()), this, SLOT(updateDisplay()));
    QObject::connect(this, SIGNAL(updateSignal()), this->parent(), SIGNAL(updateSignal()));

    QObject::connect(this->connectButton, SIGNAL(valueChanged(bool)), this, SLOT(connectSignal(bool)));
    QObject::connect(this->writeButton, SIGNAL(valueChanged(bool)), this, SLOT(writeSignal(bool)));
    QObject::connect(this->preamp1_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(preamp1_buttonSignal(bool)));
    QObject::connect(this->distortion_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(distortion_buttonSignal(bool)));
    QObject::connect(this->compressor_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(compressor_buttonSignal(bool)));
    QObject::connect(this->ns1_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(ns1_buttonSignal(bool)));
    QObject::connect(this->ns2_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(ns2_buttonSignal(bool)));
    QObject::connect(this->fx1_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(fx1_buttonSignal(bool)));
    QObject::connect(this->fx2_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(fx2_buttonSignal(bool)));
    QObject::connect(this->reverb_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(reverb_buttonSignal(bool)));
    QObject::connect(this->delay_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(delay_buttonSignal(bool)));
    QObject::connect(this->chorus_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(chorus_buttonSignal(bool)));
    QObject::connect(this->sendreturn_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(sendreturn_buttonSignal(bool)));
    QObject::connect(this->eq_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(eq_buttonSignal(bool)));
    QObject::connect(this->pedal_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(pedal_buttonSignal(bool)));

    QObject::connect(this->temp1_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp1_copy(bool)));
    QObject::connect(this->temp1_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp1_paste(bool)));
    QObject::connect(this->temp2_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp2_copy(bool)));
    QObject::connect(this->temp2_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp2_paste(bool)));
    QObject::connect(this->temp3_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp3_copy(bool)));
    QObject::connect(this->temp3_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp3_paste(bool)));
    QObject::connect(this->temp4_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp4_copy(bool)));
    QObject::connect(this->temp4_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp4_paste(bool)));

    set_temp();
    QString midiIn = preferences->getPreferences("Midi", "MidiIn", "device");
    QString midiOut = preferences->getPreferences("Midi", "MidiOut", "device");
    if(midiIn!="" && midiOut!="") {autoconnect(); };
}

QPoint floorBoardDisplay::getPos()
{
    return this->pos;
}

void floorBoardDisplay::setPos(QPoint newPos)
{
    this->move(newPos);
    this->pos = newPos;
}

void floorBoardDisplay::setValueDisplay(QString fxName, QString valueName, QString value)
{
    this->valueDisplay->setMainText(fxName);
    this->valueDisplay->setSubText(valueName, value);
}

void floorBoardDisplay::setPatchDisplay(QString patchName)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(sysxIO->getFileName() != ":default.syx") // Keep the initial version display if the default.syx is loaded at startup.
    {
        QString fileName = sysxIO->getFileName();
        this->patchDisplay->setMainText(patchName);
        this->patchDisplay->setSubText(fileName.section('/', -1, -1));
        this->patchName = patchName;
    };
    if(sysxIO->getFileName() == tr("init patch") || sysxIO->getFileName() == ":default.syx")
    {
        sysxIO->setFileName("");
        this->patchName = "Empty";
    }
    else
    {
        if(sysxIO->getFileName() == deviceType + tr(" patch"))
        {
            sysxIO->setFileName("");
            if(this->patchLoadError)
            {
                QMessageBox *msgBox = new QMessageBox();
                msgBox->setWindowTitle(deviceType + tr(" Fx FloorBoard"));
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setTextFormat(Qt::RichText);
                QString msgText;
                msgText.append("<font size='+1'><b>");
                msgText.append(tr("Error while changing banks."));
                msgText.append("<b></font><br>");
                msgText.append(tr("An incorrect patch has been loaded. Please try to load the patch again."));
                msgBox->setText(msgText);
                msgBox->setStandardButtons(QMessageBox::Ok);
                msgBox->exec();

                sysxIO->setBank(0);
                sysxIO->setPatch(0);
            };
        };
        this->initPatch->setIndex(0);
    };
}

void floorBoardDisplay::setPatchNumDisplay(int bank, int patch)
{
    if(bank > 0)
    {
        if(bank <= bankTotalUser)
        {
            this->patchNumDisplay->setAllColor(QColor(0,255,204));
            this->patchNumDisplay->setSubText(tr("User"));
        }
        else
        {
            this->patchNumDisplay->setAllColor(QColor(255,0,0));
            this->patchNumDisplay->setSubText(tr("Preset"));
        };
        QString str;
        if(bank < 36)
        {
            str.append("U");
            if(bank < 10) {	str.append("0"); };
            str.append(QString::number(bank, 10));
        }
        else
        { str.append("P");
            str.append(QString::number(bank, 10));
        };
        str.append(":");
        str.append(QString::number(patch, 10));
        this->patchNumDisplay->setMainText(str, Qt::AlignCenter);
    }
    else
    {
        //this->patchNumDisplay->clearAll();
        this->patchNumDisplay->setSubText(tr("Temp"));
        QString str = tr("Buffer");
        this->patchNumDisplay->setMainText(str, Qt::AlignCenter);

    };
}

void floorBoardDisplay::set_temp()
{

    SysxIO *sysxIO = SysxIO::Instance();
    QByteArray data;

    QFile file1("temp-1.syx");   // Read the default GT-8 sysx file so we don't start empty handed.
    if (file1.open(QIODevice::ReadOnly))
    {	data = file1.readAll(); };

    QList<QString> sysxBuffer;
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == patchSize)
    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp1Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp1_sysxMsg = sysxBuffer;
    };

    data.clear();
    QFile file2("temp-2.syx");   // Read the default GT-8 sysx file so we don't start empty handed.
    if (file2.open(QIODevice::ReadOnly))
    {	data = file2.readAll(); };

    sysxBuffer.clear();
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == patchSize)
    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp2Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp2_sysxMsg = sysxBuffer;
    };

    data.clear();
    QFile file3("temp-3.syx");   // Read the default GT-8 sysx file so we don't start empty handed.
    if (file3.open(QIODevice::ReadOnly))
    {	data = file3.readAll(); };

    sysxBuffer.clear();
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == patchSize)
    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp3Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp3_sysxMsg = sysxBuffer;
    };

    data.clear();
    QFile file4("temp-4.syx");   // Read the default GT-8 sysx file so we don't start empty handed.
    if (file4.open(QIODevice::ReadOnly))
    {	data = file4.readAll(); };

    sysxBuffer.clear();
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == patchSize)
    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp4Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp4_sysxMsg = sysxBuffer;
    };
}

void floorBoardDisplay::temp1_copy(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();

    QList<QString> sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size() == patchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp1Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp1_sysxMsg = sysxMsg;
        save_temp("temp-1.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size(), 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp1_paste(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp1_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        QList<QString> temp = sysxIO->temp1_sysxMsg;
        QString sysxMsg;
        int l=temp.size();
        for (int x=0; x<l; x++)
        {
            QString hex;
            hex = temp.at(x);
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
        sysxIO->setFileSource("Structure", sysxMsg);
        emit updateSignal();
        sysxIO->writeToBuffer();
    }else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to Temp-1 clipboard first"));
    };
}

void floorBoardDisplay::temp2_copy(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();

    QList<QString> sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size() == patchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp2Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp2_sysxMsg = sysxMsg;
        save_temp("temp-2.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size(), 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp2_paste(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp2_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        QList<QString> temp = sysxIO->temp2_sysxMsg;
        QString sysxMsg;
        int l=temp.size();
        for (int x=0; x<l; x++)
        {
            QString hex;
            hex = temp.at(x);
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
        sysxIO->setFileSource("Structure", sysxMsg);
        //sysxIO->setFileSource("patch", temp2_sysxMsg);
        emit updateSignal();
        sysxIO->writeToBuffer();
    }else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to Temp-2 clipboard first"));
    };
}

void floorBoardDisplay::temp3_copy(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();

    QList<QString> sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size() == patchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp3Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp3_sysxMsg = sysxMsg;
        save_temp("temp-3.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size(), 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp3_paste(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp3_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        QList<QString> temp = sysxIO->temp3_sysxMsg;
        QString sysxMsg;
        int l=temp.size();
        for (int x=0; x<l; x++)
        {
            QString hex;
            hex = temp.at(x);
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
        sysxIO->setFileSource("Structure", sysxMsg);
        emit updateSignal();
        sysxIO->writeToBuffer();
    }else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to Temp-3 clipboard first"));
    };
}

void floorBoardDisplay::temp4_copy(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();

    QList<QString> sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size() == patchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp4Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp4_sysxMsg = sysxMsg;
        save_temp("temp-4.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size(), 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp4_paste(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp4_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        QList<QString> temp = sysxIO->temp4_sysxMsg;
        QString sysxMsg;
        int l=temp.size();
        for (int x=0; x<l; x++)
        {
            QString hex;
            hex = temp.at(x);
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
        sysxIO->setFileSource("Structure", sysxMsg);
        emit updateSignal();
        sysxIO->writeToBuffer();
    }else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to Temp-4 clipboard first"));
    };
}

void floorBoardDisplay::save_temp(QString fileName, QList<QString> sysxMsg)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QByteArray out;
        unsigned int count=0;
        QList<QString> data = sysxMsg;
        for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
        {
            QString str(*code);
            bool ok;
            unsigned int n = str.toInt(&ok, 16);
            out[count] = (char)n;
            count++;
        };
        file.write(out);
    };
}

void floorBoardDisplay::updateDisplay()
{
    SysxIO *sysxIO = SysxIO::Instance();
    QList<QString> nameArray = sysxIO->getFileSource("Structure", nameAddress, "00");

    //MidiTable *midiTable = MidiTable::Instance();
    QString name;
    for(int i=sysxDataOffset;i<nameArray.size() - 2;i++ )
    {
        //name.append( midiTable->getMidiMap("Structure", "0B", "00", "00", nameArray.at(i)).name );

        QString hexStr = nameArray.at(i);
        if(hexStr == "7E")
        {
            name.append((QChar)(0x2192));
        }
        else if (hexStr == "7F")
        {
            name.append((QChar)(0x2190));
        }
        else
        {
            bool ok;
            name.append( (char)(hexStr.toInt(&ok, 16)) );
        };
    };

    QString patchName = name.trimmed();
    sysxIO->setCurrentPatchName(patchName);
    if(sysxIO->getRequestName().trimmed() != patchName.trimmed())
    {
        this->patchLoadError = true;
    }
    else
    {
        this->patchLoadError = false;
    };


    this->setPatchDisplay(patchName);
    if(sysxIO->isDevice())
    {
        int bank = sysxIO->getBank();
        int patch = sysxIO->getPatch();
        this->setPatchNumDisplay(bank, patch);
    }
    else
    {
        this->patchNumDisplay->clearAll();
    };
    this->valueDisplay->clearAll();

    if(sysxIO->isDevice() )  // comment out from here
    {
        if(sysxIO->getBank() > 35)
        {
            this->writeButton->setBlink(false);
            this->writeButton->setValue(true);
        }
        else
        {
            this->writeButton->setBlink(false);
            this->writeButton->setValue(true);
        };
        int bank = sysxIO->getBank();
        int patch = sysxIO->getPatch();
        this->setPatchNumDisplay(bank, patch);
    }
    else if(sysxIO->getBank() != 0)
    {
        if(sysxIO->isConnected())
        {
            this->writeButton->setBlink(true);
            this->writeButton->setValue(false);
        };
        int bank = sysxIO->getBank();
        int patch = sysxIO->getPatch();
        this->setPatchNumDisplay(bank, patch);
    }
    else
    {
        patchNumDisplay->clearAll();
        this->writeButton->setBlink(false);
        this->writeButton->setValue(false);
    };
}

void floorBoardDisplay::autoconnect()
{
    QString replyMsg;
    SysxIO *sysxIO = SysxIO::Instance();
    //this->connectButtonActive = value;
    if(!sysxIO->isConnected() && sysxIO->deviceReady())
    {
        emit setStatusSymbol(2);
        emit setStatusMessage(tr("Connecting"));

        this->connectButton->setBlink(true);
        sysxIO->setDeviceReady(false); // Reserve the device for interaction.

        QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)));
        QObject::connect(sysxIO, SIGNAL(sysxReply(QString)),
                         this, SLOT(autoConnectionResult(QString)));

        sysxIO->sendSysx(idRequestString); // GT-8 Identity Request.
    }
    else
    {
        emit notConnected();
        sysxIO->setNoError(true);		// Reset the error status (else we could never retry :) ).
    };
}

void floorBoardDisplay::autoConnectionResult(QString sysxMsg)
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)),
                        this, SLOT(autoConnectionResult(QString)));


    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
    if(sysxIO->noError())
    {
        if( sysxMsg.contains(idReplyPatern) )
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(true);
            sysxIO->setConnected(true);
            emit connectedSignal();

            if(sysxIO->getBank() != 0)
            {
                this->writeButton->setBlink(true);
                this->writeButton->setValue(false);
            };
        }else
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(false);
            sysxIO->setConnected(false);
        };
    }
    else
    {
        notConnected();
        sysxIO->setNoError(true);		// Reset the error status (else we could never retry :) ).
    };
}



void floorBoardDisplay::connectSignal(bool value)
{
    QString replyMsg;
    qDebug() << "connecting";
    SysxIO *sysxIO = SysxIO::Instance();
    this->connectButtonActive = value;
    if(connectButtonActive == true && sysxIO->deviceReady())
    {
        emit setStatusSymbol(2);
        emit setStatusMessage(tr("Connecting"));

        this->connectButton->setBlink(true);
        sysxIO->setDeviceReady(false); // Reserve the device for interaction.

        QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)));
        QObject::connect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(connectionResult(QString)));

        sysxIO->sendSysx(idRequestString); // GT Identity Request.
    }
    else
    {
        emit notConnected();
        sysxIO->setNoError(true);		// Reset the error status (else we could never retry :) ).
    };
}

void floorBoardDisplay::connectionResult(QString sysxMsg)
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)),
                        this, SLOT(connectionResult(QString)));

    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.

    //DeBugGING OUTPUT
    Preferences *preferences = Preferences::Instance(); // Load the preferences.
    if(preferences->getPreferences("Midi", "DBug", "bool")=="true")
    {
        this->connectButton->setBlink(false);
        this->connectButton->setValue(true);
        sysxIO->setConnected(true);
        emit connectedSignal();
        emit setStatusMessage(tr("Ready"));

        if(sysxIO->getBank() != 0)
        {
            this->writeButton->setBlink(true);
            this->writeButton->setValue(false);
        };
    }

    else if(sysxIO->noError())
    {
        if(sysxMsg.contains(idReplyPatern) && connectButtonActive == true)
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(true);
            sysxIO->setConnected(true);
            emit connectedSignal();

            if(sysxIO->getBank() != 0)
            {
                this->writeButton->setBlink(true);
                this->writeButton->setValue(false);
            };
        }
        else if(!sysxMsg.isEmpty())
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(false);
            sysxIO->setConnected(false);

            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(deviceType + tr(" Fx FloorBoard connection Error !!"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("<font size='+1'><b>");
            msgText.append(tr("The device connected is either not a Boss ") + deviceType + (" Effects Processor.<br>"));
            msgText.append(tr("or the GT-8 device ID is not set to [1]"));
            if (sysxMsg.contains(idRequestString))
            {msgText.append(tr("<br>Midi loopback detected, ensure midi device 'thru' is switched off.")); };
            msgText.append("<b></font>");
            msgBox->setText(msgText);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();

            notConnected();

            emit setStatusSymbol(0);
            emit setStatusProgress(0);
            emit setStatusMessage(tr("Not connected"));
        }
        else
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(false);
            sysxIO->setConnected(false);

            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(deviceType + tr(" Fx FloorBoard connection Error !!"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("<font size='+1'><b>");
            msgText.append(tr("The Boss ") + deviceType + (" Effects Processor was not found."));
            msgText.append(tr("<br><br>Ensure correct midi device is selected in Menu, "));
            msgText.append(tr("<br>midi cables are connected and the GT-8 is switched on,"));
            msgText.append("<b></font><br>");
            msgBox->setText(msgText);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();

            notConnected();

            emit setStatusSymbol(0);
            emit setStatusProgress(0);
            emit setStatusMessage(tr("Not connected"));
        };
    }
    else
    {
        notConnected();
        sysxIO->setNoError(true);		// Reset the error status (else we could never retry :) ).
    };
}

void floorBoardDisplay::writeSignal(bool)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(sysxIO->isConnected() && sysxIO->deviceReady()) /* Check if we are connected and if the device is free. */
    {
        this->writeButton->setBlink(true);

        if(sysxIO->getBank() == 0) /* Check if a bank is sellected. */
        {
            sysxIO->setDeviceReady(false);			// Reserve the device for interaction.
            writeToBuffer();
        }
        else /* Bank is sellected. */
        {
            sysxIO->setDeviceReady(false);			// Reserve the device for interaction.

            if(sysxIO->getBank() > bankTotalUser) // Preset banks are NOT writable so we check.
            {
                QMessageBox *msgBox = new QMessageBox();
                msgBox->setWindowTitle(deviceType + tr(" Fx FloorBoard"));
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setTextFormat(Qt::RichText);
                QString msgText;
                msgText.append("<font size='+1'><b>");
                msgText.append(tr("You can't write to the preset banks."));
                msgText.append("<b></font><br>");
                msgText.append(tr("Please select a user bank to write this patch to and try again."));
                msgBox->setText(msgText);
                msgBox->setStandardButtons(QMessageBox::Ok);
                msgBox->exec();
                this->writeButton->setBlink(false); // Allready sync with the buffer so no blinking
                this->writeButton->setValue(true);	// and so we will also leave the write button active.
                sysxIO->setDeviceReady(true);
            }
            else /* User bank so we can write to it after confirmation to overwrite stored data. */
            {
                QString bankNum;
                QString patchNum;
                int bank = sysxIO->getBank();
                int patch = sysxIO->getPatch();
                bankNum.append(QString::number(bank, 10));
                patchNum.append(QString::number(patch, 10));
                if (bankNum.size() < 2){ bankNum.prepend("0"); };
                bankNum.prepend(" U");

                QMessageBox *msgBox = new QMessageBox();
                msgBox->setWindowTitle(deviceType + tr(" Fx FloorBoard"));
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setTextFormat(Qt::RichText);
                QString msgText;
                msgText.append("<font size='+1'><b>");
                msgText.append(tr("You have chosen to write the patch permanently into ") + deviceType + (" memory."));
                msgText.append("<b></font><br>");
                msgText.append(tr("This will overwrite the patch currently stored at patch location\n"));
                msgText.append("<font size='+3'><b>");
                msgText.append(bankNum +(":") +patchNum	);
                msgText.append("<b></font><br>");
                msgText.append(tr (" and can't be undone. "));

                msgBox->setInformativeText(tr("Are you sure you want to continue?"));
                msgBox->setText(msgText);
                msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

                if(msgBox->exec() == QMessageBox::Yes)
                {	/* Accepted to overwrite data. So now we have to set destination address by replacing the default (buffer). */

                    writeToMemory();
                }
                else if(sysxIO->isConnected())
                {
                    sysxIO->setDeviceReady(true);
                    this->writeButton->setBlink(false);
                    this->writeButton->setValue(true);
                };
            };
        };
    }

    if((sysxIO->isConnected() == !true) && sysxIO->deviceReady())
    {
        writeToBuffer();                           // update patch to temp buffer only if not in bulk mode
    };
    //sysxIO->getCurrentPatchName();
}

void floorBoardDisplay::writeToBuffer() 
{
    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->writeToBuffer();
    this->writeButton->setBlink(false);	// Sync so we stop blinking the button
    this->writeButton->setValue(false);	// and activate the write button.

}

void floorBoardDisplay::writeToMemory() 
{
    SysxIO *sysxIO = SysxIO::Instance();

    QString sysxMsg; bool ok;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    QList<QString> patchAddress = sysxIO->getFileSource().address;

    emit setStatusSymbol(2);
    emit setStatusMessage(tr("Writing to Patch"));

    int bank = sysxIO->getBank();
    int patch = sysxIO->getPatch();
    int patchOffset = (((bank - 1 ) * patchPerBank) + patch) - 1;
    int memmorySize = QString("7F").toInt(&ok, 16) + 1;
    int emptyAddresses = (memmorySize) - ((bankTotalUser * patchPerBank) - (memmorySize));
    if(bank > bankTotalUser) patchOffset += emptyAddresses; //System patches start at a new memmory range.
    int addrMaxSize = QString("80").toInt(&ok, 16);
    int n = (int)(patchOffset / addrMaxSize);

    QString addr1 = QString::number(8 + n, 16).toUpper();
    QString addr2 = QString::number(patchOffset - (addrMaxSize * n), 16).toUpper();
    if (addr1.length() < 2) addr1.prepend("0");
    if (addr2.length() < 2) addr2.prepend("0");

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            {
                hex = addr1;
            }
            else if(x == sysxAddressOffset + 1)
            {
                hex = addr2;
            }
            else
            {
                hex = data.at(x);
            };
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    sysxIO->setSyncStatus(true);		// Still in sync
    this->writeButton->setBlink(false); // so no blinking here either...
    this->writeButton->setValue(true);	// ... and still the button will be active also ...

    QObject::connect(sysxIO, SIGNAL(sysxReply(QString)),	// Connect the result signal
                     this, SLOT(resetDevice(QString)));					// to a slot that will reset the device after sending.

    sysxIO->sendSysx(sysxMsg);								// Send the data.
}

void floorBoardDisplay::patchChangeFailed()
{
    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setBank(sysxIO->getLoadedBank());
    sysxIO->setPatch(sysxIO->getLoadedPatch());
    setPatchNumDisplay(sysxIO->getLoadedBank(), sysxIO->getLoadedPatch());
}

void floorBoardDisplay::resetDevice(QString replyMsg) 
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)),
                        this, SLOT(resetDevice(QString)));

    if(sysxIO->getBank() != sysxIO->getLoadedBank() || sysxIO->getPatch() != sysxIO->getLoadedPatch())
    {
        sysxIO->setLoadedBank(sysxIO->getBank());
        sysxIO->setLoadedPatch(sysxIO->getPatch());
    };
    emit setStatusProgress(33); // time wasting sinusidal statusbar progress
    SLEEP(100);
    emit setStatusProgress(66);
    SLEEP(100);
    emit setStatusProgress(100);
    SLEEP(100);
    emit setStatusProgress(75);
    SLEEP(100);
    emit setStatusProgress(42);
    SLEEP(100);
    emit setStatusProgress(25);
    SLEEP(100);
    sysxIO->setDeviceReady(true);	// Free the device after finishing interaction.
    emit connectedSignal();			// Emit this signal to tell we are still connected and to update the patch names in case they have changed.
}

void floorBoardDisplay::patchSelectSignal(int bank, int patch)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(blinkCount == 0)
    {
        currentSyncStatus = sysxIO->getSyncStatus();
        sysxIO->setSyncStatus(false);
        writeButton->setBlink(true);
    };

    //if( sysxIO->getLoadedBank() != bank ||  sysxIO->getLoadedPatch() != patch)
    //{
    sysxIO->setBank(bank);
    sysxIO->setPatch(patch);

    if(blinkCount == 0)
    {
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(blinkSellectedPatch()));
        timer->start(sellectionBlinkInterval);
    }
    else
    {
        blinkCount = 0;
    };
    //}
    //else
    //{
    //	blinkSellectedPatch(false);
    //};
}

void floorBoardDisplay::blinkSellectedPatch(bool active)
{
    SysxIO *sysxIO = SysxIO::Instance();
    int bank = sysxIO->getBank();
    int patch = sysxIO->getPatch();

    if(active && blinkCount <= (sellectionBlinks * 2) - 1)
    {
        if(blinkCount % 2 == 0)
        {
            this->patchNumDisplay->clearAll();
        }
        else
        {
            setPatchNumDisplay(bank, patch);
        };
        blinkCount++;
    }
    else
    {
        QObject::disconnect(timer, SIGNAL(timeout()), this, SLOT(blinkSellectedPatch()));
        timer->stop();
        blinkCount = 0;
        //sysxIO->setBank(sysxIO->getLoadedBank());
        //sysxIO->setPatch(sysxIO->getLoadedPatch());
        sysxIO->setSyncStatus(currentSyncStatus);
        if(currentSyncStatus || sysxIO->getLoadedBank() == 0)
        {
            writeButton->setBlink(false);
        };
        setPatchNumDisplay(bank,patch);
    };
    emit setStatusSymbol(1);
    emit setStatusMessage(tr("Ready"));
}

void floorBoardDisplay::patchLoadSignal(int bank, int patch)
{
    blinkSellectedPatch(false);

    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setBank(bank);
    sysxIO->setPatch(patch);
}

void floorBoardDisplay::notConnected()
{
    this->connectButton->setBlink(false);
    this->connectButton->setValue(false);
    this->writeButton->setBlink(false);
    this->writeButton->setValue(false);

    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setConnected(false);
    sysxIO->setSyncStatus(false);
    sysxIO->setDeviceReady(true);	// Free the device after finishing interaction.

    emit setStatusSymbol(0);
    emit setStatusProgress(0);
    emit setStatusMessage(tr("Not connected"));

}

void floorBoardDisplay::valueChanged(bool value, QString hex1, QString hex2, QString hex3)
{
    value = value;
    hex1 = hex1;
    hex2 = hex2;
    hex3 = hex3;
}
