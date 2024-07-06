#include "openglcomputegeodesic.h"

#include <iostream>

#define UNIFORM_NB_VERTICES   "u_nbVertices"
#define UNIFORM_NB_EDGES      "u_nbEdges"
#define UNIFORM_CENTER        "u_center"
#define UNIFORM_PROGRESS      "u_progress"
#define UNIFORM_COMPUTE_STEP  "u_computeStep"

OpenGLComputeGeodesic::OpenGLComputeGeodesic()
	: OpenGLCompute(), _flipIn(true), _computed(false),
	  _nbVerticesInit(0), _verticesInit(nullptr),
	  _nbEdgesInit(0), _edgesInit(nullptr),
	  _nbFacesInit(0), _facesInit(nullptr), _facesOutInit(nullptr),
	  _nbVerticesCurrent(0), _nbEdgesCurrent(0), _nbFacesCurrent(0),
	  _center(), _progress(0.0), _nbSwitch(0)

{
	connect(this, SIGNAL(startInitialization()), this, SLOT(initializeBuffer()));
	connect(this, SIGNAL(startReadVertices(t_vertice*)), this, SLOT(readVertices(t_vertice*)));
	connect(this, SIGNAL(startReadFaces(t_face*)), this, SLOT(readFaces(t_face*)));
}

OpenGLComputeGeodesic::~OpenGLComputeGeodesic()
{
	if (_verticesInit != nullptr)
	{
		delete[] _verticesInit;
		_verticesInit = nullptr;
	}
	if (_edgesInit != nullptr)
	{
		delete[] _edgesInit;
		_edgesInit = nullptr;
	}
	if (_facesInit != nullptr)
	{
		delete[] _facesInit;
		_facesInit = nullptr;
	}
	if (_facesOutInit != nullptr)
	{
		delete[] _facesOutInit;
		_facesOutInit = nullptr;
	}
}

void OpenGLComputeGeodesic::cleanup()
{
	if (_programCompute == nullptr)
	{
		return;
	}
	delete _vboVerticesFlip;
	delete _vboVerticesFlop;
	delete _vboEdgesFlip;
	delete _vboEdgesFlop;
	delete _vboFacesFlip;
	delete _vboFacesFlop;
	delete _vboFacesVertOut;
	delete _vaoCompute;
	delete _programCompute;
	_programCompute = nullptr;
}

void OpenGLComputeGeodesic::initializeGL()
{
	_oFct.initializeOpenGLFunctions();

	_programCompute = new QOpenGLShaderProgram;
	_programCompute->addShaderFromSourceFile(QOpenGLShader::Compute, ":/shader.comp");
	_programCompute->link();
	_programCompute->bind();

	_vaoCompute = new QOpenGLVertexArrayObject;
	_vaoCompute->create();
	_vaoCompute->bind();

	_vboVerticesFlip = new QOpenGLBuffer;
	_vboVerticesFlip->create();

	_vboVerticesFlop = new QOpenGLBuffer;
	_vboVerticesFlop->create();

	_vboEdgesFlip = new QOpenGLBuffer;
	_vboEdgesFlip->create();

	_vboEdgesFlop = new QOpenGLBuffer;
	_vboEdgesFlop->create();

	_vboFacesFlip = new QOpenGLBuffer;
	_vboFacesFlip->create();

	_vboFacesFlop = new QOpenGLBuffer;
	_vboFacesFlop->create();

	_vboFacesVertOut = new QOpenGLBuffer;
	_vboFacesVertOut->create();

	_vaoCompute->release();
	_programCompute->release();
}

void OpenGLComputeGeodesic::setInitialData(unsigned int nbVertices, t_vertice* vertices, unsigned int nbFaces, t_face* faces)
{
	if ((vertices == nullptr) || (faces == nullptr))
	{
		return;
	}
	_mutexCompute.lock();
	if (_verticesInit != nullptr)
	{
		delete[] _verticesInit;
		_verticesInit = nullptr;
	}
	if (_edgesInit != nullptr)
	{
		delete[] _edgesInit;
		_edgesInit = nullptr;
	}
	if (_facesInit != nullptr)
	{
		delete[] _facesInit;
		_facesInit = nullptr;
	}
	if (_facesOutInit != nullptr)
	{
		delete[] _facesOutInit;
		_facesOutInit = nullptr;
	}
	_nbVerticesInit = 0;
	_nbVerticesCurrent = 0;
	_nbEdgesInit = 0;
	_nbEdgesCurrent = 0;
	_nbFacesInit = 0;
	_nbFacesCurrent = 0;

	_verticesInit = new t_vertice[nbVertices];
	memcpy(_verticesInit, vertices, nbVertices * sizeof(t_vertice));
	_nbVerticesInit = nbVertices;

	_facesOutInit = new t_face[nbFaces];
	memcpy(_facesOutInit, faces, nbFaces * sizeof(t_face));

	_facesInit = new t_face[nbFaces];
	// calcul des arrets a partir des faces
	QVector<t_edge> lEdges;
	for (unsigned int i = 0; i < nbFaces; i++)
	{
		t_edge e1(faces[i].v2, faces[i].v3);
		int i1 = lEdges.indexOf(e1);
		if (i1 >= 0)
		{
			_facesInit[i].v1 = static_cast<unsigned int>(i1);
		}
		else
		{
			_facesInit[i].v1 = static_cast<unsigned int>(lEdges.size());
			lEdges.append(e1);
		}
		t_edge e2(faces[i].v3, faces[i].v1);
		int i2 = lEdges.indexOf(e2);
		if (i2 >= 0)
		{
			_facesInit[i].v2 = static_cast<unsigned int>(i2);
		}
		else
		{
			_facesInit[i].v2 = static_cast<unsigned int>(lEdges.size());
			lEdges.append(e2);
		}
		t_edge e3(faces[i].v1, faces[i].v2);
		int i3 = lEdges.indexOf(e3);
		if (i3 >= 0)
		{
			_facesInit[i].v3 = static_cast<unsigned int>(i3);
		}
		else
		{
			_facesInit[i].v3 = static_cast<unsigned int>(lEdges.size());
			lEdges.append(e3);
		}
	}
	_nbFacesInit = nbFaces;
	_nbEdgesInit = static_cast<unsigned int>(lEdges.size());
	_edgesInit = new t_edge[_nbEdgesInit];
	memcpy(_edgesInit, lEdges.data(), static_cast<unsigned int>(lEdges.size()) * sizeof(t_edge));

	_mutexCompute.unlock();
	emit startInitialization();
}

void OpenGLComputeGeodesic::reset()
{
	emit startInitialization();
}

void OpenGLComputeGeodesic::startComputeNext()
{
	if (_computed)
	{
		_mutexCompute.lock();
		_nbSwitch++;
		_flipIn = !_flipIn;
		_nbVerticesCurrent = nbVerticesNext();
		_nbEdgesCurrent = nbEdgesNext();
		_nbFacesCurrent = nbFacesNext();
		_computed = false;
		_mutexCompute.unlock();
	}
	startCompute();
}

void OpenGLComputeGeodesic::initializeBuffer()
{
	_mutexCompute.lock();
	_flipIn = true;
	_computed = false;
	_nbSwitch = 0;
	_nbFacesCurrent = _nbFacesInit;
	_nbEdgesCurrent = _nbEdgesInit;
	_nbVerticesCurrent = _nbVerticesInit;

	_programCompute->bind();
	_vaoCompute->bind();

	vboVerticesIn()->bind();
	vboVerticesIn()->allocate(static_cast<int>(_nbVerticesInit*sizeof(t_vertice)));
	vboVerticesIn()->write(0, _verticesInit, static_cast<int>(_nbVerticesInit*sizeof(t_vertice)));
	vboVerticesIn()->release();

	vboEdgesIn()->bind();
	vboEdgesIn()->allocate(static_cast<int>(_nbEdgesInit*sizeof(t_edge)));
	vboEdgesIn()->write(0, _edgesInit, static_cast<int>(_nbEdgesInit*sizeof(t_edge)));
	vboEdgesIn()->release();

	vboFacesIn()->bind();
	vboFacesIn()->allocate(static_cast<int>(_nbFacesInit*sizeof(t_face)));
	vboFacesIn()->write(0, _facesInit, static_cast<int>(_nbFacesInit*sizeof(t_face)));
	vboFacesIn()->release();

	_vaoCompute->release();
	_programCompute->release();
	_mutexCompute.unlock();
	emit computeEnd();
}

QOpenGLBuffer* OpenGLComputeGeodesic::vboVerticesIn()
{
	if (_flipIn)
	{
		return _vboVerticesFlip;
	}
	else
	{
		return _vboVerticesFlop;
	}
}

QOpenGLBuffer* OpenGLComputeGeodesic::vboVerticesOut()
{
	if (_flipIn)
	{
		return _vboVerticesFlop;
	}
	else
	{
		return _vboVerticesFlip;
	}
}

QOpenGLBuffer* OpenGLComputeGeodesic::vboEdgesIn()
{
	if (_flipIn)
	{
		return _vboEdgesFlip;
	}
	else
	{
		return _vboEdgesFlop;
	}
}

QOpenGLBuffer* OpenGLComputeGeodesic::vboEdgesOut()
{
	if (_flipIn)
	{
		return _vboEdgesFlop;
	}
	else
	{
		return _vboEdgesFlip;
	}
}

QOpenGLBuffer* OpenGLComputeGeodesic::vboFacesIn()
{
	if (_flipIn)
	{
		return _vboFacesFlip;
	}
	else
	{
		return _vboFacesFlop;
	}
}

QOpenGLBuffer* OpenGLComputeGeodesic::vboFacesOut()
{
	if (_flipIn)
	{
		return _vboFacesFlop;
	}
	else
	{
		return _vboFacesFlip;
	}
}

void OpenGLComputeGeodesic::compute()
{
	_mutexCompute.lock();
	_programCompute->bind();
	_vaoCompute->bind();

	// Prepare out buffer
	vboVerticesOut()->bind();
	vboVerticesOut()->allocate(static_cast<int>(nbVerticesNext()*sizeof(t_vertice)));
	vboVerticesOut()->release();
	_oFct.glBindBuffer(GL_COPY_READ_BUFFER, vboVerticesIn()->bufferId());
	_oFct.glBindBuffer(GL_COPY_WRITE_BUFFER, vboVerticesOut()->bufferId());
	_oFct.glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, static_cast<int>(_nbVerticesCurrent*sizeof(t_vertice)));

	vboEdgesOut()->bind();
	vboEdgesOut()->allocate(static_cast<int>(nbEdgesNext()*sizeof(t_edge)));
	vboEdgesOut()->release();

	vboFacesOut()->bind();
	vboFacesOut()->allocate(static_cast<int>(nbFacesNext()*sizeof(t_face)));
	vboFacesOut()->release();

	_vboFacesVertOut->bind();
	_vboFacesVertOut->allocate(static_cast<int>(nbFacesNext()*sizeof(t_face)));
	_vboFacesVertOut->release();

	// Bind buffer
	_oFct.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vboVerticesOut()->bufferId());
	_oFct.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vboEdgesIn()->bufferId());
	_oFct.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vboFacesIn()->bufferId());
	_oFct.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, vboEdgesOut()->bufferId());
	_oFct.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, vboFacesOut()->bufferId());
	_oFct.glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, _vboFacesVertOut->bufferId());

	// Set uniform
	_programCompute->setUniformValue(UNIFORM_NB_VERTICES, _nbVerticesCurrent);
	_programCompute->setUniformValue(UNIFORM_NB_EDGES,    _nbEdgesCurrent);
	_programCompute->setUniformValue(UNIFORM_CENTER,      _center.toVector());
	_programCompute->setUniformValue(UNIFORM_PROGRESS,    _progress);

	// Compute step 0 and 1
	_programCompute->setUniformValue(UNIFORM_COMPUTE_STEP, 0);
	_oFct.glDispatchCompute(_nbEdgesCurrent,1,1);
	_oFct.glFinish();
	_programCompute->setUniformValue(UNIFORM_COMPUTE_STEP, 1);
	_oFct.glDispatchCompute(_nbFacesCurrent,1,1);
	_oFct.glFinish();
	_vaoCompute->release();
	_programCompute->release();
	_computed = true;
	_mutexCompute.unlock();
	emit computeEnd();
}

unsigned int OpenGLComputeGeodesic::nbVerticesOut()
{
	if (_computed)
	{
		return nbVerticesNext();
	}
	else
	{
		return _nbVerticesInit;
	}
}

unsigned int OpenGLComputeGeodesic::nbFacesOut()
{
	if (_computed)
	{
		return nbFacesNext();
	}
	else
	{
		return _nbFacesInit;
	}
}

unsigned int OpenGLComputeGeodesic::nbVerticesNext()
{
	return _nbVerticesCurrent + _nbEdgesCurrent;
}

unsigned int OpenGLComputeGeodesic::nbEdgesNext()
{
	return 2*_nbEdgesCurrent + 3*_nbFacesCurrent;
}

unsigned int OpenGLComputeGeodesic::nbFacesNext()
{
	return 4*_nbFacesCurrent;
}

bool OpenGLComputeGeodesic::verticesOut(t_vertice *vertices)
{
	if (vertices == nullptr)
	{
		return false;
	}
	bool result = true;
	if (_computed)
	{
		_mutexReadData.lock();
		emit startReadVertices(vertices);
		_waitReadData.wait(&_mutexReadData);
		_mutexReadData.unlock();
		result = _readResult;
	}
	else
	{
		memcpy(vertices, _verticesInit, _nbVerticesInit*sizeof(t_vertice));
	}
	return result;
}

bool OpenGLComputeGeodesic::facesOut(t_face *faces)
{
	if (faces == nullptr)
	{
		return false;
	}
	bool result = true;
	if (_computed)
	{
		_mutexReadData.lock();
		emit startReadFaces(faces);
		_waitReadData.wait(&_mutexReadData);
		_mutexReadData.unlock();
		result = _readResult;
	}
	else
	{
		memcpy(faces, _facesOutInit, _nbFacesInit*sizeof(t_face));
	}
	return result;
}

void OpenGLComputeGeodesic::readVertices(t_vertice *vertices)
{
	_mutexCompute.lock();
	_programCompute->bind();
	vboVerticesOut()->bind();
	_readResult = vboVerticesOut()->read(0, vertices, static_cast<int>(nbVerticesOut()*sizeof(t_vertice)));
	vboVerticesOut()->release();
	_programCompute->release();
	_mutexCompute.unlock();

	_mutexReadData.lock();
	_waitReadData.wakeOne();
	_mutexReadData.unlock();
}

void OpenGLComputeGeodesic::readFaces(t_face *faces)
{
	_mutexCompute.lock();
	_programCompute->bind();
	_vboFacesVertOut->bind();
	_readResult = _vboFacesVertOut->read(0, faces, static_cast<int>(nbFacesOut()*sizeof(t_face)));
	_vboFacesVertOut->release();
	_programCompute->release();
	_mutexCompute.unlock();

	_mutexReadData.lock();
	_waitReadData.wakeOne();
	_mutexReadData.unlock();
}
