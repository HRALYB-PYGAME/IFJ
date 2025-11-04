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
    void updateUI(int mode);

    void on_easyButton_clicked();

private:
    Ui::Zarovka *ui;

protected:
    void resizeEvent(QResizeEvent *event) override;

};
#endif // ZAROVKA_H
