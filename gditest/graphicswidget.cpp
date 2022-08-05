#include "graphicswidget.h"

GraphicsWidget::GraphicsWidget(QWidget *parent) :
    QWidget(parent)
{
    projectionType = Ortho;
    camera.setToIdentity();
    model.setToIdentity();

    tempPix = QPixmap(width(), height());
    tempPix.fill(Qt::white);

}

QVector3D GraphicsWidget::getScreenCoordinates(QVector3D v)
{
    QVector3D m;
    if(projectionType == Ortho)
    {
        m = getScreenCoordinatesFromOrtho(v);
    }
    else
    {
        m = getScreenCoordinatesFromFrustum(v);
    }
    return m;
}

QVector3D GraphicsWidget::
getScreenCoordinatesFromOrtho(QVector3D v)
{
    QVector4D u4d;
    QVector3D u3d;
    u4d = QVector4D(v.x(), v.y(), v.z(), 1.0);
    u4d = ortho * camera * model * u4d;
    u3d = QVector3D(u4d.x(),u4d.y(),u4d.z());
//    u3d.setX((u3d.x()*0.5+0.5)*width());
//    u3d.setY(height()- (u3d.y()*0.5 + 0.5)*height());
    u3d.setX((u3d.x()*0.5+0.5)*width()+0.5*width());
    u3d.setY((u3d.y()*0.5+0.5)*height()-0.5*height());
    return u3d;
}

QVector3D GraphicsWidget::
getScreenCoordinatesFromFrustum(QVector3D v)
{
    QVector4D u4d;
    QVector3D u3d;
    u4d = QVector4D(v.x(), v.y(), v.z(), 1.0);
//    u4d = frustum * (camera * (model * u4d));
    u4d = frustum * camera * model * u4d;
//    u4d = frustum * (camera * (model * QVector4D(v)));
    u3d = u4d.toVector3DAffine();
    return QVector3D((u3d.x()*0.5+0.5)*width(),
                     height() - (u3d.y()*0.5+0.5)*height(),
                     (1.0+u3d.z())*0.5);
}

QMatrix4x4 GraphicsWidget::
getOrtho(float left, float right,
         float bottom, float top,
         float nearPlane, float farPlane)
{
    QMatrix4x4 m;
    m(0, 0) = 2.0/(right-left);
    m(0, 1) = 0.0;
    m(0, 2) = 0.0;
    m(0, 3) = -(right+left)/(right-left);

    m(1, 0) = 0.0;
    m(1, 1) = 2.0/(top-bottom);
    m(1, 2) = 0.0;
    m(1, 3) = -(top+bottom)/(top-bottom);

    m(2, 0) = 0.0;
    m(2, 1) = 0.0;
    m(2, 2) = -2.0/(farPlane-nearPlane);
    m(2, 3) = -(farPlane+nearPlane)/(farPlane-nearPlane);

    m(3, 0) = 0.0;
    m(3, 1) = 0.0;
    m(3, 2) = 0.0;
    m(3, 3) = 1.0;

    return m;
}

QMatrix4x4 GraphicsWidget::
getFrustum(float left, float right,
           float bottom, float top,
           float nearPlane, float farPlane)
{
    QMatrix4x4 m;
    m(0,0) = 2.0*nearPlane/(right-left);
    m(0,1) = 0.0;
    m(0,2) = (right+left)/(right-left);
    m(0,3) = 0.0;

    m(1,0) = 0.0;
    m(1,1) = 2.0*nearPlane/(top-bottom);
    m(1,2) = (top+bottom)/(top-bottom);
    m(1,3) = 0.0;

    m(2,0) = 0.0;
    m(2,1) = 0.0;
    m(2,2) = -(farPlane+nearPlane)/(farPlane-nearPlane);
    m(2,3) = -(2*farPlane*nearPlane)/(farPlane-nearPlane);

    m(3,0) = 0.0;
    m(3,1) = 0.0;
    m(3,2) = -1.0;
    m(3,3) = 0.0;

    return m;
}

void GraphicsWidget::
setViewPort(int left, int right, int bottom, int top)
{
    float zNear = 1.0;
    float zFar = 1000.0;
    float h = (float)top - bottom;
    float w = (float)right - left;
    qDebug() << "w" << w;
    qDebug() << "h" << h;
    ortho.setToIdentity();
//    ortho.ortho(0.0, w, 0.0, h, zNear, zFar);
    ortho.ortho(0.0, w, h, 0.0, zNear, zFar);
//    ortho = getOrtho(left, right, bottom, top, zNear, zFar);

    float aspect = w/h;
    float fW = qTan(25*M_PI/180.0)*zNear;
    float fH = fW/aspect;
    frustum.setToIdentity();
    frustum.frustum(-fW,fW,-fH,fH,zNear,zFar);
//    frustum = getFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void GraphicsWidget::paintOrigin()
{
    QPainter painter;
    painter.begin(&tempPix);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QVector3D origin = getScreenCoordinates(QVector3D(0.0, 0.0, 0.0));
    QVector3D xAxis  = getScreenCoordinates(QVector3D(10.0, 0.0, 0.0));
    QVector3D yAxis  = getScreenCoordinates(QVector3D(0.0, 10.0, 0.0));
    QVector3D zAxis  = getScreenCoordinates(QVector3D(0.0, 0.0, 10.0));

    painter.setPen(Qt::red);
    painter.drawLine(origin.x(), origin.y(), xAxis.x(), xAxis.y());

    painter.setPen(Qt::green);
    painter.drawLine(origin.x(), origin.y(), yAxis.x(), yAxis.y());

    painter.setPen(Qt::blue);
    painter.drawLine(origin.x(), origin.y(), zAxis.x(), zAxis.y());


    painter.end();
}

void GraphicsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    model.setToIdentity();
    if(projectionType==Ortho)
    {
        model.translate(displace.x(),displace.y(),0.0);
        model.scale(scale.x(),scale.y(),scale.z());
    }
    else
    {
        model.translate(displace);
    }
    model.rotate(xRot, QVector3D(1, 0, 0));
    model.rotate(yRot, QVector3D(0, 1, 0));
    model.rotate(zRot, QVector3D(0, 0, 1));
//    frustum.setToIdentity();
//    frustum.perspective(45.0f, (float)width()/(float)height(),
//                       1.0f, 100.0f);
    paintOrigin();
    QPainter painter;
    painter.begin(this);
    painter.drawPixmap(0, 0, tempPix);
    tempPix.fill(Qt::white);
    painter.end();
}

void GraphicsWidget::resizeEvent(QResizeEvent *event)
{
//    qDebug() << "width()" << event->size().width();
//    qDebug() << "height()" << event->size().height();
    setViewPort(0,event->size().width(),0,event->size().height());
    tempPix = tempPix.scaled(size());

    update();
}

void GraphicsWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void GraphicsWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastMousePos.x();
    int dy = event->y() - lastMousePos.y();

    float roll = 0.0;
    float pitch = 0.0;
    float yaw = 0.0;
    QMatrix4x4 t;
    if (event->buttons() & Qt::LeftButton)
    {
        if(event->modifiers() == Qt::ControlModifier)
        {
            yaw += dy;
        }
        else if ( event->modifiers() == Qt::AltModifier)
        {
//            t.translate(dx/32.0,-dy/32.0,0);
//            camera = t * camera;
            displace += QVector3D(dx/32.0,-dy/32.0,0.0);
        }
        else
        {
            pitch += dx;
            roll += dy;
        }
        update();
    }
   lastMousePos = event->pos();
   xRot += roll;
   if(xRot < -180)
   {
       xRot = 180;
   }
   if(xRot > 180)
   {
       xRot = 180;
   }

   yRot += pitch;
   if(yRot < -180)
   {
       yRot = 180;
   }
   if(yRot > 180)
   {
       yRot = 180;
   }

   zRot += yaw;
   if(zRot < -180)
   {
       zRot = 180;
   }
   if(zRot > 180)
   {
       zRot = 180;
   }
//   camera.rotate(roll, QVector3D(1, 0, 0));
//   camera.rotate(pitch, QVector3D(0, 1, 0));
//   camera.rotate(yaw, QVector3D(0, 0, 1));
}

void GraphicsWidget::wheelEvent(QWheelEvent* event)
{
    event->accept();

    float disz = displace.z();
    if(projectionType==Ortho)
    {
        if(event->delta()>0)
        {
            scale=scale*1.1;
        }
        else
        {
            scale=scale*0.9;
        }
    }
    else if(projectionType==Frustum)
    {
        displace.setZ(disz + event->delta()/32.0);
    }

    update();
}
