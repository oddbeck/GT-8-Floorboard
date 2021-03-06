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
#include <QDataStream>
#include <QByteArray>
#include <QMessageBox>
#include "bulkLoadDialog.h"
#include "Preferences.h"
#include "globalVariables.h"


bulkLoadDialog::bulkLoadDialog()
{ 
    failed = true;
    QLabel *startListLabel = new QLabel(tr("Starting from"));
    this->startPatchCombo = new QComboBox(this);
    QLabel *finishListLabel = new QLabel(tr("Finishing at"));
    this->finishPatchCombo = new QComboBox(this);
    QObject::connect(startPatchCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboValueChanged(int)));
    QObject::connect(finishPatchCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboValueChanged(int)));
    QVBoxLayout *comboBoxLayout = new QVBoxLayout;
    comboBoxLayout->addWidget(startListLabel);
    comboBoxLayout->addWidget(startPatchCombo);
    comboBoxLayout->addStretch(1);
    comboBoxLayout->addSpacing(12);
    comboBoxLayout->addWidget(finishListLabel);
    comboBoxLayout->addWidget(finishPatchCombo);

    QGroupBox *patchListGroup = new QGroupBox(tr("Set the Range of Patch data to restore"));
    patchListGroup->setLayout(comboBoxLayout);

    QLabel *startRangeLabel = new QLabel(tr("Start Bank."));
    QLabel *finishRangeLabel = new QLabel(tr("Finish Bank."));

    this->startRangeComboBox = new QComboBox(this);
    QObject::connect(startRangeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboValueChanged(int)));
    this->finishRange = new QLineEdit(this);
    this->finishRange->setReadOnly(true);

    QVBoxLayout *rangeLabelLayout = new QVBoxLayout;
    rangeLabelLayout->addSpacing(12);
    rangeLabelLayout->addWidget(startRangeLabel);
    rangeLabelLayout->addSpacing(12);
    rangeLabelLayout->addWidget(finishRangeLabel);

    QHBoxLayout *spinBoxLayout = new QHBoxLayout;
    spinBoxLayout->addWidget(startRangeComboBox);
    
    QVBoxLayout *rangeBoxLayout = new QVBoxLayout;
    rangeBoxLayout->addSpacing(12);
    rangeBoxLayout->addLayout(spinBoxLayout);
    rangeBoxLayout->addSpacing(12);
    rangeBoxLayout->addWidget(finishRange);

    QHBoxLayout *dataRangeLayout = new QHBoxLayout;
    dataRangeLayout->addSpacing(20);
    dataRangeLayout->addLayout(rangeLabelLayout);
    dataRangeLayout->addLayout(rangeBoxLayout);
    dataRangeLayout->addSpacing(20);

    QGroupBox *patchRangeGroup = new QGroupBox(tr("Set start location of Patch data to restore"));
    patchRangeGroup->setLayout(dataRangeLayout);

    this->startButton = new QPushButton(this);
    this->startButton->setText(tr("Start"));
    connect(startButton, SIGNAL(clicked()), this, SLOT(sendData()));

    this->completedButton = new QPushButton(this);
    this->completedButton->setText(tr("DATA TRANSFER COMPLETED"));
    this->completedButton->hide();
    connect(completedButton, SIGNAL(clicked()), this, SLOT(DialogClose()));

    this->cancelButton = new QPushButton(this);
    this->cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(DialogClose()));

    this->progressLabel = new QLabel(this);
    this->progressLabel->setText(tr("Full Restoration may take a few minutes"));
    this->bytesLabel = new QLabel(this);
    this->bytesLabel->setText("");

    this->progressBar = new QProgressBar(this);
    this->progressBar->setTextVisible(false);
    this->progressBar->setRange(0, 100);
    this->progressBar->setValue(0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addSpacing(40);
    buttonsLayout->addWidget(startButton);
    buttonsLayout->addStretch(1);
    buttonsLayout->addSpacing(20);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addSpacing(40);

    QHBoxLayout *progressBarLayout = new QHBoxLayout;
    progressBarLayout->addWidget(progressBar);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(patchListGroup, Qt::AlignCenter);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout, Qt::AlignCenter);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(patchRangeGroup, Qt::AlignCenter);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(progressLabel, Qt::AlignCenter);

    mainLayout->addWidget(bytesLabel, Qt::AlignCenter);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(completedButton);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(progressBarLayout, Qt::AlignCenter);
    setLayout(mainLayout);

    setWindowTitle(tr("Bulk File Restoration"));

    bulk.clear();


    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(this, SIGNAL(setStatusSymbol(int)), sysxIO, SIGNAL(setStatusSymbol(int)));
    QObject::connect(this, SIGNAL(setStatusProgress(int)), sysxIO, SIGNAL(setStatusProgress(int)));
    QObject::connect(this, SIGNAL(setStatusMessage(QString)), sysxIO, SIGNAL(setStatusMessage(QString)));
    QObject::connect(this, SIGNAL(setdeBugStatusMessage(QString)), sysxIO, SIGNAL(setStatusdBugMessage(QString)));

    Preferences *preferences = Preferences::Instance();
    QString dir = preferences->getPreferences("General", "Files", "dir");

    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Choose a file"),
                dir,
                tr("GT-8 Bulk Data File (*.gte *.syx *.mid)"));
    if (!fileName.isEmpty())
    {

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            this->data = file.readAll();     // read the pre-selected file, copy to 'data'
            QByteArray GTE_default;
            QFile file(":default.syx");           // Read the default GT-8 sysx file .
            if (file.open(QIODevice::ReadOnly))
            {	default_data = file.readAll(); }
            QFile GTEfile(":default.gte");           // Read the default GT-8 GTE file .
            if (GTEfile.open(QIODevice::ReadOnly))
            {	GTE_default = GTEfile.readAll(); }
            QFile hexfile(":HexLookupTable.hex");           // Read the HexLookupTable for the SMF header file .
            if (hexfile.open(QIODevice::ReadOnly))
            {	this->hextable = hexfile.readAll(); }

            QByteArray default_header = default_data.mid(0, 7);           // copy header from default.syx
            QByteArray file_header = data.mid(0, 7);                      // copy header from read file.syx
            QByteArray GTE_header = GTE_default.mid(3, 20);                // copy header from default.gte
            QByteArray SMF_header = hextable.mid(288,18);
            QByteArray SMF_file = data.mid(0, 18);
            QByteArray patch_text = default_data.mid(1010, 11);
            if(data.contains(patch_text)) {this->patch_Size = 1241; } else { this->patch_Size = 1010; }
            unsigned char r = (char)data[7];     // find patch number in file (msb))
            bool ok;
            int patchNum;
            patchNum = QString::number(r, 16).toUpper().toInt(&ok, 16);
            bool isPatch = false;
            if (patchNum >= 8) { isPatch = true; }    // check the sysx file is a valid patch & not system data.
            if (data.size()>141400 && data.contains(default_header) )
            {
                int x = data.size()-141400;             // if there is temp data or excessive patches, trim off extras.
                data.remove(0, x);
            }

            bool isHeader = false;
            if (default_header == file_header) {isHeader = true;}
            bool isGTE = false;
            if (data.contains(GTE_header)){isGTE = true; }             // see if file is a GTE type and set isGTE flag.
            bool isSMF = false;
            if (data.contains(SMF_header)) {isSMF = true; }
            failed = false;
            if (isHeader == true && isPatch == true)
            {
                loadSYX();
            }
            else if (isGTE == true)
            {
                loadGTE();
            }
            else if (isSMF == true)
            {
                loadSMF();
            }
            else
            {
                failed = true;
                QMessageBox *msgBox = new QMessageBox();
                msgBox->setWindowTitle(QObject::tr("File Restore Error"));
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setTextFormat(Qt::RichText);
                QString msgText;
                msgText.append("<font size='+1'><b>");
                msgText.append(QObject::tr("This is not a known ") + deviceType + QObject::tr(" file!"));
                msgText.append("<b></font><br>");
                msgText.append(QObject::tr("Patch file not within allowable parameters or<br>"));
                msgText.append(QObject::tr("file format unknown, please try another file."));
                msgBox->setText(msgText);
                msgBox->setStandardButtons(QMessageBox::Ok);
                msgBox->exec();

            }
        }
    }
    if (failed == true)
    {
        this->startButton->hide();
    }
}

bulkLoadDialog::~bulkLoadDialog()
{
}

void bulkLoadDialog::comboValueChanged(int value)
{
    this->bankStart = this->startRangeComboBox->currentIndex();
    this->startList = this->startPatchCombo->currentIndex();
    this->finishList = this->finishPatchCombo->currentIndex();
    if (startList > finishList) {this->startPatchCombo->setCurrentIndex(finishList); }
    else if (finishList < startList) {this->finishPatchCombo->setCurrentIndex(startList); }
    int x = (bankStart+(finishList-startList));
    if (x<0) {x=0; } else if (x>((bankTotalUser*patchPerBank)-1))
    {
        x=((bankTotalUser*patchPerBank)-1);
        bankStart=((bankTotalUser*patchPerBank)-1)-(finishList-startList);
        startRangeComboBox->setCurrentIndex(((bankTotalUser*patchPerBank)-1)-(finishList-startList));
    }
    QString text = tr("Finish at U");
    int y = x/patchPerBank; y = y*patchPerBank; y=x-y;
    text.append(QString::number((x/patchPerBank)+1, 10).toUpper() );
    text.append("-");
    text.append(QString::number(y+1, 10).toUpper() );
    this->finishRange->setText(text);
    //this->startRangeComboBox->setMaxVisibleItems((bankTotalUser*patchPerBank)-(finishList-startList));
} 

void bulkLoadDialog::sendData() 
{	
    bankStart = this->startRangeComboBox->currentIndex()+1;
    startList = this->startPatchCombo->currentIndex();
    finishList = this->finishPatchCombo->currentIndex();
    startButton->hide();
    //cancelButton->hide();
    progress = 0;
    patch = 1;
    range = (100)/((finishList-startList)+1);

    int z = (this->bankStart);
    QString q;
    QString address;
    QString msg;
    QString v;
    QString addrMSB = "08";
    QString replyMsg;
    for (int a=startList;a<(finishList+1);a++)
    {
        if (z>128) {z=z-128; addrMSB = "09"; }          // next address range when > 08 7F.
        address = QString::number(z-1, 16).toUpper();
        if (address.size()<2){ address.prepend("0"); }
        int b = a*patch_Size;                                // multiples of patch size.
        msg = this->bulk.mid(b*2, patch_Size*2);             // copy next patch from bulk patch list.
        for (int g=0;g<msg.size()/2;g++)
        {
            v = msg.mid(g*2, 2);
            if (v == "F0") {msg.replace((g*2)+(sysxAddressOffset*2), 2, addrMSB); msg.replace((g*2)+((sysxAddressOffset*2)+2), 2, address); }   // replace the message address
        }
        replyMsg.append(msg);
        ++z;
    }

    QString reBuild = "";       // Add correct checksum to patch strings
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
    this->bulk = reBuild.simplified().toUpper();
    bank = (bankStart+(patchPerBank-1));
    patch = bank/patchPerBank; patch = patch*patchPerBank; patch=bank-patch;
    steps=0;
    dataSent=0;
    sendSequence("");
}	

void bulkLoadDialog::sendPatch(QString data)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(sysxIO->isConnected() && sysxIO->deviceReady())
    {
    QObject::connect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(sendSequence(QString)));
    sysxIO->sendSysx(data);
    }
    else
    {
       DialogClose();
    }
}                                                           

void bulkLoadDialog::sendSequence(QString value)
{ 
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(sendSequence(QString)));
    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
    msg=bulk.mid(steps*(patch_Size*2), (patch_Size*2));

    progress=progress+range;
    bulkStatusProgress(this->progress);                         // advance the progressbar.

    if (steps<((finishList-startList)+2) )
    {
        bool ok;
        QString patchText;
        QString name = msg.mid((sysxNameOffset*2), (nameLength*2));  // get name from loaded patch.
        QList<QString> x;
        for (int b=0;b<nameLength;b++)
        {
            x.append(name.mid(b*2, 2));
        }
        for (int b=0;b<nameLength;b++)
        {
            QString hexStr = x.at(b);
            patchText.append( (char)(hexStr.toInt(&ok, 16)) );      // convert name to readable text characters.
        }
        int bf = (finishList-startList);
        if(steps>bf) {this->completedButton->show();
            this->progressLabel->setText(tr("Bulk data transfer completed!!"));
            this->progress=100;  }
        
        QString patchNumber = QString::number(bank/4, 10).toUpper();
        patchNumber.prepend(tr("User Patch U" ));
        patchNumber.append("-");
        patchNumber.append( QString::number(patch, 10).toUpper() );
        patchNumber.append("     ");
        patchNumber.append(patchText);
        patchText=patchNumber;
        this->progressLabel->setText(patchText);                        //display the patch number and name.

        patchNumber = tr("File transfer size = ");
        dataSent = dataSent+(msg.size()/2);
        patchNumber.append(QString::number(dataSent, 10).toUpper() );
        patchNumber.append(tr(" bytes"));
        this->bytesLabel->setText(patchNumber);                         //display the bulk data size.
        ++steps;
        ++patch;
        if(patch>patchPerBank) {patch=1; bank=bank+patchPerBank;}	       // increment patch.
        sendPatch(msg);                                 //request the next patch.
        setStatusMessage(tr("Sending Data"));
    } else {
        QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(sendSequence(QString)));
        sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
        setStatusMessage(tr("Ready"));
        DialogClose();
    }
}


void bulkLoadDialog::updatePatch()
{
    patchCount = sysxPatches.size()/(patch_Size);
    QString msgText;
    QString patchText;
    QString patchNumber;
    unsigned char r;
    this->patchList.clear();
    unsigned int a = sysxNameOffset;             // locate patch text start position from the start of the file
    for (int h=0;h<patchCount;h++)
    {
        for (int b=0;b<nameLength;b++)
        {
            r = (char)sysxPatches[a+b];
            patchText.append(r);
        }
        patchNumber = QString::number(h+1, 10).toUpper();
        msgText.append(patchNumber + " : ");
        msgText.append(patchText + "   ");
        this->patchList.append(msgText);
        patchText.clear();
        msgText.clear();
        a=a+patch_Size;                                // advance to the next patch in the bulk file.
    }
    this->startPatchCombo->addItems(patchList);            // add patch names to the combobox lists.
    this->finishPatchCombo->addItems(patchList);
    this->finishPatchCombo->setCurrentIndex(patchCount-1);     // set the finish combobox index to the end of the list.
    this->startPatchCombo->setCurrentIndex(0);
    QString text = tr("Finish at U");
    if (patchCount<4) {patchCount=4; }
    text.append(QString::number(patchCount/4, 10).toUpper() );
    text.append("-x");
    this->finishRange->setText(text);

    QString U = "U";
    for (int x=0; x<bankTotalUser; x++)
    {
        QString bnk = "U" + QString::number(x+1, 10).toUpper() + "-1";
        this->startRangeComboBox->addItem(bnk);
        bnk = "U" + QString::number(x+1, 10).toUpper() + "-2";
        this->startRangeComboBox->addItem(bnk);
        bnk = "U" + QString::number(x+1, 10).toUpper() + "-3";
        this->startRangeComboBox->addItem(bnk);
        bnk = "U" + QString::number(x+1, 10).toUpper() + "-4";
        this->startRangeComboBox->addItem(bnk);
    }
    this->startRangeComboBox->setCurrentIndex(0);

    QString sysxBuffer;
    for(int i=0;i<sysxPatches.size();i++)
    {
        unsigned char byte = (char)sysxPatches[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    }
    bulk.append(sysxBuffer);
}

void bulkLoadDialog::bulkStatusProgress(int value)
{
    if (value >100) {value = 100;}
    if (value<0) {value = 0; }
    this->progressBar->setValue(value);
}

void bulkLoadDialog::loadGTE()         // ************************************ GTE File Format***************************
{	
    unsigned char r = (char)data[35];     // find hex patch count in GTE file at byte 35, 1~140
    bool ok;
    int count;
    count = QString::number(r, 16).toUpper().toInt(&ok, 16);
    int a=160;                             // offset is set to first patch start.
    for (int h=0;h<count;h++)
    {
        QByteArray temp;
        temp = data.mid(a+8, 130);
        default_data.replace(11, 130, temp);    //address "00"  ++   fx1

        temp = data.mid(a+142, 9);
        default_data.replace(154, 9, temp);     //address "02"  ++

        temp = data.mid(a+155, 5);
        default_data.replace(176, 5, temp);     //address "03"  ++

        temp = data.mid(a+164, 5);
        default_data.replace(194, 5, temp);    //address "04"  ++

        //temp = data.mid(a+180, 1);
        //default_data.replace(220, 1, temp);     //address "05"

        temp = data.mid(a+186, 8);
        default_data.replace(212, 8, temp);     //address "06"  ++

        temp = data.mid(a+198, 40);
        default_data.replace(233, 40, temp);    //address "07"  ++

        temp = data.mid(a+242, 13);
        default_data.replace(286, 13, temp);    //address "08"  ++
        
        temp = data.mid(a+259, 220);
        default_data.replace(312, 220, temp);   //address "09"  ++

        temp = data.mid(a+483, 26);
        default_data.replace(545, 26, temp);    //address "0B"  ++

        temp = data.mid(a+513, 9);
        default_data.replace(584, 9, temp);     //address "0C"  ++

        temp = data.mid(a+526, 10);
        default_data.replace(606, 10, temp);    //address "0D"  ++

        temp = data.mid(a+540, 9);
        default_data.replace(629, 9, temp);    //address "0E"  ++

        temp = data.mid(a+554, 1);
        default_data.replace(651, 1, temp);     //address "0F"  ++

        //temp = data.mid(a+559, 1);
        //default_data.replace(688, 1, temp);     //address "10"

        temp = data.mid(a+564, 14);
        default_data.replace(665, 14, temp);    //address "11" ++   chain

        temp = data.mid(a+582, 16);
        default_data.replace(692, 16, temp);    //address "12" ++   name

        temp = data.mid(a+598, 1);
        default_data.replace(1007, 1, temp);     //address "1E"
        /*
   temp = data.mid(a+794, 17);
   default_data.replace(772, 17, temp);    //address "20"
   
   temp = data.mid(a+815, 17);
   default_data.replace(802, 17, temp);    //address "21"
   
   temp = data.mid(a+836, 17);
   default_data.replace(832, 17, temp);    //address "22"
   
   temp = data.mid(a+857, 17);
   default_data.replace(862, 17, temp);    //address "23"
   
   temp = data.mid(a+878, 17);
   default_data.replace(892, 17, temp);    //address "24"
   
   temp = data.mid(a+899, 17);
   default_data.replace(922, 17, temp);    //address "25"

   temp = data.mid(a+920, 17);
   default_data.replace(952, 17, temp);    //address "26"
   
   temp = data.mid(a+941, 17);
   default_data.replace(982, 17, temp);    //address "27"
   
   temp = data.mid(a+962, 17);
   default_data.replace(1012, 17, temp);   //address "28"
   
   temp = data.mid(a+983, 17);
   default_data.replace(1042, 17, temp);   //address "29"
   
   temp = data.mid(a+1004, 17);
   default_data.replace(1072, 17, temp);   //address "2A"
   
   temp = data.mid(a+1025, 17);
   default_data.replace(1102, 17, temp);   //address "2B"

   temp = data.mid(a+1046, 3);
   default_data.replace(1132, 3, temp);    //address "2C"

   temp = data.mid(a+1053, 3);
   default_data.replace(1142, 3, temp);    //address "2D"       */
        default_data.remove(1010, 231);         // trim the user text content

        a=a+1060;                      // offset is set for next patch.
        this->sysxPatches.append(default_data);
    }
    updatePatch();
}

void bulkLoadDialog::loadSYX()        //********************************* SYX File Format *****************************
{	
    sysxPatches = data;
    updatePatch();
}

void bulkLoadDialog::loadSMF()    // **************************** SMF FILE FORMAT ***************************
{	

    QByteArray temp;                         // TRANSLATION of GT-10B SMF PATCHES, data read from smf patch **************
    if ( data.at(37) != default_data.at(5) ){    // check if a valid GT-10B file
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(QObject::tr("SMF file import"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText;
        msgText.append("<font size='+1'><b>");
        msgText.append(QObject::tr("This is not a GT-8 file!"));
        msgText.append("<b></font><br>");
        msgText.append(QObject::tr("this file is a GT-Pro version<br>"));
        msgText.append(QObject::tr("*Loading this file may have unpredictable results*."));
        msgBox->setText(msgText);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    }
    int count = (data.size()-32)/1806;
    int a=0;                             // offset is set to first patch
    for (int h=0;h<count;h++)
    {
        temp = data.mid(a+43, 128);            // copy SMF 128 bytes
        default_data.replace(11, 128, temp);             // replace gt10 address "00"...
        temp = data.mid(a+171, 114);           // copy SMF part1...
        temp.append(data.mid(a+301,14));       // copy SMF part2...
        default_data.replace(152, 128, temp);            // replace gt10 address "01"...
        temp = data.mid(a+315, 128);           // copy SMF part1...
        default_data.replace(293, 128, temp);            // replace gt10 address "02"...
        temp = data.mid(a+443, 100);           // copy SMF part1...
        default_data.replace(434, 100, temp);            // replace gt10 address "03"...
        temp = data.mid(a+715, 86);            // copy SMF part1...
        temp.append(data.mid(a+817,42));       // copy SMF part2...
        default_data.replace(547, 128, temp);             // replace gt10 address "05"...
        temp = data.mid(a+859, 128);           // copy SMF part1...
        default_data.replace(688,128, temp);             // replace gt10 address "06"...
        temp = data.mid(a+987, 72);            // copy SMF part1...
        temp.append(data.mid(a+1075,28));      // copy SMF part2...
        default_data.replace(829, 100, temp);            // replace gt10 address "07"...
        temp = data.mid(a+1259, 58);           // copy SMF part1...
        temp.append(data.mid(a+1333,42));      // copy SMF part2...
        default_data.replace(942, 100, temp);           // replace gt10 address "09"...
        temp = data.mid(a+1403, 128);          // copy SMF part1...
        default_data.replace(1083,128, temp);            // replace gt10 address "0A"...
        temp = data.mid(a+1531, 44);           // copy SMF part1...
        temp.append(data.mid(a+1591,84));      // copy SMF part2...
        default_data.replace(1224, 128, temp);           // replace gt10 address "0B"...
        temp = data.mid(a+1675, 128);          // copy SMF part1...
        default_data.replace(1365,128, temp);            // replace gt10 address "0C"...
        a=a+1806;                      // offset is set in front of marker
        temp = default_data;
        //this->sysxPatches.append(temp);
    }
    //updatePatch();
}

void bulkLoadDialog::DialogClose()
{
    steps = 300;
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(sendSequence(QString)));
    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
    setStatusMessage(tr("Ready"));
    this->deleteLater();
}
