#ifndef MYFRAME_H
#define MYFRAME_H

#include <QFrame>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

class MyFrame : public QFrame
  {
    Q_OBJECT
  public:
    explicit MyFrame(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *event);
  };

#endif // MYFRAME_H
