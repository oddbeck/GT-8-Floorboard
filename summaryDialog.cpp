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
#include "summaryDialog.h"
#include "Preferences.h"
#include "SysxIO.h"
#include "MidiTable.h"
#include "globalVariables.h"

summaryDialog::summaryDialog(QWidget *parent)
    : QWidget(parent)
{
    MidiTable *midiTable = MidiTable::Instance();
    this->mode = "Compact";
    this->filter = "off";
    this->textDialog = new QTextEdit(parent);
    textDialog->setReadOnly(true);
    //textDialog->setWordWrapMode(QTextOption::NoWrap);
    textDialog->setDocumentTitle(deviceType + tr(" Patch settings"));
    textDialog->setMinimumHeight(600);

    SysxIO *sysxIO = SysxIO::Instance();
    QString sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            hex = data.at(x);
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
            if(hex == "F7"){ sysxMsg.append("<br>"); };
        };
    };
    /******************************************************
  ******QString "sysxMsg" contains current patch data *****
  ******************************************************/
    QDateTime dateTime = QDateTime::currentDateTime();
    text = dateTime.toString();
    text.append("<br>");
    small_text = text;
    small_text.append("<b><u>Boss GT-8 Compact Patch Summary</u></b><br>");
    small_text.append("a listing of active only effects.<br>");
    large_text = text;
    large_text.append("<b><u>Boss GT-8 Complete Patch Summary</u></b><br>");
    large_text.append("a list of all parameters with-in this patch.<br>");



    QString patchName = sysxIO->getCurrentPatchName();
    text.append(tr("<br><b><u>Patch name = </u>") + patchName + "</b>");

    /*text.append(tr("<br><br><b>Patch Mode Output Select = </b>"));
  int value = sysxIO->getSourceValue("Structure", "00", "00", "11");
  QString valueHex = QString::number(value, 16).toUpper();
  if(valueHex.length() < 2) {valueHex.prepend("0"); };
  text.append(midiTable->getValue("Structure", "00", "00", "11", valueHex) ); */

    QList<QString> fxChain = sysxIO->getFileSource("Structure", "11", "00");
    QString chainText = "<br><br><b><u>**********Signal Chain**********</u></b><br>Input = -> ";
    QString chainData;
    QString chain;
    for(int i= sysxDataOffset;i< (sysxDataOffset + 14);i++ )
    {
        chain.append(" [");
        chain.append( midiTable->getMidiMap("Structure", "11", "00", "00", fxChain.at(i)).name );
        chain.append("]");
    };
    chain.replace("PRE", "PreAmp");
    chain.replace("LP", "Send/Return");
    chain.replace("CS", "Comp");
    chain.replace("OD", "Dist/ODrive");
    chain.replace("NS", "N.Suppr");
    chain.replace("FV", "Foot Vol");
    chain.replace("DD", "Delay");
    chain.replace("RV", "Reverb");
    chain.replace("CE", "Chorus");
    chain.replace("DGT", "Digital-Out");
    text.append(chainText);
    text.append(chain + " ->");

    this->effect = "off";
    text = "<br><br><b><u>**********PreAmp/Channel Control***********</b></u>";
    address= "07";
    start = 0;
    finish = 6;
    makeList();

    text.append("<br><br><b><u>**********Pre Amp A***********</b></u>");
    address= "07";
    start = 6;
    finish = 23;
    makeList();

    text.append("<br><br><b><u>**********Pre Amp B***********</b></u>");
    address= "07";
    start = 23;
    finish = 40;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Compressor***********</b></u>";
    address= "02";
    start = 0;
    finish = 9;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Distortion***********</b></u>";
    address= "06";
    start = 0;
    finish = 8;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Equalizer***********</b></u>";
    address= "08";
    start = 0;
    finish = 13;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Delay***********</b></u>";
    address= "0B";
    start = 0;
    finish = 26;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Chorus***********</b></u>";
    address= "0C";
    start = 0;
    finish = 9;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Reverb***********</b></u>";
    address= "0D";
    start = 0;
    finish = 10;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Wah***********</b></u>";
    address= "03";
    start = 0;
    finish = 5;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "on";
    text = "<br><br><b><u>**********Pedal & Misc***********</b></u>";
    address= "15";
    start = 0;
    finish = 3;
    makeList();
    address= "14";
    start = 0;
    finish = 2;
    makeList();
    address= "13";
    start = 0;
    finish = 2;
    makeList();
    address= "1E";
    start = 0;
    finish = 1;
    makeList();
    address= "0F";
    start = 0;
    finish = 1;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Master***********</b></u>";
    address= "0E";
    start = 4;
    finish = 9;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Noise Suppressor***********</b></u>";
    address= "0E";
    start = 0;
    finish = 4;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Loop***********</b></u>";
    address= "04";
    start = 0;
    finish = 5;
    makeList();

    this->effect = "off";
    text = "<br><br><b><u>**********FX-1***********</b></u>";
    text2 = text;
    address= "00";
    start = 0;
    finish = 128;
    makeList();
    address= "01";
    start = 0;
    finish = 2;
    makeList();
    large_text.append(text2);
    if(effect == "on") { small_text.append(text); };
    this->filter = "off";

    this->effect = "off";
    text = "<br><br><b><u>**********FX-2***********</b></u>";
    text2 = text;
    address= "09";
    start = 0;
    finish = 128;
    makeList();
    address= "0A";
    start = 0;
    finish = 92;
    makeList();
    large_text.append(text2);
    if(effect == "on") { small_text.append(text); };
    this->filter = "off";
    
    this->effect = "off";
    text = "<br><br><b><u>**********Assign 1***********</b></u>";
    address= "16";
    start = 0;
    finish = 17;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Assign 2***********</b></u>";
    address= "17";
    start = 0;
    finish = 17;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Assign 3***********</b></u>";
    address= "18";
    start = 0;
    finish = 17;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Assign 4***********</b></u>";
    address= "19";
    start = 0;
    finish = 17;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Assign 5***********</b></u>";
    address= "1A";
    start = 0;
    finish = 17;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Assign 6***********</b></u>";
    address= "1B";
    start = 0;
    finish = 17;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Assign 7***********</b></u>";
    address= "1C";
    start = 0;
    finish = 17;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    this->effect = "off";
    text = "<br><br><b><u>**********Assign 8***********</b></u>";
    address= "1D";
    start = 0;
    finish = 17;
    makeList();
    large_text.append(text);
    if(effect == "on") { small_text.append(text); };

    text = "<br><br><b><u>**********Patch Data***********</b></u><br>";
    text.append(sysxMsg);
    large_text.append(text);

    textDialog->setText(small_text);
    textDialog->show();


    QPushButton *cancelButton = new QPushButton(tr("Close"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    cancelButton->setWhatsThis(tr("Will close the currently open Dialog page."));

    QPushButton *printButton = new QPushButton(tr("Print"));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printFile()));
    printButton->setWhatsThis(tr("Will Print the current Dialog to the prefered printer<br>printed text will be simular to the screen layout."));

    QPushButton *printPreviewButton = new QPushButton(tr("Print Preview"));
    connect(printPreviewButton, SIGNAL(clicked()), this, SLOT(printPreview()));
    printPreviewButton->setWhatsThis(tr("Will Print the current Dialog to the prefered printer<br>printed text will be simular to the screen layout."));

    QPushButton *saveAsButton = new QPushButton(tr("Save As"));
    connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
    saveAsButton->setWhatsThis(tr("Will save the current dialog page to file in a *.txt format."));

    QPushButton *viewButton = new QPushButton(tr("Change View"));
    connect(viewButton, SIGNAL(clicked()), this, SLOT(view()));
    viewButton->setWhatsThis(tr("Will Expand the summary dialog to include all patch parameters."));



    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(textDialog);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(printButton);
    //buttonsLayout->addSpacing(12);
    //buttonsLayout->addWidget(printPreviewButton);
    buttonsLayout->addSpacing(12);
    buttonsLayout->addWidget(saveAsButton);
    buttonsLayout->addSpacing(12);
    buttonsLayout->addWidget(viewButton);
    buttonsLayout->addSpacing(12);
    buttonsLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(deviceType + tr(" Patch Summary of ")+ patchName);
}

void summaryDialog::makeList()
{
    // construct a text string using address and locator parameters to read from midi.xml
    SysxIO *sysxIO = SysxIO::Instance();
    MidiTable *midiTable = MidiTable::Instance();
    QString assign_desc;
    QString assign_customdesc;
    for(int i=start;i<finish;i++ )  //start and finish range defined above.
    {
        QString temp;
        QString pos = QString::number(i, 16).toUpper();
        if(pos.size()<2){ pos.prepend("0"); };
        QString txt = midiTable->getMidiMap("Structure", address, "00", pos).customdesc;  //trawl through midi.xml
        if(!txt.isEmpty() && txt != "") // skip the empty midi.xml .desc section and move to the next.
        {
            QString pretxt =  midiTable->getMidiMap("Structure", address, "00", pos).desc;
            int value = sysxIO->getSourceValue("Structure", address, "00", pos);
            QString valueHex = QString::number(value, 16).toUpper();
            if(valueHex.length() < 2) {valueHex.prepend("0"); };
            temp.append("<br>");
            temp.append("[");
            if(!pretxt.isEmpty() && txt != "") { temp.append(pretxt + " "); };
            temp.append(txt);
            temp.append("] = ");
            //QString x = midiTable->getValue("Structure", address, "00", pos, valueHex);
            //{temp.append(x); };
            QString x;
            if(pretxt.contains("Assign") && (txt.contains("Target")) )
            {
                bool ok;
                temp.append(midiTable->getValue("Structure", address, "00", pos, valueHex));
                int maxRange = 128;
                value = valueHex.toInt(&ok, 16);
                int dif = value/maxRange;
                QString valueHex1 = QString::number(dif, 16).toUpper();
                if(valueHex1.length() < 2) valueHex1.prepend("0");
                QString valueHex2 = QString::number(value - (dif * maxRange), 16).toUpper();
                if(valueHex2.length() < 2) valueHex2.prepend("0");
                QString hex4 = valueHex1;
                QString hex5 = valueHex2;
                Midi items = midiTable->getMidiMap("Structure", "16", "00", "02", hex4, hex5);
                assign_desc = items.desc;
                assign_customdesc = items.customdesc;
            }
            else if(pretxt.contains("Assign") && ((txt.contains("Min") || txt.contains("Max"))))
            {
                temp.append(midiTable->getValue("Structure", assign_desc, "00", assign_customdesc, valueHex));
            }
            else
            {
                temp.append(midiTable->getValue("Structure", address, "00", pos, valueHex));
            };
            text2.append(temp);

            if (this->filter != "off")
            {
                if (pretxt.contains(this->filter)) { text.append(temp); };
            } else if(!pretxt.contains("Custom:")){text.append(temp); };
            if(i == start && x == "On") { this->effect = "on"; }; // first byte is usually the effect on/off switch
            if((pretxt == "FX1:" || pretxt == "FX2:") && (txt == "Type"))
            {this->filter = midiTable->getMidiMap("Structure", address, "00", pos, valueHex).desc;};
        };
    };
}

summaryDialog::~summaryDialog()
{
    this->deleteLater();
}

void summaryDialog::view()
{
    if (mode == "Compact")
    {
        textDialog->setText(large_text);
        this->mode = "Expanded";
    }
    else
    {
        textDialog->setText(small_text);
        this->mode = "Compact";
    };
}

void summaryDialog::cancel()
{
    this->close();
}

void summaryDialog::printFile()
{
#ifndef QT_NO_PRINTER

    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    //QPrintPreviewDialog *dialog = new QPrintPreviewDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted) { return; }
    else { textDialog->print(&printer); };
#endif
}

void summaryDialog::printPreview()
{
#ifndef QT_NO_PRINTER

    QPrinter printer;
    //QPrintDialog *dialog = new QPrintDialog(&printer, this);
    QPrintPreviewDialog *dialog = new QPrintPreviewDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Preview"));
    if (dialog->exec() != QDialog::Accepted) { return; }
    else { textDialog->print(&printer); };
#endif
}

void summaryDialog::saveAs()
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
        };
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
        };

        if (file.open(QIODevice::WriteOnly))
        {
            file.write(out);
        };
    };
}
