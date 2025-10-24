#include "zarovka.h"
#include "./ui_zarovka.h"

Zarovka::Zarovka(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Zarovka)
{
    ui->setupUi(this);
    this->mode = 0;
}

Zarovka::~Zarovka()
{
    delete ui;
}

void Zarovka::on_playButton_clicked()
{
    ui->playButton->setText("ahoj Karle");
    this->mode = 1;
    ui->stackedWidget->setCurrentIndex(0);
}

void Zarovka::updateUI(int mode){
    ui->playButton->setVisible(true);
    //QBitmap* bitmap = new QBitmap(ui->playButton->width(), ui->playButton->height());
    //this->setMask(*bitmap);
    switch(mode){
    case 0:
        this->activateWindow();
        //ui->gameMenu->widget()->setVisible(true);
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 1:
        //ui->gameMenu->widget()->setVisible(false);
        //ui->gameMenuWidget->setVisible(false);
        //ui->widgetTest->setVisible(false);
        //this->close();
        break;
    }
}

