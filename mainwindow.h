#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QMessageBox>
#include <QTimer>
#include "highlighter.h"
#include "assembler017.h"
#include "assembler144.h"
#include "emulator.h"

#define ARCHITECTURE_SIGMA16_0_1_7 1
#define ARCHITECTURE_SIGMA16_1_4_4 2

#define SPEED_SLOW 0
#define SPEED_MEDIUM 1
#define SPEED_FAST 2
#define SPEED_REALTIME 3
#define SPEED_NOUI 4

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void toggleSigma();  //action handler for toggling the architecture
    void toggleSpeed();  //action handler for toggling the speed
    void toggleTheme();  //action handler for toggling the theme
    void about();        //action handler for the about menu
    void newFile();      //action handler for the new menu
    void openFile();     //action handler for the open menu
    void saveFile();     //action handler for the save menu
    void saveAsFile();   //action handler for the save as menu
private slots:
    void assemble();          //action handler for the assemble button
    void resetEmulator();     //action handler for the reset emulator button
    void loadData();          //action handler for the load button
    void stepEmulator();      //action handler for the step button
    void runEmulator();       //action handler for the run button
    void timerRunEmulator();  //timer handler for the run button
    void stopEmulator();      //action handler for the stop button
    void clearTrace();        //action handler for the clear trace button
private:
    Ui::MainWindow *ui;
    Highlighter *highlighter;
    void setupEditor();        //setup the code highlighting
    int architecture=ARCHITECTURE_SIGMA16_0_1_7;  //default architecture
    QString assembly;      //holds the assembly shown in the assembly tab
    QString rawAssembly;   //holds the raw assembly code used in the emulator
    Emulator emulator;     //emulator class for running the program
    QTextCharFormat blue,background;   //the basic colours used for the themes
    QTextCursor oldLeft,oldRight;      //the selection in the memory on the previous step
    QTextCursor oldSourceCodeCursor;   //the selection in the source code on the previous step
    void updateEmulator();   //function for the updating the emulator tab
    bool running=false;      //is the program still running
    QTimer* timer;           //timer handler
    QPalette pal;            //the palette used to hold the theme colours
    void addToTrace();       //function for adding an instruction result to the trace
    void applyTheme();       //function for the applying the theme
    void showWarning(QString warning);
    int theme;   //holder for the theme
    QString workingFile="";  //the file currently being opened
    void closeEvent(QCloseEvent *event) override;  //overriden close event
    bool uienabled=true;     //is the ui enabled
    int speed=SPEED_MEDIUM;  //the speed of the emulation
};

#endif // MAINWINDOW_H
