#ifndef ZAROVKA_H
#define ZAROVKA_H

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
    void turn(QPushButton *btn, int row, int col);
    game activegame = game(0,0);
    std::vector<QPushButton*> buttons;

    void updateboard(int sidesize);
    void updateboard(int sidesize, int cols);
private slots:
    void on_playButton_clicked();
    void on_settingsButton_clicked();
    void on_backButton_clicked();
    void updateUI(int mode);

    void on_easyButton_clicked();

    void on_colorWhiteButton_clicked();
    void on_colorBlackButton_clicked();
    void on_colorBlueButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_widthplus_clicked();

    void on_widthminus_clicked();

    void on_heightplus_clicked();

    void on_heightminus_clicked();

private:
    Ui::Zarovka *ui;
    QColor selectedBgColor;  // current bg color

    void loadSettings();
    void applySettings();
    void saveSettings();
    void updateColorButtons();

protected:
    void resizeEvent(QResizeEvent *event) override;

};
#endif // ZAROVKA_H
