#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>

class SquareButton : public QPushButton {
    Q_OBJECT
public:
    using QPushButton::QPushButton;
    void setSquareSize(int size) {
        setFixedSize(size, size);
    }
};

class GridWidget : public QWidget {
    Q_OBJECT
public:
    GridWidget(QWidget *parent = nullptr);
    void createGrid(int rows, int cols);

signals:
    void cellClicked(int row, int col, bool checked);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QGridLayout *m_layout = nullptr;
    QVector<SquareButton*> m_buttons;
    int m_rows = 0, m_cols = 0;
};
