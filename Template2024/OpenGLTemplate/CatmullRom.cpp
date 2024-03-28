#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCountCentre = 0;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	m_controlPoints.push_back(glm::vec3(71, 5, 71));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(-71, 5, 71));
	m_controlPoints.push_back(glm::vec3(-100, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 5, -71));
	m_controlPoints.push_back(glm::vec3(0, 5, -100));
	m_controlPoints.push_back(glm::vec3(71, 5, -71));
	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up,glm::vec3 &forward)
{
	if (d < 0)
		return false;

	int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size()-1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if(m_controlTangentVectors.size() == m_controlPoints.size())
	forward = glm::normalize(Interpolate(m_controlTangentVectors[iPrev], m_controlTangentVectors[iCur], m_controlTangentVectors[iNext], m_controlTangentVectors[iNextNext], t));
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;
	glm::vec3 nextP, nextup;
	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		
		Sample(i * fSpacing, p, up);
		Sample((i + 1)* fSpacing, nextP, nextup);
		m_centrelinePoints.push_back(p);
		m_centrelineTangentVectors.push_back(glm::normalize(nextP - p));
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_controlTangentVectors = m_centrelineTangentVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_centrelineTangentVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		Sample((i + 1) * fSpacing, nextP, nextup);
		m_centrelinePoints.push_back(p);
		m_centrelineTangentVectors.push_back(glm::normalize(nextP - p));
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();
	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(500);
	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	m_vboCentreLine.Create();
	m_vboCentreLine.Bind();

	for (unsigned int i = 0; i < m_centrelinePoints.size(); i++)
	{
		glm::vec3 normal = glm::vec3(0, 1, 0);
		glm::vec2 texture_pos = glm::vec2(0, 0);
		m_vboCentreLine.AddVertexData(&m_centrelinePoints[i], sizeof(glm::vec3));
		m_vboCentreLine.AddVertexData(&texture_pos, sizeof(glm::vec2));
		m_vboCentreLine.AddVertexData(&normal, sizeof(glm::vec3));
		m_vboCentreLine.AddIndexData(&i, sizeof(unsigned int));
		m_vertexCountCentre++;
	}
	m_vboCentreLine.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}


void CCatmullRom::CreateOffsetCurves()
{
	SetControlPoints();
	UniformlySampleControlPoints(500);
	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		glm::vec3 normal = glm::cross(m_centrelineUpVectors[i], m_centrelineTangentVectors[i]);
		m_leftOffsetPoints.push_back(m_centrelinePoints[i] + normal * 10.f);
		m_rightOffsetPoints.push_back(m_centrelinePoints[i] - normal * 10.f);
	}

	
	
}


void CCatmullRom::CreateTrack()
{
	CreateOffsetCurves();
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	m_vboLeftOffsetLine.Create();
	m_vboLeftOffsetLine.Bind();
	for (int i = 0; i < m_leftOffsetPoints.size(); i++)
	{
		glm::vec3 p0, p1, p2, p3, p4, p5, p6, p7;
		glm::vec2 t0, t1, t2, t3, t4, t5, t6, t7;
		glm::vec3 n01, n12, n23, n30, n45, n56, n67, n74;

		glm::vec3 tan0, norm0, binorm0;
		glm::vec3 tan1, norm1, binorm1;
		tan0 = m_centrelineTangentVectors[i];
		tan1 = m_centrelineTangentVectors[(i + 1)% m_leftOffsetPoints.size()];
		norm0 = glm::cross(m_centrelineTangentVectors[i], m_centrelineUpVectors[i]);
		norm1 = glm::cross(m_centrelineTangentVectors[(i + 1) % m_leftOffsetPoints.size()], m_centrelineUpVectors[(i + 1) % m_leftOffsetPoints.size()]);
		binorm0 = glm::cross(norm0, tan0);
		binorm1 = glm::cross(norm1, tan1);
		tan0 = glm::normalize(tan0);
		tan1 = glm::normalize(tan1);
		norm0 = glm::normalize(norm0);
		norm1 = glm::normalize(norm1);
		binorm0 = glm::normalize(binorm0);
		binorm1 = glm::normalize(binorm1);

		p0 = m_leftOffsetPoints[i] + (norm0 * thickness) + (binorm0 * thickness);
		p1 = m_leftOffsetPoints[i] - (norm0 * thickness) + (binorm0 * thickness);
		p2 = m_leftOffsetPoints[i] - (norm0 * thickness) - (binorm0 * thickness);
		p3 = m_leftOffsetPoints[i] + (norm0 * thickness) - (binorm0 * thickness);

		p4 = m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] + (norm1 * thickness) + (binorm1 * thickness);
		p5 = m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] - (norm1 * thickness) + (binorm1 * thickness);
		p6 = m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] - (norm1 * thickness) - (binorm1 * thickness);
		p7 = m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] + (norm1 * thickness) - (binorm1 * thickness);

		n01 = binorm0;
		n12 = -norm0;
		n23 = -binorm0;
		n30 = norm0;

		n45 = binorm1;
		n56 = -norm1;
		n67 = -binorm1;
		n74 = norm1;

		t0 = glm::vec2(0.f, i * tiling / m_leftOffsetPoints.size());
		t1 = glm::vec2(0.33f, i * tiling / m_leftOffsetPoints.size());
		t2 = glm::vec2(0.66f, i * tiling / m_leftOffsetPoints.size());
		t3 = glm::vec2(1.f, i * tiling / m_leftOffsetPoints.size());

		t4 = glm::vec2(0.f, (i+1) * tiling / m_leftOffsetPoints.size());
		t5 = glm::vec2(0.33f, (i+1) * tiling / m_leftOffsetPoints.size());
		t6 = glm::vec2(0.66f, (i + 1) * tiling / m_leftOffsetPoints.size());
		t7 = glm::vec2(1.f, (i + 1) * tiling / m_leftOffsetPoints.size());
		

		//upload to vbo;
		CVertex cv0, cv1, cv2, cv3;
		//rectangle 0145
		cv0.position = p0;
		cv0.texture = t0;
		cv0.normal = n01;

		cv1.position = p1;
		cv1.texture = t1;
		cv1.normal = n01;

		cv2.position = p4;
		cv2.texture = t4;
		cv2.normal = n45;

		cv3.position = p5;
		cv3.texture = t5;
		cv3.normal = n45;

		m_vboLeftOffsetLine.AddVertexData(&cv0, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv1, sizeof(CVertex));

		m_vboLeftOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv3, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexLeft += 6;
		//rectangle1256

		cv0.position = p1;
		cv0.texture = t1;
		cv0.normal = n12;

		cv1.position = p2;
		cv1.texture = t2;
		cv1.normal = n12;

		cv2.position = p5;
		cv2.texture = t5;
		cv2.normal = n56;

		cv3.position = p6;
		cv3.texture = t6;
		cv3.normal = n56;

		m_vboLeftOffsetLine.AddVertexData(&cv0, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv1, sizeof(CVertex));

		m_vboLeftOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv3, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexLeft += 6;
		//rectangle2367

		cv0.position = p2;
		cv0.texture = t2;
		cv0.normal = n23;

		cv1.position = p3;
		cv1.texture = t3;
		cv1.normal = n23;

		cv2.position = p6;
		cv2.texture = t6;
		cv2.normal = n67;

		cv3.position = p7;
		cv3.texture = t7;
		cv3.normal = n67;

		m_vboLeftOffsetLine.AddVertexData(&cv0, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv1, sizeof(CVertex));

		m_vboLeftOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv3, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexLeft += 6;
		//rectangle3174

		cv0.position = p3;
		cv0.texture = t3;
		cv0.normal = n30;

		cv1.position = p0;
		cv1.texture = t0;
		cv1.normal = n30;

		cv2.position = p7;
		cv2.texture = t7;
		cv2.normal = n74;

		cv3.position = p4;
		cv3.texture = t4;
		cv3.normal = n74;

		m_vboLeftOffsetLine.AddVertexData(&cv0, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv1, sizeof(CVertex));

		m_vboLeftOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv3, sizeof(CVertex));
		m_vboLeftOffsetLine.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexLeft += 6;
	}
	for (unsigned int i = 0; i < m_vertexLeft; i++)
	{
		m_vboLeftOffsetLine.AddIndexData(&i, sizeof(unsigned int));
	}
	m_vboLeftOffsetLine.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card




	//Right offset cureve

	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);
	m_vboRightOffsetLine.Create();
	m_vboRightOffsetLine.Bind();
	for (int i = 0; i < m_rightOffsetPoints.size(); i++)
	{
		glm::vec3 p0, p1, p2, p3, p4, p5, p6, p7;
		glm::vec2 t0, t1, t2, t3, t4, t5, t6, t7;
		glm::vec3 n01, n12, n23, n30, n45, n56, n67, n74;

		glm::vec3 tan0, norm0, binorm0;
		glm::vec3 tan1, norm1, binorm1;
		tan0 = m_centrelineTangentVectors[i];
		tan1 = m_centrelineTangentVectors[(i + 1) % m_leftOffsetPoints.size()];
		norm0 = glm::cross(m_centrelineTangentVectors[i], m_centrelineUpVectors[i]);
		norm1 = glm::cross(m_centrelineTangentVectors[(i + 1) % m_leftOffsetPoints.size()], m_centrelineUpVectors[(i + 1) % m_leftOffsetPoints.size()]);
		binorm0 = glm::cross(norm0, tan0);
		binorm1 = glm::cross(norm1, tan1);
		tan0 = glm::normalize(tan0);
		tan1 = glm::normalize(tan1);
		norm0 = glm::normalize(norm0);
		norm1 = glm::normalize(norm1);
		binorm0 = glm::normalize(binorm0);
		binorm1 = glm::normalize(binorm1);

		p0 = m_rightOffsetPoints[i] + (norm0 * thickness) + (binorm0 * thickness);
		p1 = m_rightOffsetPoints[i] - (norm0 * thickness) + (binorm0 * thickness);
		p2 = m_rightOffsetPoints[i] - (norm0 * thickness) - (binorm0 * thickness);
		p3 = m_rightOffsetPoints[i] + (norm0 * thickness) - (binorm0 * thickness);

		p4 = m_rightOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] + (norm1 * thickness) + (binorm1 * thickness);
		p5 = m_rightOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] - (norm1 * thickness) + (binorm1 * thickness);
		p6 = m_rightOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] - (norm1 * thickness) - (binorm1 * thickness);
		p7 = m_rightOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] + (norm1 * thickness) - (binorm1 * thickness);

		n01 = binorm0;
		n12 = -norm0;
		n23 = -binorm0;
		n30 = norm0;

		n45 = binorm1;
		n56 = -norm1;
		n67 = -binorm1;
		n74 = norm1;

		t0 = glm::vec2(0.f, i * tiling / m_rightOffsetPoints.size());
		t1 = glm::vec2(0.33f, i * tiling / m_rightOffsetPoints.size());
		t2 = glm::vec2(0.66f, i * tiling / m_rightOffsetPoints.size());
		t3 = glm::vec2(1.f, i * tiling / m_rightOffsetPoints.size());

		t4 = glm::vec2(0.f, (i + 1) * tiling / m_rightOffsetPoints.size());
		t5 = glm::vec2(0.33f, (i + 1) * tiling / m_rightOffsetPoints.size());
		t6 = glm::vec2(0.66f, (i + 1) * tiling / m_rightOffsetPoints.size());
		t7 = glm::vec2(1.f, (i + 1) * tiling / m_rightOffsetPoints.size());


		//upload to vbo;
		CVertex cv0, cv1, cv2, cv3;
		//rectangle 0145
		cv0.position = p0;
		cv0.texture = t0;
		cv0.normal = n01;

		cv1.position = p1;
		cv1.texture = t1;
		cv1.normal = n01;

		cv2.position = p4;
		cv2.texture = t4;
		cv2.normal = n45;

		cv3.position = p5;
		cv3.texture = t5;
		cv3.normal = n45;

		m_vboRightOffsetLine.AddVertexData(&cv0, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv1, sizeof(CVertex));

		m_vboRightOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv3, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexRight += 6;
		//rectangle1256

		cv0.position = p1;
		cv0.texture = t1;
		cv0.normal = n12;

		cv1.position = p2;
		cv1.texture = t2;
		cv1.normal = n12;

		cv2.position = p5;
		cv2.texture = t5;
		cv2.normal = n56;

		cv3.position = p6;
		cv3.texture = t6;
		cv3.normal = n56;

		m_vboRightOffsetLine.AddVertexData(&cv0, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv1, sizeof(CVertex));

		m_vboRightOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv3, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexRight += 6;
		//rectangle2367

		cv0.position = p2;
		cv0.texture = t2;
		cv0.normal = n23;

		cv1.position = p3;
		cv1.texture = t3;
		cv1.normal = n23;

		cv2.position = p6;
		cv2.texture = t6;
		cv2.normal = n67;

		cv3.position = p7;
		cv3.texture = t7;
		cv3.normal = n67;

		m_vboRightOffsetLine.AddVertexData(&cv0, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv1, sizeof(CVertex));

		m_vboRightOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv3, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexRight += 6;
		//rectangle3174

		cv0.position = p3;
		cv0.texture = t3;
		cv0.normal = n30;

		cv1.position = p0;
		cv1.texture = t0;
		cv1.normal = n30;

		cv2.position = p7;
		cv2.texture = t7;
		cv2.normal = n74;

		cv3.position = p4;
		cv3.texture = t4;
		cv3.normal = n74;

		m_vboRightOffsetLine.AddVertexData(&cv0, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv1, sizeof(CVertex));

		m_vboRightOffsetLine.AddVertexData(&cv2, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv3, sizeof(CVertex));
		m_vboRightOffsetLine.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexRight += 6;
	}
	for (unsigned int i = 0; i < m_vertexLeft; i++)
	{
		m_vboRightOffsetLine.AddIndexData(&i, sizeof(unsigned int));
	}
	m_vboRightOffsetLine.UploadDataToGPU(GL_STATIC_DRAW);

	stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	
	glPointSize(7.f);
	glLineWidth(5.f);
	glDrawElements(GL_POINTS,m_centrelinePoints.size(), GL_UNSIGNED_INT, 0);
	glDrawElements(GL_LINE_LOOP, m_centrelinePoints.size(), GL_UNSIGNED_INT, 0);
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it

	// Bind the VAO m_vaoRightOffsetCurve and render it
}


void CCatmullRom::RenderTrack()
{
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_TRIANGLES,0,m_vertexLeft);
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_TRIANGLES, 0, m_vertexRight);
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);