#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;


private slots:
    void on_addButton_clicked();

    void on_eraseButton_clicked();

    void on_saveAction_triggered();

    void on_loadAction_triggered();

    void on_getWinnersAction_triggered();

    void on_cleanAction_triggered();

    void on_lineEdit_textChanged(const QString& key);
private:

    void update_table();

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
