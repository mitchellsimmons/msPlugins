#pragma once

#include <cassert>

#include <maya/MEulerRotation.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatVector.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MVector.h>

#include "math_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ------ Helpers -------------------------------------------------------------------------------------------------------------------------------------------------------------

// The following helper functions assume the given input matrices represent an orthogonal vector space with the absence of any shear transformations
// We are only supporting the most common transformations to optimize for the general case
// Transformation space parameters are only provided for translation (kPreTransform assumes T * S * R, kPostTransform assumes S * R * T)
// There is no functional difference between pre and post transformed scale as by default scale is applied relative to all other transforms (ie. S * R * T)
// MTransformationMatrix accepts MSpace enumeration types (kTransform, kPreTransform and kPostTransform)
// Functionally, kTransform and kPostTransform will be treated the same as the input data does not infer anything about hierarchy

MMatrix matrixFromVectors(const MVector& x, const MVector& y, const MVector& z, const MVector& p);

MFloatMatrix matrixFromVectors(const MFloatVector& x, const MFloatVector& y, const MFloatVector& z, const MFloatVector& p);

MMatrix composeMatrix(const MVector& translation, const MQuaternion& rotation, const MVector& scale);

MMatrix composeMatrix(const MVector& translation, const MEulerRotation& rotation, const MVector& scale);

void decomposeMatrix(const MMatrix& matrix, MVector& outTranslation, MQuaternion& outRotation, MVector& outScale);

void decomposeMatrix(const MMatrix& matrix, MEulerRotation::RotationOrder rotationOrder, MVector& outTranslation, MEulerRotation& outRotation, MVector& outScale);

MMatrix extractScaleMatrix(const MMatrix& matrix);

MFloatMatrix extractScaleMatrix(const MFloatMatrix& matrix);

MVector extractScale(const MMatrix& matrix);

MFloatVector extractScale(const MFloatMatrix& matrix);

MMatrix extractRotationMatrix(const MMatrix& matrix);

MFloatMatrix extractRotationMatrix(const MFloatMatrix& matrix);

MEulerRotation extractEulerRotation(const MMatrix& matrix, MEulerRotation::RotationOrder rotationOrder = MEulerRotation::kXYZ);

MQuaternion extractQuaternionRotation(const MMatrix& matrix);

MMatrix extractTranslationMatrix(const MMatrix& matrix, MSpace::Space space = MSpace::kPostTransform);

MFloatMatrix extractTranslationMatrix(const MFloatMatrix& matrix, MSpace::Space space = MSpace::kPostTransform);

MVector extractTranslation(const MMatrix& matrix, MSpace::Space space = MSpace::kPostTransform);

MFloatVector extractTranslation(const MFloatMatrix& matrix, MSpace::Space space = MSpace::kPostTransform);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class Matrix44;

/*	Simple 3x3 row-major matrix class, used for basic transformation operations, interfaces with Maya API    */
template<typename T>
class Matrix33
{
public:
	class Row
	{
	private:
		unsigned int m_row;
		Matrix33<T>& m_parent;

		friend class Matrix33;
		Row(Matrix33<T>& parent, unsigned int row) :
			m_parent{ parent }, m_row{ row } {}

	public:
		T& operator[](unsigned int col) { return m_parent(m_row, col); }

		void get(float(&vec)[3]) const { return m_parent.getRow(m_row, vec); }
		void get(double(&vec)[3]) const { return m_parent.getRow(m_row, vec); }
		void get(MFloatVector& vec) const { return m_parent.getRow(m_row, vec); }
		void get(MVector& vec) const { return m_parent.getRow(m_row, vec); }
	};

public:
	Matrix33() :
		m_data{
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
	} {}

	Matrix33(T m[3][3]) :
		m_data{
			m[0][0], m[0][1], m[0][2],
			m[1][0], m[1][1], m[1][2],
			m[2][0], m[2][1], m[2][2]
	} {}

	Matrix33(T rowX[3], T rowY[3], T rowZ[3]) :
		m_data{
			rowX[0], rowX[1], rowX[2],
			rowY[0], rowY[1], rowY[2],
			rowZ[0], rowZ[1], rowZ[2]
	} {}

	Matrix33(MFloatVector rowX, MFloatVector rowY, MFloatVector rowZ) :
		m_data{
			(T)rowX.x, (T)rowX.y, (T)rowX.z,
			(T)rowY.x, (T)rowY.y, (T)rowY.z,
			(T)rowZ.x, (T)rowZ.y, (T)rowZ.z
	} {}

	Matrix33(MVector rowX, MVector rowY, MVector rowZ) :
		m_data{
			(T)rowX.x, (T)rowX.y, (T)rowX.z,
			(T)rowY.x, (T)rowY.y, (T)rowY.z,
			(T)rowZ.x, (T)rowZ.y, (T)rowZ.z
	} {}

	Matrix33(T a, T b, T c, T d, T e, T f, T g, T h, T i) :
		m_data{
			a, b, c,
			d, e, f,
			g, h, i
	} {}

	Row operator[](unsigned int row)
	{
		return Row(*this, row);
	}

	T& operator()(unsigned int row, unsigned int col)
	{
		return m_data[row * 3 + col];
	}

	Matrix33<T> operator*(const Matrix33<T>& rhs) const
	{
		return Matrix33<T>{
			m_data[0] * rhs.m_data[0] + m_data[1] * rhs.m_data[3] + m_data[2] * rhs.m_data[6], m_data[0] * rhs.m_data[1] + m_data[1] * rhs.m_data[4] + m_data[2] * rhs.m_data[7], m_data[0] * rhs.m_data[2] + m_data[1] * rhs.m_data[5] + m_data[2] * rhs.m_data[8],
			m_data[3] * rhs.m_data[0] + m_data[4] * rhs.m_data[3] + m_data[5] * rhs.m_data[6], m_data[3] * rhs.m_data[1] + m_data[4] * rhs.m_data[4] + m_data[5] * rhs.m_data[7], m_data[3] * rhs.m_data[2] + m_data[4] * rhs.m_data[5] + m_data[5] * rhs.m_data[8],
			m_data[6] * rhs.m_data[0] + m_data[7] * rhs.m_data[3] + m_data[8] * rhs.m_data[6], m_data[6] * rhs.m_data[1] + m_data[7] * rhs.m_data[4] + m_data[8] * rhs.m_data[7], m_data[6] * rhs.m_data[2] + m_data[7] * rhs.m_data[5] + m_data[8] * rhs.m_data[8],
		};
	}

	void get(float(&dest)[3][3]) const
	{
		dest[0][0] = (float)m_data[0]; dest[0][1] = (float)m_data[1]; dest[0][2] = (float)m_data[2];
		dest[1][0] = (float)m_data[3]; dest[1][1] = (float)m_data[4]; dest[1][2] = (float)m_data[5];
		dest[2][0] = (float)m_data[6]; dest[2][1] = (float)m_data[7]; dest[2][2] = (float)m_data[8];
	}

	void get(double(&dest)[3][3]) const
	{
		dest[0][0] = (double)m_data[0]; dest[0][1] = (double)m_data[1]; dest[0][2] = (double)m_data[2];
		dest[1][0] = (double)m_data[3]; dest[1][1] = (double)m_data[4]; dest[1][2] = (double)m_data[5];
		dest[2][0] = (double)m_data[6]; dest[2][1] = (double)m_data[7]; dest[2][2] = (double)m_data[8];
	}

	void get(float(&dest)[4][4]) const
	{
		dest[0][0] = (float)m_data[0]; dest[0][1] = (float)m_data[1]; dest[0][2] = (float)m_data[2];
		dest[1][0] = (float)m_data[3]; dest[1][1] = (float)m_data[4]; dest[1][2] = (float)m_data[5];
		dest[2][0] = (float)m_data[6]; dest[2][1] = (float)m_data[7]; dest[2][2] = (float)m_data[8];
	}

	void get(double(&dest)[4][4]) const
	{
		dest[0][0] = (double)m_data[0]; dest[0][1] = (double)m_data[1]; dest[0][2] = (double)m_data[2];
		dest[1][0] = (double)m_data[3]; dest[1][1] = (double)m_data[4]; dest[1][2] = (double)m_data[5];
		dest[2][0] = (double)m_data[6]; dest[2][1] = (double)m_data[7]; dest[2][2] = (double)m_data[8];
	}

	void get(MFloatMatrix& dest) const
	{
		dest[0][0] = (float)m_data[0]; dest[0][1] = (float)m_data[1]; dest[0][2] = (float)m_data[2];
		dest[1][0] = (float)m_data[3]; dest[1][1] = (float)m_data[4]; dest[1][2] = (float)m_data[5];
		dest[2][0] = (float)m_data[6]; dest[2][1] = (float)m_data[7]; dest[2][2] = (float)m_data[8];
	}

	void get(MMatrix& dest) const
	{
		dest[0][0] = (double)m_data[0]; dest[0][1] = (double)m_data[1]; dest[0][2] = (double)m_data[2];
		dest[1][0] = (double)m_data[3]; dest[1][1] = (double)m_data[4]; dest[1][2] = (double)m_data[5];
		dest[2][0] = (double)m_data[6]; dest[2][1] = (double)m_data[7]; dest[2][2] = (double)m_data[8];
	}

	void get(Matrix44<T>& dest) const
	{
		dest[0][0] = m_data[0]; dest[0][1] = m_data[1]; dest[0][2] = m_data[2];
		dest[1][0] = m_data[3]; dest[1][1] = m_data[4]; dest[1][2] = m_data[5];
		dest[2][0] = m_data[6]; dest[2][1] = m_data[7]; dest[2][2] = m_data[8];
	}

	void getRow(unsigned int row, float(&dest)[3]) const
	{
		assert(row < 3);
		unsigned int entry = row * 3;
		dest[0] = (float)m_data[entry]; dest[1] = (float)m_data[entry + 1]; dest[2] = (float)m_data[entry + 2];
	}

	void getRow(unsigned int row, double(&dest)[3]) const
	{
		assert(row < 3);
		unsigned int entry = row * 3;
		dest[0] = (double)m_data[entry]; dest[1] = (double)m_data[entry + 1]; dest[2] = (double)m_data[entry + 2];
	}

	void getRow(unsigned int row, MFloatVector& dest) const
	{
		assert(row < 3);
		unsigned int entry = row * 3;
		dest[0] = (float)m_data[entry]; dest[1] = (float)m_data[entry + 1]; dest[2] = (float)m_data[entry + 2];
	}

	void getRow(unsigned int row, MVector& dest) const
	{
		assert(row < 3);
		unsigned int entry = row * 3;
		dest[0] = (double)m_data[entry]; dest[1] = (double)m_data[entry + 1]; dest[2] = (double)m_data[entry + 2];
	}

	// --- Transpose ---
	Matrix33<T> transpose() const
	{
		return Matrix33<T>{
			m_data[0], m_data[3], m_data[6],
				m_data[1], m_data[4], m_data[7],
				m_data[2], m_data[5], m_data[8]
		};
	}

	// --- Pre Rotate ---
	// This matrix is pre-multiplied by the given rotation
	// The resulting matrix consists of the given rotation 'applied' relative to the existing transformations
	// Warning : If the current matrix is composed of non-uniform scaling, applying rotation relative to this will likely produce an unintended result
	Matrix33<T> preRotateInX(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix33<T>{
			m_data[0], m_data[1], m_data[2],
			(m_data[3] * x) + (m_data[6] * y), (m_data[4] * x) + (m_data[7] * y), (m_data[5] * x) + (m_data[8] * y),
			(m_data[6] * x) - (m_data[3] * y), (m_data[7] * x) - (m_data[4] * y), (m_data[8] * x) - (m_data[5] * y)
		};
	}

	Matrix33<T> preRotateInY(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix33<T>{
			(m_data[0] * x) - (m_data[6] * y), (m_data[1] * x) - (m_data[7] * y), (m_data[2] * x) - (m_data[8] * y),
			m_data[3], m_data[4], m_data[5],
			(m_data[6] * x) + (m_data[0] * y), (m_data[7] * x) + (m_data[1] * y), (m_data[8] * x) + (m_data[2] * y),
		};
	}

	Matrix33<T> preRotateInZ(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix33<T>{
			(m_data[0] * x) + (m_data[3] * y), (m_data[1] * x) + (m_data[4] * y), (m_data[2] * x) + (m_data[5] * y),
			(m_data[3] * x) - (m_data[0] * y), (m_data[4] * x) - (m_data[1] * y), (m_data[5] * x) - (m_data[2] * y),
			m_data[6], m_data[7], m_data[8]
		};
	}

	// --- Post Rotate ---
	// This matrix is post-multiplied by the given rotation
	// The resulting matrix consists of the existing transformations 'applied' relative to the given rotation
	Matrix33<T> postRotateInX(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix33<T>{
			m_data[0], m_data[1] * x - m_data[2] * y, m_data[2] * x + m_data[1] * y,
			m_data[3], m_data[4] * x - m_data[5] * y, m_data[5] * x + m_data[4] * y,
			m_data[6], m_data[7] * x - m_data[8] * y, m_data[8] * x + m_data[7] * y
		};
	}

	Matrix33<T> postRotateInY(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix33<T>{
			m_data[0] * x + m_data[2] * y, m_data[1], m_data[2] * x - m_data[0] * y,
			m_data[3] * x + m_data[5] * y, m_data[4], m_data[5] * x - m_data[3] * y,
			m_data[6] * x + m_data[8] * y, m_data[7], m_data[8] * x - m_data[6] * y
		};
	}

	Matrix33<T> postRotateInZ(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix33<T>{
			m_data[0] * x - m_data[1] * y, m_data[1] * x + m_data[0] * y, m_data[2],
			m_data[3] * x - m_data[4] * y, m_data[4] * x + m_data[3] * y, m_data[5],
			m_data[6] * x - m_data[7] * y, m_data[7] * x + m_data[6] * y, m_data[8]
		};
	}

	// --- Pre Scale ---
	// This matrix is pre-multiplied by the given scale
	// The resulting matrix consists of the given scale 'applied' relative to the existing transformations
	Matrix33<T> preScale(const T scale[3]) const
	{
		return Matrix33<T>{
			m_data[0] * scale[0], m_data[1] * scale[0], m_data[2] * scale[0],
			m_data[3] * scale[1], m_data[4] * scale[1], m_data[5] * scale[1],
			m_data[6] * scale[2], m_data[7] * scale[2], m_data[8] * scale[2]
		};
	}

	// --- Post Scale ---
	// This matrix is post-multiplied by the given scale
	// The resulting matrix consists of the existing transformations 'applied' relative to the given scale
	// Warning : If the current matrix is composed of rotation, post-multiplying a non-uniform scaling transformation will likely produce an unintended result
	Matrix33<T> postScale(const T scale[3]) const
	{
		return Matrix33<T>{
			m_data[0] * scale[0], m_data[1] * scale[1], m_data[2] * scale[2],
			m_data[3] * scale[0], m_data[4] * scale[1], m_data[5] * scale[2],
			m_data[6] * scale[0], m_data[7] * scale[1], m_data[8] * scale[2]
		};
	}

private:
	// Using single allocation (instead of array of pointers)
	T m_data[9];
};


/*	Simple 4x4 row-major matrix class, used for basic transformation operations, interfaces with Maya API    */
template<typename T>
class Matrix44
{
public:
	class Row
	{
	private:
		unsigned int m_row;
		Matrix44<T>& m_parent;

		friend class Matrix44;
		Row(Matrix44<T>& parent, unsigned int row) :
			m_parent{ parent }, m_row{ row } {}

	public:
		T& operator[](unsigned int col) { return m_parent(m_row, col); }

		void get(float(&vec)[4]) const { return m_parent.getRow(m_row, vec); }
		void get(double(&vec)[4]) const { return m_parent.getRow(m_row, vec); }
		void get(MFloatVector& vec) const { return m_parent.getRow(m_row, vec); }
		void get(MVector& vec) const { return m_parent.getRow(m_row, vec); }
	};

public:
	Matrix44() :
		m_data{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	} {}

	Matrix44(T m[4][4]) :
		m_data{
			m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]
	} {}

	Matrix44(T rowX[4], T rowY[4], T rowZ[4], T rowP[4]) :
		m_data{
			rowX[0], rowX[1], rowX[2], rowX[3],
			rowY[0], rowY[1], rowY[2], rowY[3],
			rowZ[0], rowZ[1], rowZ[2], rowZ[3],
			rowP[0], rowP[1], rowP[2], rowP[3]
	} {}

	Matrix44(MFloatVector rowX, MFloatVector rowY, MFloatVector rowZ, MFloatVector rowP) :
		m_data{
			(T)rowX.x, (T)rowX.y, (T)rowX.z, (T)0,
			(T)rowY.x, (T)rowY.y, (T)rowY.z, (T)0,
			(T)rowZ.x, (T)rowZ.y, (T)rowZ.z, (T)0,
			(T)rowP.x, (T)rowP.y, (T)rowP.z, (T)1
	} {}

	Matrix44(MVector rowX, MVector rowY, MVector rowZ, MVector rowP) :
		m_data{
			(T)rowX.x, (T)rowX.y, (T)rowX.z, (T)0,
			(T)rowY.x, (T)rowY.y, (T)rowY.z, (T)0,
			(T)rowZ.x, (T)rowZ.y, (T)rowZ.z, (T)0,
			(T)rowP.x, (T)rowP.y, (T)rowP.z, (T)1
	} {}

	Matrix44(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p) :
		m_data{
			a, b, c, d,
			e, f, g, h,
			i, j, k, l,
			m, n, o, p
	} {}

	Row operator[](unsigned int row)
	{
		return Row(*this, row);
	}

	T& operator()(unsigned int row, unsigned int col)
	{
		return m_data[row * 4 + col];
	}

	Matrix44<T> operator*(const Matrix44<T>& rhs) const
	{
		return Matrix44<T>{
			m_data[0] * rhs.m_data[0] + m_data[1] * rhs.m_data[4] + m_data[2] * rhs.m_data[8] + m_data[3] * rhs.m_data[12], 
			m_data[0] * rhs.m_data[1] + m_data[1] * rhs.m_data[5] + m_data[2] * rhs.m_data[9] + m_data[3] * rhs.m_data[13],
			m_data[0] * rhs.m_data[2] + m_data[1] * rhs.m_data[6] + m_data[2] * rhs.m_data[10] + m_data[3] * rhs.m_data[14],
			m_data[0] * rhs.m_data[3] + m_data[1] * rhs.m_data[7] + m_data[2] * rhs.m_data[11] + m_data[3] * rhs.m_data[15],

			m_data[4] * rhs.m_data[0] + m_data[5] * rhs.m_data[4] + m_data[6] * rhs.m_data[8] + m_data[7] * rhs.m_data[12], 
			m_data[4] * rhs.m_data[1] + m_data[5] * rhs.m_data[5] + m_data[6] * rhs.m_data[9] + m_data[7] * rhs.m_data[13],
			m_data[4] * rhs.m_data[2] + m_data[5] * rhs.m_data[6] + m_data[6] * rhs.m_data[10] + m_data[7] * rhs.m_data[14],
			m_data[4] * rhs.m_data[3] + m_data[5] * rhs.m_data[7] + m_data[6] * rhs.m_data[11] + m_data[7] * rhs.m_data[15],

			m_data[8] * rhs.m_data[0] + m_data[9] * rhs.m_data[4] + m_data[10] * rhs.m_data[8] + m_data[11] * rhs.m_data[12],
			m_data[8] * rhs.m_data[1] + m_data[9] * rhs.m_data[5] + m_data[10] * rhs.m_data[9] + m_data[11] * rhs.m_data[13],
			m_data[8] * rhs.m_data[2] + m_data[9] * rhs.m_data[6] + m_data[10] * rhs.m_data[10] + m_data[11] * rhs.m_data[14],
			m_data[8] * rhs.m_data[3] + m_data[9] * rhs.m_data[7] + m_data[10] * rhs.m_data[11] + m_data[11] * rhs.m_data[15],

			m_data[12] * rhs.m_data[0] + m_data[13] * rhs.m_data[4] + m_data[14] * rhs.m_data[8] + m_data[15] * rhs.m_data[12],
			m_data[12] * rhs.m_data[1] + m_data[13] * rhs.m_data[5] + m_data[14] * rhs.m_data[9] + m_data[15] * rhs.m_data[13],
			m_data[12] * rhs.m_data[2] + m_data[13] * rhs.m_data[6] + m_data[14] * rhs.m_data[10] + m_data[15] * rhs.m_data[14],
			m_data[12] * rhs.m_data[3] + m_data[13] * rhs.m_data[7] + m_data[14] * rhs.m_data[11] + m_data[15] * rhs.m_data[15],
		};
	}

	void get(float(&dest)[3][3]) const
	{
		dest[0][0] = (float)m_data[0]; dest[0][1] = (float)m_data[1]; dest[0][2] = (float)m_data[2];
		dest[1][0] = (float)m_data[4]; dest[1][1] = (float)m_data[5]; dest[1][2] = (float)m_data[6];
		dest[2][0] = (float)m_data[8]; dest[2][1] = (float)m_data[9]; dest[2][2] = (float)m_data[10];
	}

	void get(double(&dest)[3][3]) const
	{
		dest[0][0] = (double)m_data[0]; dest[0][1] = (double)m_data[1]; dest[0][2] = (double)m_data[2];
		dest[1][0] = (double)m_data[4]; dest[1][1] = (double)m_data[5]; dest[1][2] = (double)m_data[6];
		dest[2][0] = (double)m_data[8]; dest[2][1] = (double)m_data[9]; dest[2][2] = (double)m_data[10];
	}

	void get(Matrix33<T>& dest) const
	{
		dest[0][0] = m_data[0]; dest[0][1] = m_data[1]; dest[0][2] = m_data[2];
		dest[1][0] = m_data[4]; dest[1][1] = m_data[5]; dest[1][2] = m_data[6];
		dest[2][0] = m_data[8]; dest[2][1] = m_data[9]; dest[2][2] = m_data[10];
	}

	void get(float(&dest)[4][4]) const
	{
		dest[0][0] = (float)m_data[0]; dest[0][1] = (float)m_data[1]; dest[0][2] = (float)m_data[2]; dest[0][3] = (float)m_data[3];
		dest[1][0] = (float)m_data[4]; dest[1][1] = (float)m_data[5]; dest[1][2] = (float)m_data[6]; dest[1][3] = (float)m_data[7];
		dest[2][0] = (float)m_data[8]; dest[2][1] = (float)m_data[9]; dest[2][2] = (float)m_data[10]; dest[2][3] = (float)m_data[11];
		dest[3][0] = (float)m_data[12]; dest[3][1] = (float)m_data[13]; dest[3][2] = (float)m_data[14]; dest[3][3] = (float)m_data[15];
	}

	void get(double(&dest)[4][4]) const
	{
		dest[0][0] = (double)m_data[0]; dest[0][1] = (double)m_data[1]; dest[0][2] = (double)m_data[2]; dest[0][3] = (double)m_data[3];
		dest[1][0] = (double)m_data[4]; dest[1][1] = (double)m_data[5]; dest[1][2] = (double)m_data[6]; dest[1][3] = (double)m_data[7];
		dest[2][0] = (double)m_data[8]; dest[2][1] = (double)m_data[9]; dest[2][2] = (double)m_data[10]; dest[2][3] = (double)m_data[11];
		dest[3][0] = (double)m_data[12]; dest[3][1] = (double)m_data[13]; dest[3][2] = (double)m_data[14]; dest[3][3] = (double)m_data[15];
	}

	void get(MFloatMatrix& dest) const
	{
		dest[0][0] = (float)m_data[0]; dest[0][1] = (float)m_data[1]; dest[0][2] = (float)m_data[2]; dest[0][3] = (float)m_data[3];
		dest[1][0] = (float)m_data[4]; dest[1][1] = (float)m_data[5]; dest[1][2] = (float)m_data[6]; dest[1][3] = (float)m_data[7];
		dest[2][0] = (float)m_data[8]; dest[2][1] = (float)m_data[9]; dest[2][2] = (float)m_data[10]; dest[2][3] = (float)m_data[11];
		dest[3][0] = (float)m_data[12]; dest[3][1] = (float)m_data[13]; dest[3][2] = (float)m_data[14]; dest[3][3] = (float)m_data[15];
	}

	void get(MMatrix& dest) const
	{
		dest[0][0] = (double)m_data[0]; dest[0][1] = (double)m_data[1]; dest[0][2] = (double)m_data[2]; dest[0][3] = (double)m_data[3];
		dest[1][0] = (double)m_data[4]; dest[1][1] = (double)m_data[5]; dest[1][2] = (double)m_data[6]; dest[1][3] = (double)m_data[7];
		dest[2][0] = (double)m_data[8]; dest[2][1] = (double)m_data[9]; dest[2][2] = (double)m_data[10]; dest[2][3] = (double)m_data[11];
		dest[3][0] = (double)m_data[12]; dest[3][1] = (double)m_data[13]; dest[3][2] = (double)m_data[14]; dest[3][3] = (double)m_data[15];
	}

	void getRow(unsigned int row, float(&dest)[3]) const
	{
		assert(row < 4);
		unsigned int entry = row * 4;
		dest[0] = (float)m_data[entry]; dest[1] = (float)m_data[entry + 1]; dest[2] = (float)m_data[entry + 2];
	}

	void getRow(unsigned int row, double(&dest)[3]) const
	{
		assert(row < 4);
		unsigned int entry = row * 4;
		dest[0] = (double)m_data[entry]; dest[1] = (double)m_data[entry + 1]; dest[2] = (double)m_data[entry + 2];
	}

	void getRow(unsigned int row, float(&dest)[4]) const
	{
		assert(row < 4);
		unsigned int entry = row * 4;
		dest[0] = (float)m_data[entry]; dest[1] = (float)m_data[entry + 1]; dest[2] = (float)m_data[entry + 2]; dest[3] = (float)m_data[entry + 3];
	}

	void getRow(unsigned int row, double(&dest)[4]) const
	{
		assert(row < 4);
		unsigned int entry = row * 4;
		dest[0] = (double)m_data[entry]; dest[1] = (double)m_data[entry + 1]; dest[2] = (double)m_data[entry + 2]; dest[3] = (double)m_data[entry + 3];
	}

	void getRow(unsigned int row, MFloatVector& dest) const
	{
		assert(row < 4);
		unsigned int entry = row * 4;
		dest[0] = (float)m_data[entry]; dest[1] = (float)m_data[entry + 1]; dest[2] = (float)m_data[entry + 2];
	}

	void getRow(unsigned int row, MVector& dest) const
	{
		assert(row < 4);
		unsigned int entry = row * 4;
		dest[0] = (double)m_data[entry]; dest[1] = (double)m_data[entry + 1]; dest[2] = (double)m_data[entry + 2];
	}

	// --- Transpose ---
	Matrix44<T> transpose() const
	{
		return Matrix44<T>{
			m_data[0], m_data[4], m_data[8], m_data[12],
			m_data[1], m_data[5], m_data[9], m_data[13],
			m_data[2], m_data[6], m_data[10] m_data[14],
			m_data[3], m_data[7], m_data[11] m_data[15]
		};
	}

	// --- Pre Translate ---
	// This matrix is pre-multiplied by the given translation
	// The resulting matrix consists of the given translation 'applied' relative to the existing transformations
	Matrix44<T> preTranslate(const T translation[3]) const
	{
		return Matrix44<T>{
			m_data[0], m_data[1], m_data[2], (T)0,
			m_data[4], m_data[5], m_data[6], (T)0,
			m_data[8], m_data[9], m_data[10], (T)0,
			m_data[0] * translation[0] + m_data[4] * translation[1] + m_data[8] * translation[2] + m_data[12],
			m_data[1] * translation[0] + m_data[5] * translation[1] + m_data[9] * translation[2] + m_data[13],
			m_data[2] * translation[0] + m_data[6] * translation[1] + m_data[10] * translation[2] + m_data[14],
			(T)1
		};
	}

	// --- Post Translate ---
	// This matrix is post-multiplied by the given translation
	// The resulting matrix consists of the existing transformations 'applied' relative to the given translation
	Matrix44<T> postTranslate(const T translation[3]) const
	{
		return Matrix44<T>{
			m_data[0], m_data[1], m_data[2], (T)0,
			m_data[4], m_data[5], m_data[6], (T)0,
			m_data[8], m_data[9], m_data[10], (T)0,
			m_data[12] + translation[0], m_data[13] * translation[1], m_data[14] * translation[2], (T)1
		};
	}

	// --- Pre Rotate ---
	// This matrix is pre-multiplied by the given rotation
	// The resulting matrix consists of the given rotation 'applied' relative to the existing transformations
	// Warning : If the current matrix is composed of non-uniform scaling, applying rotation relative to this will likely produce an unintended result
	Matrix44<T> preRotateInX(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix44<T>{
			m_data[0], m_data[1], m_data[2], (T)0,
			(m_data[4] * x) + (m_data[8] * y), (m_data[5] * x) + (m_data[9] * y), (m_data[6] * x) + (m_data[10] * y), (T)0,
			(m_data[8] * x) - (m_data[4] * y), (m_data[9] * x) - (m_data[5] * y), (m_data[10] * x) - (m_data[6] * y), (T)0,
			m_data[12], m_data[13], m_data[14], (T)1
		};
	}

	Matrix44<T> preRotateInY(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix44<T>{
			(m_data[0] * x) - (m_data[8] * y), (m_data[1] * x) - (m_data[9] * y), (m_data[2] * x) - (m_data[10] * y), (T)0,
			m_data[4], m_data[5], m_data[6], (T)0,
			(m_data[8] * x) + (m_data[0] * y), (m_data[9] * x) + (m_data[1] * y), (m_data[10] * x) + (m_data[2] * y), (T)0,
			m_data[12], m_data[13], m_data[14], (T)1
		};
	}

	Matrix44<T> preRotateInZ(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix44<T>{
			(m_data[0] * x) + (m_data[4] * y), (m_data[1] * x) + (m_data[5] * y), (m_data[2] * x) + (m_data[6] * y), (T)0,
			(m_data[4] * x) - (m_data[0] * y), (m_data[5] * x) - (m_data[1] * y), (m_data[6] * x) - (m_data[2] * y), (T)0,
			m_data[8], m_data[9], m_data[10], (T)0,
			m_data[12], m_data[13], m_data[14], (T)1
		};
	}

	// --- Post Rotate ---
	// This matrix is post-multiplied by the given rotation
	// The resulting matrix consists of the existing transformations 'applied' relative to the given rotation
	Matrix44<T> postRotateInX(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix44<T>{
			m_data[0], m_data[1] * x - m_data[2] * y, m_data[2] * x + m_data[1] * y, (T)0,
			m_data[4], m_data[5] * x - m_data[6] * y, m_data[6] * x + m_data[5] * y, (T)0,
			m_data[8], m_data[9] * x - m_data[10] * y, m_data[10] * x + m_data[9] * y, (T)0,
			m_data[12], m_data[13] * x - m_data[14] * y, m_data[14] * x + m_data[13] * y, (T)1
		};
	}

	Matrix44<T> postRotateInY(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix44<T>{
			m_data[0] * x + m_data[2] * y, m_data[1], m_data[2] * x - m_data[0] * y, (T)0,
			m_data[4] * x + m_data[6] * y, m_data[5], m_data[6] * x - m_data[4] * y, (T)0,
			m_data[8] * x + m_data[10] * y, m_data[9], m_data[10] * x - m_data[8] * y, (T)0,
			m_data[12] * x + m_data[14] * y, m_data[13], m_data[14] * x - m_data[12] * y, (T)1
		};
	}

	Matrix44<T> postRotateInZ(T rot) const
	{
		T x = cos(rot);
		T y = sin(rot);

		return Matrix44<T>{
			m_data[0] * x - m_data[1] * y, m_data[1] * x + m_data[0] * y, m_data[2], (T)0,
			m_data[4] * x - m_data[5] * y, m_data[5] * x + m_data[4] * y, m_data[6], (T)0,
			m_data[8] * x - m_data[9] * y, m_data[9] * x + m_data[8] * y, m_data[10], (T)0,
			m_data[12] * x - m_data[13] * y, m_data[13] * x + m_data[12] * y, m_data[14], (T)1,
		};
	}

	// --- Pre Scale ---
	// This matrix is pre-multiplied by the given scale
	// The resulting matrix consists of the given scale 'applied' relative to the existing transformations
	Matrix44<T> preScale(const T scale[3]) const
	{
		return Matrix44<T>{
			m_data[0] * scale[0], m_data[1] * scale[0], m_data[2] * scale[0], (T)0,
			m_data[4] * scale[1], m_data[5] * scale[1], m_data[6] * scale[1], (T)0,
			m_data[8] * scale[2], m_data[9] * scale[2], m_data[10] * scale[2], (T)0,
			m_data[12], m_data[13], m_data[14], (T)1
		};
	}

	// --- Post Scale ---
	// This matrix is post-multiplied by the given scale
	// The resulting matrix consists of the existing transformations 'applied' relative to the given scale
	// Warning : If the current matrix is composed of rotation, post-multiplying a non-uniform scaling transformation will likely produce an unintended result
	Matrix44<T> postScale(const T scale[3]) const
	{
		return Matrix44<T>{
			m_data[0] * scale[0], m_data[1] * scale[1], m_data[2] * scale[2], (T)0,
			m_data[4] * scale[0], m_data[5] * scale[1], m_data[6] * scale[2], (T)0,
			m_data[8] * scale[0], m_data[9] * scale[1], m_data[10] * scale[2], (T)0,
			m_data[12] * scale[0], m_data[13] * scale[1], m_data[14] * scale[2], (T)1
		};
	}

private:
	// Using single allocation (instead of array of pointers)
	T m_data[16];
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

} // MRS

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //