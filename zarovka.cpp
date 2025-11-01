#include "zarovka.h"
#include "./ui_zarovka.h"
#include "BE/game.h"
#include "BE/gametypes.h"

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
    this->mode = 1;
    ui->stackedWidget->setCurrentIndex(0);
}

/*void Zarovka::resizeEvent(QResizeEvent *event){
    int size = qMin(width(), height());
    ui->stackedWidget->setFixedSize(size, size);

    QMainWindow::resizeEvent(event);
}*/

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


void Zarovka::on_easyButton_clicked()
{
    //game newgame(4, 4);

}

