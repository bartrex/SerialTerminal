#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QDesktopWidget>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();
    move(center.x()-width()/2, center.y()-height()/2);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushAboutCloseButton_clicked()
{
    this->close();
}
