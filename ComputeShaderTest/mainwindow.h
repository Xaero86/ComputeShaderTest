#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "opengldisplay.h"
#include "openglcomputegeodesic.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

public slots:
	void loadTetraedre();
	void loadOctaedre();
	void loadIcosaedre();
	void loadTorus();

	void changeColorSolid();
	void changeColorWire();
	void changeColorBack();
	void changeColorLight();
	void changeToSolid();
	void changeToWithWire();
	void changeToWireFrame();

	void startTimer();
	void stopTimer();
	void resetTimer();
	void timerAction();

private:
	OpenGLDisplay*         _openGLDisplay;
	OpenGLComputeGeodesic* _openGLComputeGeodesic;

	QTimer                 _timer;
};

#endif // MAINWINDOW_H
