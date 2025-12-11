#ifndef ZAROVKA_H
#define ZAROVKA_H

#include <QString>
#include <QStringList>
#include <QMainWindow>

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
    nodetype selectedtype = nodetype::empty;
    std::array<bool, 4> selectedsides = {false, false, false, false};
    void turn(QPushButton *btn, int row, int col);
    game activegame = game(0,0);
    QString currentgamename;
    bool shiftheld;
    std::vector<QPushButton*> buttons;

    void updateboard(int sidesize);
    void updateboard(int sidesize, int cols);
    void createGame(int w, int h, bool empty = false);
    void createButtons();
    void clearLayour(QLayout *layout);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resetLayout();
private slots:
    void on_playButton_clicked();
    void on_settingsButton_clicked();
    void on_backButton_clicked();
    void updateUI(int mode);

    void on_easyButton_clicked();

    void on_colorWhiteButton_clicked();
    void on_colorBlackButton_clicked();
    void on_colorBlueButton_clicked();

    void on_colorWhiteButton_3_clicked();
    void on_colorBlueButton_3_clicked();
    void on_colorBlackButton_3_clicked();

    void on_colorWhiteButton_5_clicked();  // left
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

private:
    Ui::Zarovka *ui;
    QColor selectedBgColor;  // current bg color
    QColor selectedBoardColor;
    Qt::Alignment boardAlignment;

    void loadSettings();
    void applySettings();
    void saveSettings();
    void updateColorButtons();

    void loadLevelList();
    QStringList getLevelFiles();

protected:
    void resizeEvent(QResizeEvent *event) override;

};
#endif // ZAROVKA_H
