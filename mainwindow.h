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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void toggleSigma();
    void toggleSpeed();
    void toggleTheme();
    void about();
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
private slots:
    void assemble();
    void resetEmulator();
    void loadData();
    void stepEmulator();
    void runEmulator();
    void timerRunEmulator();
    void stopEmulator();
    void clearTrace();
private:
    Ui::MainWindow *ui;
    Highlighter *highlighter;
    void setupEditor();
    int architecture=ARCHITECTURE_SIGMA16_0_1_7;
    QString assembly;
    QString rawAssembly;
    Emulator emulator;
    QTextCharFormat blue,background;
    QTextCursor oldLeft,oldRight;
    void updateEmulator();
    bool running=false;
    QTimer* timer;
    QPalette pal;
    void addToTrace();
    void applyTheme();
    void showWarning(QString warning);
    int theme;
    QString workingFile="";
    void closeEvent(QCloseEvent *event) override;
    bool uienabled=true;
    int speed=2;
};

#endif // MAINWINDOW_H
