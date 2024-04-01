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
	m_controlPoints.push_back({ 510.2198641236067, -113.78207104999922, -180.12820609432634 });
	m_controlPoints.push_back({ 265.5160002690295, 9.594301977361333, 78.42151665193104 });
	m_controlPoints.push_back({ 176.53097042149386, -43.864811154874225, 47.75058016175143 });
	m_controlPoints.push_back({ 1.6276066624723367, -34.79675387337366, -13.866281996862867 });
	m_controlPoints.push_back({ -62.02226572048809, 92.14552416732394, -12.517340229687854 });
	m_controlPoints.push_back({ 29.064548644549912, -2.5068118769684737, -255.50924402137275 });
	m_controlPoints.push_back({ 389.23557673943685, 101.08086462674714, 35.48750277499863 });
	m_controlPoints.push_back({ 395.5122443538989, 7.97273118007422, -255.55316336706846 });
	m_controlPoints.push_back({ 199.49598800904135, -101.9116339123164, -269.44694682577483 });
	m_controlPoints.push_back({ 124.68614989829528, -60.852728282409565, -136.15397392996744 });
	m_controlPoints.push_back({ 200.99747765501382, -100.36899654877083, -61.61968487768833 });
	m_controlPoints.push_back({ 400.4388489006723, -120.67537037545313, -200.79066908762184 });
	m_controlPoints.push_back({ 470.2198641236067, -180.12812587036268, -246.5040924711575 });



	
	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(1, 0, 1));
	m_controlUpVectors.push_back(glm::vec3(0, -1, 0));
	m_controlUpVectors.push_back(glm::vec3(1, 0, 1));
	m_controlUpVectors.push_back(glm::vec3(1, 0, 1));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0,1, 0));
	

	
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
	UniformlySampleControlPoints(2000);
	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		glm::vec3 normal = glm::normalize(glm::cross(m_centrelineUpVectors[i], m_centrelineTangentVectors[i]));
		m_leftOffsetPoints.push_back(m_centrelinePoints[i] + normal * 10.f);
		m_rightOffsetPoints.push_back(m_centrelinePoints[i] - normal * 10.f);
	}

	
	
}


void CCatmullRom::CreateTrack()
{
	m_texture.Load("resources\\textures\\TrackEdge.jpg");

	
	CreateOffsetCurves();
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	m_vboTrack.Create();
	m_vboTrack.Bind();
	for (int i = 0; i < m_leftOffsetPoints.size(); i++)
	{
		glm::vec3 lp0, lp1, lp2, lp3, lp4, lp5, lp6, lp7;
		glm::vec2 lt0, lt1, lt2, lt3, lt4, lt5, lt6, lt7;
		glm::vec3 ln01, ln12, ln23, ln30, ln45, ln56, ln67, ln74;

		glm::vec3 rp0, rp1, rp2, rp3, rp4, rp5, rp6, rp7;
		glm::vec2 rt0, rt1, rt2, rt3, rt4, rt5, rt6, rt7;
		glm::vec3 rn01, rn12, rn23, rn30, rn45, rn56, rn67, rn74;

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

		lp0 = m_leftOffsetPoints[i] + (norm0 * thickness) + (binorm0 * thickness);
		lp1 = m_leftOffsetPoints[i] - (norm0 * thickness) + (binorm0 * thickness);
		lp2 = m_leftOffsetPoints[i] - (norm0 * thickness) - (binorm0 * thickness);
		lp3 = m_leftOffsetPoints[i] + (norm0 * thickness) - (binorm0 * thickness);

		lp4 = m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] + (norm1 * thickness) + (binorm1 * thickness);
		lp5 = m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] - (norm1 * thickness) + (binorm1 * thickness);
		lp6 = m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] - (norm1 * thickness) - (binorm1 * thickness);
		lp7 = m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] + (norm1 * thickness) - (binorm1 * thickness);

		ln01 = binorm0;
		ln12 = -norm0;
		ln23 = -binorm0;
		ln30 = norm0;

		ln45 = binorm1;
		ln56 = -norm1;
		ln67 = -binorm1;
		ln74 = norm1;

		lt0 = glm::vec2(0.f, i * tiling / m_leftOffsetPoints.size());
		lt1 = glm::vec2(0.33f, i * tiling / m_leftOffsetPoints.size());
		lt2 = glm::vec2(0.66f, i * tiling / m_leftOffsetPoints.size());
		lt3 = glm::vec2(1.f, i * tiling / m_leftOffsetPoints.size());

		lt4 = glm::vec2(0.f, (i+1) * tiling / m_leftOffsetPoints.size());
		lt5 = glm::vec2(0.33f, (i+1) * tiling / m_leftOffsetPoints.size());
		lt6 = glm::vec2(0.66f, (i + 1) * tiling / m_leftOffsetPoints.size());
		lt7 = glm::vec2(1.f, (i + 1) * tiling / m_leftOffsetPoints.size());
		

		rp0 = m_rightOffsetPoints[i] + (norm0 * thickness) + (binorm0 * thickness);
		rp1 = m_rightOffsetPoints[i] - (norm0 * thickness) + (binorm0 * thickness);
		rp2 = m_rightOffsetPoints[i] - (norm0 * thickness) - (binorm0 * thickness);
		rp3 = m_rightOffsetPoints[i] + (norm0 * thickness) - (binorm0 * thickness);

		rp4 = m_rightOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] + (norm1 * thickness) + (binorm1 * thickness);
		rp5 = m_rightOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] - (norm1 * thickness) + (binorm1 * thickness);
		rp6 = m_rightOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] - (norm1 * thickness) - (binorm1 * thickness);
		rp7 = m_rightOffsetPoints[(i + 1) % m_leftOffsetPoints.size()] + (norm1 * thickness) - (binorm1 * thickness);

		rn01 = binorm0;
		rn12 = -norm0;
		rn23 = -binorm0;
		rn30 = norm0;

		rn45 = binorm1;
		rn56 = -norm1;
		rn67 = -binorm1;
		rn74 = norm1;

		rt0 = glm::vec2(0.f, i * tiling / m_rightOffsetPoints.size());
		rt1 = glm::vec2(0.33f, i * tiling / m_rightOffsetPoints.size());
		rt2 = glm::vec2(0.66f, i * tiling / m_rightOffsetPoints.size());
		rt3 = glm::vec2(1.f, i * tiling / m_rightOffsetPoints.size());

		rt4 = glm::vec2(0.f, (i + 1) * tiling / m_rightOffsetPoints.size());
		rt5 = glm::vec2(0.33f, (i + 1) * tiling / m_rightOffsetPoints.size());
		rt6 = glm::vec2(0.66f, (i + 1) * tiling / m_rightOffsetPoints.size());
		rt7 = glm::vec2(1.f, (i + 1) * tiling / m_rightOffsetPoints.size());

		//upload to vbo;
		CVertex cv0, cv1, cv2, cv3;
		//rectangle 0145
		cv0.position = lp0;
		cv0.texture = lt0;
		cv0.normal = ln01;

		cv1.position = lp1;
		cv1.texture = lt1;
		cv1.normal = ln01;

		cv2.position = lp4;
		cv2.texture = lt4;
		cv2.normal = ln45;

		cv3.position = lp5;
		cv3.texture = lt5;
		cv3.normal = ln45;

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexTrack += 6;
		//rectangle1256

		cv0.position = lp1;
		cv0.texture = lt1;
		cv0.normal = ln12;

		cv1.position = lp2;
		cv1.texture = lt2;
		cv1.normal = ln12;

		cv2.position = lp5;
		cv2.texture = lt5;
		cv2.normal = ln56;

		cv3.position = lp6;
		cv3.texture = lt6;
		cv3.normal = ln56;

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexTrack += 6;
		//rectangle2367

		cv0.position = lp2;
		cv0.texture = lt2;
		cv0.normal = ln23;

		cv1.position = lp3;
		cv1.texture = lt3;
		cv1.normal = ln23;

		cv2.position = lp6;
		cv2.texture = lt6;
		cv2.normal = ln67;

		cv3.position = lp7;
		cv3.texture = lt7;
		cv3.normal = ln67;

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexTrack += 6;
		//rectangle3174

		cv0.position = lp3;
		cv0.texture = lt3;
		cv0.normal = ln30;

		cv1.position = lp0;
		cv1.texture = lt0;
		cv1.normal = ln30;

		cv2.position = lp7;
		cv2.texture = lt7;
		cv2.normal = ln74;

		cv3.position = lp4;
		cv3.texture = lt4;
		cv3.normal = ln74;

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexTrack += 6;

		cv0.position = rp0;
		cv0.texture = rt0;
		cv0.normal = rn01;

		cv1.position = rp1;
		cv1.texture = rt1;
		cv1.normal = rn01;

		cv2.position = rp4;
		cv2.texture = rt4;
		cv2.normal = rn45;

		cv3.position = rp5;
		cv3.texture = rt5;
		cv3.normal = rn45;

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexTrack += 6;
		//rectangle1256

		cv0.position = rp1;
		cv0.texture = rt1;
		cv0.normal = rn12;

		cv1.position = rp2;
		cv1.texture = rt2;
		cv1.normal = rn12;

		cv2.position = rp5;
		cv2.texture = rt5;
		cv2.normal = rn56;

		cv3.position = rp6;
		cv3.texture = rt6;
		cv3.normal = rn56;

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexTrack += 6;
		//rectangle2367

		cv0.position = rp2;
		cv0.texture = rt2;
		cv0.normal = rn23;

		cv1.position = rp3;
		cv1.texture = rt3;
		cv1.normal = rn23;

		cv2.position = rp6;
		cv2.texture = rt6;
		cv2.normal = rn67;

		cv3.position = rp7;
		cv3.texture = rt7;
		cv3.normal = rn67;

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexTrack += 6;
		//rectangle3174

		cv0.position = rp3;
		cv0.texture = rt3;
		cv0.normal = rn30;

		cv1.position = rp0;
		cv1.texture = rt0;
		cv1.normal = rn30;

		cv2.position = rp7;
		cv2.texture = rt7;
		cv2.normal = rn74;

		cv3.position = rp4;
		cv3.texture = rt4;
		cv3.normal = rn74;

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vertexTrack += 6;

		//base track

		cv0.position = lp3;
		cv0.normal = binorm0;
		cv0.texture= glm::vec2(0.f, i * tiling / m_leftOffsetPoints.size());

		cv1.position = rp2;
		cv1.normal = binorm0;
		cv1.texture = glm::vec2(1.f, i * tiling / m_leftOffsetPoints.size());

		cv2.position = lp7;
		cv2.normal = binorm1;
		cv2.texture = glm::vec2(0.f, (i+1) * tiling / m_leftOffsetPoints.size());

		cv3.position = rp6;
		cv3.normal = binorm1;
		cv3.texture = glm::vec2(1.f, (i + 1) * tiling / m_leftOffsetPoints.size());

		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv1, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));

		m_vboTrack.AddVertexData(&cv3, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv2, sizeof(CVertex));
		m_vboTrack.AddVertexData(&cv0, sizeof(CVertex));

		m_vertexTrack += 6;
	}
	for (unsigned int i = 0; i < m_vertexTrack; i++)
	{
		m_vboTrack.AddIndexData(&i, sizeof(unsigned int));
	}
	m_vboTrack.UploadDataToGPU(GL_STATIC_DRAW);

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
	m_texture.Bind();
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_TRIANGLES,0,m_vertexTrack);

}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}
 float CCatmullRom::totalDist()
{
	return m_distances.back();
}
 void CCatmullRom::ReleaseTrack()
 {
	 m_texture.Release();
	 glDeleteVertexArrays(1, &m_vaoTrack);
	 m_vboTrack.Release();
 }
glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);