#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
using namespace std;


namespace libPRGR {

	typedef struct {

		union {
			struct {
				float x, y, z;
			};
			struct {
				float r, g, b;
			};
			float data[3];
		};

	} vector3f;

	typedef struct {

		union {
			struct {
				float x, y, z, w;
			};
			struct {
				float r, g, b, a;
			};
			float data[4];
		};

	} vector4f;

	typedef struct {
		union {
			float mat2D[3][3];
			float mat1[9];
			vector3f rows[3];
		};

	} matrix3x3f;

	typedef struct {
		union {
			float mat2D[4][4];
			float mat1[16];
			vector4f rows[4];
		};

	} matrix4x4f;

	inline std::ostream& operator<<(std::ostream& os, vector3f v) {
		os << "{ " << v.data[0] << ", " << v.data[1] << ", " <<
			v.data[2] << " }" << endl;
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, vector4f v) {
		os << "{ " << v.data[0] << ", " << v.data[1] << ", " <<
			v.data[2] << ", " << v.data[3] << " }" << endl;
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, matrix3x3f m) {
		os << m.rows[0] << m.rows[1] << m.rows[2] << endl;
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, matrix4x4f m) {
		os << m.rows[0] << m.rows[1] << m.rows[2] << m.rows[3] << endl;
		return os;
	}

	inline float toRadians(float angle) {
		return (float)(angle * M_PI / (180.0f));
	}

	//				MÉTODOS SOBRE VECTORES.
	// ------------------------------------------------


	// Inicializador del vector3x3 (lo usamos para cálcular la inversa).
	inline vector3f make_vector(float x, float y, float z) {
		vector3f vectorRes = { x, y, z };
		return vectorRes;
	}

	// Inicializador del vector4x4.
	inline vector4f make_vector(float x, float y, float z, float w) {
		vector4f vectorRes = { x, y, z, w };
		return vectorRes;
	}

	// Normalización del vector.
	inline vector4f normalize(vector4f v) {
		float aux = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		vector4f vectorRes = { v.x / aux, v.y / aux, v.z / aux, v.w };
		return vectorRes;
	}

	// Función para calcular la distancia entre dos vectores
	inline float distance(const vector4f& v1, const vector4f& v2) {
		float dx = v2.x - v1.x;
		float dy = v2.y - v1.y;
		float dz = v2.z - v1.z;
		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	// Tamaño del vector
	inline float length(vector4f v) {
		return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	}

	// Suma de vectores.
	inline vector4f operator+(vector4f v1, vector4f v2) {
		vector4f vectorRes = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, 1 };
		return vectorRes;
	}


	// Resta de vectores.
	inline vector4f operator-(vector4f v1, vector4f v2) {
		vector4f vectorRes = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, 1 };
		return vectorRes;
	}

	// Multiplicación de vectores (por componentes).
	inline float operator*(vector4f v1, vector4f v2) {
		float res = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		return res;
	}


	// Multiplicacion vectorial.
	inline vector4f operator^(vector4f v1, vector4f v2) {
		vector4f vectorRes = {
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x,
			0 //¿1 o 2?
		};
		return vectorRes;
	}

	// Multiplicacion escalar.
	inline vector4f operator*(float c, vector4f v1) {
		vector4f res = { v1.x * c, v1.y * c, v1.z * c, v1.w };
		return res;
	}

	// Multiplicacion float
	inline vector4f operator*(vector4f v, float s) {
		vector4f res = { 0, 0, 0, 0 };

		for (int i = 0; i < 4; i++) {
			res.data[i] = v.data[i] * s;
		}

		return res;
	}

	// División escalar.
	inline vector4f operator/(vector4f v1, float c) {
		vector4f res = { v1.x / c, v1.y / c, v1.z / c, v1.w };
		return res;
	}


	//					MATRIX4X4F
	// ------------------------------------------------

	// Metodo de creacion de matrices de tipo identidad
	inline matrix4x4f make_identity() {
		matrix4x4f matrixRes = {
			.rows = {
				{1,0,0,0},
				{0,1,0,0},
				{0,0,1,0},
				{0,0,0,1}
			}
		};
		return matrixRes;
	}

	// Metodo que crea una matriz de traslacion de N unidades en los ejes X, Y, Z respectivamente
	inline matrix4x4f make_translate(float X, float Y, float Z) {
		matrix4x4f matrixRes = {
			.rows = {
				{1,0,0,X},
				{0,1,0,Y},
				{0,0,1,Z},
				{0,0,0,1}
			}
		};
		return matrixRes;
	}

	// Multiplicacion de matrices.

	inline matrix3x3f operator*(const matrix3x3f m1, const matrix3x3f m2) {

		matrix3x3f result;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				result.mat2D[i][j] = 0;
				for (int k = 0; k < 3; k++) {
					result.mat2D[i][j] += m1.mat2D[i][k] * m2.mat2D[k][j];
				}
			}
		}

		return result;
	}

	inline matrix4x4f operator*(matrix4x4f m1, matrix4x4f m2) {

		matrix4x4f matrixRes = make_identity();

		for (int i = 0; i < 4; i++) {

			for (int j = 0; j < 4; j++) {
				matrixRes.mat2D[i][j] = 0;

				for (int k = 0; k < 4; k++)
					matrixRes.mat2D[i][j] += m1.mat2D[i][k] * m2.mat2D[k][j];

			}
		}
		return matrixRes;
	}

	// Multiplicación de matriz y vector.
	inline vector4f operator*(matrix4x4f m, vector4f v) {

		vector4f res{};

		for (int i = 0; i < 4; i++) {

			res.data[i] = 0;

			for (int j = 0; j < 4; j++)
				res.data[i] += m.mat2D[i][j] * v.data[j];

		}
		return res;
	}


	// Multiplicación de matriz por escalar.

	inline matrix4x4f operator*(float s, matrix4x4f m) {

		matrix4x4f matrixRes = make_identity();

		for (int i = 0; i < 4; i++) {

			for (int j = 0; j < 4; j++) {
				matrixRes.mat2D[i][j] = s * m.mat2D[i][j];
			}

		}
		return matrixRes;
	}

	// División de matriz y escalar.

	inline matrix4x4f operator/(matrix4x4f m, float s) {

		matrix4x4f matrixRes = make_identity();

		for (int i = 0; i < 4; i++) {

			for (int j = 0; j < 4; j++) {
				matrixRes.mat2D[i][j] = m.mat2D[i][j] / s;
			}

		}
		return matrixRes;
	}

	// Suma de matrices.

	inline matrix4x4f operator+(matrix4x4f m1, matrix4x4f m2) {

		matrix4x4f matrixRes = make_identity();

		for (int i = 0; i < 4; i++) {

			for (int j = 0; j < 4; j++)
				matrixRes.mat2D[i][j] = m1.mat2D[i][j] + m2.mat2D[i][j];

		}
		return matrixRes;
	}

	// Resta de matrices

	inline matrix4x4f operator-(matrix4x4f m1, matrix4x4f m2) {

		matrix4x4f matrixRes = make_identity();

		for (int i = 0; i < 4; i++) {

			for (int j = 0; j < 4; j++)
				matrixRes.mat2D[i][j] = m1.mat2D[i][j] - m2.mat2D[i][j];

		}
		return matrixRes;
	}

	// Metodo que crea una matriz de rotacion de N radianes en X, Y, Z respectivamente
	inline matrix4x4f make_rotate(float angleX, float angleY, float angleZ) {

		// Convertimos los ángulos de grados a radianes
		angleX = toRadians(angleX);
		angleY = toRadians(angleY);
		angleZ = toRadians(angleZ);

		// Inicializamos las matrices de rotación
		matrix4x4f matrixResX = make_identity();
		matrix4x4f matrixResY = make_identity();
		matrix4x4f matrixResZ = make_identity();

		// Rotación sobre el eje X
		if (angleX != 0) {
			matrixResX = {
				.rows = {
					{1, 0, 0, 0},
					{0, cos(angleX), -sin(angleX), 0},
					{0, sin(angleX), cos(angleX), 0},
					{0, 0, 0, 1}
				}
			};
		}

		// Rotación sobre el eje Y
		if (angleY != 0) {
			matrixResY = {
				.rows = {
					{cos(angleY), 0, sin(angleY), 0},
					{0, 1, 0, 0},
					{-sin(angleY), 0, cos(angleY), 0},
					{0, 0, 0, 1}
				}
			};
		}

		// Rotación sobre el eje Z
		if (angleZ != 0) {
			matrixResZ = {
				.rows = {
					{cos(angleZ), -sin(angleZ), 0, 0},
					{sin(angleZ), cos(angleZ), 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 1}
				}
			};
		}

		// Calculamos la matriz de rotación final
		matrix4x4f matrixRes = make_identity();
		matrixRes = matrixResX * matrixResY * matrixResZ;

		return matrixRes;
	}

	// Metodo que crea una matriz de escalado de N unidades en los ejes X, Y, Z respectivamente
	inline matrix4x4f make_scale(float X, float Y, float Z) {
		matrix4x4f matrixRes = {
			.rows = {
				{X,0,0,0},
				{0,Y,0,0},
				{0,0,Z,0},
				{0,0,0,1}
			}
		};
		return matrixRes;
	}


	// Transpuesta
	inline matrix4x4f transpose(matrix4x4f m) {

		matrix4x4f matrixRes = make_identity();

		for (int i = 0; i < 4; i++) {

			for (int j = 0; j < 4; j++)
				matrixRes.mat2D[i][j] = m.mat2D[j][i];

		}
		return matrixRes;
	}

	// Determinante 3x3.
	inline float determinant(matrix3x3f m) {
		return m.mat2D[0][0] * (m.mat2D[1][1] * m.mat2D[2][2] - m.mat2D[1][2] * m.mat2D[2][1])
			- m.mat2D[0][1] * (m.mat2D[1][0] * m.mat2D[2][2] - m.mat2D[1][2] * m.mat2D[2][0])
			+ m.mat2D[0][2] * (m.mat2D[1][0] * m.mat2D[2][1] - m.mat2D[1][1] * m.mat2D[2][0]);
	}

	// Determinante 4x4.
	inline float determinant(matrix4x4f m) {

		matrix3x3f m11;
		m11.mat2D[0][0] = m.mat2D[1][1]; m11.mat2D[0][1] = m.mat2D[1][2]; m11.mat2D[0][2] = m.mat2D[1][3];
		m11.mat2D[1][0] = m.mat2D[2][1]; m11.mat2D[1][1] = m.mat2D[2][2]; m11.mat2D[1][2] = m.mat2D[2][3];
		m11.mat2D[2][0] = m.mat2D[3][1]; m11.mat2D[2][1] = m.mat2D[3][2]; m11.mat2D[2][2] = m.mat2D[3][3];
		float m11Det = determinant(m11);

		matrix3x3f m12;
		m12.mat2D[0][0] = m.mat2D[1][0]; m12.mat2D[0][1] = m.mat2D[1][2]; m12.mat2D[0][2] = m.mat2D[1][3];
		m12.mat2D[1][0] = m.mat2D[2][0]; m12.mat2D[1][1] = m.mat2D[2][2]; m12.mat2D[1][2] = m.mat2D[2][3];
		m12.mat2D[2][0] = m.mat2D[3][0]; m12.mat2D[2][1] = m.mat2D[3][2]; m12.mat2D[2][2] = m.mat2D[3][3];
		float m12Det = determinant(m12);

		matrix3x3f m13;
		m13.mat2D[0][0] = m.mat2D[1][0]; m13.mat2D[0][1] = m.mat2D[1][1]; m13.mat2D[0][2] = m.mat2D[1][3];
		m13.mat2D[1][0] = m.mat2D[2][0]; m13.mat2D[1][1] = m.mat2D[2][1]; m13.mat2D[1][2] = m.mat2D[2][3];
		m13.mat2D[2][0] = m.mat2D[3][0]; m13.mat2D[2][1] = m.mat2D[3][1]; m13.mat2D[2][2] = m.mat2D[3][3];
		float m13Det = determinant(m13);

		matrix3x3f m14;
		m14.mat2D[0][0] = m.mat2D[1][0]; m14.mat2D[0][1] = m.mat2D[1][1]; m14.mat2D[0][2] = m.mat2D[1][2];
		m14.mat2D[1][0] = m.mat2D[2][0]; m14.mat2D[1][1] = m.mat2D[2][1]; m14.mat2D[1][2] = m.mat2D[2][2];
		m14.mat2D[2][0] = m.mat2D[3][0]; m14.mat2D[2][1] = m.mat2D[3][1]; m14.mat2D[2][2] = m.mat2D[3][2];
		float m14Det = determinant(m14);

		return m.mat2D[0][0] * m11Det - m.mat2D[0][1] * m12Det + m.mat2D[0][2] * m13Det - m.mat2D[0][3] * m14Det;
	}


	// Adjunto.
	inline matrix4x4f adjoint(matrix4x4f m) {

		// Podriamos hacer esto con for e if pero creo que las comprobaciones harían que fuese mas lento.
		// Como esto a la larga se va a usar para hacer cálculos muy recurrentemente, lo dejaré así.

		matrix3x3f m11;
		m11.mat2D[0][0] = m.mat2D[1][1]; m11.mat2D[0][1] = m.mat2D[1][2]; m11.mat2D[0][2] = m.mat2D[1][3];
		m11.mat2D[1][0] = m.mat2D[2][1]; m11.mat2D[1][1] = m.mat2D[2][2]; m11.mat2D[1][2] = m.mat2D[2][3];
		m11.mat2D[2][0] = m.mat2D[3][1]; m11.mat2D[2][1] = m.mat2D[3][2]; m11.mat2D[2][2] = m.mat2D[3][3];
		float m11Det = determinant(m11);

		matrix3x3f m12;
		m12.mat2D[0][0] = m.mat2D[1][0]; m12.mat2D[0][1] = m.mat2D[1][2]; m12.mat2D[0][2] = m.mat2D[1][3];
		m12.mat2D[1][0] = m.mat2D[2][0]; m12.mat2D[1][1] = m.mat2D[2][2]; m12.mat2D[1][2] = m.mat2D[2][3];
		m12.mat2D[2][0] = m.mat2D[3][0]; m12.mat2D[2][1] = m.mat2D[3][2]; m12.mat2D[2][2] = m.mat2D[3][3];
		float m12Det = determinant(m12);

		matrix3x3f m13;
		m13.mat2D[0][0] = m.mat2D[1][0]; m13.mat2D[0][1] = m.mat2D[1][1]; m13.mat2D[0][2] = m.mat2D[1][3];
		m13.mat2D[1][0] = m.mat2D[2][0]; m13.mat2D[1][1] = m.mat2D[2][1]; m13.mat2D[1][2] = m.mat2D[2][3];
		m13.mat2D[2][0] = m.mat2D[3][0]; m13.mat2D[2][1] = m.mat2D[3][1]; m13.mat2D[2][2] = m.mat2D[3][3];
		float m13Det = determinant(m13);

		matrix3x3f m14;
		m14.mat2D[0][0] = m.mat2D[1][0]; m14.mat2D[0][1] = m.mat2D[1][1]; m14.mat2D[0][2] = m.mat2D[1][2];
		m14.mat2D[1][0] = m.mat2D[2][0]; m14.mat2D[1][1] = m.mat2D[2][1]; m14.mat2D[1][2] = m.mat2D[2][2];
		m14.mat2D[2][0] = m.mat2D[3][0]; m14.mat2D[2][1] = m.mat2D[3][1]; m14.mat2D[2][2] = m.mat2D[3][2];
		float m14Det = determinant(m14);

		matrix3x3f m21;
		m21.mat2D[0][0] = m.mat2D[0][1]; m21.mat2D[0][1] = m.mat2D[0][2]; m21.mat2D[0][2] = m.mat2D[0][3];
		m21.mat2D[1][0] = m.mat2D[2][1]; m21.mat2D[1][1] = m.mat2D[2][2]; m21.mat2D[1][2] = m.mat2D[2][3];
		m21.mat2D[2][0] = m.mat2D[3][1]; m21.mat2D[2][1] = m.mat2D[3][2]; m21.mat2D[2][2] = m.mat2D[3][3];
		float m21Det = determinant(m21);

		matrix3x3f m22;
		m22.mat2D[0][0] = m.mat2D[0][0]; m22.mat2D[0][1] = m.mat2D[0][2]; m22.mat2D[0][2] = m.mat2D[0][3];
		m22.mat2D[1][0] = m.mat2D[2][0]; m22.mat2D[1][1] = m.mat2D[2][2]; m22.mat2D[1][2] = m.mat2D[2][3];
		m22.mat2D[2][0] = m.mat2D[3][0]; m22.mat2D[2][1] = m.mat2D[3][2]; m22.mat2D[2][2] = m.mat2D[3][3];
		float m22Det = determinant(m22);

		matrix3x3f m23;
		m23.mat2D[0][0] = m.mat2D[0][0]; m23.mat2D[0][1] = m.mat2D[0][1]; m23.mat2D[0][2] = m.mat2D[0][3];
		m23.mat2D[1][0] = m.mat2D[2][0]; m23.mat2D[1][1] = m.mat2D[2][1]; m23.mat2D[1][2] = m.mat2D[2][3];
		m23.mat2D[2][0] = m.mat2D[3][0]; m23.mat2D[2][1] = m.mat2D[3][1]; m23.mat2D[2][2] = m.mat2D[3][3];
		float m23Det = determinant(m23);

		matrix3x3f m24;
		m24.mat2D[0][0] = m.mat2D[0][0]; m24.mat2D[0][1] = m.mat2D[0][1]; m24.mat2D[0][2] = m.mat2D[0][2];
		m24.mat2D[1][0] = m.mat2D[2][0]; m24.mat2D[1][1] = m.mat2D[2][1]; m24.mat2D[1][2] = m.mat2D[2][2];
		m24.mat2D[2][0] = m.mat2D[3][0]; m24.mat2D[2][1] = m.mat2D[3][1]; m24.mat2D[2][2] = m.mat2D[3][2];
		float m24Det = determinant(m24);

		matrix3x3f m31;
		m31.mat2D[0][0] = m.mat2D[0][1]; m31.mat2D[0][1] = m.mat2D[0][2]; m31.mat2D[0][2] = m.mat2D[0][3];
		m31.mat2D[1][0] = m.mat2D[1][1]; m31.mat2D[1][1] = m.mat2D[1][2]; m31.mat2D[1][2] = m.mat2D[1][3];
		m31.mat2D[2][0] = m.mat2D[3][1]; m31.mat2D[2][1] = m.mat2D[3][2]; m31.mat2D[2][2] = m.mat2D[3][3];
		float m31Det = determinant(m31);

		matrix3x3f m32;
		m32.mat2D[0][0] = m.mat2D[0][0]; m32.mat2D[0][1] = m.mat2D[0][2]; m32.mat2D[0][2] = m.mat2D[0][3];
		m32.mat2D[1][0] = m.mat2D[1][0]; m32.mat2D[1][1] = m.mat2D[1][2]; m32.mat2D[1][2] = m.mat2D[1][3];
		m32.mat2D[2][0] = m.mat2D[3][0]; m32.mat2D[2][1] = m.mat2D[3][2]; m32.mat2D[2][2] = m.mat2D[3][3];
		float m32Det = determinant(m32);

		matrix3x3f m33;
		m33.mat2D[0][0] = m.mat2D[0][0]; m33.mat2D[0][1] = m.mat2D[0][1]; m33.mat2D[0][2] = m.mat2D[0][3];
		m33.mat2D[1][0] = m.mat2D[1][0]; m33.mat2D[1][1] = m.mat2D[1][1]; m33.mat2D[1][2] = m.mat2D[1][3];
		m33.mat2D[2][0] = m.mat2D[3][0]; m33.mat2D[2][1] = m.mat2D[3][1]; m33.mat2D[2][2] = m.mat2D[3][3];
		float m33Det = determinant(m33);

		matrix3x3f m34;
		m34.mat2D[0][0] = m.mat2D[0][0]; m34.mat2D[0][1] = m.mat2D[0][1]; m34.mat2D[0][2] = m.mat2D[0][2];
		m34.mat2D[1][0] = m.mat2D[1][0]; m34.mat2D[1][1] = m.mat2D[1][1]; m34.mat2D[1][2] = m.mat2D[1][2];
		m34.mat2D[2][0] = m.mat2D[3][0]; m34.mat2D[2][1] = m.mat2D[3][1]; m34.mat2D[2][2] = m.mat2D[3][2];
		float m34Det = determinant(m34);

		matrix3x3f m41;
		m41.mat2D[0][0] = m.mat2D[0][1]; m41.mat2D[0][1] = m.mat2D[0][2]; m41.mat2D[0][2] = m.mat2D[0][3];
		m41.mat2D[1][0] = m.mat2D[1][1]; m41.mat2D[1][1] = m.mat2D[1][2]; m41.mat2D[1][2] = m.mat2D[1][3];
		m41.mat2D[2][0] = m.mat2D[2][1]; m41.mat2D[2][1] = m.mat2D[2][2]; m41.mat2D[2][2] = m.mat2D[2][3];
		float m41Det = determinant(m41);

		matrix3x3f m42;
		m42.mat2D[0][0] = m.mat2D[0][0]; m42.mat2D[0][1] = m.mat2D[0][2]; m42.mat2D[0][2] = m.mat2D[0][3];
		m42.mat2D[1][0] = m.mat2D[1][0]; m42.mat2D[1][1] = m.mat2D[1][2]; m42.mat2D[1][2] = m.mat2D[1][3];
		m42.mat2D[2][0] = m.mat2D[2][0]; m42.mat2D[2][1] = m.mat2D[2][2]; m42.mat2D[2][2] = m.mat2D[2][3];
		float m42Det = determinant(m42);

		matrix3x3f m43;
		m43.mat2D[0][0] = m.mat2D[0][0]; m43.mat2D[0][1] = m.mat2D[0][1]; m43.mat2D[0][2] = m.mat2D[0][3];
		m43.mat2D[1][0] = m.mat2D[1][0]; m43.mat2D[1][1] = m.mat2D[1][1]; m43.mat2D[1][2] = m.mat2D[1][3];
		m43.mat2D[2][0] = m.mat2D[2][0]; m43.mat2D[2][1] = m.mat2D[2][1]; m43.mat2D[2][2] = m.mat2D[2][3];
		float m43Det = determinant(m43);

		matrix3x3f m44;
		m44.mat2D[0][0] = m.mat2D[0][0]; m44.mat2D[0][1] = m.mat2D[0][1]; m44.mat2D[0][2] = m.mat2D[0][2];
		m44.mat2D[1][0] = m.mat2D[1][0]; m44.mat2D[1][1] = m.mat2D[1][1]; m44.mat2D[1][2] = m.mat2D[1][2];
		m44.mat2D[2][0] = m.mat2D[2][0]; m44.mat2D[2][1] = m.mat2D[2][1]; m44.mat2D[2][2] = m.mat2D[2][2];
		float m44Det = determinant(m44);

		matrix4x4f adj;
		adj.mat2D[0][0] = m11Det;
		adj.mat2D[0][1] = -m12Det;
		adj.mat2D[0][2] = m13Det;
		adj.mat2D[0][3] = -m14Det;

		adj.mat2D[1][0] = -m21Det;
		adj.mat2D[1][1] = m22Det;
		adj.mat2D[1][2] = -m23Det;
		adj.mat2D[1][3] = m24Det;

		adj.mat2D[2][0] = m31Det;
		adj.mat2D[2][1] = -m32Det;
		adj.mat2D[2][2] = m33Det;
		adj.mat2D[2][3] = -m34Det;

		adj.mat2D[3][0] = -m41Det;
		adj.mat2D[3][1] = m42Det;
		adj.mat2D[3][2] = -m43Det;
		adj.mat2D[3][3] = m44Det;

		return adj;

	}

	// Inversa
	inline matrix4x4f inverse(matrix4x4f m) {

		float det = determinant(m);

		if (det == 0) {

			std::cerr << "Advertencia: La matriz no tiene inversa." << std::endl;
			return make_identity();
		}

		matrix4x4f adj = adjoint(m);
		matrix4x4f adjTrans = transpose(adj);
		return adjTrans / det;
	}

	inline vector4f make_quaternion(float x, float y, float z, float angle) {

		// Convertir el ángulo a radianes y calcular la mitad del ángulo
		float halfAngle = toRadians(angle) / 2;
		float w = cos(halfAngle);

		// Normalizar el vector de rotación
		vector4f normalizedVec = normalize(make_vector(x, y, z, 1));

		// Calcular el seno de la mitad del ángulo
		float s = sin(halfAngle);

		// Devolver el cuaternión como un vector4f
		return make_vector(normalizedVec.x * s, normalizedVec.y * s, normalizedVec.z * s, w);
	}

	inline matrix4x4f make_rotate_quaternion(vector4f quaternion) {

		float x = quaternion.x;
		float y = quaternion.y;
		float z = quaternion.z;
		float w = quaternion.w;

		// Calcular los elementos de la matriz de rotación 4x4
		matrix4x4f rst = { .rows{
			{1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w), 0},
			{2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w), 0},
			{2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y), 0},
			{0, 0, 0, 1}
		} };

		return rst;
	}

}