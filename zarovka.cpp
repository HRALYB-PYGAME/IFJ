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
#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>


Zarovka::Zarovka(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Zarovka)
{
    qDebug("neco");
    ui->setupUi(this);
    this->mode = 0;

    qDebug("halo");

    selectedBgColor = QColor(0, 0, 0);  // default background color
    selectedBoardColor = QColor(171, 205, 239);
    boardAlignment = Qt::AlignLeft;
    ui->stackedWidget->setCurrentIndex(3);
    loadSettings();
    applySettings();

    qDebug("hello");

    QDir saveDir("save");
    if (!saveDir.exists()) {
        qDebug() << "Složka save/ neexistuje, vytvářím...";
        bool created = QDir().mkdir("save");
    }

    qDebug("hohoho");

    this->show();
    this->showNormal();
    raise();
    activateWindow();
}

void Zarovka::createButtons(){
    int rows = activegame.board.rows;
    int cols = activegame.board.cols;

    buttons.clear();
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QPushButton *btn = new QPushButton(QString(""));
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
    createGame(5,5);

    ui->stackedWidget->setCurrentIndex(1);
    QWidget *page = ui->stackedWidget->widget(1);

    QResizeEvent event(this->size(), this->size());
    QCoreApplication::sendEvent(this, &event);
}

void Zarovka::createGame(int w, int h, bool empty){
    activegame = game(w, h);
    if (!empty) activegame.gamecreate(0);
    clearLayour(ui->gameboard);
    clearLayour(ui->editoroptions);
    createButtons();
}

void Zarovka::clearLayour(QLayout* layout){
    QLayoutItem* item;
    while((item = layout->takeAt(0)) != nullptr){
        if (QWidget* widget = item->widget()) {
            delete widget;
        }
        delete item;
    }
}

void Zarovka::turn(QPushButton *btn, int row, int col)
{
    if (selectedtype == 0){
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
    else{
        if (activegame.getnodeat(row, col)->type == nodetype::power){
            activegame.board.powerrow = -1;
            activegame.board.powercol = -1;
            buttons[buttons.size()-1]->show();
        }
        activegame.createnode(selectedtype, row, col, selectedsides);
        if (selectedtype == nodetype::power){
            activegame.board.powerrow = row;
            activegame.board.powercol = col;
            buttons[buttons.size()-1]->hide();
        }
        activegame.update();
        updateboard(buttons[0]->width(), activegame.board.cols);
        if (!shiftheld || selectedtype==nodetype::power) selectedtype = nodetype::empty;
    }
}

void Zarovka::keyPressEvent(QKeyEvent* event) {
    shiftheld = (event->modifiers() & Qt::ShiftModifier);

    QWidget::keyPressEvent(event);
}

void Zarovka::keyReleaseEvent(QKeyEvent* event) {
    if (!(event->modifiers() & Qt::ShiftModifier)) {
        shiftheld = false;
    }

    QWidget::keyReleaseEvent(event);
}

void Zarovka::updateboard(int sidesize, int cols){
    for (int i = 0; i < buttons.size(); i++){
        QPushButton *button = buttons[i];
        button->setFixedHeight(sidesize);
        button->setFixedWidth(sidesize);
        //std::cout << "getting img for" << i/cols << " " << i%cols << std::endl;
        int row = i/cols;
        if (ui->stackedWidget->currentIndex() == 1 && row < activegame.board.rows)
            button->setIcon(QIcon(activegame.getimage(i/cols, i%cols)));
        else if (row < activegame.board.rows)
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

    // 4x draty, 4x zdroje, 1x zarovka

    int sidesize = (qMin(newsize.height(), newsize.width())/(rows+1))*0.8;

    for (auto& btn : buttons){
        btn->setFixedHeight(sidesize);
        btn->setFixedWidth(sidesize);
        btn->setIconSize(btn->size());
    }

    if (activegame.editing && ui->editoroptions->isEmpty()){
        qDebug("ag.empty");
        std::vector<std::string> icons = {":linkI.png", ":linkL.png", ":linkT.png", ":linkX.png", ":bulb.png", ":power.png"};
        std::vector<nodetype> nodetypes = {nodetype::link, nodetype::link, nodetype::link, nodetype::link, nodetype::bulb, nodetype::power};
        std::vector<std::array<bool, 4>> sides = {{true, false, true, false},{true, true, false, false},{true, true, true, false},
                                                {true, true, true, true},{true, false, false, false},{true, false, false, false}};

        QPushButton *btn = new QPushButton(QString("SAVE"));
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
        std::string name = "test";
        connect(btn, &QPushButton::clicked, this, [this, name](){
            activegame.savegame(currentgamename);
        });
        ui->editoroptions->addWidget(btn);
        buttons.insert(buttons.end(), btn);

        for(int i=0; i<icons.size(); i++){
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
            btn->setIcon(QIcon(QString::fromStdString(icons[i])));
            nodetype nt = nodetypes[i];
            std::array<bool, 4> s = sides[i];
            connect(btn, &QPushButton::clicked, this, [this, nt, s](){
                selectedtype = nt;
                selectedsides = s;
            });
            ui->editoroptions->addWidget(btn);
            buttons.insert(buttons.end(), btn);
        }
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
    ui->stackedWidget->setCurrentIndex(6);
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
    currentgamename = ui->levelname->toPlainText();
    activegame.editing = true;
    ui->stackedWidget->setCurrentIndex(1);
    QWidget *page = ui->stackedWidget->widget(1);

    QResizeEvent event(this->size(), this->size());
    QCoreApplication::sendEvent(this, &event);
}

void Zarovka::on_backToMenuButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Zarovka::on_myLevelsButton_clicked()
{
    loadLevelList();
    ui->stackedWidget->setCurrentIndex(7);
}

void Zarovka::on_createNewButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void Zarovka::on_backToEditorMenuButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}

QStringList Zarovka::getLevelFiles()
{
    QDir dir("save"); //asi spis v save
    QStringList filters;
    filters << "*.zvaz";
    QStringList files = dir.entryList(filters, QDir::Files, QDir::Name);
    return files;
}

void Zarovka::loadLevelList()
{
    QLayoutItem *item;
    while ((item = ui->levelListLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    QStringList levelFiles = getLevelFiles();

    if (levelFiles.isEmpty()) {
        QLabel *noLevelsLabel = new QLabel("Zatím nemáte žádné uložené levely");
        noLevelsLabel->setAlignment(Qt::AlignCenter);
        noLevelsLabel->setStyleSheet("font-size: 18px; color: gray;");
        ui->levelListLayout->addWidget(noLevelsLabel);
    } else {
        for (const QString &filename : levelFiles) {
            QWidget *rowWidget = new QWidget();
            QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);

            QString levelName = filename;
            levelName.chop(5);

            QLabel *nameLabel = new QLabel(levelName);
            nameLabel->setStyleSheet("font-size: 16px; padding: 10px;");
            nameLabel->setMinimumWidth(300);

            // Editovat
            QPushButton *editBtn = new QPushButton("Editovat");
            editBtn->setMinimumSize(100, 40);
            editBtn->setStyleSheet("font-size: 14px;");
            connect(editBtn, &QPushButton::clicked, this, [this, filename]() {
                activegame.loadgame(filename);
                qDebug() << "Editovat level:" << filename;
                // TODO
            });

            // Přejmenovat
            QPushButton *renameBtn = new QPushButton("Přejmenovat");
            renameBtn->setMinimumSize(120, 40);
            renameBtn->setStyleSheet("font-size: 14px;");
            connect(renameBtn, &QPushButton::clicked, this, [this, filename]() {
                qDebug() << "Přejmenovat level:" << filename;
                // TODO
            });

            // Odstranit
            QPushButton *deleteBtn = new QPushButton("Odstranit");
            deleteBtn->setMinimumSize(100, 40);
            deleteBtn->setStyleSheet("font-size: 14px; background-color: #ff4444; color: white;");
            connect(deleteBtn, &QPushButton::clicked, this, [this, filename]() {
                activegame.deletegame(filename);
                loadLevelList();
                qDebug() << "Odstranit level:" << filename;
                // TODO
            });

            rowLayout->addWidget(nameLabel);
            rowLayout->addStretch();
            rowLayout->addWidget(editBtn);
            rowLayout->addWidget(renameBtn);
            rowLayout->addWidget(deleteBtn);

            ui->levelListLayout->addWidget(rowWidget);

            QFrame *line = new QFrame();
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            ui->levelListLayout->addWidget(line);
        }
    }
}

void Zarovka::on_createLevelButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

