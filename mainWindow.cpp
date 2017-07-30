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
#include "mainWindow.h"
#include "Preferences.h"
#include "preferencesDialog.h"
#include "statusBarWidget.h"
#include "SysxIO.h"
#include "bulkSaveDialog.h"
#include "bulkLoadDialog.h"
#include "summaryDialog.h"
#include "summaryDialogPatchList.h"
#include "summaryDialogSystem.h"
#include "globalVariables.h"

mainWindow::mainWindow(QWidget *parent)
    : QWidget(parent)
    /* For a stange reason when deriving from QMainWindow
	the performance is dead slow???

mainWindow::mainWindow(QWidget *parent)
	: QMainWindow(parent) */
{
    fxsBoard = new floorBoard(this);

    /* Loads the stylesheet for the current platform if present */
#ifdef Q_OS_WIN
    /* This set the floorboard default style to the "plastique" style,
	   as it comes the nearest what the stylesheet uses. */
    //fxsBoard->setStyle(QStyleFactory::create("plastique"));
    if(QFile(":qss/windows.qss").exists())
    {
        QFile file(":qss/windows.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        fxsBoard->setStyleSheet(styleSheet);
    }
#endif

#ifdef Q_OS_LINUX
    /* This set the floorboard default style to the "plastique" style,
	   as it comes the nearest what the stylesheet uses. */
    fxsBoard->setStyle(QStyleFactory::create("plastique"));
    if(QFile(":qss/linux.qss").exists())
    {
        QFile file(":qss/linux.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        fxsBoard->setStyleSheet(styleSheet);
    }
#endif

#ifdef Q_OS_MAC
    /* This set the floorboard default style to the "macintosh" style,
	   as it comes the nearest what the stylesheet uses. */
    fxsBoard->setStyle(QStyleFactory::create("plastique"));
    if(QFile(":qss/macosx.qss").exists())
    {
        QFile file(":qss/macosx.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        fxsBoard->setStyleSheet(styleSheet);
    }
#endif


    this->setWindowTitle(deviceType + tr(" Fx FloorBoard"));
    //this->setCentralWidget(fxsBoard);

    this->createActions();
    this->createMenus();
    this->createStatusBar();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(fxsBoard);
    mainLayout->addWidget(statusBar);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    this->statusBar->setWhatsThis("StatusBar<br>midi activity is displayed here<br>and some status messages are displayed.");
    setLayout(mainLayout);

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QObject::connect(fxsBoard, SIGNAL( sizeChanged(QSize, QSize) ),
                     this, SLOT( updateSize(QSize, QSize) ) );
}

mainWindow::~mainWindow()
{
    Preferences *preferences = Preferences::Instance();
    if(preferences->getPreferences("Window", "Restore", "window")=="true")
    {
        QString posx, width;
        if(preferences->getPreferences("Window", "Restore", "sidepanel")=="true" &&
           preferences->getPreferences("Window", "Collapsed", "bool")=="true")
        {
            width = QString::number(this->geometry().width(), 10);
            posx = QString::number(this->geometry().x(), 10);
        }
        else
        {
            bool ok;
            width = preferences->getPreferences("Window", "Size", "minwidth");
            posx = QString::number(this->geometry().x()+((this->geometry().width()-width.toInt(&ok,10))/2), 10);
        }
        preferences->setPreferences("Window", "Position", "x", posx);
        preferences->setPreferences("Window", "Position", "y", QString::number(this->geometry().y(), 10));
        preferences->setPreferences("Window", "Size", "width", width);
        preferences->setPreferences("Window", "Size", "height", QString::number(this->geometry().height(), 10));
    }
    else
    {
        preferences->setPreferences("Window", "Position", "x", "");
        preferences->setPreferences("Window", "Position", "y", "");
        preferences->setPreferences("Window", "Size", "width", "");
        preferences->setPreferences("Window", "Size", "height", "");
    }
    preferences->savePreferences();
}

void mainWindow::updateSize(QSize floorSize, QSize oldFloorSize)
{
    this->setFixedWidth(floorSize.width());
    int x = this->geometry().x() - ((floorSize.width() - oldFloorSize.width()) / 2);
    int y = this->geometry().y();
    this->setGeometry(x, y, floorSize.width(), this->geometry().height());
}

void mainWindow::createActions()
{
    openAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Load Patch File... *.syx *.gte *.mid"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    openAct->setWhatsThis(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save Patch..."), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    saveAct->setWhatsThis(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/filesave.png"), tr("Save &As Patch..."), this);
    saveAsAct->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    saveAsAct->setWhatsThis(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    systemLoadAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Load System and Global Data..."), this);
    systemLoadAct->setShortcut(tr("Ctrl+L"));
    systemLoadAct->setStatusTip(tr("Load System Data to ")+deviceType);
    systemLoadAct->setWhatsThis(tr("Load System Data to ")+deviceType);
    connect(systemLoadAct, SIGNAL(triggered()), this, SLOT(systemLoad()));

    systemSaveAct = new QAction(QIcon(":/images/filesave.png"), tr("Save System and Global Data..."), this);
    systemSaveAct->setShortcut(tr("Ctrl+D"));
    systemSaveAct->setStatusTip(tr("Save System Data to File"));
    systemSaveAct->setWhatsThis(tr("Save System Data to File"));
    connect(systemSaveAct, SIGNAL(triggered()), this, SLOT(systemSave()));

    bulkLoadAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Load Bulk Patch File to GT-8..."), this);
    bulkLoadAct->setShortcut(tr("Ctrl+B"));
    bulkLoadAct->setStatusTip(tr("Load Bulk Data to ")+ deviceType);
    bulkLoadAct->setWhatsThis(tr("Load Bulk Data to ")+ deviceType);
    connect(bulkLoadAct, SIGNAL(triggered()), this, SLOT(bulkLoad()));

    bulkSaveAct = new QAction(QIcon(":/images/filesave.png"), tr("Save Bulk GT-8 Patches to File..."), this);
    bulkSaveAct->setShortcut(tr("Ctrl+G"));
    bulkSaveAct->setStatusTip(tr("Save Bulk Data to File"));
    bulkSaveAct->setWhatsThis(tr("Save Bulk Data to File"));
    connect(bulkSaveAct, SIGNAL(triggered()), this, SLOT(bulkSave()));

    exitAct = new QAction(QIcon(":/images/exit.png"),tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setWhatsThis(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    settingsAct = new QAction(QIcon(":/images/preferences.png"), tr("&Preferences"), this);
    settingsAct->setShortcut(tr("Ctrl+P"));
    settingsAct->setStatusTip(tr("FxFloorBoard Preferences<br>Select midi device, language,splash, directories"));
    settingsAct->setWhatsThis(tr("FxFloorBoard Preferences<br>Select midi device, language,splash, directories"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(settings()));

    uploadAct = new QAction(QIcon(":/images/upload.png"), tr("Upload patch to V-Guitars Forum"), this);
    uploadAct->setStatusTip(tr("Upload any saved patch file to a shared patch library<br>via the internet."));
    uploadAct->setWhatsThis(tr("Upload any saved patch file to a shared patch library<br>via the internet."));
    connect(uploadAct, SIGNAL(triggered()), this, SLOT(upload()));

    summaryAct = new QAction(QIcon(":/images/copy.png"), tr("Summary Page Text"), this);
    summaryAct->setWhatsThis(tr("Display the current patch parameters<br>in a readable text format, which<br>can be printed or saved to file."));
    connect(summaryAct, SIGNAL(triggered()), this, SLOT(summaryPage()));

    summarySystemAct = new QAction(QIcon(":/images/copy.png"), tr("System/Global Text Summary"), this);
    summarySystemAct->setWhatsThis(tr("Display the current System and Global parameters<br>in a readable text format, which<br>can be printed or saved to file."));
    connect(summarySystemAct, SIGNAL(triggered()), this, SLOT(summarySystemPage()));

    summaryPatchListAct = new QAction(QIcon(":/images/copy.png"), tr("GT-8 Patch List Summary"), this);
    summaryPatchListAct->setWhatsThis(tr("Display the current GT-8 patch listing<br>in a readable text format, which<br>can be printed or saved to file."));
    connect(summaryPatchListAct, SIGNAL(triggered()), this, SLOT(summaryPatchList()));

    helpAct = new QAction(QIcon(":/images/help.png"), deviceType + tr(" Fx FloorBoard &Help"), this);
    helpAct->setShortcut(tr("Ctrl+F1"));
    helpAct->setStatusTip(tr("Help page to assist with FxFloorBoard functions."));
    helpAct->setWhatsThis(tr("Help page to assist with FxFloorBoard functions."));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

    whatsThisAct = new QAction(QIcon(":/images/help.png"), tr("Whats This? description of items under the mouse cursor"), this);
    whatsThisAct->setShortcut(tr("F1"));
    whatsThisAct->setWhatsThis(tr("ha..ha..ha..!!"));
    connect(whatsThisAct, SIGNAL(triggered()), this, SLOT(whatsThis()));

    homepageAct = new QAction(QIcon(":/images/upload.png"),deviceType + tr(" Fx FloorBoard &Webpage"), this);
    homepageAct->setStatusTip(tr("download Webpage for FxFloorBoard<br>find if the latest version is available."));
    homepageAct->setWhatsThis(tr("download Webpage for FxFloorBoard<br>find if the latest version is available."));
    connect(homepageAct, SIGNAL(triggered()), this, SLOT(homepage()));

    donationAct = new QAction(QIcon(":/images/donate.png"), tr("Donate if you appreciate this software"), this);
    donationAct->setStatusTip(tr("Even though the software is free,<br>an occassional donation is very much appreciated<br> since i am not paid for this work."));
    donationAct->setWhatsThis(tr("Even though the software is free,<br>an occassional donation is very much appreciated<br>since i am not paid for this work."));
    connect(donationAct, SIGNAL(triggered()), this, SLOT(donate()));

    licenseAct = new QAction(QIcon(":/images/licence.png"), tr("&License"), this);
    licenseAct->setStatusTip(tr("licence agreement which you<br>have accepted by installing this software."));
    licenseAct->setWhatsThis(tr("licence agreement which you<br>have accepted by installing this software."));
    connect(licenseAct, SIGNAL(triggered()), this, SLOT(license()));

    aboutAct = new QAction(QIcon(":/images/GT-8FxFloorBoard.png"),tr("&About FxFloorBoard"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    aboutAct->setWhatsThis(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(":/images/qt-logo.png"),tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    aboutQtAct->setWhatsThis(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void mainWindow::createMenus()
{
    menuBar = new QMenuBar;

    QMenu *fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    //fileMenu->addSeparator();
    //fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(bulkLoadAct);
    fileMenu->addAction(bulkSaveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(systemLoadAct);
    fileMenu->addAction(systemSaveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    fileMenu->setWhatsThis(tr("File Saving and Loading,<br> and application Exit."));
    menuBar->addMenu(fileMenu);

    QMenu *toolsMenu = new QMenu(tr("&Tools"), this);
    toolsMenu->addAction(settingsAct);
    toolsMenu->addAction(uploadAct);
    fileMenu->addSeparator();
    toolsMenu->addAction(summaryAct);
    toolsMenu->addAction(summarySystemAct);
    toolsMenu->addAction(summaryPatchListAct);
    menuBar->addMenu(toolsMenu);


    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(helpAct);
    helpMenu->addAction(whatsThisAct);
    helpMenu->addAction(homepageAct);
    helpMenu->addSeparator();
    helpMenu->addAction(donationAct);
    helpMenu->addAction(licenseAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    menuBar->addMenu(helpMenu);
}

void mainWindow::createStatusBar()
{
    SysxIO *sysxIO = SysxIO::Instance();

    statusBarWidget *statusInfo = new statusBarWidget(this);
    statusInfo->setStatusSymbol(0);
    statusInfo->setStatusMessage(tr("Not Connected"));

    QObject::connect(sysxIO, SIGNAL(setStatusSymbol(int)), statusInfo, SLOT(setStatusSymbol(int)));
    QObject::connect(sysxIO, SIGNAL(setStatusProgress(int)), statusInfo, SLOT(setStatusProgress(int)));;
    QObject::connect(sysxIO, SIGNAL(setStatusMessage(QString)), statusInfo, SLOT(setStatusMessage(QString)));
    QObject::connect(sysxIO, SIGNAL(setStatusdBugMessage(QString)), statusInfo, SLOT(setStatusdBugMessage(QString)));

    statusBar = new QStatusBar;
    statusBar->addWidget(statusInfo);
    statusBar->setSizeGripEnabled(false);
}

/* FILE MENU */
void mainWindow::open()
{
    Preferences *preferences = Preferences::Instance();
    QString dir = preferences->getPreferences("General", "Files", "dir");

    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Choose a file"),
            dir,
            "GT8, GT-Pro, GT10 patch (*.syx *.gte)");
    if (!fileName.isEmpty())
    {
        file.setFile(fileName);
        if(file.readFile())
        {
            // DO SOMETHING AFTER READING THE FILE (UPDATE THE GUI)
            SysxIO *sysxIO = SysxIO::Instance();
            QString area = "Structure";
            sysxIO->setFileSource(area, file.getFileSource());
            sysxIO->setFileName(fileName);
            sysxIO->setSyncStatus(false);
            sysxIO->setDevice(false);
            emit updateSignal();
            if(sysxIO->isConnected())
            {sysxIO->writeToBuffer(); }
        }
    }
}

void mainWindow::save()
{
    Preferences *preferences = Preferences::Instance();
    QString dir = preferences->getPreferences("General", "Files", "dir");

    SysxIO *sysxIO = SysxIO::Instance();
    file.setFile(sysxIO->getFileName());

    if(file.getFileName().isEmpty())
    {
        QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save As"),
                dir,
                tr("System Exclusive (*.syx)"));
        if (!fileName.isEmpty())
        {
            if(!fileName.contains(".syx"))
            {
                fileName.append(".syx");
            }
            file.writeFile(fileName);

            file.setFile(fileName);
            if(file.readFile())
            {
                // DO SOMETHING AFTER READING THE FILE (UPDATE THE GUI)
                SysxIO *sysxIO = SysxIO::Instance();
                QString area = "Structure";
                sysxIO->setFileSource(area, file.getFileSource());
                emit updateSignal();
            }
        }
    }
    else
    {
        file.writeFile(file.getFileName());
    }
}

void mainWindow::saveAs()
{
    Preferences *preferences = Preferences::Instance();
    QString dir = preferences->getPreferences("General", "Files", "dir");

    QString fileName = QFileDialog::getSaveFileName(
            this,
            tr("Save As"),
            dir,
            tr("System Exclusive (*.syx)"));
    if (!fileName.isEmpty())
    {
        if(!fileName.contains(".syx"))
        {
            fileName.append(".syx");
        }
        file.writeFile(fileName);

        file.setFile(fileName);
        if(file.readFile())
        {
            // DO SOMETHING AFTER READING THE FILE (UPDATE THE GUI)
            SysxIO *sysxIO = SysxIO::Instance();
            QString area = "Structure";
            sysxIO->setFileSource(area, file.getFileSource());
            emit updateSignal();
        }
    }
}

void mainWindow::systemLoad()
{
    SysxIO *sysxIO = SysxIO::Instance();

    Preferences *preferences = Preferences::Instance();
    QString dir = preferences->getPreferences("General", "Files", "dir");

    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Choose a file"),
            dir,
            tr("GT-8 System Data File (*.GT8_system_syx)"));
    if (!fileName.isEmpty())
    {
        file.setFile(fileName);
        if(file.readFile())
        {
            // DO SOMETHING AFTER READING THE FILE (UPDATE THE GUI)

            SysxIO *sysxIO = SysxIO::Instance();
            QString area = "System";
            sysxIO->setFileSource(area, file.getSystemSource());
            sysxIO->setFileName(fileName);
            emit updateSignal();

            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(deviceType + tr(" Fx FloorBoard"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("<font size='+1'><b>");
            msgText.append(tr("You have chosen to load a SYSTEM DATA file."));
            msgText.append("<b></font><br>");
            msgText.append(tr("This will overwrite the SYSTEM DATA currently stored in the ")+ deviceType);
            msgText.append(tr (" <br> and can't be undone.<br>"));
            msgText.append(tr("Select 'NO' to only update the Editor - Select 'YES' to update the ")+ deviceType +tr(" memory<br>"));


            msgBox->setInformativeText(tr("Are you sure you want to write to the ")+ deviceType + "?");
            msgBox->setText(msgText);
            msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

            if(msgBox->exec() == QMessageBox::Yes)
            {	// Accepted to overwrite system data.
                sysxIO->systemWrite();
            }
        }
    }
    if (!sysxIO->isConnected())
    {
        QString snork = tr("DATA TRANSFER REQUIRED<br>");
        snork.append(tr("Ensure connection is active, and<br>"));
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(deviceType + tr(" Connection required!!"));
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(snork);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    }
}

void mainWindow::systemSave()
{ 
    SysxIO *sysxIO = SysxIO::Instance();
    if (sysxIO->isConnected())
    {
        sysxIO->systemDataRequest();
        //SLEEP(3000);

	Preferences *preferences = Preferences::Instance();
	QString dir = preferences->getPreferences("General", "Files", "dir");

	QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save System Data"),
                dir,
                tr("System Exclusive File (*.GT8_system_syx)"));
	if (!fileName.isEmpty())	
	{
            if(!fileName.contains(".GT8_system_syx"))
            {
                fileName.append(".GT8_system_syx");
            }

            file.writeSystemFile(fileName);

            file.setFile(fileName);
            if(file.readFile())
            {
                SysxIO *sysxIO = SysxIO::Instance();
                QString area = "System";
                sysxIO->setFileSource(area, file.getSystemSource());
                emit updateSignal();
            }
	}
    }
    else
    {
        QString snork = tr("DATA TRANSFER REQUIRED<br>");
        snork.append(tr("Ensure connection is active"));
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(deviceType + tr(" Connection required!!"));
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(snork);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    }
}

void mainWindow::bulkLoad()
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (sysxIO->isConnected())
    {
	
        bulkLoadDialog *loadDialog = new bulkLoadDialog();
        loadDialog->exec();
    }
    else
    {
        QString snork = tr("Ensure connection is active and retry");
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(deviceType + tr(" not connected !!"));
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(snork);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    }
}

void mainWindow::bulkSave()
{ 

    SysxIO *sysxIO = SysxIO::Instance();
    if (sysxIO->isConnected())
    {
        bulkSaveDialog *bulkDialog = new bulkSaveDialog();
        bulkDialog->exec();
    }
    else
    {
        QString snork = tr("Ensure connection is active and retry");
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(deviceType + tr(" not connected !!"));
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(snork);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    }
}

/* TOOLS MENU */
void mainWindow::settings()
{
    preferencesDialog *dialog = new preferencesDialog();
    if (dialog->exec())
    {
        Preferences *preferences = Preferences::Instance();

        QString dir = dialog->generalSettings->dirEdit->text();
        QString sidepanel = (dialog->windowSettings->sidepanelCheckBox->checkState())?QString("true"):QString("false");
        QString window = (dialog->windowSettings->windowCheckBox->checkState())?QString("true"):QString("false");
        QString splash = (dialog->windowSettings->splashCheckBox->checkState())?QString("true"):QString("false");
        QString dBug = (dialog->midiSettings->dBugCheckBox->checkState())?QString("true"):QString("false");
        QString midiIn = QString::number(dialog->midiSettings->midiInCombo->currentIndex() - 1, 10); // -1 because there is a default entry at index 0
        QString midiOut = QString::number(dialog->midiSettings->midiOutCombo->currentIndex() - 1, 10);
        //QString midiTimeSet =QString::number(dialog->midiSettings->midiTimeSpinBox->value());
        QString receiveTimeout =QString::number(dialog->midiSettings->midiDelaySpinBox->value());
        QString sys_Byte1 = QString::number(dialog->midiSettings->sysByteSpinBox1->value());
        QString sys_Byte2 = QString::number(dialog->midiSettings->sysByteSpinBox2->value());
        QString sys_Byte3 = QString::number(dialog->midiSettings->sysByteSpinBox3->value());
        QString sys_Byte4 = QString::number(dialog->midiSettings->sysByteSpinBox4->value());
        QString lang;
        if (dialog->languageSettings->chineseButton->isChecked() ) {lang="3"; }
        else if (dialog->languageSettings->germanButton->isChecked() ) {lang="2"; }
        else if (dialog->languageSettings->frenchButton->isChecked() ) {lang="1"; }
        else /*if (dialog->languageSettings->englishButton->isChecked() )*/ {lang="0"; }
        preferences->setPreferences("Language", "Locale", "select", lang);


        if(midiIn=="-1") { midiIn = ""; }
        if(midiOut=="-1") {	midiOut = ""; }

        preferences->setPreferences("General", "Files", "dir", dir);
        preferences->setPreferences("Midi", "MidiIn", "device", midiIn);
        preferences->setPreferences("Midi", "MidiOut", "device", midiOut);
        preferences->setPreferences("Midi", "DBug", "bool", dBug);
        //preferences->setPreferences("Midi", "Time", "set", midiTimeSet);
        preferences->setPreferences("Midi", "Delay", "set", receiveTimeout);
        preferences->setPreferences("Midi", "System", "byte1", sys_Byte1);
        preferences->setPreferences("Midi", "System", "byte2", sys_Byte2);
        preferences->setPreferences("Midi", "System", "byte3", sys_Byte3);
        preferences->setPreferences("Midi", "System", "byte4", sys_Byte4);

        preferences->setPreferences("Window", "Restore", "sidepanel", sidepanel);
        preferences->setPreferences("Window", "Restore", "window", window);
        preferences->setPreferences("Window", "Splash", "bool", splash);
        preferences->savePreferences();
    }
}

/* HELP MENU */
void mainWindow::help()
{
    Preferences *preferences = Preferences::Instance();
    QDesktopServices::openUrl(QUrl( preferences->getPreferences("General", "Help", "url") ));
}

void mainWindow::whatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void mainWindow::upload()
{
    QDesktopServices::openUrl(QUrl( "http://www.vguitarforums.com/smf/index.php?topic=12957.0" ));
}

void mainWindow::summaryPage()
{
    summaryDialog *summary = new summaryDialog();
    summary->setMinimumWidth(800);
    summary->setMinimumHeight(650);
    summary->show();
}

void mainWindow::summarySystemPage()
{
    summaryDialogSystem *summarySystem = new summaryDialogSystem();
    summarySystem->setMinimumWidth(800);
    summarySystem->setMinimumHeight(650);
    summarySystem->show();
}

void mainWindow::summaryPatchList()
{
    summaryDialogPatchList *summaryPatchList = new summaryDialogPatchList();
    summaryPatchList->setMinimumWidth(800);
    summaryPatchList->setMinimumHeight(650);
    summaryPatchList->show();
}

void mainWindow::homepage()
{
    Preferences *preferences = Preferences::Instance();
    QDesktopServices::openUrl(QUrl( preferences->getPreferences("General", "Webpage", "url") ));
}

void mainWindow::donate()
{
    Preferences *preferences = Preferences::Instance();
    QDesktopServices::openUrl(QUrl( preferences->getPreferences("General", "Donate", "url") ));
}

void mainWindow::license()
{
    QDesktopServices::openUrl(QUrl("file:license.txt"));
}

void mainWindow::about()
{
    Preferences *preferences = Preferences::Instance();
    QString version = preferences->getPreferences("General", "Application", "version");

    QFile file(":about");
    if(file.open(QIODevice::ReadOnly))
    {
        QMessageBox::about(this, deviceType + tr(" Fx FloorBoard - About"),
                           deviceType + tr(" Fx FloorBoard, ") + tr("version") + " " + version + "<br>" + file.readAll());
    }
}

void mainWindow::closeEvent(QCloseEvent* ce)
{
    Preferences *preferences = Preferences::Instance();
    preferences->savePreferences();
    ce->accept();
    emit closed();
}

