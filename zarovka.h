#ifndef ZAROVKA_H
#define ZAROVKA_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class Zarovka;
}
QT_END_NAMESPACE

#include <QPushButton>
#include "BE/game.h"

class Zarovka : public QMainWindow
{
    Q_OBJECT

public:
    Zarovka(QWidget *parent = nullptr);
    ~Zarovka();
    int mode;
    int previousPage = 0;
    language language;
    nodetype selectedtype = nodetype::empty;
    std::array<bool, 4> selectedsides = {false, false, false, false};
    void turn(QPushButton *btn, int row, int col);
    game activegame = game(0, 0);
    QString currentgamename;
    bool shiftheld;
    std::vector<QPushButton *> buttons;
    QPushButton *savebtn;

    void updateboard(int sidesize);
    void updateboard(int sidesize, int cols);
    void createGame(int w, int h, bool empty = false);
    void createButtons();
    void clearLayour(QLayout *layout);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resetLayout();
    void openGameFile(QString filename, bool editing = false);
    void updateStatsDisplay();
    void updateLevelButton(QPushButton *button, QString name, int level);
private slots:
    void on_playButton_clicked();
    void on_settingsButton_clicked();
    void on_backButton_clicked();
    void updateUI(int mode);

    void on_randomButton_clicked();

    void on_colorWhiteButton_clicked();
    void on_colorBlackButton_clicked();
    void on_colorBlueButton_clicked();

    void on_colorWhiteButton_3_clicked();
    void on_colorBlueButton_3_clicked();
    void on_colorBlackButton_3_clicked();

    void on_colorWhiteButton_5_clicked(); // left
    void on_colorBlackButton_5_clicked(); // right

    void on_backToMenuButton_clicked();
    void on_myLevelsButton_clicked();
    void on_createNewButton_clicked();
    void on_backToEditorMenuButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_widthplus_clicked();

    void on_widthminus_clicked();

    void on_heightplus_clicked();

    void on_heightminus_clicked();

    void on_pushButton_4_clicked();

    void on_createLevelButton_clicked();

    void on_czechButton_clicked();

    void on_englishButton_clicked();

    void on_buttonEasyLevel1_clicked();
    void on_buttonEasyLevel2_clicked();
    void on_buttonEasyLevel3_clicked();
    void on_buttonEasyLevel4_clicked();
    void on_buttonEasyLevel5_clicked();

    void on_buttonMediumLevel1_clicked();
    void on_buttonMediumLevel2_clicked();
    void on_buttonMediumLevel3_clicked();
    void on_buttonMediumLevel4_clicked();
    void on_buttonMediumLevel5_clicked();

    void on_buttonHardLevel1_clicked();
    void on_buttonHardLevel2_clicked();
    void on_buttonHardLevel3_clicked();
    void on_buttonHardLevel4_clicked();
    void on_buttonHardLevel5_clicked();

    void on_easyButton_clicked();
    void on_mediumButton_clicked();
    void on_hardButton_clicked();

    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void onBackFromGame();

    void onTimerTick();

    void on_backFromDifficultySelect_clicked();

private:
    Ui::Zarovka *ui;
    QColor selectedBgColor; // current bg color
    QColor selectedBoardColor;
    Qt::Alignment boardAlignment;
    QLabel *timeLabel = nullptr;
    QLabel *movesLabel = nullptr;
    QWidget *statsWidget = nullptr;
    QTimer *gameTimer = nullptr;
    int sec = 0;

    void loadSettings();
    void applySettings();
    void saveSettings();
    void updateColorButtons();

    void loadLevelList();
    QStringList getLevelFiles();
    void createStatsDisplay();
    void hideStatsDisplay();

protected:
    void resizeEvent(QResizeEvent *event) override;
};
#endif // ZAROVKA_H
