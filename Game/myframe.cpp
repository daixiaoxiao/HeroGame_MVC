#include "myframe.h"
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QLabel>

MyFrame::MyFrame(QWidget *parent) :
    QFrame(parent)
{
}

void MyFrame::mousePressEvent(QMouseEvent * event)
{
    QLabel *curLabel = dynamic_cast<QLabel*>(childAt(event->pos()));
    if (!curLabel)
        return;


    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QPixmap pixmap = *curLabel->pixmap();
    dataStream << pixmap << curLabel->size()<< QPoint(event->pos() - curLabel->pos());
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/Demo-QLabeldata", itemData);
    drag->setMimeData(mimeData);
    drag->setPixmap(*curLabel->pixmap());
    drag->setHotSpot(event->pos() - curLabel->pos());

    if(drag->exec(Qt::MoveAction | Qt::CopyAction) == Qt::CopyAction)
        delete curLabel;
}

void MyFrame::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/Demo-QLabeldata"))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }

}

void MyFrame::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/Demo-QLabeldata"))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}


