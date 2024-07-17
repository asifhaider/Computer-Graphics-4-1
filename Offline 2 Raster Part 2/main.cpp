// Raster based graphics pipeline
#include "utils.h"
#include "bitmap_image.hpp"

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
            stage1_file << p1.x << " " << p1.y << " " << p1.z << endl
                        << p2.x << " " << p2.y << " " << p2.z << endl
                        << p3.x << " " << p3.y << " " << p3.z << endl
                        << endl;
        }
        else if (command == "translate")
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
        }
        else if (command == "scale")
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
        }
        else if (command == "rotate")
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
        }
        else if (command == "push")
        {
            transformation_matrices.push(transformation_matrices.top());
        }
        else if (command == "pop")
        {
            if (transformation_matrices.size() > 1)
                transformation_matrices.pop();
            else
                cout << "Stack underflow" << endl;
        }
        else if (command == "end")
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
        stage2_file << p1.x << " " << p1.y << " " << p1.z << endl
                    << p2.x << " " << p2.y << " " << p2.z << endl
                    << p3.x << " " << p3.y << " " << p3.z << endl
                    << endl;
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

    for (int i = 0; i < triangles; i++)
    {
        point p1, p2, p3;
        file_input >> p1.x >> p1.y >> p1.z;
        file_input >> p2.x >> p2.y >> p2.z;
        file_input >> p3.x >> p3.y >> p3.z;

        p1 = projection_transformation_matrix * p1;
        p2 = projection_transformation_matrix * p2;
        p3 = projection_transformation_matrix * p3;

        stage3_file << fixed << setprecision(7);
        stage3_file << p1.x << " " << p1.y << " " << p1.z << endl
                    << p2.x << " " << p2.y << " " << p2.z << endl
                    << p3.x << " " << p3.y << " " << p3.z << endl
                    << endl;
    }

    file_input.close();
    stage3_file.close();

    // Stage 4: Z-buffer Hidden Surface Removal Algorithm

    file_input.open("config.txt");
    ofstream stage4_file("z_buffer.txt");

    // input the screen width and height from the config file (500, 500 -> -0.5 -> -0.5 -> 0, 2)

    int screen_width, screen_height;
    file_input >> screen_width >> screen_height;
    file_input.close();

    double leftside_viewbox = -1.0, rightside_viewbox = 1.0, bottomside_viewbox = -1.0, topside_viewbox = 1.0;

    // step values

    double dx = (rightside_viewbox - leftside_viewbox) / screen_width;
    double dy = (topside_viewbox - bottomside_viewbox) / screen_height;

    double top_y = topside_viewbox - dy / 2;
    double left_x = leftside_viewbox + dx / 2;
    double bottom_y = bottomside_viewbox + dy / 2;
    double right_x = rightside_viewbox - dx / 2;

    // z buffer range values

    double z_min = -1.0;
    double z_max = 1.0;

    // initialize the z-buffer in a 2D vector

    vector<vector<double>> z_buffer(screen_height, vector<double>(screen_width, z_max));

    // initialize the image as black pixels

    bitmap_image image(screen_width, screen_height);
    for (int i = 0; i < screen_width; i++)
    {
        for (int j = 0; j < screen_height; j++)
        {
            image.set_pixel(i, j, 0, 0, 0);
        }
    }

    file_input.open("stage3.txt");

    // use the stage3 points p1, p2, p3 to draw the triangles

    for (int t = 0; t < triangles; t++)
    {
        point p1, p2, p3;
        file_input >> p1.x >> p1.y >> p1.z;
        file_input >> p2.x >> p2.y >> p2.z;
        file_input >> p3.x >> p3.y >> p3.z;

        Triangle t1;
        t1.points[0] = p1;
        t1.points[1] = p2;
        t1.points[2] = p3;

        // min max values of x and y to determine the bounding box

        double min_x, max_x, min_y, max_y;

        min_x = min(min(p1.x, p2.x), p3.x);
        max_x = max(max(p1.x, p2.x), p3.x);
        min_y = min(min(p1.y, p2.y), p3.y);
        max_y = max(max(p1.y, p2.y), p3.y);

        // clip the triangles that are outside the viewbox

        min_x = max(min_x, left_x);
        max_x = min(max_x, right_x);
        min_y = max(min_y, bottom_y);
        max_y = min(max_y, top_y);

        // find the start and end point of iteration

        int start_y = round((top_y - min_y) / dy); // bottom scanline
        int end_y = round((top_y - max_y) / dy);   // top scanline

        // start the scanline along y values

        for (int scan_y = end_y; scan_y <= start_y; scan_y++)
        {
            double y = top_y - scan_y * dy;

            vector<double> x_val(2), z_val(2);
            int count = 0;

            // find the intersection points of the scanline with the triangle

            for (int i = 0; i < 3; i++)
            {
                int j = (i + 1) % 3;

                // horizontal edges are ignored
                if (t1.points[i].y == t1.points[j].y)
                    continue;

                if (y >= min(t1.points[i].y, t1.points[j].y) && y <= max(t1.points[i].y, t1.points[j].y))
                {

                    // find the intersection point of the scanline with the edge
                    // based on the linear interpolation formula
                    x_val[count] = t1.points[i].x - (t1.points[i].y - y) * (t1.points[i].x - t1.points[j].x) / (t1.points[i].y - t1.points[j].y);
                    z_val[count] = t1.points[i].z - (t1.points[i].y - y) * (t1.points[i].z - t1.points[j].z) / (t1.points[i].y - t1.points[j].y);

                    count++;
                }
            }

            // Necessary Clipping

            vector<double> clipped_temp(2);
            clipped_temp = x_val;

            for (int i = 0; i < 2; i++)
            {
                // clip x values on both sides
                if (x_val[i] < min_x)
                {
                    x_val[i] = min_x;
                }
                if (x_val[i] > max_x)
                {
                    x_val[i] = max_x;
                }
            }

            // readjust the z values based on the clipping of x values
            z_val[0] = z_val[1] - (z_val[1] - z_val[0]) * (clipped_temp[1] - x_val[0]) / (clipped_temp[1] - clipped_temp[0]);
            z_val[1] = z_val[1] - (z_val[1] - z_val[0]) * (clipped_temp[1] - x_val[1]) / (clipped_temp[1] - clipped_temp[0]);

            // store the x and z values
            double x1 = x_val[0], x2 = x_val[1];
            double z1 = z_val[0], z2 = z_val[1];

            // swap to ensure x1 < x2
            if (x1 >= x2)
            {
                swap(x1, x2);
                swap(z1, z2);
                swap(clipped_temp[0], clipped_temp[1]);
            }

            // find the start and end point of iteration along x values
            int start_x = round((x1 - left_x) / dx);
            int end_x = round((x2 - left_x) / dx);

            // start the scanline along x values
            for (int scan_x = start_x; scan_x <= end_x; scan_x++)
            {
                double x = left_x + scan_x * dx;

                // find the z value based on the linear interpolation formula
                double z = z2 - (z2 - z1) * ((x2 - x) / (x2 - x1));

                if (z < z_min)
                    continue;
                // if the z value is less than the z-buffer value, update the z-buffer and color the pixel
                if (z < z_buffer[scan_y][scan_x])
                {
                    z_buffer[scan_y][scan_x] = z;
                    image.set_pixel(scan_x, scan_y, t1.rgb_values[0], t1.rgb_values[1], t1.rgb_values[2]);
                }
            }
        }
    }
    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            if (z_buffer[i][j] < z_max)
            {
                // save values in txt file
                stage4_file << setprecision(6) << fixed << z_buffer[i][j] << "\t";
            }
        }
        stage4_file << endl;
    }

    stage4_file.close();

    // save the image
    image.save_image("out.bmp");

    // clear the z-buffer
    z_buffer.clear();
    // free the memory
    z_buffer.shrink_to_fit();
    // free the image memory
    image.clear();    
    return 0;
}