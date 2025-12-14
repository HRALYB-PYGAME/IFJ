#include "zarovka.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>
#include "./ui_zarovka.h"
#include "BE/game.h"
#include "BE/gametypes.h"
#include <iostream>

/**
 * @brief Konstruktor třídy
 *        nastaví výchozí nastavení, případně vytvoří složku save a aktivuje okno
 * @author Matyáš Hebert
 */
Zarovka::Zarovka(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Zarovka)
{
    ui->setupUi(this);                 // inicializace UI prvků vytvořených v Qt Designeru
    this->mode = 0;                    // nastavení výchozího herního režimu
    selectedBgColor = QColor(0, 0, 0); // výchozí barva pozadí
    selectedBoardColor = QColor(171, 205, 239); // výchozí barva herní plochy
    boardAlignment = Qt::AlignLeft;    // výchozí zarovnání herní desky

    previousPage = ui->stackedWidget->currentIndex(); // uložení aktuální stránky
    ui->stackedWidget->setCurrentIndex(3);            // přepnutí na úvodní / výchozí stránku

    loadSettings();                    // načtení uložených nastavení ze souboru
    applySettings();                   // aplikace načtených nastavení na UI

    QDir saveDir("save");              // práce se složkou pro ukládání dat
    if (!saveDir.exists()) {
        qDebug() << "Složka save/ neexistuje, vytvářím...";
        bool created = QDir().mkdir("save"); // vytvoření složky save
    }

    connect(ui->backFromGameButton, &QPushButton::clicked,
            this, &Zarovka::onBackFromGame); // návrat z hry zpět do menu

    gameTimer = new QTimer(this);       // vytvoření časovače pro herní logiku
    connect(gameTimer, &QTimer::timeout,
            this, &Zarovka::onTimerTick); // obsluha časového kroku hry

    this->show();                       // zobrazení hlavního okna
    this->showNormal();                 // zajištění normální velikosti okna
    raise();                            // přesunutí okna do popředí
    activateWindow();                  // aktivace okna pro uživatelský vstup
}

/**
 * @brief Vytvoří tlačítka herního pole
 * @author Matyáš Hebert
 */
void Zarovka::createButtons()
{
    int rows = activegame.board.rows;   // získání počtu řádků herního pole
    int cols = activegame.board.cols;   // získání počtu sloupců herního pole

    buttons.clear();                    // vyčištění seznamu tlačítek
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QPushButton *btn = new QPushButton(QString("")); // vytvoření nového tlačítka bez textu

            btn->setStyleSheet("QPushButton {"
                               "    border: none;"
                               "    border-radius: 0;"
                               "    background-color: #abcdef;" // výchozí barva tlačítka
                               "    outline: none;"
                               "}"
                               "QPushButton:pressed {"
                               "    padding-left: 1px;"          // vizuální efekt stisknutí
                               "    padding-top: 1px;"
                               "}");

            connect(btn, &QPushButton::clicked, this, [this, row, col]() {
                turn(qobject_cast<QPushButton *>(sender()), row, col); // obsluha kliknutí na konkrétní pole
            });

            ui->gameboard->addWidget(btn, row, col); // přidání tlačítka do mřížky herního pole
            buttons.insert(buttons.end(), btn);      // uložení tlačítka do seznamu
        }
    }
}

/**
 * @brief Destruktor třídy
 * @author Matyáš Hebert
 */
Zarovka::~Zarovka()
{
    delete ui;
}

/**
 * @brief Přesune uživatele na výběr obtížnosti
 * @author Klára Čoupková
 */
void Zarovka::on_playButton_clicked()
{
    this->mode = 1;
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(0);
}

/**
 * @brief Přesune uživatele do nastavení
 * @author Klára Čoupková
 */
void Zarovka::on_settingsButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(4);
}

/**
 * @brief Přesune uživatele do menu
 * @author Matyáš Hebert
 */
void Zarovka::on_backButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(3);
}

/**
 * @brief Vytvoří a spustí náhodnou hru
 * @author Matyáš Hebert
 */
void Zarovka::on_randomButton_clicked()
{
    createGame(5, 5); // vytvoř náhodnou hru
    //previousPage = 0;

    currentgamename = ":random";

    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(1);
    QWidget *page = ui->stackedWidget->widget(1);

    QResizeEvent event(this->size(), this->size());
    QCoreApplication::sendEvent(this, &event);
}

/**
 * @brief Vytvoří novou hru se zadanými rozměry
 * @author Matyáš Hebert
 */
void Zarovka::createGame(int w, int h, bool empty)
{
    activegame = game(w, h);
    if (!empty)
        activegame.gamecreate(0);
    resetLayout();
}

/**
 * @brief  Vymaže všechny child widgety z herního plánu a editor možností.
 * @author Matyáš Hebert
 */
void Zarovka::resetLayout()
{
    hideStatsDisplay();
    clearLayour(ui->gameboard);
    clearLayour(ui->editoroptions);
    createButtons();
}

/**
 * @brief Odstraní všechny child widgety z layoutu
 * @author Matyáš Hebert
 */
void Zarovka::clearLayour(QLayout *layout)
{
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (QWidget *widget = item->widget()) {
            delete widget;
        }
        delete item;
    }
}

/**
 * @brief Při hraní otočí tlačítko, aktualizuje herní pole
 *            a zkontroluje zda jsou žárovky rosvíceny (v tomto případě přesunu uživatele na výherní obrazovku)
 *        Při editaci se dle vybraného typu (zdroj, žárovka, drát) rozhodne zda otáčet či vložit vybraný typ na pole
 * @author Matyáš Hebert, Jan Ostatnický
 */
void Zarovka::turn(QPushButton *btn, int row, int col)
{
    if (activegame.editing) {
        savebtn->setText(language == language::czech ? "ULOŽIT" : "SAVE"); // změna textu tlačítka pro uložení
        savebtn->setStyleSheet("QPushButton {"
                               "    border: none;"
                               "    border-radius: 0;"
                               "    background-color: #abcdef;" // zvýraznění tlačítka po úpravě
                               "    outline: none;"
                               "}"
                               "QPushButton:pressed {"
                               "    padding-left: 1px;"          // vizuální efekt stisknutí
                               "    padding-top: 1px;"
                               "}");
    }

    if (selectedtype == 0) {            // není označené žádná možnost -> tlačítko se bude otáčet
        activegame.rotate(row, col);    // otočení vybraného prvku

        if (!activegame.editing) {
            activegame.moveCount++;     // zvýšení počtu tahů
            updateStatsDisplay();       // aktualizace statistiky hry
        }

        QPixmap rotated = activegame.getimage(row, col); // získání nového obrázku prvku
        btn->setIcon(QIcon(rotated));   // nastavení ikony tlačítka
        btn->setIconSize(btn->size());  // přizpůsobení velikosti ikony
        btn->setText("");               // odstranění textu tlačítka

        activegame.update();            // aktualizace hry
        updateboard(buttons[0]->width(), activegame.board.cols); // překreslení herní desky

        if (activegame.arebulbslit()) { // kontrola výherního stavu
            activegame.addrecord(currentgamename, sec, activegame.moveCount); // uložení rekordu

            int totalSeconds = sec / 1000;
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            int deciseconds = (sec % 1000) / 100;

            gameTimer->stop();           // zastavení herního časovače
            ui->label->setText(language == language::czech
                                   ? QString("Gratuluji, vyhrál jsi\n%1 krok%2\t%3:%4.%5s")
                                         .arg(activegame.moveCount)
                                         .arg(activegame.moveCount == 1 ? "" : activegame.moveCount > 4 ? "ů" : "y")
                                         .arg(minutes)
                                         .arg(seconds, 2, 10, QChar('0'))
                                         .arg(deciseconds)
                                   : QString("Congrats you won\n%1 move%2\t%3:%4.%5s")
                                         .arg(activegame.moveCount)
                                         .arg(activegame.moveCount > 1 ? "s" : "")
                                         .arg(minutes)
                                         .arg(seconds, 2, 10, QChar('0'))
                                         .arg(deciseconds)); // zobrazení výherní zprávy

            previousPage = ui->stackedWidget->currentIndex(); // uložení předchozí stránky
            ui->stackedWidget->setCurrentIndex(2);            // přepnutí na výherní obrazovku
        }
    } else {                             // editační režim – vkládání prvků
        if (activegame.getnodeat(row, col)->type == nodetype::power) {
            activegame.board.powerrow = -1; // odstranění původního zdroje
            activegame.board.powercol = -1;
            buttons[buttons.size() - 1]->show(); // opětovné povolení tlačítka zdroje
        }

        activegame.createnode(selectedtype, row, col, selectedsides); // vložení nového prvku

        if (selectedtype == nodetype::power) {
            activegame.board.powerrow = row; // nastavení nové pozice zdroje
            activegame.board.powercol = col;
            buttons[buttons.size() - 1]->hide(); // zakázání dalšího vkládání zdroje
        }

        activegame.update();             // aktualizace herního stavu
        updateboard(buttons[0]->width(), activegame.board.cols); // překreslení desky

        if (!shiftheld || selectedtype == nodetype::power)
            selectedtype = nodetype::empty; // reset vybraného typu po vložení
    }
}


/**
 * @brief Reakce na zmáčknutí klávesy Shift (pokládání více polí stejného typu v editoru)
 * @author Jan Ostatnický
 */
void Zarovka::keyPressEvent(QKeyEvent *event)
{
    shiftheld = (event->modifiers() & Qt::ShiftModifier);

    QWidget::keyPressEvent(event);
}

/**
 * @brief Reakce na puštění klávesy Shift (pokládání více polí stejného typu v editoru)
 * @author Jan Ostatnický
 */
void Zarovka::keyReleaseEvent(QKeyEvent *event)
{
    if (!(event->modifiers() & Qt::ShiftModifier)) {
        shiftheld = false;
    }

    QWidget::keyReleaseEvent(event);
}

/**
 * @brief Aktualizuje herní pole a jejich rozměry a ikony
 * @author Matyáš Hebert
 */
void Zarovka::updateboard(int sidesize, int cols)
{
    for (int i = 0; i < buttons.size(); i++) {
        QPushButton *button = buttons[i];
        button->setFixedHeight(sidesize);
        button->setFixedWidth(sidesize);
        int row = i / cols;
        if (ui->stackedWidget->currentIndex() == 1 && row < activegame.board.rows)
            button->setIcon(QIcon(activegame.getimage(i / cols, i % cols)));
        button->setIconSize(button->size());
    }
}

/**
 * @brief Funkce zajišťující správnou velikost jednotlivých polí
 *        při zvětšení okna
 * @author Matyáš Hebert
 */
void Zarovka::resizeEvent(QResizeEvent *event)
{
    ui->gameboard->setSpacing(0);                 // odstranění mezer mezi jednotlivými poli
    ui->gameboard->setContentsMargins(0, 0, 0, 0); // odstranění okrajů layoutu
    ui->gameboard->setAlignment(boardAlignment);  // nastavení zarovnání herní desky

    const int rows = activegame.board.rows;       // počet řádků herního pole
    const int cols = activegame.board.cols;       // počet sloupců herního pole

    QWidget::resizeEvent(event);                  // zavolání rodičovské implementace resizeEvent
    QSize newsize = event->size();                // získání nové velikosti okna

    int sidesize = (qMin(newsize.height(), newsize.width()) / (rows + 1)) * 0.8;
    // výpočet velikosti jednoho pole podle velikosti okna

    for (auto &btn : buttons) {
        btn->setFixedHeight(sidesize);            // nastavení výšky tlačítka
        btn->setFixedWidth(sidesize);             // nastavení šířky tlačítka
        btn->setIconSize(btn->size());            // přizpůsobení velikosti ikony tlačítku
    }

    if (!activegame.editing && ui->stackedWidget->currentIndex() == 1) {
        createStatsDisplay();                     // vytvoření panelu statistik
        updateStatsDisplay();                     // aktualizace statistik hry
    } else if (activegame.editing) {
        hideStatsDisplay();                       // skrytí statistik v editačním režimu
    }

    if (activegame.editing && ui->editoroptions->isEmpty()) {
        qDebug("ag.empty");                       // ladicí výpis – prázdné editační menu

        std::vector<std::string> icons
            = {":linkI.png", ":linkL.png", ":linkT.png", ":linkX.png", ":bulb.png", ":power.png"};
        // seznam ikon jednotlivých prvků
        std::vector<nodetype> nodetypes = {nodetype::link,
                                           nodetype::link,
                                           nodetype::link,
                                           nodetype::link,
                                           nodetype::bulb,
                                           nodetype::power};
        // typy uzlů odpovídající ikonám
        std::vector<std::array<bool, 4>> sides = {{true, false, true, false},
                                                  {true, true, false, false},
                                                  {true, true, true, false},
                                                  {true, true, true, true},
                                                  {true, false, false, false},
                                                  {true, false, false, false}};
        // definice směrů propojení jednotlivých prvků

        savebtn = new QPushButton(QString(language == language::czech ? "ULOŽENO" : "SAVED"));
        // vytvoření tlačítka pro uložení mapy
        savebtn->setFixedHeight(sidesize);
        savebtn->setFixedWidth(sidesize);
        savebtn->setStyleSheet("QPushButton {"
                               "    border: none;"
                               "    border-radius: 0;"
                               "    background-color: #00ff00;" // barva indikující uložený stav
                               "    outline: none;"
                               "}"
                               "QPushButton:pressed {"
                               "    padding-left: 1px;"
                               "    padding-top: 1px;"
                               "}");
        std::string name = "test";
        connect(savebtn, &QPushButton::clicked, this, [this, name]() {
            savebtn->setText(language == language::czech ? "ULOŽENO" : "SAVED"); // změna textu po uložení
            savebtn->setStyleSheet("QPushButton {"
                                   "    border: none;"
                                   "    border-radius: 0;"
                                   "    background-color: #00ff00;"
                                   "    outline: none;"
                                   "}"
                                   "QPushButton:pressed {"
                                   "    padding-left: 1px;"
                                   "    padding-top: 1px;"
                                   "}");
            activegame.savegame(currentgamename);  // uložení aktuální hry
        });
        ui->editoroptions->addWidget(savebtn);    // přidání tlačítka do editačního menu
        buttons.insert(buttons.end(), savebtn);   // uložení tlačítka do seznamu

        QPushButton *btn2 = new QPushButton(QString("MIX")); // tlačítko pro náhodné otočení polí
        btn2->setFixedHeight(sidesize);
        btn2->setFixedWidth(sidesize);
        btn2->setStyleSheet("QPushButton {"
                            "    border: none;"
                            "    border-radius: 0;"
                            "    background-color: #abcdef;"
                            "    outline: none;"
                            "}"
                            "QPushButton:pressed {"
                            "    padding-left: 1px;"
                            "    padding-top: 1px;"
                            "}");
        connect(btn2, &QPushButton::clicked, this, [this, sidesize]() {
            activegame.randomlyrotate();           // náhodné otočení všech prvků
            activegame.update();                   // aktualizace hry
            savebtn->setText(language == language::czech ? "ULOŽIT" : "SAVE"); // označení neuloženého stavu
            savebtn->setStyleSheet("QPushButton {"
                                   "    border: none;"
                                   "    border-radius: 0;"
                                   "    background-color: #abcdef;"
                                   "    outline: none;"
                                   "}"
                                   "QPushButton:pressed {"
                                   "    padding-left: 1px;"
                                   "    padding-top: 1px;"
                                   "}");
            updateboard(buttons[0]->width(), activegame.board.cols); // aktualizace zobrazení desky
        });
        ui->editoroptions->addWidget(btn2);       // přidání tlačítka do menu
        buttons.insert(buttons.end(), btn2);      // uložení tlačítka do seznamu

        for (int i = 0; i < icons.size(); i++) {
            QPushButton *btn = new QPushButton(QString("")); // vytvoření tlačítka pro výběr prvku
            btn->setFixedHeight(sidesize);
            btn->setFixedWidth(sidesize);
            btn->setStyleSheet("QPushButton {"
                               "    border: none;"
                               "    border-radius: 0;"
                               "    background-color: #abcdef;"
                               "    outline: none;"
                               "}"
                               "QPushButton:pressed {"
                               "    padding-left: 1px;"
                               "    padding-top: 1px;"
                               "}");
            btn->setIcon(QIcon(QString::fromStdString(icons[i]))); // nastavení ikony prvku
            nodetype nt = nodetypes[i];
            std::array<bool, 4> s = sides[i];
            connect(btn, &QPushButton::clicked, this, [this, nt, s]() {
                selectedtype = nt;                // nastavení vybraného typu prvku
                selectedsides = s;                // nastavení jeho propojení
            });
            ui->editoroptions->addWidget(btn);    // přidání tlačítka do editačního menu
            buttons.insert(buttons.end(), btn);   // uložení tlačítka do seznamu
        }

        if (activegame.board.powerrow >= 0)
            buttons[buttons.size() - 1]->hide();  // skrytí tlačítka zdroje, pokud už existuje
    }

    updateboard(sidesize, cols);                  // aktualizace herního pole
}


/**
 * @brief Načte uživatelská nastavení ze souboru settings.json
 * @author Klára Čoupková
 */
void Zarovka::loadSettings()
{
    QFile file("settings.json");                         // otevření souboru s nastavením
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();                // načtení celého obsahu souboru
        file.close();                                    // uzavření souboru

        QJsonDocument doc = QJsonDocument::fromJson(data); // vytvoření JSON dokumentu
        QJsonObject obj = doc.object();                  // získání kořenového objektu

        if (obj.contains("backgroundColor")) {
            QString colorStr = obj["backgroundColor"].toString(); // načtení barvy pozadí
            selectedBgColor = QColor(colorStr);
        } else {
            selectedBgColor = QColor(0, 0, 0);           // výchozí barva pozadí
        }

        if (obj.contains("boardColor")) {
            QString colorStr = obj["boardColor"].toString(); // načtení barvy herní desky
            selectedBoardColor = QColor(colorStr);
        } else {
            selectedBoardColor = QColor(171, 205, 239);  // výchozí barva herní desky
        }

        if (obj.contains("boardAlignment")) {
            QString alignment = obj["boardAlignment"].toString(); // načtení zarovnání desky
            boardAlignment = (alignment == "right") ? Qt::AlignRight : Qt::AlignLeft;
        } else {
            boardAlignment = Qt::AlignLeft;              // výchozí zarovnání desky
        }

        if (obj.contains("language")) {
            int lang = obj["language"].toInt();          // načtení jazykového nastavení
            language = lang == 0 ? language::czech : language::english;
        } else {
            language = language::czech;                  // výchozí jazyk aplikace
        }
    } else {
        selectedBgColor = QColor(0, 0, 0);               // výchozí barva pozadí při chybě souboru
        selectedBoardColor = QColor(171, 205, 239);      // výchozí barva herní desky
        boardAlignment = Qt::AlignLeft;                  // výchozí zarovnání desky
    }

    updateColorButtons();                                // aktualizace tlačítek
}

/**
 * @brief Uloží uživatelská nastavení do souboru settings.json
 * @author Klára Čoupková
 */
void Zarovka::saveSettings()
{
    QJsonObject obj;
    obj["backgroundColor"] = selectedBgColor.name();
    obj["boardColor"] = selectedBoardColor.name();
    obj["boardAlignment"] = (boardAlignment == Qt::AlignRight) ? "right" : "left";
    obj["language"] = language;
    //obj["language"] =

    QJsonDocument doc(obj);

    QFile file("settings.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson());
        file.close();
    }
}

/**
 * @brief Aplikuje načtená nastavení na UI aplikace
 * @author Klára Čoupková
 */
void Zarovka::applySettings()
{
    QString style = QString("QMainWindow { background-color: %1; }")
    .arg(selectedBgColor.name()); // vytvoření stylu pro barvu pozadí okna
    this->setStyleSheet(style);                        // aplikace stylu na hlavní okno

    if (ui->gameboard) {
        ui->gameboard->setAlignment(boardAlignment);   // nastavení zarovnání herní desky
    }

    ui->colorWhiteButton_5->setText(language == language::czech ? "Vlevo" : "Left");  // text tlačítka zarovnání vlevo
    ui->colorBlackButton_5->setText(language == language::czech ? "Vpravo" : "Right"); // text tlačítka zarovnání vpravo

    ui->playAgainButton->setText(language == language::czech ? "Hrát Znovu" : "Play Again"); // tlačítko opakování hry

    ui->settingsLabel->setText(language == language::czech ? "Nastavení" : "Settings");      // nadpis nastavení
    ui->languageLabel->setText(language == language::czech ? "Jazyk" : "Language");          // popisek jazyka
    ui->gameboardAlignLabel->setText(language == language::czech ? "Herní pole" : "Game board"); // popisek zarovnání pole
    ui->bgColorLabel->setText(language == language::czech ? "Barva pozadí" : "Background color"); // popisek barvy pozadí
    ui->gameboardColorLabel->setText(language == language::czech
                                         ? "Barva hrací plochy"
                                         : "Gameboard background color");                          // popisek barvy herní plochy

    ui->playButton->setText(language == language::czech ? "Hrát" : "Play");                  // tlačítko spuštění hry
    ui->settingsButton->setText(language == language::czech ? "Nastavení" : "Settings");     // tlačítko nastavení
    ui->hardButton->setText(language == language::czech ? "Těžké" : "Hard");                 // volba obtížnosti – těžká
    ui->mediumButton->setText(language == language::czech ? "Střední" : "Medium");           // volba obtížnosti – střední
    ui->easyButton->setText(language == language::czech ? "Lehké" : "Easy");                 // volba obtížnosti – lehká
    ui->randomButton->setText(language == language::czech ? "Náhodné" : "Random");           // volba náhodné hry

    updateColorButtons();                               // aktualizace vzhledu tlačítek
}

/**
 * @brief Zvýrazní vybraná nastavení
 * @author Klára Čoupková
 */
void Zarovka::updateColorButtons()
{
    ui->colorWhiteButton->setStyleSheet("background-color: #969696; "
                                        "min-width: 20px; min-height: 20px; "
                                        "border-radius: 5px;");

    ui->colorBlackButton->setStyleSheet("background-color: black; "
                                        "min-width: 20px; min-height: 20px; "
                                        "border-radius: 5px;");

    ui->colorBlueButton->setStyleSheet("background-color: #003366; "
                                       "min-width: 20px; min-height: 20px; "
                                       "border-radius: 5px;");

    if (selectedBgColor == QColor(150, 150, 150)) {
        ui->colorWhiteButton->setStyleSheet("background-color: #969696; "
                                            "min-width: 20px; min-height: 20px; "
                                            "border: 4px solid red;"
                                            "border-radius: 5px;");
    } else if (selectedBgColor == QColor(0, 0, 0)) {
        ui->colorBlackButton->setStyleSheet("background-color: black; "
                                            "min-width: 20px; min-height: 20px; "
                                            "border: 4px solid red;"
                                            "border-radius: 5px;");
    } else if (selectedBgColor == QColor(0, 51, 102)) {
        ui->colorBlueButton->setStyleSheet("background-color: #003366; "
                                           "min-width: 20px; min-height: 20px; "
                                           "border: 4px solid red;"
                                           "border-radius: 5px;");
    }

    ui->colorWhiteButton_3->setStyleSheet("background-color: white; "
                                          "min-width: 20px; min-height: 20px; "
                                          "border-radius: 5px;");

    ui->colorBlueButton_3->setStyleSheet("background-color: #87CEEB; "
                                         "min-width: 20px; min-height: 20px; "
                                         "border-radius: 5px;");

    ui->colorBlackButton_3->setStyleSheet("background-color: #F2FDD0; "
                                          "min-width: 20px; min-height: 20px; "
                                          "border-radius: 5px;");

    if (selectedBoardColor == QColor(255, 255, 255)) {
        ui->colorWhiteButton_3->setStyleSheet("background-color: white; "
                                              "border: 4px solid red; "
                                              "border-radius: 5px;");
    } else if (selectedBoardColor == QColor(135, 206, 235)) {
        ui->colorBlueButton_3->setStyleSheet("background-color: #87CEEB; "
                                             "border: 4px solid red; "
                                             "border-radius: 5px;");
    } else if (selectedBoardColor == QColor(255, 253, 208)) {
        ui->colorBlackButton_3->setStyleSheet("background-color: #F2FDD0; "
                                              "border: 4px solid red; "
                                              "border-radius: 5px;");
    }

    ui->colorWhiteButton_5->setStyleSheet(
        boardAlignment == Qt::AlignLeft
            ? "background-color: white; color: black; border: 4px solid red; border-radius: 5px;"
            : "background-color: white; color: black; border-radius: 5px;");

    ui->colorBlackButton_5->setStyleSheet(
        boardAlignment == Qt::AlignRight
            ? "background-color: white; color: black; border: 4px solid red; border-radius: 5px;"
            : "background-color: white; color: black; border-radius: 5px;");

    ui->czechButton->setStyleSheet(
        language == language::czech
            ? "background-color: white; color: black; border: 4px solid red; border-radius: 5px;"
            : "background-color: white; color: black; border-radius: 5px;");

    ui->englishButton->setStyleSheet(
        language == language::english
            ? "background-color: white; color: black; border: 4px solid red; border-radius: 5px;"
            : "background-color: white; color: black; border-radius: 5px;");
}


/**
 * @brief Nastavuje pozici hrací plochy doleva
 * @author Klára Čoupková
 */
void Zarovka::on_colorWhiteButton_5_clicked() // left
{
    boardAlignment = Qt::AlignLeft;
    applySettings();
    saveSettings();
}

/**
 * @brief Nastavuje pozici hrací plochy doprava
 * @author Klára Čoupková
 */
void Zarovka::on_colorBlackButton_5_clicked() // right
{
    boardAlignment = Qt::AlignRight;
    applySettings();
    saveSettings();
}

/**
 * @brief Nastavuje barvu pozadí hrací plochy na bílou
 * @author Klára Čoupková
 */
void Zarovka::on_colorWhiteButton_3_clicked()
{
    selectedBoardColor = QColor(255, 255, 255);
    applySettings();
    saveSettings();
}

/**
 * @brief Nastavuje barvu pozadí hrací plochy na modrou
 * @author Klára Čoupková
 */
void Zarovka::on_colorBlueButton_3_clicked()
{
    selectedBoardColor = QColor(135, 206, 235);
    applySettings();
    saveSettings();
}

/**
 * @brief Nastavuje barvu pozadí hrací plochy na žlutou
 * @author Klára Čoupková
 */
void Zarovka::on_colorBlackButton_3_clicked()
{
    selectedBoardColor = QColor(255, 253, 208);
    applySettings();
    saveSettings();
}

/**
 * @brief Nastavuje barvu pozadí na šedou
 * @author Klára Čoupková
 */
void Zarovka::on_colorWhiteButton_clicked()
{
    selectedBgColor = QColor(150, 150, 150);
    applySettings();
    saveSettings();
}

/**
 * @brief Nastavuje barvu pozadí na černou
 * @author Klára Čoupková
 */
void Zarovka::on_colorBlackButton_clicked()
{
    selectedBgColor = QColor(0, 0, 0);
    applySettings();
    saveSettings();
}

/**
 * @brief Nastavuje barvu pozadí na modrou
 * @author Klára Čoupková
 */
void Zarovka::on_colorBlueButton_clicked()
{
    selectedBgColor = QColor(0, 51, 102);
    applySettings();
    saveSettings();
}

/**
 * @brief Přesune uživatele do menu
 * @author Matyáš Hebert
 */
void Zarovka::on_pushButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(3);
}

/**
 * @brief Přesune uživatele z úvodní obrayovky do editoru
 * @author Klára Čoupková
 */
void Zarovka::on_pushButton_3_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(6);
}

/**
 * @brief Inkrement šířky nového levelu
 * @author Jan Ostatnický
 */
void Zarovka::on_widthplus_clicked()
{
    auto current = ui->widthlabel->text().toInt();
    current++;
    if (current > 20)
        current = 20;
    ui->widthlabel->setText(QString::number(current));
}

/**
 * @brief Dekrement šířky nového levelu
 * @author Jan Ostatnický
 */
void Zarovka::on_widthminus_clicked()
{
    auto current = ui->widthlabel->text().toInt();
    current--;
    if (current < 2)
        current = 2;
    if (ui->heightlabel->text().toInt() > current)
        ui->heightlabel->setText(QString::number(current));
    ui->widthlabel->setText(QString::number(current));
}

/**
 * @brief Inkrement výšky nového levelu
 * @author Jan Ostatnický
 */
void Zarovka::on_heightplus_clicked()
{
    auto current = ui->heightlabel->text().toInt();
    current++;
    if (current > 20)
        current = 20;
    if (ui->widthlabel->text().toInt() < current)
        ui->widthlabel->setText(QString::number(current));
    ui->heightlabel->setText(QString::number(current));
}

/**
 * @brief Dekrement výšky nového levelu
 * @author Jan Ostatnický
 */
void Zarovka::on_heightminus_clicked()
{
    auto current = ui->heightlabel->text().toInt();
    current--;
    if (current < 2)
        current = 2;
    ui->heightlabel->setText(QString::number(current));
}

/**
 * @brief Přesun ze zadání parametrů pro vytvoření nového levelu na obrazovku editoru a uložení zadaných parametrů nového levelu
 * @author Jan Ostatnický
 */
void Zarovka::on_pushButton_4_clicked()
{
    createGame(ui->widthlabel->text().toInt(), ui->heightlabel->text().toInt(), true);
    currentgamename = ui->levelname->toPlainText();
    activegame.editing = true;
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(1);
    QWidget *page = ui->stackedWidget->widget(1);

    QResizeEvent event(this->size(), this->size());
    QCoreApplication::sendEvent(this, &event);
    //previousPage = 7;
}

/**
 * @brief Přesune uživatele zpět do menu
 * @author Klára Čoupková
 */
void Zarovka::on_backToMenuButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(3);
}

/**
 * @brief Přesune uživatele do seznamu levelů
 * @author Klára Čoupková
 */
void Zarovka::on_myLevelsButton_clicked()
{
    loadLevelList();
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(7);
}

/**
 * @brief Přesune uživatele do editoru
 * @author Klára Čoupková
 */
void Zarovka::on_createNewButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(5);
}

/**
 * @brief Vrátí uživatele zpět do hlavního menu editoru
 * @author Klára Čoupková
 */
void Zarovka::on_backToEditorMenuButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();ui->stackedWidget->setCurrentIndex(6);
}

/**
 * @brief Vrátí seznam všеch uložených levelů ze složky save
 * @author Klára Čoupková
 */
QStringList Zarovka::getLevelFiles()
{
    QDir dir("save");
    QStringList filters;
    filters << "*.zvaz";
    QStringList files = dir.entryList(filters, QDir::Files, QDir::Name);
    return files;
}

/**
 * @brief Načte hru uloženou v souboru a spustí jí
 * @author Jan Ostatnický
 */
void Zarovka::openGameFile(QString filename, bool editing)
{
    activegame.loadgame(filename);              // načtení herních dat ze souboru
    resetLayout();                              // vyčištění a znovuvytvoření rozložení UI

    currentgamename = filename.chopped(5);      // odstranění přípony souboru
    if (currentgamename.startsWith("save"))
        currentgamename = currentgamename.mid(5); // odstranění prefixu "save" z názvu

    activegame.editing = editing;               // nastavení herního nebo editačního režimu

    previousPage = ui->stackedWidget->currentIndex(); // uložení aktuální stránky
    ui->stackedWidget->setCurrentIndex(1);            // přepnutí na herní obrazovku

    QWidget *page = ui->stackedWidget->widget(1);     // získání widgetu herní stránky
    QResizeEvent event(this->size(), this->size());
    QCoreApplication::sendEvent(this, &event);        // vynucení přepočtu velikostí prvků

    activegame.update();                        // aktualizace herní logiky
    updateboard(buttons[0]->width(), activegame.board.cols); // vykreslení herní desky

    if (!editing) {
        activegame.resetMoveCount();            // vynulování počtu tahů
        createStatsDisplay();                   // vytvoření panelu statistik
        updateStatsDisplay();                   // aktualizace zobrazených statistik
    }
}

/**
 * @brief Načte a zobrazí seznam uložených uživatelských levelů
 * @author Klára Čoupková
 */
void Zarovka::loadLevelList()
{
    QLayoutItem *item;
    while ((item = ui->levelListLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();              // odstranění widgetu z layoutu
        }
        delete item;                            // uvolnění položky layoutu
    }

    QStringList levelFiles = getLevelFiles();   // získání seznamu uložených levelů

    ui->createLevelButton->setText(language == language::czech ? "Vytvořit nový" : "Make new"); // text tlačítka vytvoření
    ui->levellistlabel->setText(language == language::czech ? "Moje levely" : "Your levels");   // nadpis seznamu levelů

    if (levelFiles.isEmpty()) {
        QLabel *noLevelsLabel = new QLabel(language == language::czech
                                               ? "Zatím nemáte žádné uložené levely"
                                               : "You have no saved levels yet"); // zpráva při prázdném seznamu
        noLevelsLabel->setAlignment(Qt::AlignCenter);
        noLevelsLabel->setStyleSheet("font-size: 18px; color: gray;");
        ui->levelListLayout->addWidget(noLevelsLabel); // přidání informace do layoutu
    } else {
        for (const QString &filename : levelFiles) {
            QWidget *rowWidget = new QWidget();         // widget jednoho řádku seznamu
            QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget); // horizontální rozložení řádku

            QString levelName = filename;
            levelName.chop(5);                          // odstranění přípony souboru

            QTextEdit *nameLabel = new QTextEdit(levelName); // editovatelný název levelu
            nameLabel->setFixedHeight(40);
            nameLabel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            nameLabel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            nameLabel->setStyleSheet("font-size: 16px; padding: 10px;");
            nameLabel->setMinimumWidth(150);

            QLabel *record = new QLabel();               // popisek rekordu levelu
            record->setMinimumSize(150, 40);
            if (activegame.iscompleted(levelName)) {
                int sec = activegame.getrecordtime(levelName); // načtení času rekordu
                int totalSeconds = sec / 1000;
                int minutes = totalSeconds / 60;
                int seconds = totalSeconds % 60;
                int deciseconds = (sec % 1000) / 100;
                int steps = activegame.getrecordsteps(levelName); // načtení počtu kroků

                record->setText(QString("%1: %2\t%3: %4:%5.%6")
                                    .arg(language == language::czech ? "Kroky" : "Steps")
                                    .arg(steps)
                                    .arg(language == language::czech ? "Čas" : "Time")
                                    .arg(minutes)
                                    .arg(seconds, 2, 10, QChar('0'))
                                    .arg(deciseconds)); // zobrazení rekordu
            } else {
                record->setText(language == language::czech ? "NEDOKONČENO" : "NOT COMPLETED"); // level bez rekordu
            }

            QPushButton *playBtn = new QPushButton(language == language::czech ? "Hrát" : "Play"); // tlačítko spuštění
            playBtn->setMinimumSize(100, 40);
            playBtn->setStyleSheet("font-size: 14px; background-color: #007dff; color: white;");
            connect(playBtn, &QPushButton::clicked, this, [this, filename]() {
                openGameFile(QString("save/%1").arg(filename), false); // spuštění levelu v herním režimu
            });

            QPushButton *editBtn = new QPushButton(language == language::czech ? "Editovat" : "Edit"); // tlačítko editace
            editBtn->setMinimumSize(100, 40);
            editBtn->setStyleSheet("font-size: 14px;");
            connect(editBtn, &QPushButton::clicked, this, [this, filename]() {
                openGameFile(QString("save/%1").arg(filename), true); // otevření levelu v editoru
            });

            QPushButton *renameBtn = new QPushButton(language == language::czech ? "Přejmenovat" : "Rename"); // přejmenování
            renameBtn->setMinimumSize(120, 40);
            renameBtn->setStyleSheet("font-size: 14px;");
            connect(renameBtn, &QPushButton::clicked, this, [this, filename, nameLabel]() {
                int steps = -1;
                int time = -1;
                activegame.loadgame(QString("save/%1").arg(filename)); // načtení původního levelu
                if (activegame.iscompleted(filename.chopped(5))) {
                    steps = activegame.getrecordsteps(filename.chopped(5)); // záloha rekordu
                    time = activegame.getrecordtime(filename.chopped(5));
                }
                activegame.deletegame(filename);          // smazání původního souboru
                activegame.savegame(nameLabel->toPlainText()); // uložení pod novým názvem
                if (steps != -1) {
                    activegame.addrecord(nameLabel->toPlainText(), time, steps); // obnova rekordu
                }
                loadLevelList();                          // obnovení seznamu levelů
            });

            QPushButton *deleteBtn = new QPushButton(language == language::czech ? "Odstranit" : "Delete"); // mazání levelu
            deleteBtn->setMinimumSize(100, 40);
            deleteBtn->setStyleSheet("font-size: 12px; background-color: #ff4444; color: white;");
            deleteBtn->setCheckable(true);
            deleteBtn->setChecked(false);

            connect(deleteBtn, &QPushButton::clicked, this,
                    [this, filename, deleteBtn](bool checked) {
                        if (checked) {
                            deleteBtn->setText(language == language::czech
                                                   ? "Opravdu smazat?"
                                                   : "Really delete?"); // potvrzení mazání
                        } else {
                            activegame.deletegame(filename);      // odstranění levelu
                            loadLevelList();                      // aktualizace seznamu
                        }
                    });

            rowLayout->addWidget(nameLabel);              // název levelu
            rowLayout->addWidget(record);                 // rekord levelu
            rowLayout->addStretch();                      // odsazení ovládacích prvků
            rowLayout->addWidget(playBtn);                // tlačítko hrát
            rowLayout->addWidget(editBtn);                // tlačítko editovat
            rowLayout->addWidget(renameBtn);              // tlačítko přejmenovat
            rowLayout->addWidget(deleteBtn);              // tlačítko odstranit

            ui->levelListLayout->addWidget(rowWidget);    // přidání řádku do seznamu

            QFrame *line = new QFrame();                  // oddělovací čára
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            ui->levelListLayout->addWidget(line);
        }
    }
}

/**
 * @brief Přesun z listu uživatelských levelů na obrazovku zadání parametrů nového levelu k vytvoření
 * @author Jan Ostatnický
 */
void Zarovka::on_createLevelButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(5);
}


/**
 * @brief Nastavuje jazyk na češtinu
 * @author Klára Čoupková
 */
void Zarovka::on_czechButton_clicked()
{
    language = language::czech;
    applySettings();
    saveSettings();
}

/**
 * @brief Nastavuje jazyk na angličtinu
 * @author Klára Čoupková
 */
void Zarovka::on_englishButton_clicked()
{
    language = language::english;
    applySettings();
    saveSettings();
}

/**
 * @brief Spuštění prvního levelu lehké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonEasyLevel1_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_E01.zvaz");
    //previousPage = 10;
}

/**
 * @brief Spuštění druhého levelu lehké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonEasyLevel2_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_E02.zvaz");
    //previousPage = 10;
}

/**
 * @brief Spuštění třetího levelu lehké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonEasyLevel3_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_E03.zvaz");
    //previousPage = 10;
}

/**
 * @brief Spuštění čtvrtého levelu lehké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonEasyLevel4_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_E04.zvaz");
    //previousPage = 10;
}

/**
 * @brief Spuštění pátého levelu lehké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonEasyLevel5_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_E05.zvaz");
    //previousPage = 10;
}

/**
 * @brief Spuštění prvního levelu střední obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonMediumLevel1_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_M01.zvaz");
    //previousPage = 8;
}

/**
 * @brief Spuštění druhého levelu střední obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonMediumLevel2_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_M02.zvaz");
    //previousPage = 8;
}

/**
 * @brief Spuštění třetího levelu střední obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonMediumLevel3_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_M03.zvaz");
    //previousPage = 8;
}

/**
 * @brief Spuštění čtvrtého levelu střední obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonMediumLevel4_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_M04.zvaz");
    //previousPage = 8;
}

/**
 * @brief Spuštění pátého levelu střední obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonMediumLevel5_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_M05.zvaz");
    //previousPage = 8;
}

/**
 * @brief Spuštění prvního levelu těžké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonHardLevel1_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_H01.zvaz");
    //previousPage = 9;
}

/**
 * @brief Spuštění druhého levelu těžké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonHardLevel2_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_H02.zvaz");
    //previousPage = 9;
}

/**
 * @brief Spuštění třetího levelu těžké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonHardLevel3_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_H03.zvaz");
    //previousPage = 9;
}

/**
 * @brief Spuštění čtvrtého levelu těžké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonHardLevel4_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_H04.zvaz");
    //previousPage = 9;
}

/**
 * @brief Spuštění pátého levelu těžké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_buttonHardLevel5_clicked()
{
    openGameFile(":/resources/resources/mainlevels/_H05.zvaz");
    //previousPage = 9;
}

/**
 * @brief Aktualizace zobrazení tlačítek pro spuštění levelů (primárně z důvodu zobrazení statistik)
 * @author Jan Ostatnický
 */
void Zarovka::updateLevelButton(QPushButton* button, QString name, int level){
    if (activegame.iscompleted(name)){
        int sec = activegame.getrecordtime(name);
        int totalSeconds = sec / 1000;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        int deciseconds = (sec % 1000) / 100;
        int steps = activegame.getrecordsteps(name);

        button->setText(QString("Level %1\n%2: %3\t%4: %5:%6.%7")
                            .arg(level).arg(language == language::czech ? "Kroky" : "Steps")
                            .arg(steps).arg(language == language::czech ? "Čas" : "Time")
                            .arg(minutes).arg(seconds, 2, 10, QChar('0'))
                            .arg(deciseconds));
    }
    else {
        button->setText(QString("Level %1").arg(level));
    }
}

/**
 * @brief Posun na stránku s levely lehké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_easyButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(10);

    updateLevelButton(ui->buttonEasyLevel1, ":/resources/resources/mainlevels/_E01", 1);
    updateLevelButton(ui->buttonEasyLevel2, ":/resources/resources/mainlevels/_E02", 2);
    updateLevelButton(ui->buttonEasyLevel3, ":/resources/resources/mainlevels/_E03", 3);
    updateLevelButton(ui->buttonEasyLevel4, ":/resources/resources/mainlevels/_E04", 4);
    updateLevelButton(ui->buttonEasyLevel5, ":/resources/resources/mainlevels/_E05", 5);

    //previousPage = 0;
}

/**
 * @brief Posun na stránku s levely střední obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_mediumButton_clicked()
{

    previousPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(8);

    updateLevelButton(ui->buttonMediumLevel1, ":/resources/resources/mainlevels/_M01", 1);
    updateLevelButton(ui->buttonMediumLevel2, ":/resources/resources/mainlevels/_M02", 2);
    updateLevelButton(ui->buttonMediumLevel3, ":/resources/resources/mainlevels/_M03", 3);
    updateLevelButton(ui->buttonMediumLevel4, ":/resources/resources/mainlevels/_M04", 4);
    updateLevelButton(ui->buttonMediumLevel5, ":/resources/resources/mainlevels/_M05", 5);

    //previousPage = 0;
}

/**
 * @brief Posun na stránku s levely těžké obtížnosti
 * @author Jan Ostatnický
 */
void Zarovka::on_hardButton_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(9);

    updateLevelButton(ui->buttonHardLevel1, ":/resources/resources/mainlevels/_H01", 1);
    updateLevelButton(ui->buttonHardLevel2, ":/resources/resources/mainlevels/_H02", 2);
    updateLevelButton(ui->buttonHardLevel3, ":/resources/resources/mainlevels/_H03", 3);
    updateLevelButton(ui->buttonHardLevel4, ":/resources/resources/mainlevels/_H04", 4);
    updateLevelButton(ui->buttonHardLevel5, ":/resources/resources/mainlevels/_H05", 5);

    //previousPage = 0;
}

/**
 * @brief Návrat z výběru levelů lehké obtížnosti na výběr obtížností
 * @author Jan Ostatnický
 */
void Zarovka::on_pushButton_11_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(0);
}

/**
 * @brief Návrat z výběru levelů střední obtížnosti na výběr obtížností
 * @author Jan Ostatnický
 */
void Zarovka::on_pushButton_12_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(0);
}

/**
 * @brief Návrat z výběru levelů těžké obtížnosti na výběr obtížností
 * @author Jan Ostatnický
 */
void Zarovka::on_pushButton_13_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(0);
}

/**
 * @brief  Zpět
 * @author Matyáš Hebert
 */
void Zarovka::onBackFromGame()
{
    if (activegame.editing){
        activegame.savegame(currentgamename);
    }
    selectedtype = nodetype::empty;
    ui->stackedWidget->setCurrentIndex(previousPage);
    loadLevelList();
}

/**
 * @brief  Vytvoří a zobrazí widget se statistikami hry (čas a počet tahů)
 * @author Klára Čoupková
 */
void Zarovka::createStatsDisplay()
{
    // mimo herní mód nebo při již existujících statistikách se widget znovu nevytváří
    if (statsWidget != nullptr || activegame.editing) {
        return;
    }

    statsWidget = new QWidget();                    // hlavní widget statistik
    QVBoxLayout *statsLayout = new QVBoxLayout(statsWidget);
    statsLayout->setSpacing(20);                    // mezery mezi jednotlivými částmi
    statsLayout->setContentsMargins(10, 10, 10, 10);// okraje widgetu
    statsLayout->setAlignment(Qt::AlignTop);        // zarovnání obsahu nahoru

    // část zobrazující počet tahů
    QWidget *movesWidget = new QWidget();
    QVBoxLayout *movesLayout = new QVBoxLayout(movesWidget);
    movesLayout->setSpacing(5);
    movesLayout->setContentsMargins(10, 10, 10, 10);

    QLabel *movesTitle = new QLabel(language == language::czech ? "Tahy:" : "Moves:");
    movesTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    movesLabel = new QLabel("0");                   // výchozí hodnota počtu tahů
    movesLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #007DFF;");

    movesLayout->addWidget(movesTitle);
    movesLayout->addWidget(movesLabel);

    // část zobrazující čas hry
    QWidget *timeWidget = new QWidget();
    QVBoxLayout *timeLayout = new QVBoxLayout(timeWidget);
    timeLayout->setSpacing(5);
    timeLayout->setContentsMargins(10, 10, 10, 10);

    QLabel *timeTitle = new QLabel(language == language::czech ? "Čas:" : "Time:");
    timeTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    timeLabel = new QLabel("0:00");                 // výchozí hodnota času
    timeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #007DFF;");

    timeLayout->addWidget(timeTitle);
    timeLayout->addWidget(timeLabel);

    statsLayout->addWidget(movesWidget);            // přidání části tahů
    statsLayout->addWidget(timeWidget);             // přidání části času
    statsLayout->addStretch();                      // odsazení obsahu nahoru
    statsWidget->setMinimumWidth(120);              // minimální šířka statistik
    statsWidget->setMaximumWidth(180);              // maximální šířka statistik

    int cols = activegame.board.cols;
    int rows = activegame.board.rows;
    ui->gameboard->addWidget(statsWidget, 0, cols, rows, 1); // umístění statistik vedle herního pole

    sec = 0;                                        // vynulování času hry
    gameTimer->start(100);                          // spuštění časovače (krok 100 ms)
}

/**
 * @brief Skryje a odstraní widget se statistikami hry
 * @author Klára Čoupková
 */
void Zarovka::hideStatsDisplay()
{
    gameTimer->stop();

    if (statsWidget != nullptr) {
        ui->gameboard->removeWidget(statsWidget);
        delete statsWidget;
        statsWidget = nullptr;
        timeLabel = nullptr;
        movesLabel = nullptr;
    }
}

/**
 * @brief Aktualizuje zobrazení počtu tahů ve statistikách
 * @author Klára Čoupková
 */
void Zarovka::updateStatsDisplay()
{
    if (!activegame.editing && movesLabel != nullptr) {
        movesLabel->setText(QString::number(activegame.moveCount));
    }
}

/**
 * @brief Funkce časovače aktualizuje zobrazení času každých 100ms
 * @author Klára Čoupková
 */
void Zarovka::onTimerTick()
{
    if (!activegame.editing) {
        sec += 100;

        int totalSeconds = sec / 1000;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        int deciseconds = (sec % 1000) / 100;

        if (timeLabel != nullptr) {
            timeLabel->setText(QString("%1:%2.%3")
                                   .arg(minutes)
                                   .arg(seconds, 2, 10, QChar('0'))
                                   .arg(deciseconds));
        }
    }
}

/**
 * @brief Vrácení z výběru obtížnosti na úvodní obrazovku
 * @author Jan Ostatnický
 */
void Zarovka::on_backFromDifficultySelect_clicked()
{
    previousPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(3);
}

/**
 * @brief Spustí znovu level
 * @author Matyáš Hebert
 */
void Zarovka::on_playAgainButton_clicked()
{
    openGameFile(QString("%1.zvaz").arg(currentgamename));
}

/**
 * @brief Vrácení na úvodní obrazovku ze zadávání parametrů pro nový level
 * @author Jan Ostatnický
 */
void Zarovka::on_menuButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

