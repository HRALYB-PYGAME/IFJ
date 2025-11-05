#include "zarovka.h"
#include "./ui_zarovka.h"
#include "BE/game.h"
#include "BE/gametypes.h"
#include <iostream>
#include "squarebutton.h"
#include <QWidget>
#include <QResizeEvent>
#include <QDebug>
#include <QFile>
#include <QResizeEvent>

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
    activegame = game(5,5);
    activegame.gamecreate(0);

    ui->stackedWidget->setCurrentIndex(1);
    QWidget *page = ui->stackedWidget->widget(1);

    QResizeEvent event(this->size(), this->size());
    QCoreApplication::sendEvent(this, &event);
}

void Zarovka::turn(QPushButton *btn, int row, int col)
{
    activegame.rotate(row, col);
    QPixmap rotated = activegame.getimage(row, col);
    btn->setIcon(QIcon(rotated));
    btn->setIconSize(btn->size());
    btn->setText("");
    activegame.update();
    updateboard(buttons[0]->width(), 5);
    if (activegame.arebulbslit()){
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void Zarovka::updateboard(int sidesize, int cols){
    for (int i = 0; i < buttons.size(); i++){
        QPushButton *button = buttons[i];
        button->setFixedHeight(sidesize);
        button->setFixedWidth(sidesize);
        if (ui->stackedWidget->currentIndex() == 1)
            button->setIcon(QIcon(activegame.getimage(i/cols, i%cols)));
        else
            button->setIcon(QIcon(":img.png"));
        button->setIconSize(button->size());
    }
}

void Zarovka::resizeEvent(QResizeEvent *event)
{
    ui->gameboard->setSpacing(0);
    ui->gameboard->setContentsMargins(0, 0, 0, 0);
    ui->gameboard->setAlignment(Qt::AlignLeft);
    const int rows = 5;
    const int cols = 5;

    QWidget::resizeEvent(event);
    QSize newsize = event->size();

    int sidesize = (qMin(newsize.height(), newsize.width())/rows)*0.8;

    if (ui->gameboard->isEmpty()){
        buttons = std::vector<QPushButton*>();
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                QPushButton *btn = new QPushButton(QString(""));
                btn->setFixedHeight(sidesize);
                btn->setFixedWidth(sidesize);
                btn->setStyleSheet(
                    "QPushButton {"
                    "    border: none;"
                    "    border-radius: 0;"
                    "    background-color: #ff0000;"
                    "    outline: none;"
                    "}"
                    "QPushButton:pressed {"
                    "    padding-left: 1px;"
                    "    padding-top: 1px;"
                    "}"
                    );
                connect(btn, &QPushButton::clicked, this, [this, row, col]() {
                    turn(qobject_cast<QPushButton*>(sender()), row, col);
                });
                ui->gameboard->addWidget(btn, row, col);
                buttons.insert(buttons.end(), btn);
            }
        }
        updateboard(sidesize, cols);
    }

    updateboard(sidesize, cols);
}
