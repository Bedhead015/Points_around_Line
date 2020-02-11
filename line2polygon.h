#ifndef LINE2POLYGON_H
#define LINE2POLYGON_H

#include <QMainWindow>

namespace Ui {
class line2polygon;
}

class line2polygon : public QMainWindow
{
    Q_OBJECT

public:
    explicit line2polygon(QWidget *parent = nullptr);
    ~line2polygon();

private slots:
    void on_BrowseButton_clicked();

    void on_converButton_clicked();

private:
    Ui::line2polygon *ui;
};

#endif // LINE2POLYGON_H
