#pragma once
#include <cmath>
#include <algorithm>
#define M_PI 3.141592654f
#define SQ(x) ((x) * (x))

template<typename T>
T _clamp(const T& v, const T& low, const T& high) {
    return (v < low) ? low : (v > high) ? high : v;
}

class Quaternion;

class Vec2
{
public:
    float x, y;

    Vec2(float _x = 0.0f, float _y = 0.0f)
        : x(_x), y(_y) {
    }

    Vec2 operator+(const Vec2& r) const { return { x + r.x, y + r.y }; }
    Vec2 operator-(const Vec2& r) const { return { x - r.x, y - r.y }; }
    Vec2 operator*(float s) const { return { x * s, y * s }; }
};

class Vec3 {
public:
    union {
        float v[3];
        struct { float x, y, z; };
    };

    Vec3(float xx = 0, float yy = 0, float zz = 0)
        : x(xx), y(yy), z(zz) {
    }

    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }

    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }

    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }

    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }

    Vec3 operator/(float s) const { float inverse = 1.0f / s; return Vec3(x * inverse, y * inverse, z * inverse); }

    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }

    Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

    Vec3& operator*=(const Vec3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }

    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    Vec3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

    float lengthSqrt() const { return (SQ(x) + SQ(y) + SQ(z)); }

    float length() const { return std::sqrt(lengthSqrt()); }

    Vec3 normalized() const {
        float len = length();
        if (len <= 0.0f) return Vec3(0, 0, 0);
        float inv = 1.0f / len;
        return Vec3(x * inv, y * inv, z * inv);
    }

	float dot(const Vec3& v) const { return (x * v.x + y * v.y + z * v.z); }
	Vec3 cross(const Vec3& v) const { return Vec3((y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x)); }

    static Vec3 Min(const Vec3& a, const Vec3& b) {
        return Vec3(
            (std::min)(a.x, b.x),
            (std::min)(a.y, b.y),
            (std::min)(a.z, b.z)
        );
    }

    static Vec3 Max(const Vec3& a, const Vec3& b) {
        return Vec3(
            (std::max)(a.x, b.x),
            (std::max)(a.y, b.y),
            (std::max)(a.z, b.z)
        );
    }

};

class alignas(64) Matrix {
public:
    union {
        float a[4][4];
        float m[16];
    };

    Matrix() { identity(); }

    Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) {
        a[0][0] = m00;
        a[0][1] = m01;
        a[0][2] = m02;
        a[0][3] = m03;
        a[1][0] = m10;
        a[1][1] = m11;
        a[1][2] = m12;
        a[1][3] = m13;
        a[2][0] = m20;
        a[2][1] = m21;
        a[2][2] = m22;
        a[2][3] = m23;
        a[3][0] = m30;
        a[3][1] = m31;
        a[3][2] = m32;
        a[3][3] = m33;
    }
    
    void identity() { memset(m, 0, 16 * sizeof(float)); m[0] = m[5] = m[10] = m[15] = 1.0f; }

    static Matrix Identity() { return Matrix(); }

    static Matrix Zero() {
        Matrix z;
        std::memset(z.m, 0, sizeof(z.m));
        return z;
    }

    static Matrix Translation(const Vec3& v) {
        Matrix output = Identity();
        output.a[0][3] = v.x;
        output.a[1][3] = v.y;
        output.a[2][3] = v.z;
        return output;
    }

    static Matrix Scale(const Vec3& v) {
        Matrix output = Identity();
        output.m[0] = v.x;
        output.m[5] = v.y;
        output.m[10] = v.z;
        return output;
    }

    static Matrix RotateX(float rad) {
        Matrix output = Identity();
        float ct = cosf(rad);
        float st = sinf(rad);
        output.m[5] = ct;
        output.m[6] = st;
        output.m[9] = -st;
        output.m[10] = ct;
        return output;
    }

    static Matrix RotateY(float rad) {
        Matrix output = Identity();
        float ct = cosf(rad);
        float st = sinf(rad);
        output.m[0] = ct;
        output.m[2] = -st;
        output.m[8] = st;
        output.m[10] = ct;
        return output;
    }

    static Matrix RotateZ(float rad) {
        Matrix output = Identity();
        float ct = cosf(rad);
        float st = sinf(rad);
        output.m[0] = ct;
        output.m[1] = st;
        output.m[4] = -st;
        output.m[5] = ct;
        return output;
    }

    float& operator[](int index) {
        return m[index];
    }

    Vec3 operator*(const Vec3& vec) const {
        Vec3 output;
        output.x = vec.x * m[0] + vec.y * m[1] + vec.z * m[2];
        output.y = vec.x * m[4] + vec.y * m[5] + vec.z * m[6];
        output.z = vec.x * m[8] + vec.y * m[9] + vec.z * m[10];
        return output;
	}

    Matrix operator*(const Matrix& matrix) const {
        Matrix output;

        output.m[0] = m[0] * matrix.m[0] + m[4] * matrix.m[1] + m[8] * matrix.m[2] + m[12] * matrix.m[3];
        output.m[1] = m[1] * matrix.m[0] + m[5] * matrix.m[1] + m[9] * matrix.m[2] + m[13] * matrix.m[3];
        output.m[2] = m[2] * matrix.m[0] + m[6] * matrix.m[1] + m[10] * matrix.m[2] + m[14] * matrix.m[3];
        output.m[3] = m[3] * matrix.m[0] + m[7] * matrix.m[1] + m[11] * matrix.m[2] + m[15] * matrix.m[3];

        output.m[4] = m[0] * matrix.m[4] + m[4] * matrix.m[5] + m[8] * matrix.m[6] + m[12] * matrix.m[7];
        output.m[5] = m[1] * matrix.m[4] + m[5] * matrix.m[5] + m[9] * matrix.m[6] + m[13] * matrix.m[7];
        output.m[6] = m[2] * matrix.m[4] + m[6] * matrix.m[5] + m[10] * matrix.m[6] + m[14] * matrix.m[7];
        output.m[7] = m[3] * matrix.m[4] + m[7] * matrix.m[5] + m[11] * matrix.m[6] + m[15] * matrix.m[7];

        output.m[8] = m[0] * matrix.m[8] + m[4] * matrix.m[9] + m[8] * matrix.m[10] + m[12] * matrix.m[11];
        output.m[9] = m[1] * matrix.m[8] + m[5] * matrix.m[9] + m[9] * matrix.m[10] + m[13] * matrix.m[11];
        output.m[10] = m[2] * matrix.m[8] + m[6] * matrix.m[9] + m[10] * matrix.m[10] + m[14] * matrix.m[11];
        output.m[11] = m[3] * matrix.m[8] + m[7] * matrix.m[9] + m[11] * matrix.m[10] + m[15] * matrix.m[11];

        output.m[12] = m[0] * matrix.m[12] + m[4] * matrix.m[13] + m[8] * matrix.m[14] + m[12] * matrix.m[15];
        output.m[13] = m[1] * matrix.m[12] + m[5] * matrix.m[13] + m[9] * matrix.m[14] + m[13] * matrix.m[15];
        output.m[14] = m[2] * matrix.m[12] + m[6] * matrix.m[13] + m[10] * matrix.m[14] + m[14] * matrix.m[15];
        output.m[15] = m[3] * matrix.m[12] + m[7] * matrix.m[13] + m[11] * matrix.m[14] + m[15] * matrix.m[15];

        return output;
    }

    Matrix invert() {
		Matrix inv = Matrix::Zero();
        inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
        inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
        inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
        inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
        inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
        inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
        inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
        inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
        inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
        inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
        inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
        inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
        inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
        inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
        inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
        inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
        
        float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
        if (det == 0) 
            exit(0);
        det = 1.0 / det;
        for (int i = 0; i < 16; i++) 
            inv[i] = inv[i] * det;
        return inv;
    }

    Matrix transpose() {
        return Matrix(
            a[0][0], a[1][0], a[2][0], a[3][0],
            a[0][1], a[1][1], a[2][1], a[3][1],
            a[0][2], a[1][2], a[2][2], a[3][2],
            a[0][3], a[1][3], a[2][3], a[3][3]);
    }

	// Right-handed lookAt matrix
	// Forward vector points towards +Z
    static Matrix lookAt(const Vec3& from, const Vec3& to, const Vec3& up) {
        Matrix view;
        Vec3 dir = (to - from).normalized();
        Vec3 left = up.cross(dir).normalized();
        Vec3 newUp = dir.cross(left).normalized();
        view.a[0][0] = left.x;
        view.a[0][1] = left.y;
        view.a[0][2] = left.z;
        view.a[1][0] = newUp.x;
        view.a[1][1] = newUp.y;
        view.a[1][2] = newUp.z;
        view.a[2][0] = dir.x;
        view.a[2][1] = dir.y;
        view.a[2][2] = dir.z;
        view.a[0][3] = -from.dot(left);
        view.a[1][3] = -from.dot(newUp);
        view.a[2][3] = -from.dot(dir);
        view.a[3][3] = 1;
        return view;
    }

	// Right-handed perspective projection matrix
    static Matrix perspective(float n, float f, float aspect, float fovDeg) {
        Matrix pers = Matrix::Zero();
        float t = 1.0f / (tanf(fovDeg * 0.5f * 3.141592654f / 180.0f));
        pers.a[0][0] = t / aspect;
        pers.a[1][1] = t;
        pers.a[2][2] = f / (f - n);
        pers.a[2][3] = -(f * n) / (f - n);
        pers.a[3][2] = 1.0f;
        return pers;
    }

    Matrix withoutTranslation() const {
        Matrix r = *this;
        r.a[0][3] = 0.0f;
        r.a[1][3] = 0.0f;
        r.a[2][3] = 0.0f;
        return r;
    }
};

class Quaternion {
public:
    union {
        struct { float x, y, z, w; };
        float q[4];
    };

    Quaternion() : x(0), y(0), z(0), w(1) {}

    Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    float length() const { return std::sqrt(SQ(x) + SQ(y) + SQ(z) + SQ(w)); }

    Quaternion normalized() const {
        float len = length();
        if (len <= 0.0f) return Quaternion(0, 0, 0, 1);
        float inv = 1.0f / len;
        return Quaternion(x * inv, y * inv, z * inv, w * inv);
    }

    Quaternion conjugate() const { return Quaternion(-x, -y, -z, w); }

    Quaternion inverse() const {
        Quaternion n = normalized();
        return n.conjugate();
    }

    Quaternion operator*(const Quaternion& r) const {
        return Quaternion(
            w * r.x + x * r.w + y * r.z - z * r.y,
            w * r.y - x * r.z + y * r.w + z * r.x,
            w * r.z + x * r.y - y * r.x + z * r.w,
            w * r.w - x * r.x - y * r.y - z * r.z
        );
    }

    static Quaternion fromAxisAngle(const Vec3& axis, float rad) {
        Vec3 u = axis.normalized();
        float s = std::sinf(rad * 0.5f);
        return Quaternion(u.x * s, u.y * s, u.z * s, std::cosf(rad * 0.5f));
    }

    Vec3 rotate(const Vec3& v) const {
        Quaternion p(v.x, v.y, v.z, 0);
        Quaternion r = (*this) * p * inverse();
        return Vec3(r.x, r.y, r.z);
    }

    Matrix toMatrix() const {
        Quaternion n = normalized();
        float xx = n.x * n.x, yy = n.y * n.y, zz = n.z * n.z;
        float xy = n.x * n.y, xz = n.x * n.z, yz = n.y * n.z;
        float wx = n.w * n.x, wy = n.w * n.y, wz = n.w * n.z;
        Matrix m;
        m[0] = 1 - 2 * (yy + zz);
        m[1] = 2 * (xy - wz);
        m[2] = 2 * (xz + wy);
        m[3] = 0;
        m[4] = 2 * (xy + wz);
        m[5] = 1 - 2 * (xx + zz);
        m[6] = 2 * (yz - wx);
        m[7] = 0;
        m[8] = 2 * (xz - wy);
        m[9] = 2 * (yz + wx);
        m[10] = 1 - 2 * (xx + yy);
        m[11] = 0;
        m[12] = m[13] = m[14] = 0;
        m[15] = 1;
        return m;
    }

    static Quaternion slerp(const Quaternion& a, const Quaternion& b, float t) {
        Quaternion q1 = a.normalized();
        Quaternion q2 = b.normalized();
        float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
        if (dot < 0.0f) { dot = -dot; q2 = Quaternion(-q2.x, -q2.y, -q2.z, -q2.w); }
        if (dot > 0.9995f) {
            Quaternion r(
                q1.x + t * (q2.x - q1.x),
                q1.y + t * (q2.y - q1.y),
                q1.z + t * (q2.z - q1.z),
                q1.w + t * (q2.w - q1.w));
            return r.normalized();
        }
        float theta = std::acos(_clamp(dot, -1.0f, 1.0f));
        float s0 = std::sinf((1 - t) * theta) / std::sinf(theta);
        float s1 = std::sinf(t * theta) / std::sinf(theta);
        return Quaternion(
            s0 * q1.x + s1 * q2.x,
            s0 * q1.y + s1 * q2.y,
            s0 * q1.z + s1 * q2.z,
            s0 * q1.w + s1 * q2.w);
    }

};

class Colour {
public:
    float r, g, b, a;

    Colour() : r(0), g(0), b(0), a(1) {}
    Colour(float R, float G, float B, float A = 1.0f)
        : r(R), g(G), b(B), a(A) {
    }

    Colour operator+(const Colour& c) const {
        return Colour(r + c.r, g + c.g, b + c.b, a + c.a);
    }

    Colour operator*(const Colour& c) const {
        return Colour(r * c.r, g * c.g, b * c.b, a * c.a);
    }

    Colour operator*(float s) const {
        return Colour(r * s, g * s, b * s, a * s);
    }

    Colour operator/(float s) const {
        float inv = 1.0f / s;
        return Colour(r * inv, g * inv, b * inv, a * inv);
    }
};