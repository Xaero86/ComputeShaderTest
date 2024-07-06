#ifndef OPENGLDISPLAY_H
#define OPENGLDISPLAY_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <QtWidgets>
#include <QMouseEvent>

#include "openglcomputegeodesic.h"

class OpenGLDisplay : public QOpenGLWidget
{
	Q_OBJECT
public:
	enum DisplayType
	{
		Solid        = 0,
		WithWire     = 1,
		WireFrame    = 2
	};

	OpenGLDisplay(QWidget* parent, OpenGLComputeGeodesic* calculator);
	~OpenGLDisplay() override;

	QColor colorSolid() {return _colorSolid;}
	QColor colorWire() {return _colorWire;}
	QColor colorBack() {return _colorBack;}
	QColor colorLight() {return _colorLight;}

	void setColorSolid(QColor colorSolid);
	void setColorWire(QColor colorWire);
	void setColorBack(QColor colorBack);
	void setColorLight(QColor colorLight);
	void setDisplayType(DisplayType displayType);

public slots:
	void loadData();

protected:
	void initializeGL() override;
	void resizeGL(int width, int height) override;
	void paintGL() override;

	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

private:
	QOpenGLExtraFunctions     _oFct;
	QOpenGLShaderProgram*     _programDisplay;
	QOpenGLBuffer             _vboVertices;
	QOpenGLBuffer             _vboFaces;

	OpenGLComputeGeodesic*    _calculator;

	QMutex                    _mutexProgram;

	float                     _cameraDist;
	QMatrix4x4                _projection;
	QVector2D                 _mousePressPos;
	QQuaternion               _rotationCurrent;
	QQuaternion               _rotationTotal;

	QColor                    _colorSolid;
	QColor                    _colorWire;
	QColor                    _colorBack;
	QColor                    _colorLight;
	DisplayType               _displayType;
};

#endif // OPENGLDISPLAY_H
