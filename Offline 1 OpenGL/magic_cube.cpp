#include <GL/glut.h>
#include <bits/stdc++.h>
#include "Cubesphere.cpp"

using namespace std;

#define CUBE_SPHERE_MAX_POINT 65

struct point
{
    GLfloat x, y, z;
};

// GLfloat eyex = 4, eyey = 4, eyez = 4;
// GLfloat centerx = 0, centery = 0, centerz = 0;
// GLfloat upx = 0, upy = 1, upz = 0;

struct point pos; // position of the eye
struct point L, R, U;
struct point l; // look/forward direction
struct point r; // right direction
struct point u; // up direction

point vertices[CUBE_SPHERE_MAX_POINT][CUBE_SPHERE_MAX_POINT];
int stacks = CUBE_SPHERE_MAX_POINT, slices = CUBE_SPHERE_MAX_POINT;

/* Initialize OpenGL Graphics */
void initGL()
{
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}

float scale = 1, scale_steps = 0.0625, base = 1;
float sphere_radius = base / sqrt(3);
float cylinder_angle = 70.5287794;

float scaled(float x)
{
    return (x - (1 / 3.0));
}

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes()
{
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); // Red
    // X axis
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);

    glColor3f(0, 1, 0); // Green
    // Y axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);

    glColor3f(0, 0, 1); // Blue
    // Z axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);
    glEnd();
}

// draw the base triangle with vertices at (1,0,0), (0,1,0) and (0,0,1)
void drawTriangle()
{
    glPushMatrix();
    glScalef(scale, scale, scale);

    // translation needed later
    glTranslatef(1 / (3.0 * scale), 1 / (3.0 * scale), 1 / (3.0 * scale));

    glBegin(GL_TRIANGLES);
    glVertex3f(scaled(1), scaled(0), scaled(0));
    glVertex3f(scaled(0), scaled(1), scaled(0));
    glVertex3f(scaled(0), scaled(0), scaled(1));
    glEnd();

    glPopMatrix();
}

void drawOctahedron()
{
    glPushMatrix();
    // cyan color
    glColor3f(0, 1, 1);
    // draw the upper top of the pyramid
    drawTriangle();

    // rotate about 90 degrees about the y-axis and re-draw the triangle
    glRotatef(90, 0, 1, 0);
    // pink color
    glColor3f(1, 0, 1);
    drawTriangle();

    // rotate about 90 degrees about the y-axis and re-draw the triangle
    glRotatef(90, 0, 1, 0);
    // cyan color
    glColor3f(0, 1, 1);
    drawTriangle();

    // rotate about 90 degrees about the y-axis and re-draw the triangle
    glRotatef(90, 0, 1, 0);
    // pink color
    glColor3f(1, 0, 1);
    drawTriangle();

    // rotate about 180 degrees about the x-axis and re-draw the triangle
    glRotatef(180, 1, 0, 0);
    // pink color
    glColor3f(1, 0, 1);
    drawTriangle();

    // rotate about 90 degrees about the y-axis and re-draw the triangle
    glRotatef(90, 0, 1, 0);
    // cyan color
    glColor3f(0, 1, 1);
    drawTriangle();
    // rotate about 90 degrees about the y-axis and re-draw the triangle
    glRotatef(90, 0, 1, 0);
    // pink color
    glColor3f(1, 0, 1);
    drawTriangle();
    // rotate about 90 degrees about the y-axis and re-draw the triangle
    glRotatef(90, 0, 1, 0);
    // cyan color
    glColor3f(0, 1, 1);
    drawTriangle();
    glPopMatrix();
}

void drawCubeSpheres()
{
    glPushMatrix();
    float current_radius = (1 - scale) * sphere_radius;
    float initial_center = 1.0f;
    float current_center = initial_center * scale;

    Cubesphere sphere(current_radius, 3);

    glTranslatef(current_center, 0, 0);
    glColor3f(0, 0, 1);
    sphere.drawFace(0);

    glTranslatef(-current_center, 0, 0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(current_center, 0, 0);
    glColor3f(0, 1, 0);
    sphere.drawFace(0);

    glTranslatef(-current_center, 0, 0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(current_center, 0, 0);
    glColor3f(0, 0, 1);
    sphere.drawFace(0);

    glTranslatef(-current_center, 0, 0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(current_center, 0, 0);
    glColor3f(0, 1, 0);
    sphere.drawFace(0);

    glTranslatef(-current_center, 0, 0);
    glRotatef(90, 0, 0, 1);
    glTranslatef(current_center, 0, 0);
    glColor3f(1, 0, 0);
    sphere.drawFace(0);

    glTranslatef(-current_center, 0, 0);
    glRotatef(180, 0, 0, 1);
    glTranslatef(current_center, 0, 0);
    glColor3f(1, 0, 0);
    sphere.drawFace(0);

    glPopMatrix();
}

void drawCylinder(double height, double radius, int segments)
{
    glPushMatrix();
    double tempx = radius, tempy = 0;
    double currx, curry;
    glBegin(GL_QUADS);
    for (int i = 1; i <= segments; i++)
    {
        double theta = i * cylinder_angle * (M_PI / 180) / segments;
        currx = radius * cos(theta);
        curry = radius * sin(theta);

        // yellow color
        glColor3f(1, 1, 0);
        glVertex3f(currx, curry, height / 2);
        glVertex3f(currx, curry, -height / 2);

        glVertex3f(tempx, tempy, -height / 2);
        glVertex3f(tempx, tempy, height / 2);

        tempx = currx;
        tempy = curry;
    }
    glEnd();
    glPopMatrix();
}

void fixCylinder()
{
    glPushMatrix();
    float current_radius = (1 - scale) * sphere_radius;
    float d = current_radius / (sin(((180 - cylinder_angle) / 2) * (M_PI / 180)));
    glRotatef(-45, 0, 1, 0);
    glTranslatef(-d + (1 / sqrt(2)), 0, 0);
    glRotatef(-cylinder_angle / 2, 0, 0, 1);
    drawCylinder(scale * sqrt(2), current_radius, 30);
    glPopMatrix();
}

void fillGap()
{
    fixCylinder();
    glRotatef(90, 0, 1, 0);
    fixCylinder();
    glRotatef(90, 0, 1, 0);
    fixCylinder();
    glRotatef(90, 0, 1, 0);
    fixCylinder();

    glRotatef(90, 1, 0, 0);
    fixCylinder();
    glRotatef(90, 0, 0, 1);
    fixCylinder();
    glRotatef(90, 0, 0, 1);
    fixCylinder();
    glRotatef(90, 0, 0, 1);
    fixCylinder();

    glRotatef(180, 1, 0, 0);
    fixCylinder();
    glRotatef(90, 0, 0, 1);
    fixCylinder();
    glRotatef(90, 0, 0, 1);
    fixCylinder();
    glRotatef(90, 0, 0, 1);
    fixCylinder();
}

void display()
{
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
    glLoadIdentity();           // Reset the model-view matrix

    // default arguments of gluLookAt
    // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

    // control viewing (or camera)
    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + l.x, pos.y + l.y, pos.z + l.z,
              u.x, u.y, u.z);

    // gluLookAt(eyex,eyey,eyez,
    //           centerx,centery,centerz,
    //           upx,upy,upz);

    // drawAxes();
    drawOctahedron();
    drawCubeSpheres();
    fillGap();

    glutSwapBuffers(); // Render now
}

void reshapeListener(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset the projection matrix
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void keyboardListener(unsigned char key, int xx, int yy)
{
    double rate = 0.01, s;
    switch (key)
    {

    case '1':
        pos.x -= r.x;
        pos.y -= r.y;
        pos.z -= r.z;
        break;

    case '2':
        pos.x += r.x;
        pos.y += r.y;
        pos.z += r.z;
        break;

    case '3':
        // pos.y += rate;
        pos.x += u.x;
        pos.y += u.y;
        pos.z += u.z;
        break;
    case '4':
        // pos.y -= rate;
        pos.x -= u.x;
        pos.y -= u.y;
        pos.z -= u.z;
        break;

    case 'w':
        break;

    case 's':
        break;

    case '5':
        u.x = u.x * cos(rate) + r.x * sin(rate);
        u.y = u.y * cos(rate) + r.y * sin(rate);
        u.z = u.z * cos(rate) + r.z * sin(rate);

        r.x = r.x * cos(rate) - u.x * sin(rate);
        r.y = r.y * cos(rate) - u.y * sin(rate);
        r.z = r.z * cos(rate) - u.z * sin(rate);
        break;

    case '6':
        u.x = u.x * cos(-rate) + r.x * sin(-rate);
        u.y = u.y * cos(-rate) + r.y * sin(-rate);
        u.z = u.z * cos(-rate) + r.z * sin(-rate);

        r.x = r.x * cos(-rate) - u.x * sin(-rate);
        r.y = r.y * cos(-rate) - u.y * sin(-rate);
        r.z = r.z * cos(-rate) - u.z * sin(-rate);
        break;


    case '.':
        if (scale <= 1 - scale_steps)
            scale += scale_steps;
        break;
    case ',':
        if (scale >= scale_steps)
            scale -= scale_steps;
        break;
    case 27: // ESC key
        exit(0);
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y)
{
    double rate = 0.01;
    switch (key)
    {
    case GLUT_KEY_PAGE_DOWN: // down arrow key
        l.x = l.x * cos(rate) + u.x * sin(rate);
        l.y = l.y * cos(rate) + u.y * sin(rate);
        l.z = l.z * cos(rate) + u.z * sin(rate);

        u.x = u.x * cos(rate) - l.x * sin(rate);
        u.y = u.y * cos(rate) - l.y * sin(rate);
        u.z = u.z * cos(rate) - l.z * sin(rate);

        break;
    case GLUT_KEY_PAGE_UP: // up arrow key
        l.x = l.x * cos(-rate) + u.x * sin(-rate);
        l.y = l.y * cos(-rate) + u.y * sin(-rate);
        l.z = l.z * cos(-rate) + u.z * sin(-rate);

        u.x = u.x * cos(-rate) - l.x * sin(-rate);
        u.y = u.y * cos(-rate) - l.y * sin(-rate);
        u.z = u.z * cos(-rate) - l.z * sin(-rate);

        break;

    case GLUT_KEY_RIGHT:
        r.x = r.x * cos(rate) + l.x * sin(rate);
        r.y = r.y * cos(rate) + l.y * sin(rate);
        r.z = r.z * cos(rate) + l.z * sin(rate);

        l.x = l.x * cos(rate) - r.x * sin(rate);
        l.y = l.y * cos(rate) - r.y * sin(rate);
        l.z = l.z * cos(rate) - r.z * sin(rate);

        break;
    case GLUT_KEY_LEFT:
        r.x = r.x * cos(-rate) + l.x * sin(-rate);
        r.y = r.y * cos(-rate) + l.y * sin(-rate);
        r.z = r.z * cos(-rate) + l.z * sin(-rate);

        l.x = l.x * cos(-rate) - r.x * sin(-rate);
        l.y = l.y * cos(-rate) - r.y * sin(-rate);
        l.z = l.z * cos(-rate) - r.z * sin(-rate);
        break;

    case GLUT_KEY_UP:
        pos.x += l.x;
        pos.y += l.y;
        pos.z += l.z;

        break;
    case GLUT_KEY_DOWN:
        // pos.x -= u.x;
        // pos.y -= u.y;
        // pos.z -= u.z;
        pos.x -= l.x;
        pos.y -= l.y;
        pos.z -= l.z;

        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

// /* Callback handler for normal-key event */
// void keyboardListener(unsigned char key, int x, int y) {
//     float v = 0.1;
//     switch (key) {
//     // Control eye (location of the eye)
//     // control eyex
//     case '.':
//         if (scale <= 1 - scale_steps)
//             scale += scale_steps;
//         break;
//     case ',':
//         if (scale >= scale_steps)
//             scale -= scale_steps;
//         break;
//     case '1':
//         eyex += v;
//         break;
//     case '2':
//         eyex -= v;
//         break;
//     // control eyey
//     case '3':
//         eyey += v;
//         break;
//     case '4':
//         eyey -= v;
//         break;
//     // control eyez
//     case '5':
//         eyez += v;
//         break;
//     case '6':
//         eyez -= v;
//         break;

//     // Control center (location where the eye is looking at)
//     // control centerx
//     case 'q':
//         centerx += v;
//         break;
//     case 'w':
//         centerx -= v;
//         break;
//     // control centery
//     case 'e':
//         centery += v;
//         break;
//     case 'r':
//         centery -= v;
//         break;
//     // control centerz
//     case 't':
//         centerz += v;
//         break;
//     case 'y':
//         centerz -= v;
//         break;

//     // Control exit
//     case 27:    // ESC key
//         exit(0);    // Exit window
//         break;
//     }
//     glutPostRedisplay();    // Post a paint request to activate display()
// }

// /* Callback handler for special-key event */
// void specialKeyListener(int key, int x,int y) {
//     double v = 0.25;
//     double lx = centerx - eyex;
//     double lz = centerz - eyez;
//     double s;
//     switch (key) {
//     case GLUT_KEY_LEFT:
//         eyex += v * (upy*lz);
//         eyez += v * (-lx*upy);
//         s = sqrt(eyex*eyex + eyez*eyez) / (4 * sqrt(2));
//         eyex /= s;
//         eyez /= s;
//         break;
//     case GLUT_KEY_RIGHT:
//         eyex += v * (-upy*lz);
//         eyez += v * (lx*upy);
//         s = sqrt(eyex*eyex + eyez*eyez) / (4 * sqrt(2));
//         eyex /= s;
//         eyez /= s;
//         break;
//     case GLUT_KEY_UP:
//         eyey += v;
//         break;
//     case GLUT_KEY_DOWN:
//         eyey -= v;
//         break;
    
//     default:
//         return;
//     }
//     glutPostRedisplay();    // Post a paint request to activate display()
// }

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    pos.x = 3;
    pos.y = 3;
    pos.z = 3;
    L.x = -2;
    L.y = -2;
    L.z = -2;
    U.x = -2;
    U.y = -2 * L.y;
    U.z = -2;
    // l is the unit vector of L
    l.x = L.x / sqrt(L.x * L.x + L.y * L.y + L.z * L.z);
    l.y = L.y / sqrt(L.x * L.x + L.y * L.y + L.z * L.z);
    l.z = L.z / sqrt(L.x * L.x + L.y * L.y + L.z * L.z);
    // u is the unit vector of U
    u.x = U.x / sqrt(U.x * U.x + U.y * U.y + U.z * U.z);
    u.y = U.y / sqrt(U.x * U.x + U.y * U.y + U.z * U.z);
    u.z = U.z / sqrt(U.x * U.x + U.y * U.y + U.z * U.z);

    // R is the cross product of L and U
    R.x = L.y * U.z - L.z * U.y;
    R.y = L.z * U.x - L.x * U.z;
    R.z = L.x * U.y - L.y * U.x;
    // r is the unit vector of R
    r.x = R.x / sqrt(R.x * R.x + R.y * R.y + R.z * R.z);
    r.y = R.y / sqrt(R.x * R.x + R.y * R.y + R.z * R.z);
    r.z = R.z / sqrt(R.x * R.x + R.y * R.y + R.z * R.z);

    glutInit(&argc, argv);                                    // Initialize GLUT
    glutInitWindowSize(640, 640);                             // Set the window's initial width & height
    glutInitWindowPosition(800, 200);                         // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
    glutCreateWindow("Magic Cube");                           // Create a window with the given title
    glutDisplayFunc(display);                                 // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);                         // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);                       // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);                      // Register callback handler for special-key event
    initGL();                                                 // Our own OpenGL initialization
    glutMainLoop();                                           // Enter the event-processing loop
    return 0;
}
