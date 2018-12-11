#ifndef QHEXTEXTEDIT_H
#define QHEXTEXTEDIT_H
#include <QtWidgets>
#include <QByteArray>

class QHexTextEdit : public QTextEdit
{
private:
    QColor firstColor;
    QColor secondColor;
    QColor textColor;
    bool toogleColor;
public:
    QHexTextEdit(QWidget *parent = nullptr): QTextEdit(parent), firstColor(QColor(Qt::GlobalColor::black)), secondColor(QColor(Qt::GlobalColor::black)), toogleColor(false) {}
    QHexTextEdit(const QColor firstr, const QColor second, const QColor txtColor = QColor(Qt::GlobalColor::black), QWidget *parent = nullptr):
        QTextEdit(parent), firstColor(firstr), secondColor(second), textColor(txtColor), toogleColor(false) {}

    virtual void setColors(const QColor firstr, const QColor second)
    {
        firstColor = firstr;
        secondColor = second;
    }
    virtual void addHex(const QByteArray& dataArray);
    virtual void addText(const QString& text);
};

#endif // QHEXTEXTEDIT_H
