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
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>


Zarovka::Zarovka(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Zarovka)
{
    ui->setupUi(this);
    this->mode = 0;

    selectedBgColor = QColor(0, 0, 0);  // default background color
    selectedBoardColor = QColor(171, 205, 239);
    boardAlignment = Qt::AlignLeft;
    ui->stackedWidget->setCurrentIndex(3);
    loadSettings();
    applySettings();
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

void Zarovka::on_settingsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void Zarovka::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
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
    createGame(50,50);

    ui->stackedWidget->setCurrentIndex(1);
    QWidget *page = ui->stackedWidget->widget(1);

    QResizeEvent event(this->size(), this->size());
    QCoreApplication::sendEvent(this, &event);
}

void Zarovka::createGame(int w, int h, bool empty){
    activegame = game(w, h);
    if (!empty) activegame.gamecreate(0);
}

void Zarovka::turn(QPushButton *btn, int row, int col)
{
    activegame.rotate(row, col);
    QPixmap rotated = activegame.getimage(row, col);
    btn->setIcon(QIcon(rotated));
    btn->setIconSize(btn->size());
    btn->setText("");
    activegame.update();
    updateboard(buttons[0]->width(), activegame.board.cols);
    if (activegame.arebulbslit()){
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void Zarovka::updateboard(int sidesize, int cols){
    for (int i = 0; i < buttons.size(); i++){
        QPushButton *button = buttons[i];
        button->setFixedHeight(sidesize);
        button->setFixedWidth(sidesize);
        //std::cout << "getting img for" << i/cols << " " << i%cols << std::endl;
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
    ui->gameboard->setAlignment(boardAlignment);
    const int rows = activegame.board.rows;
    const int cols = activegame.board.cols;

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
                    "    background-color: #abcdef;"
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

    //applySettings();

    updateboard(sidesize, cols);
}

void Zarovka::loadSettings()
{
    QFile file("settings.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        if (obj.contains("backgroundColor")) {
            QString colorStr = obj["backgroundColor"].toString();
            selectedBgColor = QColor(colorStr);
        } else {
            selectedBgColor = QColor(0, 0, 0);
        }

        if (obj.contains("boardColor")) {
            QString colorStr = obj["boardColor"].toString();
            selectedBoardColor = QColor(colorStr);
        } else {
            selectedBoardColor = QColor(171, 205, 239);
        }

        if (obj.contains("boardAlignment")) {
            QString alignment = obj["boardAlignment"].toString();
            boardAlignment = (alignment == "right") ? Qt::AlignRight : Qt::AlignLeft;
        } else {
            boardAlignment = Qt::AlignLeft;
        }
    } else {
        selectedBgColor = QColor(0, 0, 0);
        selectedBoardColor = QColor(171, 205, 239);
        boardAlignment = Qt::AlignLeft;
    }

    updateColorButtons();
}

void Zarovka::saveSettings()
{
    QJsonObject obj;
    obj["backgroundColor"] = selectedBgColor.name();
    obj["boardColor"] = selectedBoardColor.name();
    obj["boardAlignment"] = (boardAlignment == Qt::AlignRight) ? "right" : "left";

    QJsonDocument doc(obj);

    QFile file("settings.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson());
        file.close();
    }
}

void Zarovka::applySettings()
{
    QString style = QString("QMainWindow { background-color: %1; }").arg(selectedBgColor.name());
    this->setStyleSheet(style);
    //for(QPushButton* button : buttons){
    //    button->setStyleSheet(style);
    //}

    if (ui->gameboard) {
        ui->gameboard->setAlignment(boardAlignment);
    }



    updateColorButtons();
}

void Zarovka::updateColorButtons()
{
    ui->colorWhiteButton->setStyleSheet(
        "background-color: #969696; "
        "min-width: 20px; min-height: 20px; "
        "border-radius: 5px;"
        );

    ui->colorBlackButton->setStyleSheet(
        "background-color: black; "
        "min-width: 20px; min-height: 20px; "
        "border-radius: 5px;"
        );

    ui->colorBlueButton->setStyleSheet(
        "background-color: #003366; "
        "min-width: 20px; min-height: 20px; "
        "border-radius: 5px;"
        );

    if (selectedBgColor == QColor(150, 150, 150)) {
        ui->colorWhiteButton->setStyleSheet(
            "background-color: #969696; "
            "min-width: 20px; min-height: 20px; "
            "border: 4px solid red;"
            "border-radius: 5px;"
            );
    }
    else if (selectedBgColor == QColor(0, 0, 0)) {
        ui->colorBlackButton->setStyleSheet(
            "background-color: black; "
            "min-width: 20px; min-height: 20px; "
            "border: 4px solid red;"
            "border-radius: 5px;"
            );
    }
    else if (selectedBgColor == QColor(0, 51, 102)) {
        ui->colorBlueButton->setStyleSheet(
            "background-color: #003366; "
            "min-width: 20px; min-height: 20px; "
            "border: 4px solid red;"
            "border-radius: 5px;"
            );
    }

    ui->colorWhiteButton_3->setStyleSheet(
        "background-color: white; "
        "min-width: 20px; min-height: 20px; "
        "border-radius: 5px;"
        );

    ui->colorBlueButton_3->setStyleSheet(
        "background-color: #87CEEB; "
        "min-width: 20px; min-height: 20px; "
        "border-radius: 5px;"
        );

    ui->colorBlackButton_3->setStyleSheet(
        "background-color: #F2FDD0; "
        "min-width: 20px; min-height: 20px; "
        "border-radius: 5px;"
        );

    if (selectedBoardColor == QColor(255, 255, 255)) {
        ui->colorWhiteButton_3->setStyleSheet(
            "background-color: white; "
            "border: 4px solid red; "
            "border-radius: 5px;"
            );
    }
    else if (selectedBoardColor == QColor(135, 206, 235)) {
        ui->colorBlueButton_3->setStyleSheet(
            "background-color: #87CEEB; "
            "border: 4px solid red; "
            "border-radius: 5px;"
            );
    }
    else if (selectedBoardColor == QColor(255, 253, 208)) {
        ui->colorBlackButton_3->setStyleSheet(
            "background-color: #F2FDD0; "
            "border: 4px solid red; "
            "border-radius: 5px;"
            );
    }

    ui->colorWhiteButton_5->setStyleSheet(
        boardAlignment == Qt::AlignLeft
            ? "background-color: white; color: black; border: 4px solid red; border-radius: 5px;"
            : "background-color: white; color: black; border-radius: 5px;"
        );

    ui->colorBlackButton_5->setStyleSheet(
        boardAlignment == Qt::AlignRight
            ? "background-color: white; color: black; border: 4px solid red; border-radius: 5px;"
            : "background-color: white; color: black; border-radius: 5px;"
        );
}

void Zarovka::on_colorWhiteButton_5_clicked()  // left
{
    boardAlignment = Qt::AlignLeft;
    applySettings();
    saveSettings();
}

void Zarovka::on_colorBlackButton_5_clicked()  // right
{
    boardAlignment = Qt::AlignRight;
    applySettings();
    saveSettings();
}

void Zarovka::on_colorWhiteButton_3_clicked()
{
    selectedBoardColor = QColor(255, 255, 255);
    applySettings();
    saveSettings();
}

void Zarovka::on_colorBlueButton_3_clicked()
{
    selectedBoardColor = QColor(135, 206, 235);
    applySettings();
    saveSettings();
}

void Zarovka::on_colorBlackButton_3_clicked()
{
    selectedBoardColor = QColor(255, 253, 208);
    applySettings();
    saveSettings();
}

void Zarovka::on_colorWhiteButton_clicked()
{
    selectedBgColor = QColor(150, 150, 150);
    applySettings();
    saveSettings();
}

void Zarovka::on_colorBlackButton_clicked()
{
    selectedBgColor = QColor(0, 0, 0);
    applySettings();
    saveSettings();
}

void Zarovka::on_colorBlueButton_clicked()
{
    selectedBgColor = QColor(0, 51, 102);
    applySettings();
    saveSettings();
}

void Zarovka::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void Zarovka::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}


void Zarovka::on_widthplus_clicked()
{
    auto current = ui->widthlabel->text().toInt();
    current++;
    if (current > 20) current = 20;
    ui->widthlabel->setText(QString::number(current));
}


void Zarovka::on_widthminus_clicked()
{
    auto current = ui->widthlabel->text().toInt();
    current--;
    if (current < 2) current = 2;
    if (ui->heightlabel->text().toInt() > current)
        ui->heightlabel->setText(QString::number(current));
    ui->widthlabel->setText(QString::number(current));
}


void Zarovka::on_heightplus_clicked()
{
    auto current = ui->heightlabel->text().toInt();
    current++;
    if (current > 20) current = 20;
    if (ui->widthlabel->text().toInt() < current)
        ui->widthlabel->setText(QString::number(current));
    ui->heightlabel->setText(QString::number(current));
}


void Zarovka::on_heightminus_clicked()
{
    auto current = ui->heightlabel->text().toInt();
    current--;
    if (current < 2) current = 2;
    ui->heightlabel->setText(QString::number(current));
}


void Zarovka::on_pushButton_4_clicked()
{
    createGame(ui->widthlabel->text().toInt(),ui->heightlabel->text().toInt(), true);
    activegame.editing = true;
    ui->stackedWidget->setCurrentIndex(1);
    QWidget *page = ui->stackedWidget->widget(1);

    QResizeEvent event(this->size(), this->size());
    QCoreApplication::sendEvent(this, &event);
}

