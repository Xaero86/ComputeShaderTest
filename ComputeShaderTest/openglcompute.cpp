#include "openglcompute.h"

OpenGLCompute::OpenGLCompute()
	: QObject(), _myThread(this), _offSurface()
{
    QSurfaceFormat format = _offSurface.requestedFormat();
    format.setVersion(4,3);
    _offSurface.setFormat(format);
	_offSurface.create();

	_context = new QOpenGLContext(this);
	_context->setFormat(_offSurface.requestedFormat());
	_context->create();

	_myThread.start();
	moveToThread(&_myThread);

	connect(this, SIGNAL(signalCompute()), this, SLOT(compute()));
}

void OpenGLCompute::destroy()
{
	_myThread.quit();
	_myThread.wait();
	_offSurface.destroy();
}

void OpenGLCompute::startCompute()
{
	emit signalCompute();
}

ComputeThread::ComputeThread(OpenGLCompute *openGLCompute)
	: QThread(openGLCompute), _openGLCompute(openGLCompute) {}

void ComputeThread::run()
{
	_openGLCompute->_context->makeCurrent(&_openGLCompute->_offSurface);
	_openGLCompute->initializeGL();
	exec();
	_openGLCompute->cleanup();
	delete _openGLCompute->_context;
}
