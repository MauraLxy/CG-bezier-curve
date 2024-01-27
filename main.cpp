#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    if (control_points.size() == 1) 
    {
        return control_points[0];
    }else{
        std::vector<cv::Point2f> new_control_points;
        for (size_t i = 0; i < control_points.size() - 1; ++i) 
    {
        cv::Point2f interpolated_point = (1 - t) * control_points[i] + t * control_points[i + 1];
        new_control_points.push_back(interpolated_point);
    }
        return cv::Point2f(recursive_bezier(new_control_points,t));
    }

}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    for (float t = 0.0; t <= 1.0; t += 0.0001) 
    {
        window.at<cv::Vec3b>(recursive_bezier(control_points,t))[1] = 255;
    }

}

void color(cv::Point2f &p, cv::Mat &window)
{
    float x = p.x;
    float y = p.y;
    // Get the center point for each pieces within p pixel
    cv::Point2f p1(x+0.25,y+0.25);
    cv::Point2f p2(x+0.25,y+0.75);
    cv::Point2f p3(x+0.75,y+0.25);
    cv::Point2f p4(x+0.75,y+0.75);
    
    // get the rgb color value for the four pieces of a pixel p
    float b_p1 = window.at<cv::Vec3b>(p1)[0];
    float g_p1 = window.at<cv::Vec3b>(p1)[1];
    float r_p1 = window.at<cv::Vec3b>(p1)[2];
    float b_p2 = window.at<cv::Vec3b>(p2)[0];
    float g_p2 = window.at<cv::Vec3b>(p2)[1];
    float r_p2 = window.at<cv::Vec3b>(p2)[2];
    float b_p3 = window.at<cv::Vec3b>(p3)[0];
    float g_p3 = window.at<cv::Vec3b>(p3)[1];
    float r_p3 = window.at<cv::Vec3b>(p3)[2];
    float b_p4 = window.at<cv::Vec3b>(p4)[0];
    float g_p4 = window.at<cv::Vec3b>(p4)[1];
    float r_p4 = window.at<cv::Vec3b>(p4)[2];
    // Get the average RGB color
    window.at<cv::Vec3b>(p)[0]=(b_p1+b_p2+b_p3+b_p4)/4;
    window.at<cv::Vec3b>(p)[1]=(g_p1+g_p2+g_p3+g_p4)/4;
    window.at<cv::Vec3b>(p)[2]=(r_p1+r_p2+r_p3+r_p4)/4;
}

void antialiasing(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    for (float t = 0.0; t <= 1.0; t += 0.0001) 
    {
        window.at<cv::Vec3b>((recursive_bezier(control_points,t)))[1] = 255;
    }
    // Get the smallest x
    std::vector<float> xs = {control_points[0].x,control_points[1].x,control_points[2].x,control_points[3].x};
    auto x_min_iterator = std::min_element(xs.begin(),xs.end());
    float x_min = *x_min_iterator;
    // Get the smallest y
    std::vector<float> ys = {control_points[0].y,control_points[1].y,control_points[2].y,control_points[3].y};
    auto y_min_iterator = std::min_element(ys.begin(),ys.end());
    float y_min = *y_min_iterator;
    // Get the biggest x
    auto x_max_iterator = std::max_element(xs.begin(),xs.end());
    float x_max = *x_max_iterator;
    // Get the biggest y
    auto y_max_iterator = std::max_element(ys.begin(),ys.end());
    float y_max = *y_max_iterator;
    // Set the starting x and y to be min - 6 to cover the control points
    for(float x = x_min-6; x <= x_max+6; ++x)
    {
        for(float y = y_min-6; y <= y_max+6; ++y)
        {
            // Get the current pixel
            cv::Point2f p(x,y);
            color(p,window);
        }
    }

}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4) 
        {
            naive_bezier(control_points, window);
            bezier(control_points, window);
            // antialiasing(control_points,window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
