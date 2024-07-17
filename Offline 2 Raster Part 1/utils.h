#include <bits/stdc++.h>
using namespace std;

// basic point structure

struct point
{
    double x, y, z, w;

    point()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 1;
    }

    point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1;
    }

    // normalize a point
    void normalize()
    {
        double magnitude = sqrt(x * x + y * y + z * z);

        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }

    // scale a point down
    void scaleDown()
    {
        x /= w;
        y /= w;
        z /= w;
        w /= w;
    }

    // overload operators
    point operator+(const point &p) const
    {
        return point(x + p.x, y + p.y, z + p.z);
    }

    point operator-(const point &p) const
    {
        return point(x - p.x, y - p.y, z - p.z);
    }

    point operator*(const double &d) const
    {
        return point(x * d, y * d, z * d);
    }

    double operator*(const point &p) const
    {
        // dot product of points
        return x * p.x + y * p.y + z * p.z;
    }

    point operator^(const point &p) const
    {
        // xor operation of points
        return point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
    }

};

// matrix structure

struct matrix
{
    vector<vector<double>> mat;

    // initialize as identity matrix
    matrix()
    {
        mat.resize(4, vector<double>(4, 0));
    }

    // create identity matrix
    void createIdentity()
    {
        mat.resize(4, vector<double>(4, 0));

        for (int i = 0; i < 4; i++)
        {
            mat[i][i] = 1;
        }
    }

    // create translation matrix
    void createTranslation(double tx, double ty, double tz)
    {
        createIdentity();
        mat[0][0] = 1;
        mat[1][1] = 1;
        mat[2][2] = 1;
        mat[3][3] = 1;

        mat[0][3] = tx;
        mat[1][3] = ty;
        mat[2][3] = tz;
    }

    // create scaling matrix
    void createScaling(double sx, double sy, double sz)
    {
        createIdentity();
        mat[0][0] = sx;
        mat[1][1] = sy;
        mat[2][2] = sz;
        mat[3][3] = 1;
    }

    // create rotation matrix
    void createRotation(double angle, double x, double y, double z)
    {
        createIdentity();   
        point axis(x, y, z);

        axis.normalize();
        point i(1, 0, 0), j(0, 1, 0), k(0, 0, 1);

        point c1 = applyRodriguesFormula(i, axis, angle);
        point c2 = applyRodriguesFormula(j, axis, angle);
        point c3 = applyRodriguesFormula(k, axis, angle);

        mat[0][0] = c1.x;
        mat[0][1] = c2.x;
        mat[0][2] = c3.x;

        mat[1][0] = c1.y;
        mat[1][1] = c2.y;
        mat[1][2] = c3.y;

        mat[2][0] = c1.z;
        mat[2][1] = c2.z;
        mat[2][2] = c3.z;

    }

    // apply rodrigues' rotation formula
    point applyRodriguesFormula(point p, point axis, double angle)
    {
        // convert angle to radians
        angle = angle * acos(-1) / 180.0;
        
        point result;
        result = p * cos(angle) + (axis ^ p) * sin(angle) + axis * (axis * p) * (1 - cos(angle));                
        return result;
    }

    // create view matrix
    void createViewMatrix(point eye, point look, point up)
    {
        createIdentity();

        point l = look - eye;
        l.normalize();

        point r = l ^ up;
        r.normalize();

        point u = r ^ l;
        u.normalize();

        mat[0][0] = r.x;
        mat[0][1] = r.y;
        mat[0][2] = r.z;

        mat[1][0] = u.x;
        mat[1][1] = u.y;
        mat[1][2] = u.z;

        mat[2][0] = -l.x;
        mat[2][1] = -l.y;
        mat[2][2] = -l.z;

        matrix T;
        T.createTranslation(-eye.x, -eye.y, -eye.z);

        *this = *this * T;
    }

    // create projection matrix
    void createProjectionMatrix(double fovY, double aspectRatio, double near, double far)
    {
        createIdentity();

        double fovX = fovY * aspectRatio;
        double t = near * tan(fovY * acos(-1) / 360.0);
        double r = near * tan(fovX * acos(-1) / 360.0);

        mat[0][0] = near / r;
        mat[1][1] = near / t;
        mat[2][2] = -(far + near) / (far - near);
        mat[2][3] = -(2 * far * near) / (far - near);
        mat[3][2] = -1;
        mat[3][3] = 0;
    }


    // matrix - matrix multiplication
    matrix operator*(const matrix &other) const
    {
        matrix result;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < 4; k++)
                {
                    result.mat[i][j] += mat[i][k] * other.mat[k][j];
                }
            }
        }

        return result;
    }

    // matrix - point multiplication
    point operator*(const point &other) const
    {
        point result;

        result.x = mat[0][0] * other.x + mat[0][1] * other.y + mat[0][2] * other.z + mat[0][3];
        result.y = mat[1][0] * other.x + mat[1][1] * other.y + mat[1][2] * other.z + mat[1][3];
        result.z = mat[2][0] * other.x + mat[2][1] * other.y + mat[2][2] * other.z + mat[2][3];
        result.w = mat[3][0] * other.x + mat[3][1] * other.y + mat[3][2] * other.z + mat[3][3];

        result.scaleDown();
        return result;
    }

    void print()
    {
        cout << "Matrix: " << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                cout << mat[i][j] << " ";
            }

            cout << endl;
        }
    }
};