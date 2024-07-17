#include <bits/stdc++.h>
#include <GL/glut.h>
#include "bitmap_image.hpp"

using namespace std;

#define pi (2 * acos(0.0))

extern bitmap_image image;

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

    point(const point &p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
        w = p.w;
    }

    double length()
    {
        return sqrt(x * x + y * y + z * z);
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
    point operator+(point p)
    {
        return point(x + p.x, y + p.y, z + p.z);
    }

    point operator-(point p)
    {
        return point(x - p.x, y - p.y, z - p.z);
    }

    point operator*(double d)
    {
        return point(x * d, y * d, z * d);
    }

    point operator/(double d)
    {
        return point(x / d, y / d, z / d);
    }

    double operator*(point p)
    {
        // dot product of points
        return x * p.x + y * p.y + z * p.z;
    }

    point operator^(point p)
    {
        // cross product of points
        return point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
    }
    point operator-()
    {
        return point(-x, -y, -z);
    }

    // streams
    friend ostream &operator<<(ostream &out, point p)
    {
        out << p.x << " " << p.y << " " << p.z;
        return out;
    }

    friend istream &operator>>(istream &in, point &p)
    {
        in >> p.x >> p.y >> p.z;
        return in;
    }

    friend ofstream &operator<<(ofstream &out, point &p)
    {
        out << fixed << setprecision(7) << p.x << " " << p.y << " " << p.z;
        return out;
    }
};

// basic color structure

struct color
{
    double r, g, b;

    color()
    {
        r = 0;
        g = 0;
        b = 0;
    }

    color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

// basic pointlight structure

struct pointlight
{
    point pos;
    double fallOffParam;
    color col;

    pointlight()
    {
        col.r = 1;
        col.g = 1;
        col.b = 1;
    }

    // draw in openGL

    void draw()
    {
        glPointSize(3);
        glBegin(GL_POINTS);
        glColor3f(col.r, col.g, col.b);
        glVertex3f(pos.x, pos.y, pos.z);
        glEnd();
    }

    // input stream overload

    friend istream &operator>>(istream &in, pointlight &l)
    {
        in >> l.pos.x >> l.pos.y >> l.pos.z >> l.fallOffParam;
        return in;
    }
};

// basic spotlight structure

struct spotlight
{
    pointlight pl;
    point direction;
    double cutOffAngle;

    void draw()
    {
        point pos = pl.pos;
        double fallOffParam = pl.fallOffParam;

        glPointSize(10);
        glBegin(GL_POINTS);
        glColor3f(1, 1, 1);
        glVertex3f(pos.x, pos.y, pos.z);
        glEnd();
    }

    friend istream &operator>>(istream &in, spotlight &l)
    {
        in >> l.pl.pos.x >> l.pl.pos.y >> l.pl.pos.z >> l.pl.fallOffParam;
        in >> l.direction.x >> l.direction.y >> l.direction.z >> l.cutOffAngle;
        return in;
    }
};

// basic ray structure

struct ray
{
    point start, direction;

    ray(point start, point direction)
    {
        this->start = start;
        direction.normalize(); // why normalize? - to make sure the direction is a unit vector
        this->direction = direction;
    }
};

class object;
extern int recursionLevel;
extern vector<object *> objects;
extern vector<pointlight *> pointlights;
extern vector<spotlight *> spotlights;

class object
{
public:
    point reference_point;        // reference point
    double height, width, length; // dimensions
    color col;                    // color
    vector<double> co_efficients; // reflection co-efficients (ambient, diffuse, specular, reflection)
    int shine;                    // specular exponent

    // constructor
    object()
    {
        co_efficients = vector<double>(4, 0);
    }

    void printObject()
    {
        cout << "--------------------------\n";
        cout << "Reference point: " << reference_point << endl;
        cout << "Dimensions: " << height << " " << width << " " << length << endl;
        cout << "Color: " << col.r << " " << col.g << " " << col.b << endl;
        cout << "Co-efficients: ";
        for (int i = 0; i < 4; i++)
            cout << co_efficients[i] << " ";
        cout << endl;
        cout << "Shine: " << shine << endl;
    }

    void setColor(color col)
    {
        this->col = col;
    }

    virtual color getColorAt(point p)
    {
        return color(this->col.r, this->col.g, this->col.b);
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoefficents(vector<double> co_efficients)
    {
        this->co_efficients = co_efficients;
    }

    virtual void draw() = 0;
    virtual double intersectUtility(ray r, color &col, int level) = 0;
    virtual ray getNormal(point p, ray incident) = 0;

    virtual double intersect(ray r, color &col, int level)
    {
        double t = intersectUtility(r, col, level);
        // cout << t << endl;
        if (t < 0)
            return -1;

        if (level == 0)
            return t;

        point intersectionPoint = r.start + r.direction * t;
        color localColor = getColorAt(intersectionPoint);

        // set color to col
        col.r = localColor.r * co_efficients[0];
        col.g = localColor.g * co_efficients[0];
        col.b = localColor.b * co_efficients[0];

        // further light calculations

        // for all light sources

        for (int i = 0; i < pointlights.size(); i++)
        {
            point lightSource = pointlights[i]->pos;
            point direction = intersectionPoint - lightSource;
            direction.normalize();

            // casting the incident ray
            ray lightRay(lightSource, direction);

            // normal at the intersection point
            ray normal = getNormal(intersectionPoint, lightRay);

            // checking if the light is blocked

            double t2 = (intersectionPoint - lightSource).length();
            if (t2 < 1e-5)
                continue;
            bool blocked = false;

            for (object *obj : objects)
            {
                double t3 = obj->intersectUtility(lightRay, col, 0);
                if (t3 > 0 && t3 + 1e-5 < t2)
                {
                    blocked = true;
                    break;
                }
            }

            if (!blocked)
            {
                // lambert value
                double lambert = pow(max(-lightRay.direction * normal.direction, 0.0), shine);

                ray reflectionRay = ray(intersectionPoint, lightRay.direction - normal.direction * 2 * (lightRay.direction * normal.direction));
                reflectionRay.direction.normalize();

                // phong value
                double phong = pow(max(-r.direction * reflectionRay.direction, 0.0), shine);

                // adding the color, diffuse and specular
                // multiply with the scaling factor

                col.r += pointlights[i]->col.r * co_efficients[1] * lambert * localColor.r;
                col.r += pointlights[i]->col.r * co_efficients[2] * phong * localColor.r;

                col.g += pointlights[i]->col.g * co_efficients[1] * lambert * localColor.g;
                col.g += pointlights[i]->col.g * co_efficients[2] * phong * localColor.g;

                col.b += pointlights[i]->col.b * co_efficients[1] * lambert * localColor.b;
                col.b += pointlights[i]->col.b * co_efficients[2] * phong * localColor.b;
            }
        }

        for (int i = 0; i < spotlights.size(); i++)
        {
            point lightSource = spotlights[i]->pl.pos;
            point direction = intersectionPoint - lightSource;
            direction.normalize();

            double dot = direction * spotlights[i]->direction;
            double angle = acos(dot / (direction.length() * spotlights[i]->direction.length())) * (180.0 / pi);

            if (fabs(angle) < spotlights[i]->cutOffAngle)
            {
                ray lightRay = ray(lightSource, direction);
                ray normal = getNormal(intersectionPoint, lightRay);

                double t2 = (intersectionPoint - lightSource).length();
                if (t2 < 1e-5)
                    continue;

                bool blocked = false;

                for (object *obj : objects)
                {
                    double t3 = obj->intersectUtility(lightRay, col, 0);
                    if (t3 > 0 && t3 + 1e-5 < t2)
                    {
                        blocked = true;
                        break;
                    }
                }

                if (!blocked)
                {
                    double lambert = pow(max(0.0, -lightRay.direction * normal.direction), shine);
                    double phong = pow(max(-r.direction * lightRay.direction, 0.0), shine);

                    col.r += spotlights[i]->pl.col.r * co_efficients[1] * lambert * localColor.r;
                    col.r += spotlights[i]->pl.col.r * co_efficients[2] * phong * localColor.r;

                    col.g += spotlights[i]->pl.col.g * co_efficients[1] * lambert * localColor.g;
                    col.g += spotlights[i]->pl.col.g * co_efficients[2] * phong * localColor.g;

                    col.b += spotlights[i]->pl.col.b * co_efficients[1] * lambert * localColor.b;
                    col.b += spotlights[i]->pl.col.b * co_efficients[2] * phong * localColor.b;
                }
            }
        }

        // recursive reflection

        if (level < recursionLevel)
        {
            ray normal = getNormal(intersectionPoint, r);
            ray reflectedRay = ray(intersectionPoint, r.direction - normal.direction * 2 * (r.direction * normal.direction));

            // slightly forward
            reflectedRay.start = reflectedRay.start + reflectedRay.direction * 1e-5;

            int nearestObjectIndex = -1;
            double t = -1, tMin = 1e9;

            for (int k = 0; k < (int)objects.size(); k++)
            {
                t = objects[k]->intersect(reflectedRay, col, 0);
                if (t > 0 && t < tMin)
                    tMin = t, nearestObjectIndex = k;
            }

            if (nearestObjectIndex != -1)
            {
                color temp(0, 0, 0);
                double t = objects[nearestObjectIndex]->intersect(reflectedRay, temp, level + 1);
                col.r += temp.r * co_efficients[3];
                col.g += temp.g * co_efficients[3];
                col.b += temp.b * co_efficients[3];
            }
        }
        return t;
    }

    virtual ~object()
    {
        co_efficients.clear();
        co_efficients.shrink_to_fit();
    }
};

double determinant(double ara[3][3])
{
    double v1 = ara[0][0] * (ara[1][1] * ara[2][2] - ara[1][2] * ara[2][1]);
    double v2 = ara[0][1] * (ara[1][0] * ara[2][2] - ara[1][2] * ara[2][0]);
    double v3 = ara[0][2] * (ara[1][0] * ara[2][1] - ara[1][1] * ara[2][0]);
    return v1 - v2 + v3;
}

// triangle object

struct triangle : public object
{
    point a, b, c;
    triangle() {}
    triangle(point a, point b, point c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    virtual ray getNormal(point p, ray incident)
    {
        point normal = (b - a) ^ (c - a);
        normal.normalize();

        if (incident.direction * normal > 0)
            return ray(p, -normal);
        return ray(p, normal);
    }

    virtual void draw()
    {
        glColor3f(col.r, col.g, col.b);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        }
        glEnd();
    }

    virtual double intersectUtility(ray r, color &col, int level)
    {
        double betaMatrix[3][3] = {
            {a.x - r.start.x, a.x - c.x, r.direction.x},
            {a.y - r.start.y, a.y - c.y, r.direction.y},
            {a.z - r.start.z, a.z - c.z, r.direction.z}};

        double gammaMatrix[3][3] = {
            {a.x - b.x, a.x - r.start.x, r.direction.x},
            {a.y - b.y, a.y - r.start.y, r.direction.y},
            {a.z - b.z, a.z - r.start.z, r.direction.z}};

        double tMatrix[3][3] = {
            {a.x - b.x, a.x - c.x, a.x - r.start.x},
            {a.y - b.y, a.y - c.y, a.y - r.start.y},
            {a.z - b.z, a.z - c.z, a.z - r.start.z}};

        double AMatrix[3][3]{
            {a.x - b.x, a.x - c.x, r.direction.x},
            {a.y - b.y, a.y - c.y, r.direction.y},
            {a.z - b.z, a.z - c.z, r.direction.z}};

        double det = determinant(AMatrix);
        double beta = determinant(betaMatrix) / det;
        double gamma = determinant(gammaMatrix) / det;
        double t = determinant(tMatrix) / det;

        if (beta + gamma <= 1 && beta >= 0 && gamma >= 0 && t > 0)
            return t;
        return -1;
    }

    // set point
    void setPoint(point a, point b, point c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }
};

struct sphere : public object
{
    sphere() {}
    sphere(point center, double radius)
    {
        reference_point = center;
        length = radius;
    }

    virtual ray getNormal(point p, ray incident)
    {
        return ray(p, p - reference_point);
    }

    virtual void draw()
    {
        int stacks = 30;
        int slices = 20;
        point points[100][100];
        int i, j;
        double h, r;
        // generate points
        for (i = 0; i <= stacks; i++)
        {
            h = length * sin(((double)i / (double)stacks) * (pi / 2));
            r = length * cos(((double)i / (double)stacks) * (pi / 2));
            for (j = 0; j <= slices; j++)
            {
                points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
                points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
                points[i][j].z = h;
            }
        }

        // draw quads using generated points
        for (i = 0; i < stacks; i++)
        {
            glPushMatrix();
            glTranslatef(reference_point.x, reference_point.y, reference_point.z);
            glColor3f(col.r, col.g, col.b);
            for (j = 0; j < slices; j++)
            {
                glBegin(GL_QUADS);
                {
                    glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);

                    glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
                }
                glEnd();
            }
            glPopMatrix();
        }
    }

    virtual double intersectUtility(ray r, color &col, int level)
    {
        // cout << "Sphere intersecting" << endl;
        r.start = r.start - reference_point;
        double a = 1;
        double b = 2 * (r.direction * r.start);
        double c = r.start * r.start - length * length;

        double d = b * b - 4 * a * c;
        double t = -1;
        if (d < 0)
            return t;
        else
        {
            if (fabs(a) < 1e-5)
            {
                t = -c / b;
                return t;
            }

            double t1 = (-b - sqrt(d)) / (2 * a);
            double t2 = (-b + sqrt(d)) / (2 * a);

            if (t1 > t2)
                swap(t1, t2);

            if (t1 > 0)
                t = t1;
            else if (t2 > 0)
                t = t2;
            else
                t = -1;
        }
        return t;
    }

    friend istream &operator>>(istream &in, sphere &s)
    {
        in >> s.reference_point.x >> s.reference_point.y >> s.reference_point.z >> s.length; // center and radius
        in >> s.col.r >> s.col.g >> s.col.b;                                                 // color
        for (int i = 0; i < 4; i++)
            in >> s.co_efficients[i]; // reflection co-efficients
        in >> s.shine;                // specular exponent
        return in;
    }
};

struct checkerboard : public object
{
    int tiles;
    checkerboard() { tiles = 1; }
    checkerboard(int tileWidth)
    {
        tiles = 50;
        double w = tileWidth * tiles;
        reference_point = point(-w / 2, -w / 2, 0);
        length = tileWidth;
    }

    virtual color getColorAt(point p)
    {
        int x = (p.x - reference_point.x) / length;
        int y = (p.y - reference_point.y) / length;

        if (x < 0 || y < 0 || x >= tiles || y >= tiles)
            return color(0, 0, 0);
        if ((x + y) % 2 == 0)
            return color(1, 1, 1);
        else
            return color(0, 0, 0);
    }

    virtual ray getNormal(point p, ray incident)
    {
        if (incident.direction.z > 0)
            return ray(p, point(0, 0, 1));
        else
            return ray(p, point(0, 0, -1));
    }

    virtual void draw()
    {
        for (int i = 0; i < tiles; i++)
        {
            for (int j = 0; j < tiles; j++)
            {
                if ((i + j) % 2 == 0)
                    glColor3f(1, 1, 1);
                else
                    glColor3f(0, 0, 0);
                glBegin(GL_QUADS);
                {
                    glVertex3f(reference_point.x + i * length, reference_point.y + j * length, 0);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + j * length, 0);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + (j + 1) * length, 0);
                    glVertex3f(reference_point.x + i * length, reference_point.y + (j + 1) * length, 0);
                }
                glEnd();
            }
        }
    }

    virtual double intersectUtility(ray r, color &col, int level)
    {
        point normal = point(0, 0, 1);
        double dot = r.direction * normal;

        if (fabs(dot) < 1e-5)
            return -1;
        double t = -(r.start * normal) / dot;

        point intersectionPoint = r.start + r.direction * t;
        if (intersectionPoint.x <= reference_point.x || intersectionPoint.x >= abs(reference_point.x) && intersectionPoint.y <= reference_point.y && intersectionPoint.y >= abs(reference_point.y))
            return -1;
        return t;
    }
};
