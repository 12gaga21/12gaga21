#include "AddressBar.h"
#include <QKeyEvent>

AddressBar::AddressBar(QWidget *parent)
    : QLineEdit(parent)
{
    setClearButtonEnabled(true);
}

void AddressBar::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        handleReturnKey();
        return;
    }
    
    QLineEdit::keyPressEvent(event);
}

void AddressBar::handleReturnKey()
{
    emit urlEntered();
}