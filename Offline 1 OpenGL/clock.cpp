#define _USE_MATH_DEFINES

#include <iostream>
#include <GL/glut.h>
#include <cmath>

using namespace std;

GLfloat second_angle = 30 * M_PI / 180;
GLfloat minute_angle = 120 * M_PI / 180;
GLfloat hour_angle = 0.0f; // rotational angle of the shapes
                           // OpenGL works as a state machine. 
                           // The state variables remain same unless changed by the user.
// get the current time
time_t now = time(0);
tm *ltm = localtime(&now);
// extract the hour, minute and second
int hour = ltm->tm_hour;
int minute = ltm->tm_min;
int second = ltm->tm_sec;
// cout << hour << " : " << minute << " : " << second << endl;
// calculate the angle of the hour hand in radians
// GLfloat hour_angle = (((hour%12) * 30) + minute * 0.5) * M_PI / 180;
// // calculate the angle of the minute hand in radians
// GLfloat minute_angle = ((minute * 6) + second * 0.1) * M_PI / 180;
// // calculate the angle of the second hand in radians
// GLfloat second_angle = ((second * 6)) * M_PI / 180;

// cout << "Hour: " << hour_angle << endl;
// cout << "Minute: " << minute_angle << endl;
// cout << "Second: " << second_angle << endl;

int refreshMills = 1000; // refresh interval in milliseconds

GLfloat pendulum_angle = 0.0f;
GLfloat pendulum_length = 0.2f;
GLfloat pendulum_period = 2.0f;

GLfloat horizontal_distance_pendulum, vertical_distance_pendulum;
GLfloat pendulum_swing_width = 0.3f;
// display function
/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display()
{
    // set clear color to white
    glClearColor(0.89f, 0.894f, 0.871f, 1.0f); // wall color
    // clear previous screen (color buffer)
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
    glLoadIdentity();           // Reset the model-view matrix

    // set line width
    glLineWidth(10.0);
    float radius, side, shift_y = 0.4;
    // draw octagon
    glBegin(GL_POLYGON);
    // set line color
    glColor3f(0.31f, 0.192f, 0.01f); // brown
    radius = 0.45;
    for (int i = 22.5; i < 382.5; i += 45)
    {
        float degInRad = i * M_PI / 180;
        glVertex2f(cos(degInRad) * radius, shift_y + sin(degInRad) * radius);
    }
    glEnd();

    side = 2 * radius * sin(22.5);
    // cout << side << endl;

    glBegin(GL_POLYGON);
    glVertex2d((side / 2), 0.2f);
    glVertex2d(side / 2, -0.4f);
    glVertex2d(0.0f, -0.6f);
    glVertex2d(-side / 2, -0.4f);
    glVertex2d((-side / 2), 0.2f);
    glEnd();

    // draw circle
    glBegin(GL_POLYGON);
    // set line color
    glColor3f(0.529f, 0.325f, 0.016f); // brown
    radius = 0.3;
    for (int i = 0; i < 360; i++)
    {
        float degInRad = i * M_PI / 180;
        glVertex2f(cos(degInRad) * radius, shift_y + sin(degInRad) * radius);
    }
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.7f, 0.035f);
    radius = 0.27;
    for (int i = 0; i < 360; i++)
    {
        float degInRad = i * M_PI / 180;
        glVertex2f(cos(degInRad) * radius, shift_y + sin(degInRad) * radius);
    }
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f);

    radius = 0.24;
    for (int i = 0; i < 360; i++)
    {
        float degInRad = i * M_PI / 180;
        glVertex2f(cos(degInRad) * radius, shift_y + sin(degInRad) * radius);
    }
    glEnd();

    float small_tick = 0.02, large_tick = 0.04;
    // draw the small ticks
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f); // black
    radius = 0.24;

    for (int i = 0; i < 360; i += 6)
    {
        float degInRad = i * M_PI / 180;
        if (i % 30 != 0)
        {
            glVertex2f(cos(degInRad) * radius, shift_y + sin(degInRad) * radius);
            glVertex2f(cos(degInRad) * (radius - small_tick), shift_y + sin(degInRad) * (radius - small_tick));
        }
    }
    glEnd();

    glLineWidth(5.0);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f); // black
    radius = 0.24;

    // draw the twelve large ticks
    for (int i = 0; i < 360; i += 30)
    {
        float degInRad = i * M_PI / 180;
        if (i % 30 == 0)
        {
            glVertex2f(cos(degInRad) * radius, shift_y + sin(degInRad) * radius);
            glVertex2f(cos(degInRad) * (radius - large_tick), shift_y + sin(degInRad) * (radius - large_tick));
        }
    }
    glEnd();

    // draw the point at the center of the clock

    glPointSize(10.0);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 0.0f); // black
    glVertex2f(0.0f, shift_y);
    glEnd();
    
    glPushMatrix();
    // draw the second hand from the center point above
    glLineWidth(1.7);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f); // black
    radius = 0.18;
    glRotatef(-second_angle, 0.0f, 0.0f, 1.0f); // rotation about positive z-axis
    glVertex2f(0.0f, shift_y);
    // glPushMatrix();
    glVertex2f(cos(second_angle) * radius, shift_y + sin(second_angle) * radius);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    // draw the minute hand from the center point above
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f); // black
    radius = 0.16;
    glRotatef(-minute_angle, 0.0f, 0.0f, 1.0f);
    glVertex2f(0.0f, shift_y);
    glVertex2f(cos(minute_angle) * radius, shift_y + sin(minute_angle) * radius);
    glEnd();
    glPopMatrix();

    glPushMatrix();

    // draw the hour hand from the center point above
    glLineWidth(5.0);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f); // black
    radius = 0.12;
    glRotatef(-hour_angle, 0.0f, 0.0f, 1.0f);
    glVertex2f(0.0f, shift_y);

    glVertex2f(cos(hour_angle) * radius, shift_y + sin(hour_angle) * radius);
    glEnd();
    glPopMatrix();

    // draw a rectangle just below the clock, width = 0.3, height = 0.6
    glBegin(GL_POLYGON);
    glColor3f(0.188f, 0.133f, 0.012f);
    glVertex2f(-pendulum_swing_width / 2, 0.0f);
    glVertex2f(-pendulum_swing_width / 2, -0.4f);
    glVertex2f(0.0f, -0.5f);
    glVertex2f(pendulum_swing_width / 2, -0.4f);
    glVertex2f(pendulum_swing_width / 2, 0.0f);
    glEnd();

    // draw a small circle at the top of the rectangle
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.7f, 0.035f);
    radius = 0.02;
    for (int i = 0; i < 360; i++)
    {
        float degInRad = i * M_PI / 180;
        glVertex2f(cos(degInRad) * radius, 0.0f - sin(degInRad) * radius);
    }
    glEnd();

    // draw a line from the above circle, height = 0.4
    horizontal_distance_pendulum = sin(pendulum_angle) * pendulum_length;
    vertical_distance_pendulum = -cos(pendulum_angle) * pendulum_length;
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.7f, 0.035f);
    glLineWidth(20.0);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(horizontal_distance_pendulum, vertical_distance_pendulum);
    glEnd();

    glRotatef(pendulum_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(horizontal_distance_pendulum, vertical_distance_pendulum, 0.0f);

    // draw a bigger circle at the bottom of the rectangle
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.7f, 0.035f);
    radius = 0.04;
    for (int i = 0; i < 360; i++)
    {
        float degInRad = i * M_PI / 180;
        glVertex2f(cos(degInRad) * radius, -sin(degInRad) * radius);
    }
    glEnd();

    // // flush drawing routines to the window
    // glFlush();

    // // swap buffers to display, since we're double buffered
    glutSwapBuffers();
}

// reshape callback function with proper aspect ratio
void reshape(int w, int h)
{
    // compute the aspect ration of the new window
    if (h == 0)
        h = 1;
    GLfloat aspect = (GLfloat)w / (GLfloat)h;

    // set the viewport to cover the new window
    glViewport(0, 0, w, h);

    // set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // reset the projection matrix
    if (w <= h)
    {
        // width is smaller, so stretch out the height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
    else
    {
        // height is smaller, so stretch out the width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    }
}

// idle callback function
void idle()
{
    GLfloat angular_velocity = 2.0f * M_PI / pendulum_period;
    if (horizontal_distance_pendulum == pendulum_swing_width / 2)
    {
        angular_velocity = -angular_velocity;
    }
    else if (horizontal_distance_pendulum == -pendulum_swing_width / 2)
    {
        angular_velocity = -angular_velocity;
    }
    pendulum_angle = sin(angular_velocity * glutGet(GLUT_ELAPSED_TIME) / 1000.0f);

    // force the redraw of the window
    glutPostRedisplay();
}

void Timer(int value)
{
    second_angle -= 6 * M_PI / 180;
    minute_angle -= 6 * M_PI / (180 * 60);
    hour_angle -= 6 * M_PI / (180 * 60 * 12);
    glutPostRedisplay(); // Post re-paint request to activate display()
    // recursive call
    glutTimerFunc(refreshMills, Timer, 0); // next Timer call milliseconds later
}

// main function
int main(int argc, char **argv)
{

    // initialize GLUT (must be called before any GLUT function)
    glutInit(&argc, argv);
    // enable double buffering
    glutInitDisplayMode(GLUT_DOUBLE);
    // set window size (width, height)
    glutInitWindowSize(960, 960);
    // set window position (0,0 is at the top left corner)
    glutInitWindowPosition(500, 50);
    // create window with title
    glutCreateWindow("Analog Clock");
    // register the display function (callback event handler)
    glutDisplayFunc(display);
    // register the reshape function (callback event handler)
    glutReshapeFunc(reshape);
    // idle function for animation
    glutIdleFunc(idle);
    // timer function for animation
    glutTimerFunc(0, Timer, 0);
    // event processing infinite loop
    glutMainLoop();
    return 0;
}