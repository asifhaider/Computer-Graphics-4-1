// Raster based graphics pipeline
#include "utils.h"

// string test_case_path = "/media/asif/Study/BUET 4-1/CSE 410 Graphics Lab/Offline 2 Raster Part 1/Test Cases/1";
// string current_path = "/media/asif/Study/BUET 4-1/CSE 410 Graphics Lab/Offline 2 Raster Part 1";

// string test_case_path = "";
// string current_path = "";

int main()

{
    // read from the test case files, expecting two files titled "scene.txt" and "config.txt"
    ifstream file_input("scene.txt");
    // ifstream config_file(test_case_path + "\\config.txt");

    // write to the output file
    ofstream stage1_file("stage1.txt");

    // line 1-3 of scene file contains the eye position, look at point and up vector
    // read them and tune the glulookat accordingly

    int triangles = 0;
    point eye, look, up;
    double fovY, aspectRatio, near, far;
    file_input >> eye.x >> eye.y >> eye.z;
    file_input >> look.x >> look.y >> look.z;
    file_input >> up.x >> up.y >> up.z;

    // line 4 of scene file contains the gluPerspective parameters
    // read them and tune the gluPerspective accordingly

    file_input >> fovY >> aspectRatio >> near >> far;

    // Stage 1: Modeling Transformation

    // initialize an empty stack of transformation matrices
    stack<matrix> transformation_matrices;

    // initialize an identity matrix
    matrix Mat;
    Mat.createIdentity();

    // push the identity matrix to the stack
    transformation_matrices.push(Mat);

    // read commands from the scene file in a loop until EOF
    while (!file_input.eof())
    {
        // read the command
        string command;
        file_input >> command;

        // if the command is "triangle"
        if (command == "triangle")
        {
            triangles++;
            // read the three vertices of the triangle
            point p1, p2, p3;
            file_input >> p1.x >> p1.y >> p1.z;
            file_input >> p2.x >> p2.y >> p2.z;
            file_input >> p3.x >> p3.y >> p3.z;

            // transform the vertices using the top of the stack transformation matrix and output the new points
            p1 = transformation_matrices.top() * p1;
            p2 = transformation_matrices.top() * p2;
            p3 = transformation_matrices.top() * p3;

            stage1_file << fixed << setprecision(7);
            stage1_file << p1.x << " " << p1.y << " " << p1.z << endl << p2.x << " " << p2.y << " " << p2.z << endl << p3.x << " " << p3.y << " " << p3.z << endl << endl;
        } else if (command == "translate")
        {
            double x, y, z;
            file_input >> x >> y >> z;

            // create a translation matrix using the parameters
            matrix translation_matrix;
            translation_matrix.createTranslation(x, y, z);

            // multiply the top of the stack transformation matrix with the translation matrix
            translation_matrix = transformation_matrices.top() * translation_matrix;
            transformation_matrices.pop();
            transformation_matrices.push(translation_matrix);
        } else if (command == "scale")
        {
            double x, y, z;
            file_input >> x >> y >> z;

            // create a scaling matrix using the parameters
            matrix scaling_matrix;
            scaling_matrix.createScaling(x, y, z);

            // multiply the top of the stack transformation matrix with the scaling matrix
            scaling_matrix = transformation_matrices.top() * scaling_matrix;
            transformation_matrices.pop();
            transformation_matrices.push(scaling_matrix);
        } else if (command == "rotate")
        {
            double angle, x, y, z;
            file_input >> angle >> x >> y >> z;

            // create a rotation matrix using the parameters
            matrix rotation_matrix;
            rotation_matrix.createRotation(angle, x, y, z);

            // multiply the top of the stack transformation matrix with the rotation matrix
            rotation_matrix = transformation_matrices.top() * rotation_matrix;
            transformation_matrices.pop();
            transformation_matrices.push(rotation_matrix);
        } else if (command == "push")
        {
            transformation_matrices.push(transformation_matrices.top());
        } else if (command == "pop")
        {
            if (transformation_matrices.size() > 1)
                transformation_matrices.pop();
            else
                cout << "Stack underflow" << endl;
        } else if (command == "end")
        {
            break;
        }
    }

    file_input.close();
    stage1_file.close();

    // Stage 2: View Transformation

    file_input.open("stage1.txt");
    ofstream stage2_file("stage2.txt");

    // initialize the view transformation matrix
    matrix view_transformation_matrix;

    view_transformation_matrix.createViewMatrix(eye, look, up);

    // read the triangles from the stage1 file and transform them using the view transformation matrix
    for (int i = 0; i < triangles; i++)
    {
        point p1, p2, p3;
        file_input >> p1.x >> p1.y >> p1.z;
        file_input >> p2.x >> p2.y >> p2.z;
        file_input >> p3.x >> p3.y >> p3.z;

        p1 = view_transformation_matrix * p1;
        p2 = view_transformation_matrix * p2;
        p3 = view_transformation_matrix * p3;

        stage2_file << fixed << setprecision(7);
        stage2_file << p1.x << " " << p1.y << " " << p1.z << endl << p2.x << " " << p2.y << " " << p2.z << endl << p3.x << " " << p3.y << " " << p3.z << endl << endl;
    }

    file_input.close();
    stage2_file.close();

    // Stage 3: Projection Transformation

    file_input.open("stage2.txt");
    ofstream stage3_file("stage3.txt");

    // initialize the projection transformation matrix
    matrix projection_transformation_matrix;

    projection_transformation_matrix.createProjectionMatrix(fovY, aspectRatio, near, far);

    // read the triangles from the stage2 file and transform them using the projection transformation matrix

    for (int i=0; i<triangles; i++)
    {
        point p1, p2, p3;
        file_input >> p1.x >> p1.y >> p1.z;
        file_input >> p2.x >> p2.y >> p2.z;
        file_input >> p3.x >> p3.y >> p3.z;

        p1 = projection_transformation_matrix * p1;
        p2 = projection_transformation_matrix * p2;
        p3 = projection_transformation_matrix * p3;

        stage3_file << fixed << setprecision(7);
        stage3_file << p1.x << " " << p1.y << " " << p1.z << endl << p2.x << " " << p2.y << " " << p2.z << endl << p3.x << " " << p3.y << " " << p3.z << endl << endl;

    }

    file_input.close();
    stage3_file.close();
}