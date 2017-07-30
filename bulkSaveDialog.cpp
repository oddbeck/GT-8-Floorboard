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
#include "bulkSaveDialog.h"
#include "Preferences.h"
#include "globalVariables.h"


// Platform-dependent sleep routines.
#ifdef Q_OS_WIN
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants & Mac
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

bulkSaveDialog::bulkSaveDialog()
{ 
  QLabel *startRangeLabel = new QLabel(tr("Start Bank."));
	QLabel *finishRangeLabel = new QLabel(tr("Finish Bank."));
	
  this->gxbButton = new QRadioButton(tr("*.gte Librarian file"), this );
  this->syxButton = new QRadioButton(tr("*.syx System Exclusive file"), this );
  this->midButton = new QRadioButton(tr("*.mid Standard Midi (SMF) file"), this );
  this->gxbButton->setChecked(true);
    
	QCheckBox *systemCheckBox = new QCheckBox(tr("Save System Data"));
	QSpinBox *startRangeSpinBox = new QSpinBox;
	QSpinBox *finishRangeSpinBox = new QSpinBox;

	this->systemCheckBox = systemCheckBox;
		
	this->startRangeSpinBox = startRangeSpinBox;
	startRangeSpinBox->setValue(1);
	startRangeSpinBox->setRange(1, 35);
	startRangeSpinBox->setPrefix(tr("Start at U"));
	startRangeSpinBox->setSuffix("-1");

	this->finishRangeSpinBox = finishRangeSpinBox;
	finishRangeSpinBox->setValue(35);    
	finishRangeSpinBox->setRange(1, 35);
	finishRangeSpinBox->setPrefix(tr("Finish at U"));
	finishRangeSpinBox->setSuffix("-4");

	QVBoxLayout *rangeLabelLayout = new QVBoxLayout;
	rangeLabelLayout->addSpacing(12);
	rangeLabelLayout->addWidget(startRangeLabel);
	rangeLabelLayout->addWidget(finishRangeLabel);

	QVBoxLayout *rangeBoxLayout = new QVBoxLayout;
	//rangeBoxLayout->addWidget(systemCheckBox);
	rangeBoxLayout->addSpacing(12);
	rangeBoxLayout->addWidget(startRangeSpinBox);
	rangeBoxLayout->addSpacing(12);
	rangeBoxLayout->addWidget(finishRangeSpinBox);

	QHBoxLayout *dataRangeLayout = new QHBoxLayout;
	dataRangeLayout->addSpacing(20);
	dataRangeLayout->addLayout(rangeLabelLayout);
	//dataRangeLayout->addSpacing(12);
	dataRangeLayout->addLayout(rangeBoxLayout);
	dataRangeLayout->addSpacing(20);

	QGroupBox *patchRangeGroup = new QGroupBox(tr("Set range of Patch data to save"));
  patchRangeGroup->setLayout(dataRangeLayout);

  this->startButton = new QPushButton(this);
  this->startButton->setText("Start");
  connect(startButton, SIGNAL(clicked()), this, SLOT(backup()));
  
  this->completedButton = new QPushButton(this);
  this->completedButton->setText(tr("DATA TRANSFER COMPLETED"));
  this->completedButton->hide();
  connect(completedButton, SIGNAL(clicked()), this, SLOT(DialogClose()));
  
	this->cancelButton = new QPushButton(this);
	this->cancelButton->setText(tr("Cancel"));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(DialogClose()));
  
  this->progressLabel = new QLabel(this);
  this->progressLabel->setText(tr("Full Backup may take a few minutes"));
  this->bytesLabel = new QLabel(this);
  this->bytesLabel->setText("");
  
  this->progressBar = new QProgressBar(this);
	this->progressBar->setTextVisible(false);
	this->progressBar->setRange(0, 100);
	this->progressBar->setValue(0);  
	
	QVBoxLayout *fileFormatLayout = new QVBoxLayout;
	fileFormatLayout->addStretch(1);
	fileFormatLayout->addSpacing(6);
	fileFormatLayout->addWidget(gxbButton);
	fileFormatLayout->addStretch(1);
	fileFormatLayout->addSpacing(6);    
	fileFormatLayout->addWidget(syxButton);
	fileFormatLayout->addStretch(1);
	fileFormatLayout->addSpacing(6);
	fileFormatLayout->addWidget(midButton);  
	fileFormatLayout->addSpacing(6);    
	 QGroupBox *fileFormatGroup = new QGroupBox(tr("Select File format to save"));
  fileFormatGroup->setLayout(fileFormatLayout);
 
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
	mainLayout->addWidget(fileFormatGroup);
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

	setWindowTitle(tr("Bulk File Backup"));
	
	//QObject::connect(this, SIGNAL( startRangeSpinBox->valueChanged(int) ), this, SLOT( bankStart(int) ));
	//QObject::connect(this, SIGNAL( finishRangeSpinBox->valueChanged(int) ), this, SLOT( bankFinish(int) ));
	SysxIO *sysxIO = SysxIO::Instance();
	QObject::connect(this, SIGNAL(setStatusSymbol(int)), sysxIO, SIGNAL(setStatusSymbol(int)));
	QObject::connect(this, SIGNAL(setStatusProgress(int)), sysxIO, SIGNAL(setStatusProgress(int)));
	QObject::connect(this, SIGNAL(setStatusMessage(QString)), sysxIO, SIGNAL(setStatusMessage(QString)));	  
}

bulkSaveDialog::~bulkSaveDialog()
{
}

void bulkSaveDialog::backup()
{
  this->startButton->hide();
  //this->cancelButton->hide();
  this->bankStart = this->startRangeSpinBox->value();
  this->bankFinish = this->finishRangeSpinBox->value();
  if (bankFinish<bankStart) {bankFinish = bankStart; this->finishRangeSpinBox->setValue(bankStart); };
  this->bank=bankStart*4;
	bulk.clear();
	this->progress = 0;
	this->patch = 1;
	range = 200/(bankFinish-bankStart+1);
	requestPatch(bank/4, patch);  
}

void bulkSaveDialog::requestPatch(int bank, int patch) 
{	
SysxIO *sysxIO = SysxIO::Instance();
	if(sysxIO->isConnected() && sysxIO->deviceReady() )
	{
	  SysxIO *sysxIO = SysxIO::Instance();
		QObject::connect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(updatePatch(QString)));					
		sysxIO->requestPatch(bank, patch);    		
	};
}

void bulkSaveDialog::updatePatch(QString replyMsg)
{
	SysxIO *sysxIO = SysxIO::Instance(); 
	QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(updatePatch(QString)));		
	sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
	
	replyMsg = replyMsg.remove(" ").toUpper();       // TRANSLATE SYSX MESSAGE FORMAT to 128 byte data blocks 
	if (replyMsg.size()/2 == 1010){              // size of patch sent from GT10B   
	
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
	  { dataSize += hex.mid(h*2, 2).toInt(&ok, 16); };
	 	QString base = "80";                       // checksum calculate.
	  unsigned int sum = dataSize % base.toInt(&ok, 16);
  	if(sum!=0) { sum = base.toInt(&ok, 16) - sum; };
	  QString checksum = QString::number(sum, 16).toUpper();
	   if(checksum.length()<2) {checksum.prepend("0");};
      	hex.append(checksum);
        hex.append("F7");   
        reBuild.append(hex);   
    
		hex = "";
		sysxEOF = "";
		i=i+2;
    }; 
  };    
	replyMsg = reBuild.simplified().toUpper();	
	bulk.append(replyMsg); 	                                           // add patch to the bulk string.
	}; 	       
	      ++patch; 
        if(patch>4) {patch=1; bank=bank+4;};	                      // increment patch.
        progress=progress+range;
        bulkStatusProgress(this->progress);                         // advance the progressbar.
        int bf = (bankFinish+1)*4 -2;
  if (bank >= bf) 
      {                                                            // check if nearly finished.
        this->completedButton->show();        
        this->progressLabel->setText(tr("Bulk data transfer completed!!"));              
      };              
  if (bank<(bankFinish+1)*4 )
  {      
        bool ok;
        QString patchText;
        QString name = replyMsg.mid(1384, 32);                       // get name from loaded patch.
        QList<QString> x;        
        for (int b=0;b<16;b++)
        {
          x.append(name.mid(b*2, 2));       
        };
        for (int b=0;b<16;b++)
        {
        	QString hexStr = x.at(b);			
		      patchText.append( (char)(hexStr.toInt(&ok, 16)) );      // convert name to readable text characters.
        };
        
  QString patchNumber = QString::number(bank/4, 10).toUpper();
  if (patchNumber.size()<2) { patchNumber.prepend("0"); };
  patchNumber.prepend(tr( "User Patch U" ));
  patchNumber.append("-");
  patchNumber.append( QString::number(patch, 10).toUpper() );
  patchNumber.append("     ");
  patchNumber.append(patchText);
  patchText=patchNumber;   
  this->progressLabel->setText(patchText);                        //display the patch number and name.
  
  patchNumber = "File build size = ";
  int size = (bulk.size()/2);
  patchNumber.append(QString::number(size, 10).toUpper() );
  patchNumber.append(tr(" bytes"));
  this->bytesLabel->setText(patchNumber);                         //display the bulk data size.
  
  setStatusMessage(tr("Bulk Download"));  
  requestPatch(bank/4, patch);                                   //request the next patch.
  } else {
  setStatusMessage(tr("Ready"));
  sysxIO->bulk = this->bulk;
  if (this->gxbButton->isChecked() ) { writeSYX(); };// { writeGXB(); };                      // format and write bulk patches.
  if (this->syxButton->isChecked() ) { writeSYX(); };
  if (this->midButton->isChecked() ) { writeSYX(); };//{ writeSMF(); };
  };  
}

void bulkSaveDialog::bulkStatusProgress(int value)
{
   value=value/8;
  if (value >100) {value = 100;};
  if (value<0) {value = 0; };
	this->progressBar->setValue(value);
}

void bulkSaveDialog::writeGXB()         // ************************************ GXB File Format***************************
{	
  	Preferences *preferences = Preferences::Instance();
	          QString dir = preferences->getPreferences("General", "Files", "dir");

          	QString fileName = QFileDialog::getSaveFileName(
                    this,
                    tr("Save Bulk Data"),
                    dir,
                    tr("Librarian Backup File (*.gxb)"));
	             if (!fileName.isEmpty())	
                	{
	                  if(!fileName.contains(".gxb"))
	                     	{
		                    	fileName.append(".gxb");
	                     	};		
		
	QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
	{	
     bool ok;	
		QByteArray out;
		int size = this->bulk.size()/2;
	  int patchCount = size/1010;
		for (int x=0;x<size*2;x++)
		{
		   QString hexStr = bulk.mid(x, 2);
		   out.append( (char)(hexStr.toInt(&ok, 16)) );          // convert the bulk QString to a QByteArray.
		    ++x;
		};
		QByteArray bulkFile;
		QByteArray GXB_default;
		QByteArray temp;
		QFile GXBfile(":default.gte");           // Read the default GT-8 Gte file .
    if (GXBfile.open(QIODevice::ReadOnly))
	  {	GXB_default = GXBfile.readAll(); };
	  bulkFile.append(GXB_default.mid(0, 160));
	  int b = 0;
		for (int x=0;x<patchCount;x++)
		{  
   int a = 172;    
   temp = out.mid(b+11, 128);
   GXB_default.replace(a, 128, temp);         //address "00" +   
   temp = out.mid(b+152, 128);
   GXB_default.replace(a+128, 128, temp);     //address "01" +      
   temp = out.mid(b+293, 128);
   GXB_default.replace(a+256, 128, temp);     //address "02" +    
   temp = out.mid(b+434, 100);
   GXB_default.replace(a+384, 100, temp);     //address "03" +      no "04"     
   temp = out.mid(b+547, 128);
   GXB_default.replace(a+640, 128, temp);     //address "05" +           
   temp = out.mid(b+688, 128);
   GXB_default.replace(a+768, 128, temp);     //address "06" +       
   temp = out.mid(b+829, 100);
   GXB_default.replace(a+896, 100, temp);     //address "07" +      no "08"   
   temp = out.mid(b+942, 128);
   GXB_default.replace(a+1152, 128, temp);     //address "09" +   
   temp = out.mid(b+1083, 128);
   GXB_default.replace(a+1280, 128, temp);    //address "0A" +        
   temp = out.mid(b+1224, 128);
   GXB_default.replace(a+1408, 128, temp);    //address "0B" +       
   temp = out.mid(b+1365, 128);
   GXB_default.replace(a+1536, 128, temp);    //address "0C" + 
   b=b+1010;                                  // increment point to next *.syx patch in bulk.
   bulkFile.append(GXB_default.mid(160, 1713)); // copy most of the patch + index except for 4 text chars on end.
   bulkFile.append(GXB_default.mid(80, 4));     // copy 4 bytes of "00" from no-where special.
    };
    QString hex = QString::number(patchCount, 16).toUpper();     // convert integer to QString.
		if (hex.length() < 2) hex.prepend("0");
    QByteArray count;
   count.append( (char)(hex.toInt(&ok, 16)) ); 
   bulkFile.replace(35, 1, count);     //   insert the correct patch count into the GXB file.
   file.write(bulkFile); 
	};
 };
  DialogClose();      // close the dialog page after the file has been saved or cancelled.
}

void bulkSaveDialog::writeSYX()        //********************************* SYX File Format *****************************
{	
  	Preferences *preferences = Preferences::Instance();
	          QString dir = preferences->getPreferences("General", "Files", "dir");

          	QString fileName = QFileDialog::getSaveFileName(
                    this,
                    tr("Save Bulk Data"),
                    dir,
                    tr("System Exclusive Backup File (*.syx)"));
	             if (!fileName.isEmpty())	
                	{
	                  if(!fileName.contains(".syx"))
	                     	{
		                    	fileName.append(".syx");
	                     	};		
		
	QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
	{	
     bool ok;	
		QByteArray out;
		int size = this->bulk.size()/2;
	  //int patchCount = size/1777;
		for (int x=0;x<size*2;x++)
		{
		   QString hexStr = bulk.mid(x, 2);
		   out.append( (char)(hexStr.toInt(&ok, 16)) );
		    ++x;
		};
   file.write(out); 
	};	 
 };
 DialogClose();
}

void bulkSaveDialog::writeSMF()    // **************************** SMF FILE FORMAT ***************************
{	
  	Preferences *preferences = Preferences::Instance();
	          QString dir = preferences->getPreferences("General", "Files", "dir");

          	QString fileName = QFileDialog::getSaveFileName(
                    this,
                    tr("Save Bulk Data"),
                    dir,
                    tr("Standard Midi Backup File (*.mid)"));
	             if (!fileName.isEmpty())	
                	{
	                  if(!fileName.contains(".mid"))
	                     	{
		                    	fileName.append(".mid");
	                     	};		
	
  
  	QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
	{
	   bool ok;	
		QByteArray out;
		QByteArray patches;
		int size = this->bulk.size()/2;
	  int patchCount = size/1010;
		for (int x=0;x<size*2;x++)
		{
		   QString hexStr = bulk.mid(x, 2);
		   patches.append( (char)(hexStr.toInt(&ok, 16)) );          // convert the bulk QString to a QByteArray.
		    ++x;
		};
		QByteArray bulkFile;
		QByteArray temp;                        // TRANSLATION of GT-10B PATCHES, data read from gt10B syx patch **************
	  QByteArray Qhex;                        // and used to replace gt10B patch SMF data*********************************
    QFile hexfile(":HexLookupTable.hex");   // use a QByteArray of hex numbers from a lookup table.
    if (hexfile.open(QIODevice::ReadOnly))
	{	Qhex = hexfile.readAll(); };             // read the hexlookupTable and call the QByteArray QHex.
	  bulkFile.append(Qhex.mid((288), 30));   // insert midi timing file header...
	  int b = 0;
		for (int x=0;x<patchCount;x++)
	  {
	  out.clear();
	  out.append( patches.mid(b, 1010) );
    out.remove(1010, 282);	   // remove the user text portion at the end..		
		out.remove(0, 11);         // remove address "00 00" header...
		temp = Qhex.mid((320), 13);
		out.insert(0, temp);       // insert new address "00 00" header...
		out.remove(141, 13);        // remove address "01 00" header...
		temp = Qhex.mid((336), 16);
		out.insert(255, temp);      // insert new address "01 72" header...
		out.remove(285, 13);        // remove address "02 00" header...
		out.remove(413, 13);        // remove address "03 00" header...
		temp = Qhex.mid((352), 16);
		out.insert(513, temp);      // insert new address "03 64" header...
		out.remove(529, 13);        // remove address "05 00" header...
		temp = Qhex.mid((438), 78);
		temp.append(Qhex.mid((438), 78));
		out.insert(529, temp);      // insert 156 "0"'s to replace 04 00 00 00...
		temp = Qhex.mid((368), 16);
		out.insert(771, temp);      // insert new address "05 56" header...
		out.remove(829, 13);        // remove address "06 00" header...
		out.remove(957, 13);        // remove address "07 00" header...
		temp = Qhex.mid((384), 16);
		out.insert(1029, temp);      // insert new address "07 48" header...
		temp = Qhex.mid((438), 78);
		temp.append(Qhex.mid((438), 78));
		out.insert(1073, temp);      // insert 156 "0"'s to replace 08 00 00 00...
		out.remove(1229, 13);        // remove address "09 00" header...
		temp = Qhex.mid((400), 16);
		out.insert(1287, temp);      // insert new address "09 3A" header...
		out.remove(1373, 13);        // remove address "0A 00" header...
		out.remove(1501, 13);        // remove address "0B 00" header...
		out.remove(1629, 13);        // remove address "0C 00" header...
		temp = Qhex.mid((416), 16);
		out.insert(1545, temp);      // insert new address "0B 2C" header...
		out.remove(1773, 2);        // remove file footer...
		temp = Qhex.mid((438), 29);
		out.append(temp);            // insert 29 "0"'s...
		temp = Qhex.mid((432), 3);
		out.insert(1802, temp);      // insert new file footer (part of)...
		out.append(Qhex.mid((20), 1));           // add last "14" on end.
    				
	
	QByteArray sysxBuffer;
	int dataSize = 0; int offset = 0;
	for(int i=0;i<out.size();i++)
	{
		unsigned char byte = (char)out[i];
		unsigned int n = (int)byte;
		QString hex = QString::number(n, 16).toUpper();
		if (hex.length() < 2) hex.prepend("0");
		sysxBuffer.append(hex);

		unsigned char nextbyte = (char)out[i+1];
		unsigned int nextn = (int)nextbyte;
		QString nexthex = QString::number(nextn, 16).toUpper();
		if (nexthex.length() < 2) nexthex.prepend("0");
		if(offset >= checksumOffset+3 && nexthex != "F7")   // smf offset is 8 bytes + previous byte
		{		
			dataSize += n;
		};
		if(nexthex == "F7")
		{		
			QString checksum;
					bool ok;
					int dataSize = 0;
	        for(int i=checksumOffset+3;i<sysxBuffer.size()-1;++i)
	         { 
	         QString hexStr = sysxBuffer.mid(i, 2);
		       dataSize += ( (char)(hexStr.toInt(&ok, 16)) );  
           };
	     QString base = "80";
	     int sum = dataSize % base.toInt(&ok, 16);
	     if(sum!=0) sum = base.toInt(&ok, 16) - sum;
	     checksum = QString::number(sum, 16).toUpper();
	     if(checksum.length()<2) checksum.prepend("0");
	     sysxBuffer.replace(sysxBuffer.size() - 1, checksum);
	
		};
		offset++;

		if(hex == "F7") 
		{	
		  
		  sysxBuffer.clear();
			dataSize = 0;
			offset = 0;
		};
   };
   b=b+1010;
   bulkFile.append(out);      // append the bulk file. 
  };
   bulkFile.append(Qhex.mid((436), 3));    // add file footer.
   file.write(bulkFile); 
 };
 DialogClose();
 };
}

void bulkSaveDialog::DialogClose()
{
    bank = 100;
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(updatePatch(QString)));
    this->deleteLater();
    this->close();
}
