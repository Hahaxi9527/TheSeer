#ifndef THESEER_H
#define THESEER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TheSeer; }
QT_END_NAMESPACE

class TheSeer : public QWidget
{
    Q_OBJECT

public:
    TheSeer(QWidget *parent = nullptr);
    ~TheSeer();

private:
    Ui::TheSeer *ui;
};
#endif // THESEER_H
