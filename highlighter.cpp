#include "highlighter.h"
#include <QDebug>

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    setTheme(THEME_DARK);
    lineError=-1;
}

void Highlighter::setTheme(int th) {
    theme=th;
    if(th==THEME_LIGHT) {
        commentFormat.setForeground(Qt::darkGreen);
        labelFormat.setFontWeight(QFont::Bold);
        labelFormat.setForeground(Qt::black);
        instructionFormat.setForeground(Qt::darkRed);
    }
    else if(th==THEME_DARK) {
        commentFormat.setForeground(Qt::green);
        labelFormat.setFontWeight(QFont::Bold);
        labelFormat.setForeground(Qt::white);
        instructionFormat.setForeground(Qt::red);
    }
}

void Highlighter::setLineError(int lError) {
    lineError=lError;
}

void Highlighter::highlightBlock(const QString &text) {
    int index=0;
    int begin;
    begin=index;
    while(index<text.length() && (text[index]!=' ' && text[index]!='\t' && text[index]!='\n' && text[index]!=';')) index++;
    setFormat(begin,index-begin,labelFormat);
    if(index==text.length())
        return;
    if(text[index]=='\n') {
        index++;
        return;
    }
    if(text[index]==';') {
        begin=index;
        while(index<text.length() && (text[index]!='\n')) index++;
        setFormat(begin,index-begin,commentFormat);
        return;
    }
    while(index<text.length() && (text[index]==' ' || text[index]=='\t')) index++;
    begin=index;
    while(index<text.length() && (text[index]!=' ' && text[index]!='\t' && text[index]!='\n' && text[index]!=';')) index++;
    setFormat(begin,index-begin,instructionFormat);
    if(index==text.length())
        return;
    if(text[index]=='\n') {
        return;
    }
    if(text[index]==';') {
        begin=index;
        while(index<text.length() && text[index]!='\n') index++;
        setFormat(begin,index-begin,commentFormat);
        return;
    }
    while(index<text.length() && (text[index]!=';' && text[index]!='\n')) index++;
    if(index==text.length())
        return;
    if(text[index]=='\n') {
        index++;
        return;
    }
    if(text[index]==';') {
        begin=index;
        while(index<text.length() && text[index]!='\n') index++;
        setFormat(begin,index-begin,commentFormat);
        return;
    }
}
