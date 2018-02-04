#include "highlighter.h"

/*
 * Constructor
 */
Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    // Initializes a dark theme with no error line
    setTheme(THEME_DARK);
    lineError=-1;
}

/*
 * Sets the theme
 */
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

/*
 * Sets an error on a certain line
 */
void Highlighter::setLineError(int lError) {
    lineError=lError;
}

/*
 * Highlights a QString
 * Usually a single row
 */
void Highlighter::highlightBlock(const QString &text) {
    int index=0;
    int begin;
    begin=index;

    // Label name (read until space, tab, newline, semicolon or end of string)
    while(index<text.length() && (text[index]!=' ' && text[index]!='\t' && text[index]!='\n' && text[index]!=';')) index++;
    setFormat(begin,index-begin,labelFormat);

    // We have reached the end of the text
    if(index==text.length())
        return;

    // We have reached a new line
    if(text[index]=='\n') {
        index++;
        return;
    }

    // We have reached a comment
    if(text[index]==';') {
        begin=index;

        // Highlight until we reach end of string or a new line
        while(index<text.length() && (text[index]!='\n')) index++;
        setFormat(begin,index-begin,commentFormat);
        return;
    }

    // Instruction name (read until space, tab or end of string)
    while(index<text.length() && (text[index]==' ' || text[index]=='\t')) index++;

    // Instruction arguments (read until space, tab, new line, semicolon or end of string)
    begin=index;
    while(index<text.length() && (text[index]!=' ' && text[index]!='\t' && text[index]!='\n' && text[index]!=';')) index++;
    setFormat(begin,index-begin,instructionFormat);

    // We have reached end of string
    if(index==text.length())
        return;

    // We have reached end of line
    if(text[index]=='\n') {
        return;
    }

    // We have reached a comment
    if(text[index]==';') {
        begin=index;

        // Highlight until we reach end of string or new line
        while(index<text.length() && text[index]!='\n') index++;
        setFormat(begin,index-begin,commentFormat);
        return;
    }

    // If there is a comment separated by empty spaces
    while(index<text.length() && (text[index]!=';' && text[index]!='\n')) index++;

    // We have reached end of string
    if(index==text.length())
        return;

    // We have reached end of line
    if(text[index]=='\n') {
        index++;
        return;
    }

    // We have reached a comment
    if(text[index]==';') {
        begin=index;

        // Highlight until end of line or end of string
        while(index<text.length() && text[index]!='\n') index++;
        setFormat(begin,index-begin,commentFormat);
        return;
    }
}
