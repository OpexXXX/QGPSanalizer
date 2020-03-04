#ifndef PORTSETTINGS_H
#define PORTSETTINGS_H

#include <QWidget>

namespace Ui {
class PortSettings;
}

class PortSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PortSettings(QWidget *parent = nullptr);
    ~PortSettings();

private:
    Ui::PortSettings *ui;
};

#endif // PORTSETTINGS_H
