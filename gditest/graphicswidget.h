#ifndef GRAPHICSWIDGET_H
#define GRAPHICSWIDGET_H

#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>
#include <QPixmap>
#include <QPainter>
#include <qmath.h>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QTime>
#include <qtimer.h>
class GraphicsWidget : public QWidget
{
    Q_OBJECT
public:
    GraphicsWidget(QWidget *parent = nullptr);
public:
    enum ProjectionType
    {
        Ortho,
        Frustum
    };
private:
    const float INIT_DEPTH = -100.0;
    QMatrix4x4 ortho;
    QMatrix4x4 frustum;
    QMatrix4x4 camera;
    QMatrix4x4 model;

    QVector3D displace = QVector3D(0.0,0.0,INIT_DEPTH);
    QVector3D scale = QVector3D(1.0,1.0,1.0);

//    float xRot=45.0, yRot=-45.0, zRot=0.0;
    float xRot=0.0, yRot=-0.0, zRot=0.0;
    ProjectionType projectionType;
    QVector3D getScreenCoordinatesFromOrtho(QVector3D v);
    QVector3D getScreenCoordinatesFromFrustum(QVector3D v);
    QVector3D getScreenCoordinates(QVector3D v);

    void setViewPort(int left, int right, int bottom, int top);
    QPixmap tempPix;
    void paintOrigin();

    QMatrix4x4 getOrtho(float left, float right,
                        float bottom, float top,
                        float nearPlane, float farPlane);

    QMatrix4x4 getFrustum(float left, float right,
                          float bottom, float top,
                          float nearPlane, float farPlane);

    QPointF lastMousePos;
//    float yaw, roll, pitch;

protected:
public:
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
};

#endif // GRAPHICSWIDGET_H
