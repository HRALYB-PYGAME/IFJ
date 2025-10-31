#ifndef ZAROVKA_H
#define ZAROVKA_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Zarovka;
}
QT_END_NAMESPACE

class Zarovka : public QMainWindow
{
    Q_OBJECT

public:
    Zarovka(QWidget *parent = nullptr);
    ~Zarovka();
    int mode;

private slots:
    void on_playButton_clicked();
    void updateUI(int mode);

private:
    Ui::Zarovka *ui;

};
#endif // ZAROVKA_H
