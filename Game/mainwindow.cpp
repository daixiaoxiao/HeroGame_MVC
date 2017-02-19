#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    game = std::make_shared<GameEngine>(":/images/Mario.png");

    createActions();
    createMenus();

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(createSideBar());

    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("STRATEGY"));

    GameLayout = new QHBoxLayout;
    GameLayout->addWidget(toolBox);
    GameLayout->addWidget(game->getArenaview().get());

    QWidget *widget = new QWidget;
    widget->setLayout(createEandHBar());

    setCentralWidget(widget);
    //    ui->setupUi(this);

    QObject::connect((game->getArenamodel()->getHeroModel()).get(), SIGNAL(sig_energyChanged(int)),
                     this, SLOT(setEnergyBar(int)));
    QObject::connect((game->getArenamodel()->getHeroModel()).get(), SIGNAL(sig_healthChanged(float)),
                     this, SLOT(setHealthBar(float)));

    QObject::connect((game->getArenaview()).get(),SIGNAL(updateNoOfHealthRemaing(int)),
                     this,SLOT(setNoOfHpackRemaining(int)) );
    QObject::connect((game->getArenaview()).get(),SIGNAL(updateHealthPic()),
                     this,SLOT(changeHealthPic()) );

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setEnergyBar(int value)
{
    energy->setValue(value);
    energyPercentage->setText(QString::number(energy->value()) + "/100");
    if(energy->value() < 50)
    {
        energy->setStyleSheet("QProgressBar::chunk {background-color: #CCCC00;}");
    }
    else
    {
        energy->setStyleSheet("QProgressBar::chunk {background-color: #ffff00;}");
    }
}

void MainWindow::setHealthBar(float value)
{
    health->setValue(value);
    healthPercentage->setText(QString::number(health->value()) + "/100");
    if(health->value() < 50)
    {
        health->setStyleSheet("QProgressBar::chunk {background-color: #FF0000;}");
    }
    else
    {
        health->setStyleSheet("QProgressBar::chunk {background-color: #00FF00;}");
    }
}

void MainWindow::setNoOfHpackRemaining(int x)
{
    int noHpack = x;
    QString hPacks = QString::fromStdString(std::to_string(noHpack));
    noOfHealth->setText(hPacks);
}

void MainWindow::changeHealthPic()
{
    brol->setPixmap(QPixmap(":/images/healthFinished.jpeg").scaled(30,30));
}

void MainWindow::showHInfo()
{
    QMessageBox::about(this, tr("Value to choose for Heuristic Weight"),
                       tr("The <b>MVC Protagonist Game</b> shows how to choose correct Heuristic weight for Worldmap4!.\n"
                          "(0 for Dijkstra) "
                          "(50 for A*) "
                          "(500 for Best First)"));
}

void MainWindow::showSpeedInfo()
{
    QMessageBox::about(this, tr("Value to Adjust for Animation speed"),
                       tr("The <b>MVC Protagonist Game</b> shows how to choose correct Animation Speed!.\n"
                          "from 100ms~ 1000ms "));
}


void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y() > 0)
        game->getArenaview()->scale(1.1, 1.1);
    else
        game->getArenaview()->scale(0.9, 0.9);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    game->getArenaview()->fitInView(0, 0, game->getArenamodel()->getCols() * 30, game->getArenamodel()->getRows() * 30, Qt::KeepAspectRatio);
}

QWidget *MainWindow::createCellWidget(const QString &text, const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(70, 70));
    button->setCheckable(true);

    QObject::connect(button, &QToolButton::clicked,
                     game.get(), &GameEngine::AI);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignHCenter);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    frame = new MyFrame();
    frame->setAcceptDrops(true);
    frame->setAutoFillBackground(true);
    frame->setFixedSize(30,30);

    QStackedLayout *vLayout = new QStackedLayout;

    brol = new QLabel();
    brol->setPixmap(QPixmap(":/images/health.jpeg").scaled(30,30));
    vLayout->addWidget(brol);

    frame->setLayout(vLayout);
    layout->addWidget(frame,150,130,Qt::AlignCenter);

    int noHpack = game->getArenaview()->getSizeOfHpacks();
    QString hPacks = QString::fromStdString(std::to_string(noHpack));

    noOfHealth = new QLabel(hPacks);
    QString str = "Extra Health Pack: ";
    QLabel * des = new QLabel(str);

    layout->addWidget(des,150,0,Qt::AlignLeft);
    layout->addWidget(noOfHealth,150,100,Qt::AlignRight);

    QWidget *widget = new QWidget;
    widget->setAcceptDrops(true);
    widget->setLayout(layout);

    return widget;
}

QVBoxLayout *MainWindow::createSideBar()
{
    Speedbutton = new QToolButton;
    Speedbutton->setText("AnimationSpeed");
    Speedbutton->setIcon(QIcon(":/images/animationSpeed.jpg"));
    Speedbutton->setIconSize(QSize(70, 70));
    Speedbutton->setCheckable(true);

    QHBoxLayout * speedbuttonLayout = new QHBoxLayout;
    speedbuttonLayout->addWidget(Speedbutton, Qt::AlignHCenter);

    connect(Speedbutton, &QToolButton::clicked,
            this, &MainWindow::showSpeedInfo);

    animation = new QLabel;
    animation->setText(tr("Animation Speed"));

    animation_speed =new QSlider(Qt::Horizontal);
    animation_speed->setMinimum(100);
    animation_speed->setMaximum(1000);
    QObject::connect(animation_speed, &QSlider::valueChanged,
                     game.get(), &GameEngine::setInterval);

    QVBoxLayout * AnimationLayout = new QVBoxLayout;
    AnimationLayout->addWidget(animation, Qt::AlignCenter);
    AnimationLayout->addWidget(animation_speed);

    HeuristWeight = new QLabel;
    HeuristWeight->setText(tr("Heuristic Weight"));

    HeuristicWeightCombo = new QComboBox;
    HeuristicWeightCombo->setEditable(true);
    for (int i = 0; i <= 500; i = i + 50)
        HeuristicWeightCombo->addItem(QString().setNum(i));
    HeuristicWeightCombo->addItem(QString().setNum(30000));
    QIntValidator *validator = new QIntValidator(0, 30000, this);
    HeuristicWeightCombo->setValidator(validator);
    connect(HeuristicWeightCombo, SIGNAL(currentIndexChanged(QString)),
            game.get(), SLOT(setPathFHeurist(QString)));
    connect(HeuristicWeightCombo, SIGNAL(currentIndexChanged(QString)),
            game.get(), SLOT(setStrategyHeurist(QString)));

    QVBoxLayout * HWeightLayout = new QVBoxLayout;
    HWeightLayout->addWidget(HeuristWeight);
    HWeightLayout->addWidget(HeuristicWeightCombo);

    infoButton = new QToolButton;
    infoButton->setText("Info of HeuristicWeight");
    infoButton->setIcon(QIcon(":/images/info.jpg"));
    infoButton->setIconSize(QSize(70, 70));
    infoButton->setCheckable(true);

    QHBoxLayout * infoButtonLayout = new QHBoxLayout;
    infoButtonLayout->addWidget(infoButton, Qt::AlignHCenter);
    connect(infoButton, &QToolButton::clicked,
            this, &MainWindow::showHInfo);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(createCellWidget(tr("Strategy"),
                                       ":/images/strategyImage.jpg"), Qt::AlignCenter);
    layout->addLayout(speedbuttonLayout, Qt::AlignHCenter);
    layout->addLayout(AnimationLayout, Qt::AlignHCenter);
    layout->addLayout(HWeightLayout, Qt::AlignHCenter);
    layout->addLayout(infoButtonLayout, Qt::AlignHCenter);

    return layout;
}

QVBoxLayout *MainWindow::createEandHBar()
{
    health = new QProgressBar;
    health->setRange(0, 100);
    health->setValue(game->getArenamodel()->getHeroModel()->getHero()->getHealth());
    health->setStyleSheet("QProgressBar::chunk {background-color: #00FF00;}");

    QLabel * healthLabel = new QLabel("Health");

    healthPercentage = new QLabel(QString::number(health->value()) + "/100");

    QHBoxLayout * healthBar = new QHBoxLayout;
    healthBar->addWidget(healthLabel);
    healthBar->addWidget(health);
    healthBar->addWidget(healthPercentage);

    energy = new QProgressBar;
    energy->setRange(0, 100);
    energy->setValue(game->getArenamodel()->getHeroModel()->getHero()->getEnergy());
    energy->setStyleSheet("QProgressBar::chunk {background-color: #ffff00;}");

    QLabel * energyLabel = new QLabel("Energy");

    energyPercentage = new QLabel(QString::number(energy->value()) + "/100");

    QHBoxLayout * energyBar = new QHBoxLayout;
    energyBar->addWidget(energyLabel);
    energyBar->addWidget(energy);
    energyBar->addWidget(energyPercentage);

    QVBoxLayout * BottomBar = new QVBoxLayout;
    BottomBar->addLayout(healthBar);
    BottomBar->addLayout(energyBar);

    QVBoxLayout * Layout = new QVBoxLayout;
    Layout->addLayout(GameLayout);
    Layout->addLayout(BottomBar);

    return Layout;
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Strategy"));
    itemMenu->addAction(strategyAction);
    itemMenu->addSeparator();

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}

void MainWindow::createActions()
{
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit Game"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    strategyAction = new QAction(tr("S&trategy"), this);
    connect(strategyAction, SIGNAL(triggered()), game.get(), SLOT(AI()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("F1"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About MVC Game"),
                       tr("The <b>MVC Protagonist Game</b> shows "
                          "how to play the game."));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_I)
    {
        game->moveOneStep(Up);
    }
    if(event->key() == Qt::Key_K)
    {
        game->moveOneStep(Down);
    }
    if(event->key() == Qt::Key_J)
    {
        game->moveOneStep(Left);
    }
    if(event->key() == Qt::Key_L)
    {
        game->moveOneStep(Right);
    }
    if(event->key() == Qt::Key_S)
    {
        game->AI();
    }
}


