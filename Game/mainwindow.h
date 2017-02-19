#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QKeyEvent>
#include <QProgressBar>
#include "gameengine.h"
#include "myframe.h"  // this is needed for drag and drop.

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setEnergyBar(int value);
    void setHealthBar(float value);
    void setNoOfHpackRemaining(int x);
    void changeHealthPic();
    void showHInfo();
    void showSpeedInfo();

private slots:
    void about();

protected:
    void wheelEvent(QWheelEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void resizeEvent( QResizeEvent * event);

private:
    QWidget *createCellWidget(const QString &text,const QString &image);
    QVBoxLayout *createSideBar();
    QVBoxLayout *createEandHBar();
    void createMenus();
    void createActions();

    Ui::MainWindow *ui;
    QToolBox *toolBox;

    QAction *exitAction;
    QAction *strategyAction;
    QAction *aboutAction;

    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QHBoxLayout *GameLayout;

    QProgressBar * energy;
    QProgressBar * health;
    QLabel * energyPercentage;
    QLabel * healthPercentage;

    QToolButton *Speedbutton;
    QSlider * animation_speed;
    QLabel * animation;


    QComboBox *HeuristicWeightCombo;
    QLabel * HeuristWeight;

    QToolButton *infoButton;

    QLabel * noOfHealth;
    MyFrame * frame;
    QLabel *brol;

    std::shared_ptr<GameEngine> game;
};

#endif // MAINWINDOW_H
