#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), _timer(this)
{
	_openGLComputeGeodesic = new OpenGLComputeGeodesic();

	_openGLDisplay = new OpenGLDisplay(this, _openGLComputeGeodesic);
	setCentralWidget(_openGLDisplay);

	QMenu *menuLoad = menuBar()->addMenu("&Load");
	QAction* actionTetraedre = new QAction("&Tetraedre", this);
	QAction* actionIcosaedre = new QAction("&Icosaedre", this);
	QAction* actionOctaedre = new QAction("&Octaedre", this);
	QAction* actionTorus = new QAction("&Torus", this);
	QAction* actionQuit = new QAction("&Quit", this);
	menuLoad->addAction(actionTetraedre);
	menuLoad->addAction(actionOctaedre);
	menuLoad->addAction(actionIcosaedre);
	menuLoad->addAction(actionTorus);
	menuLoad->addSeparator();
	menuLoad->addAction(actionQuit);

	QMenu *menuAnim = menuBar()->addMenu("&Animation");
	QAction* actionStart = new QAction("&Start", this);
	QAction* actionStop = new QAction("&Stop", this);
	QAction* actionReset = new QAction("&Reset", this);
	menuAnim->addAction(actionStart);
	menuAnim->addAction(actionStop);
	menuAnim->addAction(actionReset);

	QMenu *menuConfig = menuBar()->addMenu("&Configuration");
	QMenu *menuColor = menuConfig->addMenu("&Color");
	QAction* actionColorSolid = new QAction("Color &solid", this);
	QAction* actionColorWire = new QAction("Color &wire", this);
	QAction* actionColorBack = new QAction("Color &background", this);
	QAction* actionColorLight = new QAction("Color &light", this);
	menuColor->addAction(actionColorSolid);
	menuColor->addAction(actionColorWire);
	menuColor->addAction(actionColorBack);
	menuColor->addAction(actionColorLight);

	QMenu *menuDisplay = menuConfig->addMenu("&Display");
	QActionGroup *actionGroupDisplay = new QActionGroup(this);
	QAction* actionSolid = new QAction("&Solid", this);
	actionSolid->setCheckable(true);
	actionSolid->setChecked(true);
	QAction* actionWithWire = new QAction("&With wire", this);
	actionWithWire->setCheckable(true);
	QAction* actionWireframe = new QAction("Wire&frame", this);
	actionWireframe->setCheckable(true);
	actionGroupDisplay->addAction(actionSolid);
	actionGroupDisplay->addAction(actionWithWire);
	actionGroupDisplay->addAction(actionWireframe);
	menuDisplay->addAction(actionSolid);
	menuDisplay->addAction(actionWithWire);
	menuDisplay->addAction(actionWireframe);

	connect(actionTetraedre, SIGNAL(triggered()), this, SLOT(loadTetraedre()));
	connect(actionOctaedre, SIGNAL(triggered()), this, SLOT(loadOctaedre()));
	connect(actionIcosaedre, SIGNAL(triggered()), this, SLOT(loadIcosaedre()));
	connect(actionTorus, SIGNAL(triggered()), this, SLOT(loadTorus()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));

	connect(actionStart, SIGNAL(triggered()), this, SLOT(startTimer()));
	connect(actionStop, SIGNAL(triggered()), this, SLOT(stopTimer()));
	connect(actionReset, SIGNAL(triggered()), this, SLOT(resetTimer()));

	connect(actionColorSolid, SIGNAL(triggered()), this, SLOT(changeColorSolid()));
	connect(actionColorWire, SIGNAL(triggered()), this, SLOT(changeColorWire()));
	connect(actionColorBack, SIGNAL(triggered()), this, SLOT(changeColorBack()));
	connect(actionColorLight, SIGNAL(triggered()), this, SLOT(changeColorLight()));

	connect(actionSolid, SIGNAL(triggered()), this, SLOT(changeToSolid()));
	connect(actionWithWire, SIGNAL(triggered()), this, SLOT(changeToWithWire()));
	connect(actionWireframe, SIGNAL(triggered()), this, SLOT(changeToWireFrame()));

	connect(&_timer, SIGNAL(timeout()), this, SLOT(timerAction()));
}

MainWindow::~MainWindow()
{
	_openGLComputeGeodesic->destroy();
	delete _openGLComputeGeodesic;
}

void MainWindow::loadTetraedre()
{
	float taille = 1.0f;
	OpenGLComputeGeodesic::t_vertice* vertices = new OpenGLComputeGeodesic::t_vertice[4];
	vertices[0] = OpenGLComputeGeodesic::t_vertice(-taille,-taille,-taille);
	vertices[1] = OpenGLComputeGeodesic::t_vertice(-taille,taille,taille);
	vertices[2] = OpenGLComputeGeodesic::t_vertice(taille,-taille,taille);
	vertices[3] = OpenGLComputeGeodesic::t_vertice(taille,taille,-taille);
	OpenGLComputeGeodesic::t_face* faces = new OpenGLComputeGeodesic::t_face[4];
	faces[0] = OpenGLComputeGeodesic::t_face(0,1,2);
	faces[1] = OpenGLComputeGeodesic::t_face(0,3,1);
	faces[2] = OpenGLComputeGeodesic::t_face(0,2,3);
	faces[3] = OpenGLComputeGeodesic::t_face(1,3,2);

	_openGLComputeGeodesic->setInitialData(4, vertices, 4, faces);

	delete[] vertices;
	delete[] faces;

	_openGLComputeGeodesic->setProgress(0.0f);
	_timer.stop();
}

void MainWindow::loadOctaedre()
{
	float taille = 1.0f;
	OpenGLComputeGeodesic::t_vertice* vertices = new OpenGLComputeGeodesic::t_vertice[6];
	vertices[0] = OpenGLComputeGeodesic::t_vertice(taille,0.0f,0.0f);
	vertices[1] = OpenGLComputeGeodesic::t_vertice(-taille,0.0f,0.0f);
	vertices[2] = OpenGLComputeGeodesic::t_vertice(0.0f,taille,0.0f);
	vertices[3] = OpenGLComputeGeodesic::t_vertice(0.0f,-taille,0.0f);
	vertices[4] = OpenGLComputeGeodesic::t_vertice(0.0f,0.0f,taille);
	vertices[5] = OpenGLComputeGeodesic::t_vertice(0.0f,0.0f,-taille);
	OpenGLComputeGeodesic::t_face* faces = new OpenGLComputeGeodesic::t_face[8];
	faces[0] = OpenGLComputeGeodesic::t_face(0,4,2);
	faces[1] = OpenGLComputeGeodesic::t_face(0,2,5);
	faces[2] = OpenGLComputeGeodesic::t_face(0,3,4);
	faces[3] = OpenGLComputeGeodesic::t_face(0,5,3);
	faces[4] = OpenGLComputeGeodesic::t_face(1,2,4);
	faces[5] = OpenGLComputeGeodesic::t_face(1,5,2);
	faces[6] = OpenGLComputeGeodesic::t_face(1,4,3);
	faces[7] = OpenGLComputeGeodesic::t_face(1,3,5);

	_openGLComputeGeodesic->setInitialData(6, vertices, 8, faces);

	delete[] vertices;
	delete[] faces;

	_openGLComputeGeodesic->setProgress(0.0f);
	_timer.stop();
}

void MainWindow::loadIcosaedre()
{
	const double goldRatio = (1.0+sqrt(5.0))/2.0;
	float taille = 1.0f;
	float taille2 = static_cast<float>(static_cast<double>(taille) * goldRatio);
	OpenGLComputeGeodesic::t_vertice* vertices = new OpenGLComputeGeodesic::t_vertice[12];
	vertices[0] = OpenGLComputeGeodesic::t_vertice(-taille,-taille2,0.0);
	vertices[1] = OpenGLComputeGeodesic::t_vertice(-taille,taille2,0.0);
	vertices[2] = OpenGLComputeGeodesic::t_vertice(taille,-taille2,0.0);
	vertices[3] = OpenGLComputeGeodesic::t_vertice(taille,taille2,0.0);
	vertices[4] = OpenGLComputeGeodesic::t_vertice(-taille2,0.0,-taille);
	vertices[5] = OpenGLComputeGeodesic::t_vertice(taille2,0.0,-taille);
	vertices[6] = OpenGLComputeGeodesic::t_vertice(-taille2,0.0,taille);
	vertices[7] = OpenGLComputeGeodesic::t_vertice(taille2,0.0,taille);
	vertices[8] = OpenGLComputeGeodesic::t_vertice(0.0,-taille,-taille2);
	vertices[9] = OpenGLComputeGeodesic::t_vertice(0.0,-taille,taille2);
	vertices[10] = OpenGLComputeGeodesic::t_vertice(0.0,taille,-taille2);
	vertices[11] = OpenGLComputeGeodesic::t_vertice(0.0,taille,taille2);
	OpenGLComputeGeodesic::t_face* faces = new OpenGLComputeGeodesic::t_face[20];
	faces[0] = OpenGLComputeGeodesic::t_face(0,2,8);
	faces[1] = OpenGLComputeGeodesic::t_face(2,0,9);
	faces[2] = OpenGLComputeGeodesic::t_face(3,1,10);
	faces[3] = OpenGLComputeGeodesic::t_face(1,3,11);
	faces[4] = OpenGLComputeGeodesic::t_face(4,6,0);
	faces[5] = OpenGLComputeGeodesic::t_face(6,4,1);
	faces[6] = OpenGLComputeGeodesic::t_face(7,5,2);
	faces[7] = OpenGLComputeGeodesic::t_face(5,7,3);
	faces[8] = OpenGLComputeGeodesic::t_face(8,10,4);
	faces[9] = OpenGLComputeGeodesic::t_face(10,8,5);
	faces[10] = OpenGLComputeGeodesic::t_face(11,9,6);
	faces[11] = OpenGLComputeGeodesic::t_face(9,11,7);
	faces[12] = OpenGLComputeGeodesic::t_face(3,7,11);
	faces[13] = OpenGLComputeGeodesic::t_face(3,10,5);
	faces[14] = OpenGLComputeGeodesic::t_face(9,7,2);
	faces[15] = OpenGLComputeGeodesic::t_face(6,1,11);
	faces[16] = OpenGLComputeGeodesic::t_face(0,8,4);
	faces[17] = OpenGLComputeGeodesic::t_face(0,6,9);
	faces[18] = OpenGLComputeGeodesic::t_face(5,8,2);
	faces[19] = OpenGLComputeGeodesic::t_face(10,1,4);

	_openGLComputeGeodesic->setInitialData(12, vertices, 20, faces);

	delete[] vertices;
	delete[] faces;

	_openGLComputeGeodesic->setProgress(0.0f);
	_timer.stop();
}


void MainWindow::loadTorus()
{
	const float cos120 = -0.5;
	const float sin120 = static_cast<float>(sqrt(3.0)/2.0);
	float majorRadius = 3.0f;
	float minorRadius = 1.0f;
	float halfEdge = static_cast<float>(static_cast<double>(minorRadius) * 3.0 / sqrt(3.0));
	OpenGLComputeGeodesic::t_vertice* vertices = new OpenGLComputeGeodesic::t_vertice[18];
	vertices[0] = OpenGLComputeGeodesic::t_vertice(majorRadius-minorRadius,0.0,0.0);
	vertices[1] = OpenGLComputeGeodesic::t_vertice(majorRadius+minorRadius,halfEdge,0.0);
	vertices[2] = OpenGLComputeGeodesic::t_vertice(majorRadius+minorRadius,-halfEdge,0.0);
	vertices[3] = OpenGLComputeGeodesic::t_vertice(-(majorRadius+minorRadius),0.0,0.0);
	vertices[4] = OpenGLComputeGeodesic::t_vertice(-(majorRadius-minorRadius),halfEdge,0.0);
	vertices[5] = OpenGLComputeGeodesic::t_vertice(-(majorRadius-minorRadius),-halfEdge,0.0);

	vertices[6] = OpenGLComputeGeodesic::t_vertice((majorRadius-minorRadius)*cos120,0.0,(majorRadius-minorRadius)*sin120);
	vertices[7] = OpenGLComputeGeodesic::t_vertice((majorRadius+minorRadius)*cos120,halfEdge,(majorRadius+minorRadius)*sin120);
	vertices[8] = OpenGLComputeGeodesic::t_vertice((majorRadius+minorRadius)*cos120,-halfEdge,(majorRadius+minorRadius)*sin120);
	vertices[9] = OpenGLComputeGeodesic::t_vertice(-(majorRadius+minorRadius)*cos120,0.0,-(majorRadius+minorRadius)*sin120);
	vertices[10] = OpenGLComputeGeodesic::t_vertice(-(majorRadius-minorRadius)*cos120,halfEdge,-(majorRadius-minorRadius)*sin120);
	vertices[11] = OpenGLComputeGeodesic::t_vertice(-(majorRadius-minorRadius)*cos120,-halfEdge,-(majorRadius-minorRadius)*sin120);

	vertices[12] = OpenGLComputeGeodesic::t_vertice((majorRadius-minorRadius)*cos120,0.0,-(majorRadius-minorRadius)*sin120);
	vertices[13] = OpenGLComputeGeodesic::t_vertice((majorRadius+minorRadius)*cos120,halfEdge,-(majorRadius+minorRadius)*sin120);
	vertices[14] = OpenGLComputeGeodesic::t_vertice((majorRadius+minorRadius)*cos120,-halfEdge,-(majorRadius+minorRadius)*sin120);
	vertices[15] = OpenGLComputeGeodesic::t_vertice(-(majorRadius+minorRadius)*cos120,0.0,(majorRadius+minorRadius)*sin120);
	vertices[16] = OpenGLComputeGeodesic::t_vertice(-(majorRadius-minorRadius)*cos120,halfEdge,(majorRadius-minorRadius)*sin120);
	vertices[17] = OpenGLComputeGeodesic::t_vertice(-(majorRadius-minorRadius)*cos120,-halfEdge,(majorRadius-minorRadius)*sin120);

	OpenGLComputeGeodesic::t_face* faces = new OpenGLComputeGeodesic::t_face[36];
	faces[0] = OpenGLComputeGeodesic::t_face(0,11,10);
	faces[1] = OpenGLComputeGeodesic::t_face(0,10,1);
	faces[2] = OpenGLComputeGeodesic::t_face(0,2,11);
	faces[3] = OpenGLComputeGeodesic::t_face(2,1,9);
	faces[4] = OpenGLComputeGeodesic::t_face(1,10,9);
	faces[5] = OpenGLComputeGeodesic::t_face(2,9,11);

	faces[6] = OpenGLComputeGeodesic::t_face(12,10,11);
	faces[7] = OpenGLComputeGeodesic::t_face(12,13,10);
	faces[8] = OpenGLComputeGeodesic::t_face(12,11,14);
	faces[9] = OpenGLComputeGeodesic::t_face(14,9,13);
	faces[10] = OpenGLComputeGeodesic::t_face(13,9,10);
	faces[11] = OpenGLComputeGeodesic::t_face(14,11,9);

	faces[12] = OpenGLComputeGeodesic::t_face(12,5,4);
	faces[13] = OpenGLComputeGeodesic::t_face(12,4,13);
	faces[14] = OpenGLComputeGeodesic::t_face(12,14,5);
	faces[15] = OpenGLComputeGeodesic::t_face(14,13,3);
	faces[16] = OpenGLComputeGeodesic::t_face(13,4,3);
	faces[17] = OpenGLComputeGeodesic::t_face(14,3,5);

	faces[18] = OpenGLComputeGeodesic::t_face(6,4,5);
	faces[19] = OpenGLComputeGeodesic::t_face(6,7,4);
	faces[20] = OpenGLComputeGeodesic::t_face(6,5,8);
	faces[21] = OpenGLComputeGeodesic::t_face(8,3,7);
	faces[22] = OpenGLComputeGeodesic::t_face(7,3,4);
	faces[23] = OpenGLComputeGeodesic::t_face(8,5,3);

	faces[24] = OpenGLComputeGeodesic::t_face(6,17,16);
	faces[25] = OpenGLComputeGeodesic::t_face(6,16,7);
	faces[26] = OpenGLComputeGeodesic::t_face(6,8,17);
	faces[27] = OpenGLComputeGeodesic::t_face(8,7,15);
	faces[28] = OpenGLComputeGeodesic::t_face(7,16,15);
	faces[29] = OpenGLComputeGeodesic::t_face(8,15,17);

	faces[30] = OpenGLComputeGeodesic::t_face(0,16,17);
	faces[31] = OpenGLComputeGeodesic::t_face(0,1,16);
	faces[32] = OpenGLComputeGeodesic::t_face(0,17,2);
	faces[33] = OpenGLComputeGeodesic::t_face(2,15,1);
	faces[34] = OpenGLComputeGeodesic::t_face(1,15,16);
	faces[35] = OpenGLComputeGeodesic::t_face(2,17,15);

	_openGLComputeGeodesic->setInitialData(18, vertices, 36, faces);

	delete[] vertices;
	delete[] faces;

	_openGLComputeGeodesic->setProgress(0.0f);
	_timer.stop();
}

void MainWindow::changeColorSolid()
{
	QColor color = QColorDialog::getColor(_openGLDisplay->colorSolid(), this, "New solid color");
	if (color.isValid())
	{
		_openGLDisplay->setColorSolid(color);
	}
}

void MainWindow::changeColorWire()
{
	QColor color = QColorDialog::getColor(_openGLDisplay->colorWire(), this, "New wire color");
	if (color.isValid())
	{
		_openGLDisplay->setColorWire(color);
	}
}

void MainWindow::changeColorBack()
{
	QColor color = QColorDialog::getColor(_openGLDisplay->colorBack(), this, "New background color");
	if (color.isValid())
	{
		_openGLDisplay->setColorBack(color);
	}
}

void MainWindow::changeColorLight()
{
	QColor color = QColorDialog::getColor(_openGLDisplay->colorLight(), this, "New light color");
	if (color.isValid())
	{
		_openGLDisplay->setColorLight(color);
	}
}

void MainWindow::changeToSolid()
{
	_openGLDisplay->setDisplayType(OpenGLDisplay::Solid);
}

void MainWindow::changeToWithWire()
{
	_openGLDisplay->setDisplayType(OpenGLDisplay::WithWire);
}

void MainWindow::changeToWireFrame()
{
	_openGLDisplay->setDisplayType(OpenGLDisplay::WireFrame);
}

void MainWindow::startTimer()
{
	if (_openGLComputeGeodesic == nullptr)
	{
		return;
	}
	_timer.start(200);
}

void MainWindow::stopTimer()
{
	if (_openGLComputeGeodesic == nullptr)
	{
		return;
	}
	_timer.stop();
}

void MainWindow::resetTimer()
{
	if (_openGLComputeGeodesic == nullptr)
	{
		return;
	}
	_openGLComputeGeodesic->setProgress(0.0f);
	_openGLComputeGeodesic->reset();
	_timer.stop();
}

void MainWindow::timerAction()
{
	if (_openGLComputeGeodesic == nullptr)
	{
		return;
	}
	float progress = _openGLComputeGeodesic->progress();
	unsigned int nbSwitch = _openGLComputeGeodesic->nbSwitch();

	if (nbSwitch >= 5)
	{
		_timer.stop();
		return;
	}
    progress += 0.1f;
	if (progress > 1.5f)
	{
		_openGLComputeGeodesic->setProgress(0.0f);
		_openGLComputeGeodesic->startComputeNext();
	}
	else
	{
		_openGLComputeGeodesic->setProgress(progress);
		_openGLComputeGeodesic->startCompute();
	}
}
