#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QPointer>

class SquareButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SquareButton(QWidget *parent = nullptr)
        : QPushButton(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

protected:
    QSize sizeHint() const override
    {
        QSize s = QPushButton::sizeHint();
        int side = qMax(s.width(), s.height());
        s.setWidth(side);
        s.setHeight(side);
        return s;
    }
};

class GridWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GridWidget(QWidget *parent = nullptr)
        : QWidget(parent), m_layout(nullptr) {}

    // -----------------------------------------------------------------
    // Public API – call this whenever you want a new grid
    // -----------------------------------------------------------------
    void createGrid(int rows, int cols);

signals:
    void cellClicked(int row, int col, bool checked);

private:
    QGridLayout *m_layout = nullptr;
};
