#ifndef OPENGLCOMPUTEGEODESIC_H
#define OPENGLCOMPUTEGEODESIC_H

#include "openglcompute.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <QMutex>
#include <QWaitCondition>

class OpenGLComputeGeodesic : public OpenGLCompute
{
	Q_OBJECT
public:
	OpenGLComputeGeodesic();
	virtual ~OpenGLComputeGeodesic() override;

	void initializeGL() override;
	void cleanup() override;

	struct t_vertice {
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat w;
		t_vertice() : x(0.0), y(0.0), z(0.0), w(0.0) {}
		t_vertice(float px, float py, float pz) : x(px), y(py), z(pz), w(0.0) {}
		QVector4D toVector() {return QVector4D(w,y,z,w);}
		static unsigned int nbElements() {return sizeof(t_vertice)/sizeof(GLfloat);}
	};

	struct t_face {
		GLuint v1;
		GLuint v2;
		GLuint v3;
		t_face() : v1(0), v2(0), v3(0) {}
		t_face(unsigned int pv1, unsigned int pv2, unsigned int pv3) : v1(pv1), v2(pv2), v3(pv3) {}
		static unsigned int nbElements() {return sizeof(t_face)/sizeof(GLuint);}
	};

	void setInitialData(unsigned int nbVertices, t_vertice *vertices,
						unsigned int nbFaces, t_face *faces);

	void setCenter(t_vertice &center) {_center = center;}
	void setProgress(float progress) {_progress = progress;}

	t_vertice center() {return _center;}
	GLfloat progress() {return _progress;}

	unsigned int nbVerticesOut();
	unsigned int nbFacesOut();
	unsigned int nbSwitch() {return _nbSwitch;}

	bool verticesOut(t_vertice *vertices);
	bool facesOut(t_face *faces);

signals:
	void startInitialization();
	void computeEnd();
	void startReadVertices(t_vertice *vertices);
	void startReadFaces(t_face *faces);

public slots:
	void startComputeNext();
	void reset();

protected slots:
	void compute() override;

private slots:
	void initializeBuffer();
	void readVertices(t_vertice *vertices);
	void readFaces(t_face *faces);

private:
	struct t_edge {
		GLuint v1;
		GLuint v2;
		t_edge() : v1(0), v2(0) {}
		t_edge(unsigned int pv1, unsigned int pv2) : v1(pv1), v2(pv2) {}
        inline bool operator==(const t_edge& other) const
		{
			return (((this->v1==other.v1) && (this->v2==other.v2)) ||
					((this->v1==other.v2) && (this->v2==other.v1)));
		}
		static unsigned int nbElements() {return sizeof(t_edge)/sizeof(GLuint);}
	};

	unsigned int nbVerticesNext();
	unsigned int nbEdgesNext();
	unsigned int nbFacesNext();

	QOpenGLBuffer* vboVerticesIn();
	QOpenGLBuffer* vboVerticesOut();
	QOpenGLBuffer* vboEdgesIn();
	QOpenGLBuffer* vboEdgesOut();
	QOpenGLBuffer* vboFacesIn();
	QOpenGLBuffer* vboFacesOut();

	QMutex                    _mutexCompute;

	QOpenGLExtraFunctions     _oFct;
	QOpenGLShaderProgram*     _programCompute;
	QOpenGLBuffer*            _vboVerticesFlip;
	QOpenGLBuffer*            _vboVerticesFlop;
	QOpenGLBuffer*            _vboEdgesFlip;
	QOpenGLBuffer*            _vboEdgesFlop;
	QOpenGLBuffer*            _vboFacesFlip;
	QOpenGLBuffer*            _vboFacesFlop;
	QOpenGLBuffer*            _vboFacesVertOut;
	QOpenGLVertexArrayObject* _vaoCompute;

	bool                      _flipIn;
	bool                      _computed;

	unsigned int              _nbVerticesInit;
	t_vertice*                _verticesInit;

	unsigned int              _nbEdgesInit;
	t_edge*                   _edgesInit;

	unsigned int              _nbFacesInit;
	t_face*                   _facesInit;
	t_face*                   _facesOutInit;

	unsigned int              _nbVerticesCurrent;
	unsigned int              _nbEdgesCurrent;
	unsigned int              _nbFacesCurrent;

	t_vertice                 _center;
	GLfloat                   _progress;
	unsigned int              _nbSwitch;

	QMutex                    _mutexReadData;
	QWaitCondition            _waitReadData;
	bool                      _readResult;
};

#endif // OPENGLCOMPUTEGEODESIC_H
