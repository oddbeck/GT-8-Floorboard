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

#include <QtWidgets>
#include <QFile>
#include "summaryDialogSystem.h"
#include "Preferences.h"
#include "SysxIO.h"
#include "MidiTable.h"
#include "globalVariables.h"

summaryDialogSystem::summaryDialogSystem(QWidget *parent)
    : QWidget(parent)
{

    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(this, SIGNAL(setStatusSymbol(int)), sysxIO, SIGNAL(setStatusSymbol(int)));
    QObject::connect(this, SIGNAL(setStatusProgress(int)), sysxIO, SIGNAL(setStatusProgress(int)));
    QObject::connect(this, SIGNAL(setStatusMessage(QString)), sysxIO, SIGNAL(setStatusMessage(QString)));
    QObject::connect(this, SIGNAL(setStatusdBugMessage(QString)), sysxIO, SIGNAL(setStatusdBugMessage(QString)));

    this->textDialog = new QTextEdit(parent);
    textDialog->setReadOnly(true);
    textDialog->setDocumentTitle(deviceType + tr(" Patch List"));
    textDialog->setMinimumHeight(600);
    

    if (sysxIO->isConnected()&& sysxIO->deviceReady())
    {
        emit setStatusMessage(tr("Opening Page..."));
        emit setStatusSymbol(3);
        QString replyMsg;
        sysxIO->setDeviceReady(false); // Reserve the device for interaction.
        QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)));
        QObject::connect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(systemReply(QString)));
        emit setStatusProgress(100);
        emit setStatusSymbol(2);
        emit setStatusProgress(0);
        emit setStatusMessage(tr("Request System data"));
        sysxIO->sendSysx(systemRequest); // GT-8 System area data Request.
    }
    else
    {
        QString snork = tr("Ensure 'Connection' is active and retry<br>");
        snork.append(tr("System data was not transfered, current System settings are to be used<br>"));
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(deviceType + tr(" midi connection not active!!"));
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(snork);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
        emit setStatusMessage(tr("No midi connection"));
        emit setStatusSymbol(0);
        generatePage();
    }

}

void summaryDialogSystem::systemReply(QString replyMsg)
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(systemReply(QString)));
    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.

    if(sysxIO->noError())
    {
        if(replyMsg.size()/2 == 4402)
        {
            replyMsg.remove(54, 26);		// join 2 parts and remove second header.
            QString reBuild = "";       // Add correct checksum to patch strings.
            QString sysxEOF = "";
            QString hex = "";
            int msgLength = replyMsg.length()/2;
            for(int i=0;i<msgLength*2;++i)
            {
                hex.append(replyMsg.mid(i*2, 2));
                sysxEOF = (replyMsg.mid((i*2)+4, 2));
                if (sysxEOF == "F7")
                {
                    int dataSize = 0; bool ok;
                    for(int h=checksumOffset;h<hex.size()-1;++h)
                    { dataSize += hex.mid(h*2, 2).toInt(&ok, 16); }
                    QString base = "80";                       // checksum calculate.
                    unsigned int sum = dataSize % base.toInt(&ok, 16);
                    if(sum!=0) { sum = base.toInt(&ok, 16) - sum; }
                    QString checksum = QString::number(sum, 16).toUpper();
                    if(checksum.length()<2) {checksum.prepend("0");}
                    hex.append(checksum);
                    hex.append("F7");
                    reBuild.append(hex);

                    hex = "";
                    sysxEOF = "";
                    i=i+2;
                }
            }
            replyMsg = reBuild.simplified().toUpper();
        } else  {
            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(deviceType + tr(" midi connection required !!"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("<font size='+1'><b>");
            msgText.append(tr("The Boss ") + deviceType + tr(" System data was not transfered !!."));
            msgText.append(tr("<br>The currently loaded System data will be used instead."));
            msgText.append("<b></font><br>");
            msgBox->setText(msgText);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();
        }
    }
    emit setStatusMessage(tr("Ready"));
    generatePage();
}

void summaryDialogSystem::generatePage()
{
    SysxIO *sysxIO = SysxIO::Instance();
    system.clear();
    QList< QList<QString> > systemData = sysxIO->getSystemSource().hex; // Get the loaded system data.
    for(int i=0;i<systemData.size();++i)
    {
        QList<QString> data = systemData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            hex = data.at(x);
            if (hex.length() < 2) hex.prepend("0");
            system.append(hex);
        }
    }
    /*********************************************************************/
    /************** The System List **************************************/
    /*********************************************************************/
    //MidiTable *midiTable = MidiTable::Instance();
    text = "<b>Boss " + deviceType + " System/Global Summary</b>    ";
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();
    text.append(dateTimeString);
    
    text.append("<br><br><b><u>**********System Global***********</b></u><br>");
    address1= "00";
    address2= "00";
    address3= "00";
    start = 0;
    finish = 2;
    makeList();

    address1= "01";
    address2= "00";
    address3= "00";
    start = 0;
    finish = 7;
    makeList();

    address1= "01";
    address2= "03";
    address3= "00";
    start = 0;
    finish = 19;
    makeList();

    text.append("<br><br><b><u>**********System Midi**********</b></u><br>");
    address1= "01";
    address2= "04";
    address3= "00";
    start = 0;
    finish = 12;
    makeList();

    text.append("<br><br><b><u>**********Meter**********</b></u><br>");
    address1= "01";
    address2= "05";
    address3= "00";
    start = 0;
    finish = 1;
    makeList();

    text.append("<br><br><b><u>**********System PreAmp**********</b></u><br>");
    address1= "01";
    address2= "10";
    address3= "00";
    start = 0;
    finish = 40;
    makeList();

    text.append("<br><br><b><u>**********Manual**********</b></u><br>");
    address1= "02";
    address2= "00";
    address3= "00";
    start = 0;
    finish = 6;
    makeList();

    text.append("<br><br><b><u>**********Custom PreAmp 1**********</b></u><br>");
    address1= "03";
    address2= "02";
    address3= "00";
    start = 0;
    finish = 7;
    makeList();

    text.append("<br><br><b><u>**********Custom PreAmp 2**********</b></u><br>");
    address1= "03";
    address2= "02";
    address3= "01";
    start = 0;
    finish = 7;
    makeList();

    text.append("<br><br><b><u>**********Custom PreAmp 3**********</b></u><br>");
    address1= "03";
    address2= "02";
    address3= "02";
    start = 0;
    finish = 7;
    makeList();

    text.append("<br><br><b><u>**********Custom Speaker 1**********</b></u><br>");
    address1= "03";
    address2= "03";
    address3= "00";
    start = 0;
    finish = 5;
    makeList();

    text.append("<br><br><b><u>**********Custom Speaker 2**********</b></u><br>");
    address1= "03";
    address2= "03";
    address3= "01";
    start = 0;
    finish = 5;
    makeList();
    
    text.append("<br><br><b><u>**********Custom Distortion 1**********</b></u><br>");
    address1= "03";
    address2= "04";
    address3= "00";
    start = 0;
    finish = 5;
    makeList();

    text.append("<br><br><b><u>**********Custom Distortion 2**********</b></u><br>");
    address1= "03";
    address2= "04";
    address3= "01";
    start = 0;
    finish = 5;
    makeList();

    text.append("<br><br><b><u>**********Custom Distortion 3**********</b></u><br>");
    address1= "03";
    address2= "04";
    address3= "02";
    start = 0;
    finish = 5;
    makeList();

    text.append("<br><br><b><u>**********Custom Wah 1**********</b></u><br>");
    address1= "03";
    address2= "05";
    address3= "00";
    start = 0;
    finish = 5;
    makeList();

    text.append("<br><br><b><u>**********Custom Wah 2**********</b></u><br>");
    address1= "03";
    address2= "05";
    address3= "01";
    start = 0;
    finish = 5;
    makeList();

    text.append("<br><br><b><u>**********Custom Wah 3**********</b></u><br>");
    address1= "03";
    address2= "05";
    address3= "02";
    start = 0;
    finish = 5;
    makeList();


    text.append("<br><br><b><u>********** MIDI MAPPING ***********</b></u><br><br>");
    count = 1;
    patch = 1;
    address1= "10";
    address2= "00";
    makeMidiMapList();

    address1= "10";
    address2= "40";
    makeMidiMapList();

    address1= "11";
    address2= "00";
    makeMidiMapList();

    address1= "11";
    address2= "40";
    makeMidiMapList();

    address1= "12";
    address2= "00";
    makeMidiMapList();

    address1= "12";
    address2= "40";
    makeMidiMapList();

    address1= "13";
    address2= "00";
    makeMidiMapList();

    address1= "13";
    address2= "40";
    makeMidiMapList();

    address1= "14";
    address2= "00";
    makeMidiMapList();

    address1= "14";
    address2= "40";
    makeMidiMapList();

    address1= "15";
    address2= "00";
    makeMidiMapList();

    address1= "15";
    address2= "40";
    makeMidiMapList();

    address1= "16";
    address2= "00";
    makeMidiMapList();

    address1= "16";
    address2= "40";
    makeMidiMapList();

    address1= "17";
    address2= "00";
    makeMidiMapList();

    address1= "17";
    address2= "40";
    makeMidiMapList();


    textDialog->setText(text);
    textDialog->show();

    QPushButton *cancelButton = new QPushButton(tr("Close"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    cancelButton->setWhatsThis(tr("Will close the currently open Dialog page."));
    //cancelButton->hide();

    QPushButton *printButton = new QPushButton(tr("Print"));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printFile()));
    printButton->setWhatsThis(tr("Will Print the current Dialog to the prefered printer<br>printed text will be simular to the screen layout."));
    //printButton->hide();

    QPushButton *saveAsButton = new QPushButton(tr("Save As"));
    connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
    saveAsButton->setWhatsThis(tr("Will save the current dialog page to file in a *.txt format."));
    //saveAsButton->hide();

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(textDialog);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(printButton);
    buttonsLayout->addSpacing(12);
    buttonsLayout->addWidget(saveAsButton);
    buttonsLayout->addSpacing(12);
    buttonsLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(deviceType + tr(" System and Global List Summary"));
}

void summaryDialogSystem::cancel()
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(systemReply(QString)));
    sysxIO->setDeviceReady(true);
    emit setStatusSymbol(1);
    emit setStatusMessage(tr("Ready"));
    this->close();
}

void summaryDialogSystem::printFile()
{
#ifndef QT_NO_PRINTER

    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    //QPrintPreviewDialog *dialog = new QPrintPreviewDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted) { return; }
    else { textDialog->print(&printer); }
#endif
}

void summaryDialogSystem::saveAs()
{

    Preferences *preferences = Preferences::Instance();
    QString dir = preferences->getPreferences("General", "Files", "dir");

    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save As"),
                dir,
                tr("Text Document (*.txt)"));
    if (!fileName.isEmpty())
    {
        if(!fileName.contains(".txt"))
        {
            fileName.append(".txt");
        }
        QFile file(fileName);

        QByteArray out;
        text.remove("<b>");
        text.remove("</b>");
        text.remove("<u>");
        text.remove("</u>");
        QString newLine;
        newLine.append((char)13);
        newLine.append((char)10);
        text.replace("<br>", newLine);

        unsigned int size = text.size();

        for (unsigned int x=0; x<size; x++)
        {
            QString str(text.at(x));
            out.append(str);
        }

        if (file.open(QIODevice::WriteOnly))
        {
            file.write(out);
        }

    }
}

void summaryDialogSystem::makeList()
{
    SysxIO *sysxIO = SysxIO::Instance();
    MidiTable *midiTable = MidiTable::Instance();
    for(int i=start;i<finish;i++ )
    {
        QString pos = QString::number(i, 16).toUpper();
        if(pos.size()<2){ pos.prepend("0"); }
        QString pretxt =  midiTable->getMidiMap("System", address1, address2, address3, pos).desc;
        QString txt =  midiTable->getMidiMap("System", address1, address2, address3, pos).customdesc;
        if(!txt.isEmpty() && txt != "")
        {
            int value = sysxIO->getSourceValue("System"+address1, address2, address3, pos);
            QString valueHex = QString::number(value, 16).toUpper();
            if(valueHex.length() < 2) {valueHex.prepend("0"); }
            if (txt != "#")
            {
                text.append("<br>");
                text.append("[");
                if(!pretxt.isEmpty() && txt != "") { text.append(pretxt + " "); }
                text.append(txt);
                text.append("] = ");
            }
            text.append(midiTable->getValue("System" + address1, address2, address3, pos, valueHex) );
        }
    }
}

void summaryDialogSystem::makeMidiMapList()
{

    SysxIO *sysxIO = SysxIO::Instance();
    MidiTable *midiTable = MidiTable::Instance();
    if(patch > 128) {patch = 1; }
    int row = 1;
    bool ok;
    QString address = "0104" + address1;
    QList <QString> sysxData = sysxIO->systemSource.hex.at( sysxIO->systemSource.address.indexOf(address) );
    QString message = QString::number(sysxData.size(), 10);
    emit setStatusdBugMessage("size = "+message);
    for(int i=0;i<64;i++ )
    {
        QString pos = QString::number(i, 16).toUpper();
        if(pos.size()<2){ pos.prepend("0"); }

        QString txt =  QString::number(patch, 10);
        if (txt.size()<2) {txt.prepend("00"); }
        if (txt.size()<3) {txt.prepend("0"); }
        if (count < 129 )               {txt.prepend("[Bank#0:PC#"); }
        else if (count > 128 && count < 257) {txt.prepend("[Bank#1:PC#"); }
        else if (count > 256 && count < 385) {txt.prepend("[Bank#2:PC#"); }
        else                                 {txt.prepend("[Bank#3:PC#"); }
        txt.append("] = ");

        int value1 = sysxData.at(sysxDataOffset + i).toInt(&ok, 16);;
        int value2 = sysxData.at(sysxDataOffset + i + 1).toInt(&ok, 16);;
        int value = (value1*128) + value2;
        QString valueHex = QString::number(value, 16).toUpper();
        if(valueHex.length() < 2) {valueHex.prepend("0"); }

        text.append(txt);
        text.append(midiTable->getValue("System01", "04", "10", "00", valueHex) );
        text.append(".    .");
        ++row;
        if (row >= 5) {row = 1; text.append("<br>"); }
        ++count;
        ++patch;
        ++i;
    }
}



