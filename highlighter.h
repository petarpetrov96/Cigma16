#include<QSyntaxHighlighter>
#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#define THEME_LIGHT 1
#define THEME_DARK 2

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QTextDocument *parent=0);
    void setLineError(int lError);
    int theme=THEME_LIGHT;
    void setTheme(int th);
protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;
private:
    QTextCharFormat commentFormat;
    QTextCharFormat labelFormat;
    QTextCharFormat instructionFormat;
    int lineError;
};

#endif // HIGHLIGHTER_H
