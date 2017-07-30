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
#include "fileDialog.h"


fileDialog::fileDialog(QString fileName, QList<QString> patchList, QByteArray fileData, QByteArray default_data, QString type)
{
    //QObject::connect(this, SIGNAL(patchIndex(int)),
                //this->parent(), SLOT(patchIndex(int)));
  this->file_format = type;
  this->fileData = fileData;
  this->default_data = default_data;
  QLabel *patchLabel = new QLabel(tr("Select patch to load"));
  QLabel *nameLabel = new QLabel(fileName);
  QComboBox *patchCombo = new QComboBox;
  patchCombo->setMaxVisibleItems(200);
  patchCombo->addItems(patchList);
  patchCombo->setWhatsThis(tr("To auditon a multi-patch *.gte file, hover the mouse cursor over a patch and the patch data will be loaded into the GT temporary buffer"
                              "<br>a click on the patch will load it into the editor."));


  QObject::connect(patchCombo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(valueChanged(int)));

  QObject::connect(patchCombo, SIGNAL(highlighted(int)),
                this, SLOT(highlighted(int)));

  QPushButton *cancelButton = new QPushButton(tr("Cancel"));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
  cancelButton->setWhatsThis(tr("Selecting this will close the patch load window and reset the GT back to the current editor patch."));
 
	QHBoxLayout *horizontalLayout = new QHBoxLayout;	
	horizontalLayout->addWidget(patchLabel);
	horizontalLayout->addWidget(patchCombo);

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(cancelButton);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(nameLabel);
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);

	setWindowTitle(tr("Bulk File Patch Extraction"));
}

void fileDialog::valueChanged(int value)
{
  SysxIO *sysxIO = SysxIO::Instance();
  sysxIO->patchListValue = value;             
  this->close();
} 

 void fileDialog::cancel()
{
  SysxIO *sysxIO = SysxIO::Instance();
  sysxIO->patchListValue = 0;
  sysxIO->writeToBuffer();             
  this->close();
} 

void fileDialog::highlighted(int value)
{
 if (file_format == "gte")
  {
  unsigned char r = (char)fileData[35];     // find hex patch count in GTE file at byte 35, 1~140
	bool ok;
	QByteArray sysxPatches;
  int count;
  count = QString::number(r, 16).toUpper().toInt(&ok, 16);
  int a=160;                              // offset is set to first patch start. 
   a=a+(value*1060);                      // index value patch count.
   QByteArray temp;
   temp = fileData.mid(a+8, 130);
   default_data.replace(11, 130, temp);    //address "00"  ++   fx1
   
   temp = fileData.mid(a+142, 9);
   default_data.replace(154, 9, temp);     //address "02"  ++
   
   temp = fileData.mid(a+155, 5);
   default_data.replace(176, 5, temp);     //address "03"  ++
   
   temp = fileData.mid(a+164, 5);
   default_data.replace(194, 5, temp);    //address "04"  ++
   
   //temp = fileData.mid(a+180, 1);
   //default_data.replace(220, 1, temp);     //address "05"  
          
   temp = fileData.mid(a+186, 8);
   default_data.replace(212, 8, temp);     //address "06"  ++
      
   temp = fileData.mid(a+198, 40);
   default_data.replace(233, 40, temp);    //address "07"  ++ 
    
   temp = fileData.mid(a+242, 13);
   default_data.replace(286, 13, temp);    //address "08"  ++
        
   temp = fileData.mid(a+259, 220);
   default_data.replace(312, 220, temp);   //address "09"  ++
   
   temp = fileData.mid(a+483, 26);
   default_data.replace(545, 26, temp);    //address "0B"  ++
       
   temp = fileData.mid(a+513, 9);
   default_data.replace(584, 9, temp);     //address "0C"  ++  
   
   temp = fileData.mid(a+526, 10);
   default_data.replace(606, 10, temp);    //address "0D"  ++   
   
   temp = fileData.mid(a+540, 9);
   default_data.replace(629, 9, temp);    //address "0E"  ++ 
   
   temp = fileData.mid(a+554, 1);
   default_data.replace(651, 1, temp);     //address "0F"  ++ 
   
   //temp = fileData.mid(a+559, 1);
   //default_data.replace(688, 1, temp);     //address "10"   
    
   temp = fileData.mid(a+564, 14);
   default_data.replace(665, 14, temp);    //address "11" ++   chain
   
   temp = fileData.mid(a+582, 16);
   default_data.replace(692, 16, temp);    //address "12" ++   name
   
   temp = fileData.mid(a+598, 1);
   default_data.replace(1007, 1, temp);     //address "1E"  
   /*
   temp = fileData.mid(a+794, 17);
   default_data.replace(772, 17, temp);    //address "20"     
   
   temp = fileData.mid(a+815, 17);
   default_data.replace(802, 17, temp);    //address "21"     
   
   temp = fileData.mid(a+836, 17);
   default_data.replace(832, 17, temp);    //address "22"     
   
   temp = fileData.mid(a+857, 17);
   default_data.replace(862, 17, temp);    //address "23"     
   
   temp = fileData.mid(a+878, 17);
   default_data.replace(892, 17, temp);    //address "24"     
   
   temp = fileData.mid(a+899, 17);
   default_data.replace(922, 17, temp);    //address "25"   
      
   temp = fileData.mid(a+920, 17);
   default_data.replace(952, 17, temp);    //address "26"     
   
   temp = fileData.mid(a+941, 17);
   default_data.replace(982, 17, temp);    //address "27"     
   
   temp = fileData.mid(a+962, 17);
   default_data.replace(1012, 17, temp);   //address "28"     
   
   temp = fileData.mid(a+983, 17);
   default_data.replace(1042, 17, temp);   //address "29"     
   
   temp = fileData.mid(a+1004, 17);
   default_data.replace(1072, 17, temp);   //address "2A"     
   
   temp = fileData.mid(a+1025, 17);
   default_data.replace(1102, 17, temp);   //address "2B"  
    
   temp = fileData.mid(a+1046, 3);
   default_data.replace(1132, 3, temp);    //address "2C"  
     
   temp = fileData.mid(a+1053, 3);
   default_data.replace(1142, 3, temp);    //address "2D"       */
   
        





     if (value>0)
     {
     SysxIO *sysxIO = SysxIO::Instance();
     QString area = "Structure";
     sysxIO->setFileSource(area, default_data);
     sysxIO->setFileName("file audition");
     //this->fileSource = sysxIO->getFileSource();
     sysxIO->writeToBuffer();
     // QApplication::beep();
     }
 }
} 

