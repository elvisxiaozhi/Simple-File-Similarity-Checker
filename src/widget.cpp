#include "widget.h"
#include "ui_widget.h"

const int Widget::rows = 30;
const int Widget::cols = 30;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setGameLayout();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setGameLayout()
{
    ui->horizontalSlider->setMinimum(1);
    ui->horizontalSlider->setMaximum(10);

    for(int i = 0; i < rows; ++i) {
        QVector<CellLabel *> colVec;
        for(int j = 0; j < cols; ++j) {
            CellLabel *cell = new CellLabel(this, i, j);
            ui->gameLayout->addWidget(cell, i, j);

            colVec.push_back(cell);
        }
        cellLblVec.push_back(colVec);
    }

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &Widget::timerTimeOut);
    connect(ui->nextButton, &QPushButton::clicked, this, &Widget::timerTimeOut);
    connect(ui->startButton, &QPushButton::clicked, [this](){ timer->start(1000); });
    connect(ui->stopButton, &QPushButton::clicked, [this](){ timer->stop(); });
    connect(ui->clearButton, &QPushButton::clicked, this, &Widget::clearCells);
    connect(ui->horizontalSlider, &QSlider::valueChanged, [this](int value) { timer->start(1000 / value); });
}

int Widget::returnNeighborNums(int row, int col)
{
    int nums = 0;
    for(int i = -1; i < 2; ++i) {
        for(int j = -1; j < 2; ++j) {
            if(row + i >= 0 && row + i <= 29 && col + j >= 0 && col + j <= 29) {
                if(row + i == row && col + j == col) {
                    continue;
                }
                else if(cellLblVec[row + i][col + j]->isAlive) {
                    ++nums;
                }
            }
        }
    }

    return nums;
}

void Widget::addReproductCellPos(int row, int col)
{
    for(int i = -1; i < 2; ++i) {
        for(int j = -1; j < 2; ++j) {
            if(row + i >= 0 && row + i <= 29 && col + j >= 0 && col + j <= 29) {
                if(row + i == row && col + j == col) {
                    continue;
                }
                else if(returnNeighborNums(row + i, col + j) == 3) {
                    reproductCellPos.push_back(std::make_pair(row + i, col + j));
                }
            }
        }
    }
}

void Widget::addAboutToDieCellPos(int row, int col)
{
    if(returnNeighborNums(row, col) < 2 || returnNeighborNums(row, col) > 3) {
        aboutToDieCellPos.push_back(std::make_pair(row, col));
    }
}

void Widget::changeCellStatus()
{
    for(int i = 0; i < reproductCellPos.size(); ++i) {
        cellLblVec[reproductCellPos[i].first][reproductCellPos[i].second]->alive();
    }

    for(int i = 0; i < aboutToDieCellPos.size(); ++i) {
        cellLblVec[aboutToDieCellPos[i].first][aboutToDieCellPos[i].second]->dead();
    }
}

void Widget::timerTimeOut()
{
    reproductCellPos.clear();
    aboutToDieCellPos.clear();

    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            if(cellLblVec[i][j]->isAlive) {
                addReproductCellPos(i, j);
                addAboutToDieCellPos(i, j);
            }
        }
    }

    changeCellStatus();
}

void Widget::clearCells()
{
    timer->stop();

    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            if(cellLblVec[i][j]->isAlive) {
                cellLblVec[i][j]->dead();
            }
        }
    }
}
