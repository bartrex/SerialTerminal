#include "qhextextedit.h"

void QHexTextEdit::addHex(const QByteArray& dataArray)
{
    int r = 0, g = 0, b = 0;
    for(char byte : dataArray)
    {
        if(!toogleColor)
        {
            firstColor.getRgb(&r,&g,&b);
        }
        else
        {
            secondColor.getRgb(&r,&g,&b);
        }
        toogleColor = !toogleColor;
        QString data{};
        data.sprintf("<font color=#%02x%02x%02x>%02x </font>",r,g,b,byte);
        moveCursor(QTextCursor::End);
        textCursor().insertHtml(data);
    }
}

void QHexTextEdit::addText(const QString& text)
{
    int r = 0, g = 0, b = 0;
    textColor.getRgb(&r,&g,&b);
    QString data{};
    data.sprintf("<font color=#%02x%02x%02x></font>",r,g,b);
    moveCursor(QTextCursor::End);
    textCursor().insertHtml(data);
    moveCursor(QTextCursor::End);
    textCursor().insertHtml(text);
}
