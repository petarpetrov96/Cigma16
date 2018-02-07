#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPalette>
#include <QFileDialog>
#include <string>

/*
 * Constructor
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Sets up the text editor
    setupEditor();

    // Sets up the action handlers for the Compatibility menu
    QActionGroup* architectureActionGroup=new QActionGroup(this);
    architectureActionGroup->addAction(ui->actionSigma16_0_1_7);
    architectureActionGroup->addAction(ui->actionSigma16_1_4_4);
    connect(ui->actionSigma16_0_1_7,&QAction::triggered,this,&MainWindow::toggleSigma);
    connect(ui->actionSigma16_1_4_4,&QAction::triggered,this,&MainWindow::toggleSigma);

    // Sets up the action handlers for the Speed menu
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

    // Sets up the action handlers for the File menu
    connect(ui->actionNew,&QAction::triggered,this,&MainWindow::newFile);
    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::openFile);
    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::saveFile);
    connect(ui->actionSave_As,&QAction::triggered,this,&MainWindow::saveAsFile);
    connect(ui->actionExit,&QAction::triggered,this,&MainWindow::close);
    connect(ui->actionAbout,&QAction::triggered,this,&MainWindow::about);

    // Sets up the action handlers for the Theme menu
    QActionGroup* themeActionGroup=new QActionGroup(this);
    themeActionGroup->addAction(ui->actionLight);
    themeActionGroup->addAction(ui->actionDark);
    connect(ui->actionLight,&QAction::toggled,this,&MainWindow::toggleTheme);
    connect(ui->actionDark,&QAction::toggled,this,&MainWindow::toggleTheme);

    // Sets up the action handlers for the buttons in the UI
    connect(ui->assembleButton,SIGNAL(released()),this,SLOT(assemble()));
    connect(ui->resetButton,SIGNAL(released()),this,SLOT(resetEmulator()));
    connect(ui->loadButton,SIGNAL(released()),this,SLOT(loadData()));
    connect(ui->stepButton,SIGNAL(released()),this,SLOT(stepEmulator()));
    connect(ui->runButton,SIGNAL(released()),this,SLOT(runEmulator()));
    connect(ui->stopButton,SIGNAL(released()),this,SLOT(stopEmulator()));
    connect(ui->clearButton,SIGNAL(released()),this,SLOT(clearTrace()));

    // Initializes the theme
    theme=THEME_LIGHT;
    background.setForeground(Qt::black);
    blue.setForeground(Qt::blue);
    pal.setColor(QPalette::Base,Qt::white);
    pal.setColor(QPalette::Text,Qt::black);
    highlighter->setTheme(theme);

    // Applies the theme
    applyTheme();

    // Initializes the emulator
    updateEmulator();

    // Sets up the timer that runs the emulator
    timer=new QTimer(this);
    timer->setInterval(100);
    timer->connect(timer,SIGNAL(timeout()),this,SLOT(timerRunEmulator()));
}

/*
 * Changes the theme based on the user choice
 */
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

/*
 * Applies the theme to the individual elements
 */
void MainWindow::applyTheme() {
    // Text inputs
    ui->editorTextEdit->setPalette(pal);
    ui->assemblerTextEdit->setPalette(pal);

    // Labels
    ui->emEffect->setPalette(pal);
    ui->emOperands->setPalette(pal);
    ui->emOperation->setPalette(pal);
    ui->emType->setPalette(pal);
    ui->registerADR->setPalette(pal);
    ui->registerDAT->setPalette(pal);
    ui->registerFile->setPalette(pal);
    ui->registerIR->setPalette(pal);
    ui->registerPC->setPalette(pal);

    // Memory files
    ui->memoryFileLeft->setPalette(pal);
    ui->memoryFileRight->setPalette(pal);

    // Output field
    ui->inputOutput->setPalette(pal);

    // Instruction list
    ui->sourceCode->setPalette(pal);

    // Trace
    ui->traceTextEdit->setPalette(pal);
    highlighter->rehighlight();

    // Updates the highlighting in the memory files
    QTextCursor cursorLeft(ui->memoryFileLeft->document());
    cursorLeft.select(QTextCursor::Document);
    cursorLeft.setCharFormat(background);
    QTextCursor cursorRight(ui->memoryFileRight->document());
    cursorRight.select(QTextCursor::Document);
    cursorRight.setCharFormat(background);
}

/*
 * Sets up the text editor
 */
void MainWindow::setupEditor() {
    QTextEdit* textEdit=ui->editorTextEdit;

    // Courier size 10
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    textEdit->setFont(font);

    // No word wrap
    textEdit->setWordWrapMode(QTextOption::NoWrap);

    // Intializes a new highlighter
    highlighter=new Highlighter(textEdit->document());
    highlighter->setLineError(4);
}

/*
 * Changes the Sigma16 version based on user input
 */
void MainWindow::toggleSigma() {
    // Toggles the version
    if(ui->actionSigma16_0_1_7->isChecked() && architecture!=ARCHITECTURE_SIGMA16_0_1_7)
        architecture=ARCHITECTURE_SIGMA16_0_1_7;
    else if(ui->actionSigma16_1_4_4->isChecked() && architecture!=ARCHITECTURE_SIGMA16_1_4_4)
        architecture=ARCHITECTURE_SIGMA16_1_4_4;

    // Resets the emulator
    resetEmulator();
}

/*
 * Changes the speed of emulation based on user input
 */
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

/*
 * Assembles the provided source into machine code
 */
void MainWindow::assemble() {
    // Fetch a string representation of the text
    QString tempString=ui->editorTextEdit->document()->toPlainText();

    // Convert to C string
    int dataToCopyLength = strlen(tempString.toStdString().c_str());
    char* data=new char[dataToCopyLength+1];
    memcpy(data,tempString.toStdString().c_str(),dataToCopyLength);
    data[dataToCopyLength]='\0';

    // Assemble based on Sigma16 version
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

    // Remove the first five characters (ASM03)
    QString test(rawAssembly);
    if(test.left(5).compare("ASM03")!=0) {
        ui->assemblerTextEdit->setText(test);
        delete[] data;
        return;
    }
    test=test.right(test.length()-5);

    // Remove the source code
    int sourceCodeLength=0;
    int digitPosition;
    for(digitPosition=0;digitPosition<test.length();digitPosition++) {
        if(!isdigit(test[digitPosition].toLatin1())) {
            break;
        }
        sourceCodeLength=sourceCodeLength*10+(test[digitPosition].toLatin1()-'0');
    }
    test=test.right(test.length()-(sourceCodeLength+digitPosition+1));

    // Splits the assembly into groups that can be shown on the Assembly tab
    assembly="";
    int line=0;
    char temp[50];
    for(int i=0;i<test.length();i+=4) {

        // Instruction line number
        sprintf(temp,"%04hx",line);
        assembly.append(temp[0]);
        assembly.append(temp[1]);
        assembly.append(temp[2]);
        assembly.append(temp[3]);
        assembly.append('\t');

        i+=4;

        // RX instruction
        if(test[i]=='f') {

            // First part
            assembly.append(test[i]);
            assembly.append(test[i+1]);
            assembly.append(test[i+2]);
            assembly.append(test[i+3]);
            assembly.append(' ');

            i+=4;

            // Second part
            assembly.append(test[i+4]);
            assembly.append(test[i+5]);
            assembly.append(test[i+6]);
            assembly.append(test[i+7]);
            assembly.append('\n');
            i+=4;
            line++;
        }
        // RRR instruction
        else {
            assembly.append(test[i]);
            assembly.append(test[i+1]);
            assembly.append(test[i+2]);
            assembly.append(test[i+3]);
            assembly.append('\n');
        }
        line++;
    }

    // Clear the text field and sets the newly assembled text
    ui->assemblerTextEdit->clear();
    ui->assemblerTextEdit->setText(assembly);

    // Memory management
    delete[] data;
}

/*
 * Updates all emulator-related information in the UI
 */
void MainWindow::updateEmulator() {
    // If there has been any new output since the last update, add it to the UI
    QString output=emulator.getOutput().c_str();
    if(output!="") {
        ui->inputOutput->moveCursor(QTextCursor::End);
        if(architecture==ARCHITECTURE_SIGMA16_0_1_7)
            ui->inputOutput->moveCursor(QTextCursor::StartOfLine); // Sigma16 0.1.7 bugfix (printing is weird on it)
        ui->inputOutput->insertPlainText(output);
    }

    // Don't proceed if we are in No UI mode
    if(!uienabled)
        return;

    // Show PC/IR/ADR/DAT registers
    char temp[50];
    sprintf(temp,"%04hx",emulator.pc);
    ui->registerPC->setText(QString(temp));
    sprintf(temp,"%04hx",emulator.ir);
    ui->registerIR->setText(QString(temp));
    sprintf(temp,"%04hx",emulator.adr);
    ui->registerADR->setText(QString(temp));
    sprintf(temp,"%04hx",emulator.dat);
    ui->registerDAT->setText(QString(temp));

    // Update register file
    ui->registerFile->clear();
    ui->registerFile->appendPlainText(emulator.getRegisterFile().c_str());

    // Update memory file if it's been modified since the last check (performance optimization)
    if(emulator.isMemoryModified()) {
        // Doesn't update the UI until the end of the needed operation (it's faster)
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

        // The UI can be updated now
        setUpdatesEnabled(true);
    }
    else {
        oldLeft.setCharFormat(background);
        oldRight.setCharFormat(background);
    }

    // If a memory address has been modified in the last instruction, move the cursor to that address
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

    // Set the changes to the cursor in the memory files
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

    // Updates debug information about the emulator
    ui->emOperation->setText(emulator.lastOperation.c_str());
    ui->emOperands->setText(emulator.lastOperands.c_str());
    ui->emType->setText(emulator.lastType.c_str());
    ui->emEffect->setText(emulator.lastEffect.c_str());

    // Updates the line number on the source code
    ui->sourceCode->setTextCursor(QTextCursor(ui->sourceCode->document()->findBlockByLineNumber(emulator.lastLine)));

    // Checks for division by zero
    if(emulator.lastEffect=="DIVISION BY ZERO")
        showWarning("Division by zero detected!");
}

/*
 * Resets the emulator
 */
void MainWindow::resetEmulator() {
    emulator.reset();
    updateEmulator();
    ui->inputOutput->clear();
    timer->stop();
}

/*
 * Loads the machine code into the emulator
 */
void MainWindow::loadData() {
    std::string assembly = std::string(rawAssembly.toLocal8Bit().constData());
    emulator.loadMemory(assembly);

    // Updates the source code
    ui->sourceCode->setPlainText(QString(emulator.getSourceCode().c_str()));

    updateEmulator();
}

/*
 * Performs a single step in the emulator
 */
void MainWindow::stepEmulator() {
    bool temp=emulator.isHalted();
    if(architecture==ARCHITECTURE_SIGMA16_0_1_7)
        emulator.step017();
    else if(architecture==ARCHITECTURE_SIGMA16_1_4_4)
        emulator.step144();

    // If the emulator hasn't been halted before the operation, add the previous instruction to the trace
    if(!temp)
        addToTrace();
    updateEmulator();
}

/*
 * Runs the emulator
 */
void MainWindow::runEmulator() {
    running=true;
    timer->start();
}

/*
 * Helper function for the emulator run timer
 */
void MainWindow::timerRunEmulator() {
    if(!running)
        return;

    // Executes an instruction and checks if there is a halt
    if((architecture==ARCHITECTURE_SIGMA16_0_1_7 && !emulator.step017()) ||
       (architecture==ARCHITECTURE_SIGMA16_1_4_4 && !emulator.step144())) {
        // Halted
        addToTrace();
        updateEmulator();
        running=false;
        timer->stop();
        return;
    }

    // Not halted
    addToTrace();
    updateEmulator();
}

/*
 * Stops the emulator run timer
 */
void MainWindow::stopEmulator() {
    running=false;
    timer->stop();
}

/*
 * Clears the trace
 */
void MainWindow::clearTrace() {
    ui->traceTextEdit->clear();
}

/*
 * Shows an About message box
 */
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

/*
 * Adds the last instruction information to the trace
 */
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

/*
 * Action handler for the File->New menu button
 */
void MainWindow::newFile() {
    // Message box
    QMessageBox msgBox;
    msgBox.setWindowTitle("Warning");
    msgBox.setText("Are you sure you want to create a new file?");
    msgBox.addButton(QMessageBox::Yes);
    QAbstractButton* buttonNo=msgBox.addButton(QMessageBox::No);
    msgBox.exec();

    // No pressed
    if(msgBox.clickedButton()==buttonNo)
        return;

    // Yes pressed
    workingFile="";
    ui->editorTextEdit->clear();
}

/*
 * Action handler for the File->Open menu button
 */
void MainWindow::openFile() {
    if(workingFile!="" || ui->editorTextEdit->document()->toPlainText()!="") {
        // Message box warning
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Are you sure you want to dismiss all your changes?");
        msgBox.addButton(QMessageBox::Yes);
        QAbstractButton* buttonNo=msgBox.addButton(QMessageBox::No);
        msgBox.exec();
        if(msgBox.clickedButton()==buttonNo)
            return;
    }

    // Open file dialog
    QString fileName=QFileDialog::getOpenFileName(this,"Open File","","Text Files (*.txt)");
    if(fileName.isEmpty())
        return;

    // Reads the selected file
    FILE* f=fopen(fileName.toStdString().c_str(),"r");
    if(f==NULL) {
        showWarning("There was an error while trying to open the file");
        return;
    }

    // Fills the text box with the file contents
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

/*
 * Action handler for the File->Save menu button
 */
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

/*
 * Action handler for the File->Save As menu button
 */
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

/*
 * Shows a warning with a certain text
 */
void MainWindow::showWarning(QString warning) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Warning");
    msgBox.setText(warning);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.exec();
}

/*
 * Deconstructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * Handles Alt-F4 and window close events
 */
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
