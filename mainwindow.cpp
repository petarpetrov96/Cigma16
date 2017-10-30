#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPalette>
#include <QFileDialog>
#include <QDebug>
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupEditor();
    QActionGroup* architectureActionGroup=new QActionGroup(this);
    architectureActionGroup->addAction(ui->actionSigma16_0_1_7);
    architectureActionGroup->addAction(ui->actionSigma16_1_4_4);
    connect(ui->actionSigma16_0_1_7,&QAction::triggered,this,&MainWindow::toggleSigma);
    connect(ui->actionSigma16_1_4_4,&QAction::triggered,this,&MainWindow::toggleSigma);
    QActionGroup* speedActionGroup=new QActionGroup(this);
    speedActionGroup->addAction(ui->actionSlow);
    speedActionGroup->addAction(ui->actionMedium);
    speedActionGroup->addAction(ui->actionFast);
    speedActionGroup->addAction(ui->actionRealtime);
    speedActionGroup->addAction(ui->actionNo_UI);
    connect(ui->actionSlow,&QAction::toggled,this,&MainWindow::toggleSpeed);
    connect(ui->actionMedium,&QAction::toggled,this,&MainWindow::toggleSpeed);
    connect(ui->actionFast,&QAction::toggled,this,&MainWindow::toggleSpeed);
    connect(ui->actionRealtime,&QAction::toggled,this,&MainWindow::toggleSpeed);
    connect(ui->actionNo_UI,&QAction::toggled,this,&MainWindow::toggleSpeed);
    connect(ui->actionNew,&QAction::triggered,this,&MainWindow::newFile);
    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::openFile);
    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::saveFile);
    connect(ui->actionSave_As,&QAction::triggered,this,&MainWindow::saveAsFile);
    connect(ui->actionExit,&QAction::triggered,this,&MainWindow::close);
    connect(ui->actionAbout,&QAction::triggered,this,&MainWindow::about);
    QActionGroup* themeActionGroup=new QActionGroup(this);
    themeActionGroup->addAction(ui->actionLight);
    themeActionGroup->addAction(ui->actionDark);
    connect(ui->actionLight,&QAction::toggled,this,&MainWindow::toggleTheme);
    connect(ui->actionDark,&QAction::toggled,this,&MainWindow::toggleTheme);
    connect(ui->assembleButton,SIGNAL(released()),this,SLOT(assemble()));
    connect(ui->resetButton,SIGNAL(released()),this,SLOT(resetEmulator()));
    connect(ui->loadButton,SIGNAL(released()),this,SLOT(loadData()));
    connect(ui->stepButton,SIGNAL(released()),this,SLOT(stepEmulator()));
    connect(ui->runButton,SIGNAL(released()),this,SLOT(runEmulator()));
    connect(ui->stopButton,SIGNAL(released()),this,SLOT(stopEmulator()));
    connect(ui->clearButton,SIGNAL(released()),this,SLOT(clearTrace()));
    theme=THEME_LIGHT;
    background.setForeground(Qt::black);
    blue.setForeground(Qt::blue);
    pal.setColor(QPalette::Base,Qt::white);
    pal.setColor(QPalette::Text,Qt::black);
    highlighter->setTheme(theme);
    applyTheme();
    updateEmulator();
    timer=new QTimer(this);
    timer->setInterval(100);
    timer->connect(timer,SIGNAL(timeout()),this,SLOT(timerRunEmulator()));
    ui->instructionList->setPlainText(emulator.getInstructionList(architecture).c_str());
}

void MainWindow::toggleTheme() {
    if(ui->actionLight->isChecked() && theme!=THEME_LIGHT) {
        theme=THEME_LIGHT;
        background.setForeground(Qt::black);
        blue.setForeground(Qt::blue);
        pal.setColor(QPalette::Base,Qt::white);
        pal.setColor(QPalette::Text,Qt::black);
        highlighter->setTheme(theme);
        applyTheme();
    }
    else if(ui->actionDark->isChecked() && theme!=THEME_DARK) {
        theme=THEME_DARK;
        background.setForeground(Qt::white);
        blue.setForeground(Qt::blue);
        pal.setColor(QPalette::Base,Qt::black);
        pal.setColor(QPalette::Text,Qt::yellow);
        highlighter->setTheme(theme);
        applyTheme();
    }
}

void MainWindow::applyTheme() {
    ui->editorTextEdit->setPalette(pal);
    ui->assemblerTextEdit->setPalette(pal);
    ui->emEffect->setPalette(pal);
    ui->emOperands->setPalette(pal);
    ui->emOperation->setPalette(pal);
    ui->emType->setPalette(pal);
    ui->registerADR->setPalette(pal);
    ui->registerDAT->setPalette(pal);
    ui->registerFile->setPalette(pal);
    ui->registerIR->setPalette(pal);
    ui->registerPC->setPalette(pal);
    ui->memoryFileLeft->setPalette(pal);
    ui->memoryFileRight->setPalette(pal);
    ui->inputOutput->setPalette(pal);
    ui->instructionList->setPalette(pal);
    ui->traceTextEdit->setPalette(pal);
    highlighter->rehighlight();
    QTextCursor cursorLeft(ui->memoryFileLeft->document());
    cursorLeft.select(QTextCursor::Document);
    cursorLeft.setCharFormat(background);
    QTextCursor cursorRight(ui->memoryFileRight->document());
    cursorRight.select(QTextCursor::Document);
    cursorRight.setCharFormat(background);
}

void MainWindow::setupEditor() {
    QTextEdit* textEdit=ui->editorTextEdit;
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    textEdit->setFont(font);
    textEdit->setWordWrapMode(QTextOption::NoWrap);
    highlighter=new Highlighter(textEdit->document());
    highlighter->setLineError(4);
}

void MainWindow::toggleSigma() {
    if(ui->actionSigma16_0_1_7->isChecked() && architecture!=ARCHITECTURE_SIGMA16_0_1_7)
        architecture=ARCHITECTURE_SIGMA16_0_1_7;
    else if(ui->actionSigma16_1_4_4->isChecked() && architecture!=ARCHITECTURE_SIGMA16_1_4_4)
        architecture=ARCHITECTURE_SIGMA16_1_4_4;
    resetEmulator();
    ui->instructionList->setPlainText(emulator.getInstructionList(architecture).c_str());
}

void MainWindow::toggleSpeed() {
    if(ui->actionSlow->isChecked() && speed!=SPEED_SLOW) {
        timer->setInterval(1000);
        speed=SPEED_SLOW;
        uienabled=true;
    }
    else if(ui->actionMedium->isChecked() && speed!=SPEED_MEDIUM) {
        timer->setInterval(250);
        speed=SPEED_MEDIUM;
        uienabled=true;
    }
    else if(ui->actionFast->isChecked() && speed!=SPEED_FAST) {
        timer->setInterval(50);
        speed=SPEED_FAST;
        uienabled=true;
    }
    else if(ui->actionRealtime->isChecked() && speed!=SPEED_REALTIME) {
        timer->setInterval(0);
        speed=SPEED_REALTIME;
        uienabled=true;
    }
    else if(ui->actionNo_UI->isChecked() && speed!=SPEED_NOUI) {
        timer->setInterval(0);
        speed=SPEED_NOUI;
        uienabled=false;
    }
}

void MainWindow::assemble() {
    QString tempString=ui->editorTextEdit->document()->toPlainText();
    char* data=new char[tempString.length()+1];
    memcpy(data,tempString.toStdString().c_str(),tempString.length());
    data[tempString.length()]='\0';
    if(architecture==ARCHITECTURE_SIGMA16_0_1_7) {
        Assembler017 assembler(data,strlen(data));
        assembler.assemble();
        rawAssembly=QString(assembler.readAssembly().c_str());
    }
    else {
        Assembler144 assembler(data,strlen(data));
        assembler.assemble();
        rawAssembly=QString(assembler.readAssembly().c_str());
    }
    QString test(rawAssembly);
    if(test.left(5).compare("ASM02")!=0) {
        ui->assemblerTextEdit->setText(test);
        return;
    }
    test=test.right(test.length()-5);
    assembly="";
    int line=0;
    char temp[50];
    for(int i=0;i<test.length();i+=4) {
        sprintf(temp,"%04hx",line);
        assembly.append(temp[0]);
        assembly.append(temp[1]);
        assembly.append(temp[2]);
        assembly.append(temp[3]);
        assembly.append('\t');
        if(test[i]=='f') {
            assembly.append(test[i]);
            assembly.append(test[i+1]);
            assembly.append(test[i+2]);
            assembly.append(test[i+3]);
            assembly.append(' ');
            assembly.append(test[i+4]);
            assembly.append(test[i+5]);
            assembly.append(test[i+6]);
            assembly.append(test[i+7]);
            assembly.append('\n');
            i+=4;
            line++;
        }
        else {
            assembly.append(test[i]);
            assembly.append(test[i+1]);
            assembly.append(test[i+2]);
            assembly.append(test[i+3]);
            assembly.append('\n');
        }
        line++;
    }
    ui->assemblerTextEdit->clear();
    ui->assemblerTextEdit->setText(assembly);
}

void MainWindow::updateEmulator() {
    QString output=emulator.getOutput().c_str();
    if(output!="") {
        ui->inputOutput->moveCursor(QTextCursor::End);
        if(architecture==ARCHITECTURE_SIGMA16_0_1_7)
            ui->inputOutput->moveCursor(QTextCursor::StartOfLine);
        ui->inputOutput->insertPlainText(output);
    }
    if(!uienabled)
        return;
    char temp[50];
    sprintf(temp,"%04hx",emulator.pc);
    ui->registerPC->setText(QString(temp));
    sprintf(temp,"%04hx",emulator.ir);
    ui->registerIR->setText(QString(temp));
    sprintf(temp,"%04hx",emulator.adr);
    ui->registerADR->setText(QString(temp));
    sprintf(temp,"%04hx",emulator.dat);
    ui->registerDAT->setText(QString(temp));
    ui->registerFile->clear();
    ui->registerFile->appendPlainText(emulator.getRegisterFile().c_str());
    if(emulator.isMemoryModified()) {
        setUpdatesEnabled(false);
        QString memoryFile=emulator.getMemoryFile().c_str();
        ui->memoryFileLeft->setPlainText(memoryFile);
        QTextCursor cursorLeft(ui->memoryFileLeft->document());
        cursorLeft.select(QTextCursor::Document);
        cursorLeft.setCharFormat(background);
        ui->memoryFileRight->setPlainText(memoryFile);
        QTextCursor cursorRight(ui->memoryFileRight->document());
        cursorRight.select(QTextCursor::Document);
        cursorRight.setCharFormat(background);
        setUpdatesEnabled(true);
    }
    else {
        oldLeft.setCharFormat(background);
        oldRight.setCharFormat(background);
    }
    if(emulator.isMemoryAddressModified()!=-1) {
        int address=emulator.isMemoryAddressModified();
        QString memoryAddress=emulator.getMemoryAddress(address).c_str();
        QTextCursor cursorLeft(ui->memoryFileLeft->document());
        cursorLeft.setPosition(address*11,QTextCursor::MoveAnchor);
        cursorLeft.setPosition((address+1)*11,QTextCursor::KeepAnchor);
        cursorLeft.insertText(memoryAddress);
        QTextCursor cursorRight(ui->memoryFileRight->document());
        cursorRight.setPosition(address*11,QTextCursor::MoveAnchor);
        cursorRight.setPosition((address+1)*11,QTextCursor::KeepAnchor);
        cursorRight.insertText(memoryAddress);
    }
    ui->memoryFileLeft->setTextCursor(QTextCursor(ui->memoryFileLeft->document()->findBlockByLineNumber(emulator.leftAffectedBegin)));
    ui->memoryFileRight->setTextCursor(QTextCursor(ui->memoryFileRight->document()->findBlockByLineNumber(emulator.rightAffectedBegin)));
    QTextCursor cursorLeft(ui->memoryFileLeft->document());
    cursorLeft.setPosition(emulator.leftAffectedBegin*11,QTextCursor::MoveAnchor);
    cursorLeft.setPosition(emulator.leftAffectedEnd*11,QTextCursor::KeepAnchor);
    cursorLeft.setCharFormat(blue);
    QTextCursor cursorRight(ui->memoryFileRight->document());
    cursorRight.setPosition(emulator.rightAffectedBegin*11,QTextCursor::MoveAnchor);
    cursorRight.setPosition(emulator.rightAffectedEnd*11,QTextCursor::KeepAnchor);
    cursorRight.setCharFormat(blue);
    oldLeft=cursorLeft;
    oldRight=cursorRight;
    ui->emOperation->setText(emulator.lastOperation.c_str());
    ui->emOperands->setText(emulator.lastOperands.c_str());
    ui->emType->setText(emulator.lastType.c_str());
    ui->emEffect->setText(emulator.lastEffect.c_str());
    if(emulator.lastEffect=="DIVISION BY ZERO")
        showWarning("Division by zero detected!");
}

void MainWindow::resetEmulator() {
    emulator.reset();
    updateEmulator();
    ui->inputOutput->clear();
    timer->stop();
}

void MainWindow::loadData() {
    std::string assembly = std::string(rawAssembly.toLocal8Bit().constData());
    emulator.loadMemory(assembly);
    updateEmulator();
}

void MainWindow::stepEmulator() {
    bool temp=emulator.isHalted();
    if(architecture==ARCHITECTURE_SIGMA16_0_1_7)
        emulator.step017();
    else if(architecture==ARCHITECTURE_SIGMA16_1_4_4)
        emulator.step144();
    if(!temp)
        addToTrace();
    updateEmulator();
}

void MainWindow::runEmulator() {
    running=true;
    timer->start();
}

void MainWindow::timerRunEmulator() {
    if(!running)
        return;
    if((architecture==ARCHITECTURE_SIGMA16_0_1_7 && !emulator.step017()) ||
       (architecture==ARCHITECTURE_SIGMA16_1_4_4 && !emulator.step144())) {
        addToTrace();
        updateEmulator();
        running=false;
        timer->stop();
        return;
    }
    addToTrace();
    updateEmulator();
}

void MainWindow::stopEmulator() {
    running=false;
    timer->stop();
}

void MainWindow::clearTrace() {
    ui->traceTextEdit->clear();
}

void MainWindow::about() {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("Cigma16");
    msgBox.setText("C++ - based Sigma16 Assembler/Emulator\n\
Based on the architecture provided by Dr John O'Donnell\n\
Created by Petar Petrov");
    msgBox.addButton(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::addToTrace() {
    if(!uienabled)
        return;
    char temp[50];
    QString string;
    sprintf(temp,"%d",emulator.executedInstructions);
    string.append(temp);
    string.append(".  ");
    string.append(emulator.lastOperation.c_str());
    string.append(" ");
    string.append(emulator.lastOperands.c_str());
    string.append("   Effect: ");
    string.append(emulator.lastEffect.c_str());
    string.append("   pc=");
    sprintf(temp,"%04hx",emulator.pc);
    string.append(temp);
    string.append(" ir=");
    sprintf(temp,"%04hx",emulator.ir);
    string.append(temp);
    string.append(" adr=");
    sprintf(temp,"%04hx",emulator.adr);
    string.append(temp);
    string.append(" dat=");
    sprintf(temp,"%04hx",emulator.dat);
    string.append(temp);
    ui->traceTextEdit->appendPlainText(string);
}

void MainWindow::newFile() {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Warning");
    msgBox.setText("Are you sure you want to create a new file?");
    msgBox.addButton(QMessageBox::Yes);
    QAbstractButton* buttonNo=msgBox.addButton(QMessageBox::No);
    msgBox.exec();
    if(msgBox.clickedButton()==buttonNo)
        return;
    workingFile="";
    ui->editorTextEdit->clear();
}

void MainWindow::openFile() {
    if(workingFile!="" || ui->editorTextEdit->document()->toPlainText()!="") {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Are you sure you want to dismiss all your changes?");
        msgBox.addButton(QMessageBox::Yes);
        QAbstractButton* buttonNo=msgBox.addButton(QMessageBox::No);
        msgBox.exec();
        if(msgBox.clickedButton()==buttonNo)
            return;
    }
    QString fileName=QFileDialog::getOpenFileName(this,"Open File","","Text Files (*.txt)");
    if(fileName.isEmpty())
        return;
    FILE* f=fopen(fileName.toStdString().c_str(),"r");
    if(f==NULL) {
        showWarning("There was an error while trying to open the file");
        return;
    }
    ui->editorTextEdit->clear();
    char* buffer=new char[1048576+1];
    while(!feof(f)) {
        int size=fread(buffer,sizeof(char),1048576,f);
        buffer[size]='\0';
        ui->editorTextEdit->insertPlainText(QString(buffer));
    }
    fclose(f);
    delete[] buffer;
    workingFile=fileName;
}

void MainWindow::saveFile() {
    if(workingFile=="") {
        saveAsFile();
        return;
    }
    FILE* f=fopen(workingFile.toStdString().c_str(),"w");
    if(f==NULL) {
        showWarning("There was an error while saving your file!");
        return;
    }
    const char* buffer=ui->editorTextEdit->toPlainText().toStdString().c_str();
    int size=ui->editorTextEdit->toPlainText().length();
    fwrite(buffer,sizeof(char),size,f);
    fclose(f);
}

void MainWindow::saveAsFile() {
    QFileDialog fileDialog(this,Qt::Window);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDefaultSuffix("txt");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setNameFilter("Text Files (*.txt)");
    if(!fileDialog.exec())
        return;
    QStringList fileNames=fileDialog.selectedFiles();
    QString fileName=fileNames.at(0);
    if(fileName.isEmpty())
        return;
    workingFile=fileName;
    FILE* f=fopen(workingFile.toStdString().c_str(),"w");
    if(f==NULL) {
        showWarning("There was an error while saving your file!");
        return;
    }
    const char* buffer=ui->editorTextEdit->toPlainText().toStdString().c_str();
    int size=ui->editorTextEdit->toPlainText().length();
    fwrite(buffer,sizeof(char),size,f);
    fclose(f);
}

void MainWindow::showWarning(QString warning) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Warning");
    msgBox.setText(warning);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.exec();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Warning");
    msgBox.setText("Are you sure you want to exit Cigma16?");
    QAbstractButton* buttonYes=msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.exec();
    if(msgBox.clickedButton()==buttonYes)
        event->accept();
    else
        event->ignore();
}
