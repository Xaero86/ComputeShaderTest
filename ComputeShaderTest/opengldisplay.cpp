#include "opengldisplay.h"

#include <QOpenGLShader>

#include <iostream>

#define ATTRIBUTE_VERTEX     "vertex"
#define UNIFORM_MATIX        "u_mvpMatrix"
#define UNIFORM_SOLID_COLOR  "u_solidColor"
#define UNIFORM_WIRE_COLOR   "u_wireColor"
#define UNIFORM_LIGHT_COLOR  "u_lightColor"

OpenGLDisplay::OpenGLDisplay(QWidget* parent, OpenGLComputeGeodesic* calculator)
	: QOpenGLWidget(parent), _programDisplay(nullptr),
	  _vboVertices(), _vboFaces(QOpenGLBuffer::IndexBuffer), _calculator(calculator), _cameraDist(-5.0),
	  _colorSolid(38,233,38,255), _colorWire(233,38,38,255), _colorBack(38,38,233), _colorLight(255,255,255), _displayType(Solid)
{
	setMinimumSize(512,512);

	if (_calculator != nullptr)
	{
		connect(_calculator, SIGNAL(computeEnd()), this, SLOT(loadData()));
	}
}

OpenGLDisplay::~OpenGLDisplay()
{
	if (_programDisplay == nullptr)
	{
		return;
	}
	makeCurrent();
	_vboVertices.destroy();
	_vboFaces.destroy();
	delete _programDisplay;
	_programDisplay = nullptr;
	doneCurrent();
}

void OpenGLDisplay::setColorSolid(QColor colorFill)
{
	_colorSolid = colorFill;
	update();
}

void OpenGLDisplay::setColorWire(QColor colorWire)
{
	_colorWire = colorWire;
	update();
}

void OpenGLDisplay::setColorBack(QColor colorBack)
{
	_colorBack = colorBack;
	update();
}

void OpenGLDisplay::setColorLight(QColor colorLight)
{
	_colorLight = colorLight;
	update();
}

void OpenGLDisplay::setDisplayType(DisplayType displayType)
{
	_displayType = displayType;
	update();
}

void OpenGLDisplay::initializeGL()
{
	_oFct.initializeOpenGLFunctions();
	_programDisplay = new QOpenGLShaderProgram;
	_programDisplay->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert");
	_programDisplay->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shader.geom");
	_programDisplay->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag");
	_programDisplay->link();
	_vboVertices.create();
	_vboFaces.create();
}

void OpenGLDisplay::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	_oFct.glViewport((width - side) / 2, (height - side) / 2, side, side);
	_projection.setToIdentity();
	_projection.perspective(70.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 30.0f);
}

void OpenGLDisplay::paintGL()
{
	if (_calculator != nullptr)
	{
		_mutexProgram.lock();

		_oFct.glClearColor(static_cast<GLclampf>(_colorBack.redF()),
						   static_cast<GLclampf>(_colorBack.greenF()),
						   static_cast<GLclampf>(_colorBack.blueF()),1.0f);

		_oFct.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (_displayType != WireFrame)
		{
			_oFct.glEnable(GL_DEPTH_TEST);
		}
		else
		{
			_oFct.glDisable(GL_DEPTH_TEST);
		}
		_oFct.glEnable(GL_BLEND);
		_oFct.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		_programDisplay->bind();

		QMatrix4x4 cameraRotation;
		cameraRotation.translate(_calculator->center().x,_calculator->center().y,_calculator->center().z);
		cameraRotation.translate(0.0, 0.0, _cameraDist);
		cameraRotation.rotate(_rotationCurrent);

		QVector4D colorSolid = QVector4D(static_cast<float>(_colorSolid.redF()),
										static_cast<float>(_colorSolid.greenF()),
										static_cast<float>(_colorSolid.blueF()),
										(_displayType != WireFrame)?1.0f:0.0f);

		QVector4D colorWire = QVector4D(static_cast<float>(_colorWire.redF()),
										static_cast<float>(_colorWire.greenF()),
										static_cast<float>(_colorWire.blueF()),
										(_displayType != Solid)?1.0f:0.0f);

		_programDisplay->setUniformValue(UNIFORM_MATIX, _projection * cameraRotation);
		_programDisplay->setUniformValue(UNIFORM_SOLID_COLOR, colorSolid);
		_programDisplay->setUniformValue(UNIFORM_WIRE_COLOR, colorWire);
		_programDisplay->setUniformValue(UNIFORM_LIGHT_COLOR, _colorLight);

		_vboVertices.bind();
		_vboFaces.bind();

		_programDisplay->enableAttributeArray(ATTRIBUTE_VERTEX);
		_programDisplay->setAttributeBuffer(ATTRIBUTE_VERTEX, GL_FLOAT, 0, 3, sizeof(OpenGLComputeGeodesic::t_vertice));

		_oFct.glDrawElements(GL_TRIANGLES, static_cast<int>(OpenGLComputeGeodesic::t_face::nbElements()*_calculator->nbFacesOut()), GL_UNSIGNED_INT, nullptr);

		_vboFaces.release();
		_vboVertices.release();
		_programDisplay->release();
		_mutexProgram.unlock();
	}
}

void OpenGLDisplay::loadData()
{
	if (_calculator == nullptr)
	{
		return;
	}
	_mutexProgram.lock();
	_programDisplay->bind();

	_vboVertices.bind();
	unsigned int nbVertices = _calculator->nbVerticesOut();
	OpenGLComputeGeodesic::t_vertice* vertices = new OpenGLComputeGeodesic::t_vertice[nbVertices];
	_calculator->verticesOut(vertices);
	_vboVertices.allocate(vertices, static_cast<int>(nbVertices*sizeof(OpenGLComputeGeodesic::t_vertice)));
	delete[] vertices;
	_vboVertices.release();

	_vboFaces.bind();
	unsigned int nbFaces = _calculator->nbFacesOut();
	OpenGLComputeGeodesic::t_face* faces = new OpenGLComputeGeodesic::t_face[nbFaces];
	_calculator->facesOut(faces);
	_vboFaces.allocate(faces, static_cast<int>(nbFaces*sizeof(OpenGLComputeGeodesic::t_face)));
	delete[] faces;
	_vboFaces.release();

	_programDisplay->release();
	_mutexProgram.unlock();
	update();
}

void OpenGLDisplay::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
        _mousePressPos = QVector2D(event->position());
		_rotationTotal = _rotationCurrent;
	}
}

void OpenGLDisplay::mouseMoveEvent(QMouseEvent *event)
{
#define ROTATE_SPEED 0.5f
	if (event->buttons() == Qt::LeftButton)
	{
		// Distance parcourue en mouse press
        QVector2D distance = QVector2D(event->position()) - _mousePressPos;
		QVector3D rotationAxis = QVector3D(distance.y(), distance.x(), 0.0).normalized();

		_rotationCurrent = QQuaternion::fromAxisAndAngle(rotationAxis, distance.length()*ROTATE_SPEED) * _rotationTotal;
		update();
	}
}

void OpenGLDisplay::wheelEvent(QWheelEvent *event)
{
#define ZOOM_FACTOR 120
#define WHEEL_SPEED 1
    _cameraDist += static_cast<float>(event->angleDelta().y() / ZOOM_FACTOR) * WHEEL_SPEED;
	update();
}
