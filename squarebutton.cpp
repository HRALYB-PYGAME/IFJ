#include "squarebutton.h"
#include <algorithm>

QSize squarebutton::sizeHint() const
{
    // start from the normal button size
    QSize sh = QPushButton::sizeHint();

    // make it square – use the *larger* dimension so the text always fits
    int side = std::max(sh.width(), sh.height());
    return QSize(side, side);
}
