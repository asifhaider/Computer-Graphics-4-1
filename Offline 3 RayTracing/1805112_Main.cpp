#include <bits/stdc++.h>
#include <GL/glut.h>
#include "1805112_Header.h"
using namespace std;

// global variables

double cameraHeight;
double cameraAngle;
bool gridOn;
bool axesOn;
double angle;

double nearDist;
double farDist;
double fovY;
double aspectRatio;

// global variables for ray tracing and shading

int recursionLevel;
int dimension;
bitmap_image image;

vector<object *> objects;
vector<pointlight *> pointlights;
vector<spotlight *> spotlights;

// camera positioning variables

point pos(0, -230, 60);
point u(0, 0, 1);
point r(1, 0, 0);
point l(0, 1, 0);
double rotation_angle = pi / 180;

double windowWidth = 500;
double windowHeight = 500;
double viewAngle = 80;

// draw axes

void drawAxes()
{
    if (axesOn)
    {
        glPointSize(3);
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        {
            glVertex3f(100, 0, 0);
            glVertex3f(-100, 0, 0);

            glVertex3f(0, -100, 0);
            glVertex3f(0, 100, 0);

            glVertex3f(0, 0, 100);
            glVertex3f(0, 0, -100);
        }
        glEnd();
    }
}

// draw grid

void drawGrid()
{
    int i;
    if (gridOn)
    {
        glColor3f(0.6, 0.6, 0.6); // grey
        glBegin(GL_LINES);
        {
            for (i = -8; i <= 8; i++)
            {
                if (i == 0)
                    continue; // skip the main axes

                // lines parallel to Y-axis
                glVertex3f(i * 10, -90, 0);
                glVertex3f(i * 10, 90, 0);

                // lines parallel to X-axis
                glVertex3f(-90, i * 10, 0);
                glVertex3f(90, i * 10, 0);
            }
        }
        glEnd();
    }
}

// rotation helper function
void rotate(point &a, point &b, double angle)
{
    a = a * cos(angle) + (b ^ a) * sin(angle);
}

void capture()
{
    cout << "Started Capturing" << endl;

    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
            image.set_pixel(i, j, 0, 0, 0);
    }

    double planeDistance = (windowHeight / 2.0) / tan((viewAngle * pi) / 360.0);
    point topleft = pos + l * planeDistance - r * (windowWidth / 2.0) + u * (windowHeight / 2.0);
    double dx = windowWidth / (dimension * 1.0);
    double dy = windowHeight / (dimension * 1.0);

    point mid = topleft + r * (dx / 2.0) - u * (dy / 2.0);
    point camera(pos.x, pos.y, pos.z);

    int nearestObjectIndex = -1;
    double t, minT;

    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            point current_pixel = mid + r * (i * dx) - u * (j * dy);
            point pixel(current_pixel.x, current_pixel.y, current_pixel.z);
            ray ray(camera, pixel - camera);
            ray.direction.normalize();
            color col(0, 0, 0);

            minT = INT_MAX;
            nearestObjectIndex = -1;

            for (int k = 0; k < objects.size(); k++)
            {
                t = objects[k]->intersect(ray, col, 0);
                if (t > 0 && (t < minT || nearestObjectIndex == -1))
                {
                    minT = t;
                    nearestObjectIndex = k;
                }
            }

            if (nearestObjectIndex != -1)
            {
                double t = objects[nearestObjectIndex]->intersect(ray, col, 1);
                col.r > 1 ? col.r = 1 : col.r < 0 ? col.r = 0
                                                  : col.r = col.r;
                col.g > 1 ? col.g = 1 : col.g < 0 ? col.g = 0
                                                  : col.g = col.g;
                col.b > 1 ? col.b = 1 : col.b < 0 ? col.b = 0
                                                  : col.b = col.b;

                image.set_pixel(i, j, col.r * 255, col.g * 255, col.b * 255);
            }
        }
    }

    image.save_image("1805112_output.bmp");
    cout << "Finished Capturing" << endl;
}

// keyboard listener

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '0':
        // capture
        capture();
        exit(0);
    case '1':
        // look left
        rotate(r, u, rotation_angle);
        rotate(l, u, rotation_angle);
        break;
    case '2':
        // look right
        rotate(r, u, -rotation_angle);
        rotate(l, u, -rotation_angle);
        break;
    case '3':
        // look up
        rotate(u, r, rotation_angle);
        rotate(l, r, rotation_angle);
        break;
    case '4':
        // look down
        rotate(u, r, -rotation_angle);
        rotate(l, r, -rotation_angle);
        break;
    case '5':
        // tilt clockwise
        rotate(r, l, rotation_angle);
        rotate(u, l, rotation_angle);
        break;
    case '6':
        // tilt anti-clockwise
        rotate(r, l, -rotation_angle);
        rotate(u, l, -rotation_angle);
        break;
    default:
        break;
    }
}

// special key listener

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN: // down arrow key
        pos = pos - l * 3;
        break;
    case GLUT_KEY_UP: // up arrow key
        pos = pos + l * 3;
        break;
    case GLUT_KEY_RIGHT: // right arrow key
        pos = pos + r * 3;
        break;
    case GLUT_KEY_LEFT: // left arrow key
        pos = pos - r * 3;
        break;
    case GLUT_KEY_PAGE_UP: // page up arrow key
        pos = pos + u * 3;
        break;
    case GLUT_KEY_PAGE_DOWN: // page down arrow key
        pos = pos - u * 3;
        break;
    default:
        break;
    }
}

// display function

void display()

{
    // clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0); // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera position, where is it looking, which direction is up
    gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);

    glMatrixMode(GL_MODELVIEW);

    // draw axes
    drawAxes();

    // draw grid
    drawGrid();

    // draw objects

    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }

    for (int i = 0; i < pointlights.size(); i++)
    {
        pointlights[i]->draw();
    }

    for (int i = 0; i < spotlights.size(); i++)
    {
        spotlights[i]->draw();
    }

    // this buffer is ready
    glutSwapBuffers();
}

void animate()
{
    glutPostRedisplay();
}

// read input description file
void readDescription()
{
    ifstream in("description.txt");

    // read near and far distances of the view plane
    in >> nearDist >> farDist;
    printf("%lf %lf\n", nearDist, farDist);

    // read fovY and aspectRatio
    in >> fovY;
    in >> aspectRatio;
    printf("%lf %lf\n", fovY, aspectRatio);

    // read recursion level
    in >> recursionLevel;
    printf("%d\n", recursionLevel);

    // read single dimension width and height
    in >> dimension;
    printf("%d\n", dimension);

    // read tilewidth
    int tileWidth;
    in >> tileWidth;
    printf("%d\n", tileWidth);

    // read checkerboard lighting coefficents
    double amb, diff, refl;
    in >> amb >> diff >> refl;
    printf("%lf %lf %lf\n", amb, diff, refl);

    // read objects

    // read number of objects
    int numberOfObjects;
    in >> numberOfObjects;

    for (int i = 0; i < numberOfObjects; i++)
    {
        string type;
        in >> type;

        if (type == "sphere")
        {
            object *obj;
            obj = new sphere();
            in >> *((sphere *)obj);
            cout << "sphere" << endl;
            objects.push_back(obj);
        }
        else if (type == "pyramid")
        {
            double x, y, z, w, h;
            in >> x >> y >> z >> w >> h;
            // take color input
            color col;
            in >> col.r >> col.g >> col.b;
            // take coefficents input
            double amb, diff, spec, refl;
            in >> amb >> diff >> spec >> refl;
            // take shine input
            double shine;
            in >> shine;

            point a(x, y, z); point b(x + w, y, z); point c(x + w, y + w, z);
            point d(x, y + w, z); point e(x + w / 2, y + w / 2, z + h);

            // create pyramid via 6 triangles
            object *t1 = new triangle(a, b, e); object *t2 = new triangle(b, c, e);
            object *t3 = new triangle(c, d, e); object *t4 = new triangle(d, a, e);
            object *t5 = new triangle(a, b, c); object *t6 = new triangle(a, c, d);

            // set color
            t1->setColor(col); t2->setColor(col);
            t3->setColor(col); t4->setColor(col);
            t5->setColor(col); t6->setColor(col);

            // set coefficents
            t1->setCoefficents(vector<double>{amb, diff, spec, refl}); t2->setCoefficents(vector<double>{amb, diff, spec, refl});
            t3->setCoefficents(vector<double>{amb, diff, spec, refl}); t4->setCoefficents(vector<double>{amb, diff, spec, refl});
            t5->setCoefficents(vector<double>{amb, diff, spec, refl}); t6->setCoefficents(vector<double>{amb, diff, spec, refl});

            // set shine
            t1->setShine(shine); t2->setShine(shine); t3->setShine(shine);
            t4->setShine(shine); t5->setShine(shine); t6->setShine(shine);

            // add to objects
            objects.push_back(t1); objects.push_back(t2);
            objects.push_back(t3); objects.push_back(t4);
            objects.push_back(t5); objects.push_back(t6);

            cout << "pyramid" << endl;
        }
        else if (type == "cube")
        {
            double x, y, z, side;
            color col;
            double amb, diff, spec, refl;
            double shine;

            in >> x >> y >> z >> side;
            in >> col.r >> col.g >> col.b;
            in >> amb >> diff >> spec >> refl;
            in >> shine;

            // create cube via 12 triangles
            point a(x, y, z);point b(x + side, y, z);point c(x + side, y + side, z);
            point d(x, y + side, z);point e(x, y, z + side);point f(x + side, y, z + side);
            point g(x + side, y + side, z + side);point h(x, y + side, z + side);
            
            // front face
            object *t1 = new triangle(a, b, c);object *t2 = new triangle(a, c, d);object *t3 = new triangle(e, f, g);
            object *t4 = new triangle(e, g, h);object *t5 = new triangle(a, b, f);object *t6 = new triangle(a, f, e);
            object *t7 = new triangle(b, c, g);object *t8 = new triangle(b, g, f);object *t9 = new triangle(c, d, h);
            object *t10 = new triangle(c, h, g);object *t11 = new triangle(d, a, e);object *t12 = new triangle(d, e, h);

            // set color
            t1->setColor(col);t2->setColor(col);t3->setColor(col);t4->setColor(col);
            t5->setColor(col);t6->setColor(col);t7->setColor(col);t8->setColor(col);
            t9->setColor(col);t10->setColor(col);t11->setColor(col);t12->setColor(col);

            // set coefficents
            t1->setCoefficents(vector<double>{amb, diff, spec, refl});t2->setCoefficents(vector<double>{amb, diff, spec, refl});
            t3->setCoefficents(vector<double>{amb, diff, spec, refl});t4->setCoefficents(vector<double>{amb, diff, spec, refl});
            t5->setCoefficents(vector<double>{amb, diff, spec, refl});t6->setCoefficents(vector<double>{amb, diff, spec, refl});
            t7->setCoefficents(vector<double>{amb, diff, spec, refl});t8->setCoefficents(vector<double>{amb, diff, spec, refl});
            t9->setCoefficents(vector<double>{amb, diff, spec, refl});t10->setCoefficents(vector<double>{amb, diff, spec, refl});
            t11->setCoefficents(vector<double>{amb, diff, spec, refl});t12->setCoefficents(vector<double>{amb, diff, spec, refl});

            // set shine
            t1->setShine(shine);t2->setShine(shine);t3->setShine(shine);
            t4->setShine(shine);t5->setShine(shine);t6->setShine(shine);
            t7->setShine(shine);t8->setShine(shine);t9->setShine(shine);
            t10->setShine(shine);t11->setShine(shine);t12->setShine(shine);

            // add to objects
            objects.push_back(t1);objects.push_back(t2);objects.push_back(t3);
            objects.push_back(t4);objects.push_back(t5);objects.push_back(t6);
            objects.push_back(t7);objects.push_back(t8);objects.push_back(t9);
            objects.push_back(t10);objects.push_back(t11);objects.push_back(t12);

            cout << "cube" << endl;
        }
        else
        {
            cout << "Invalid object type" << endl;
        }
    }

    int numberOfPointLights;
    in >> numberOfPointLights;

    for (int i = 0; i < numberOfPointLights; i++)
    {
        pointlight *pl = new pointlight();
        in >> *pl;
        pointlights.push_back(pl);
    }

    int numberOfSpotLights;
    in >> numberOfSpotLights;

    for (int i = 0; i < numberOfSpotLights; i++)
    {
        spotlight *sl = new spotlight();
        in >> *sl;
        spotlights.push_back(sl);
    }

    // checkerboard object named checkerboard
    object *board = new checkerboard(tileWidth);
    board->setColor(color(0.5, 0.5, 0.5));
    board->setCoefficents(vector<double>{amb, diff, 0, refl});
    objects.push_back(board);

    // print all objects
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->printObject();
    }
}

// all initializations

void init()
{
    gridOn = false;
    axesOn = true;
    cameraHeight = 150.0;
    cameraAngle = 1.0;
    angle = 0.0;

    // read description file
    readDescription();
    image = bitmap_image(dimension, dimension);

    // screen clear color
    glClearColor(0, 0, 0, 0);

    // projection initialization
    glMatrixMode(GL_PROJECTION);

    // load the identity matrix
    glLoadIdentity();

    // perspective projection
    gluPerspective(fovY, aspectRatio, nearDist, farDist);
}

int main(int args, char **argv)
{
    // initialize glut
    glutInit(&args, argv);
    // set window size
    glutInitWindowSize(800, 800);
    // set window position
    glutInitWindowPosition(100, 100);
    // set display mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // create window with the given title
    glutCreateWindow("Ray Tracing");

    // initialize display mode
    init();

    glEnable(GL_DEPTH_TEST);

    // set display function
    glutDisplayFunc(display);

    // set animation function
    glutIdleFunc(animate);

    // set keyboard function
    glutKeyboardFunc(keyboardListener);

    // set special keyboard function
    glutSpecialFunc(specialKeyListener);

    // set main loop
    glutMainLoop();

    // free memory
    objects.clear();
    objects.shrink_to_fit();
    pointlights.clear();
    pointlights.shrink_to_fit();

    return 0;
}
