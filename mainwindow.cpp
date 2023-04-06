#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtWidgets/QHeaderView>
#include "hash_table.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <vector>
#include <array>

UnorderedMap<std::string> hash_table;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setColumnCount(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//private QTable update func
void MainWindow::update_table()
{
    ui->tableWidget->clearContents();
    std::vector<std::string> update_list;
    update_list = hash_table.all_keys();
    ui->tableWidget->setRowCount(update_list.size());
    for (int i = 0; i < update_list.size(); ++i){
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(update_list[i])));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(hash_table[update_list[i]])));
        }
}


void MainWindow::on_lineEdit_textChanged(const QString& key)
{
    ui->addButton->setEnabled(!key.isEmpty());
    ui->eraseButton->setEnabled(!key.isEmpty());
}

void MainWindow::on_addButton_clicked()
{
    hash_table<<ui->lineEdit->text().toStdString();
    ui->lineEdit->clear();
    update_table();
}


void MainWindow::on_eraseButton_clicked()
{
    hash_table.erase(ui->lineEdit->text().toStdString());
    ui->lineEdit->clear();
    update_table();
}

void MainWindow::on_saveAction_triggered()
{
    QMessageBox msg;
    QString filename = QFileDialog::getSaveFileName(this, tr("Open File"), "", tr("ALl Files (*)"));
    if (!hash_table.save(filename.toStdString())){
        msg.setText("Error occured while saving the file!");
        msg.exec();
    };
    update_table();
}

void MainWindow::on_loadAction_triggered()
{
    QMessageBox msg;
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("ALl Files (*)"));
    if (!hash_table.load(filename.toStdString())){
        msg.setText("Error occured while loading the file!");
        msg.exec();
    };
    update_table();
}

void MainWindow::on_getWinnersAction_triggered()
{
    std::array<std::string, 3> results = hash_table.get_winners();
    QMessageBox msg;
    msg.setWindowTitle("Winners");
    msg.setText(QString::fromStdString("Победители: \n") +
                "1. " + QString::fromStdString(results[0]) + "\n" +
                "2. " + QString::fromStdString(results[1]) + "\n" +
                "3. " + QString::fromStdString(results[2]) + "\n");
    msg.setFixedSize(400, 400);
    msg.exec();
}

void MainWindow::on_cleanAction_triggered()
{
    hash_table.clear();
    update_table();
}
