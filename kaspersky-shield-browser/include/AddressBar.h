#ifndef ADDRESSBAR_H
#define ADDRESSBAR_H

#include <QLineEdit>
#include <QUrl>
#include <QKeyEvent>

class AddressBar : public QLineEdit
{
    Q_OBJECT

public:
    explicit AddressBar(QWidget *parent = nullptr);

signals:
    void urlEntered();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void handleReturnKey();
};

#endif // ADDRESSBAR_H