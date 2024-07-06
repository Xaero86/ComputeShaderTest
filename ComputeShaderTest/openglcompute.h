#ifndef OPENGLCOMPUTE_H
#define OPENGLCOMPUTE_H

#include <QObject>
#include <QThread>
#include <QOpenGLContext>
#include <QOffscreenSurface>

class OpenGLCompute;

class ComputeThread : public QThread
{
	Q_OBJECT
public:
	ComputeThread(OpenGLCompute *openGLCompute);

	void run();

private:
	OpenGLCompute *_openGLCompute;
};

class OpenGLCompute : public QObject
{
	Q_OBJECT
public:
	OpenGLCompute();

	QOpenGLContext *context() const {return _context;}

	virtual void initializeGL() = 0;
	virtual void cleanup() = 0;

	virtual void destroy() final;

signals:
	void signalCompute();

public slots:
	virtual void startCompute() final;

protected slots:
	virtual void compute() = 0;

private:
	ComputeThread             _myThread;
	QOpenGLContext            *_context;
	QOffscreenSurface         _offSurface;

	friend class ComputeThread;
};

#endif // OPENGLCOMPUTE_H
