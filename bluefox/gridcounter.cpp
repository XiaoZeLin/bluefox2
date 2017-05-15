#include "gridcounter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <ctime>
#include <math.h>


#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;
using namespace grid;


#define RHO 2
#define THETA CV_PI/180*1
#define HOUGH_THRESHOLD 70
double rho_t = 200;
double theta_t = CV_PI*10/180;

#define MAX_SPEED_THRESHOLD 100
#define P_FX 480.49017
#define P_FY 481.19049
#define P_CX 330.99619
#define P_CY 216.09224 
#define WIDTH 640
#define HEIGHT 480


bool cmp_theta(const Vec2f& a, const Vec2f& b)
{
    return a[1] < b[1];
}



void _DISPLINES(vector<Vec2f>& lines, string title, int r, int c)
{
    Mat stage = Mat::zeros(r, c, CV_8UC3); 
    for(int i=0;i<(int)lines.size();i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = x0+800*(-b);
        pt1.y = y0+800*(a);
        pt2.x = x0-800*(-b);
        pt2.y = y0-800*(a);
        line(stage, pt1, pt2, Scalar(195,50,15), 1, CV_AA);
    }
    imshow("stage", stage);
}




void grid::gridcount( Mat &input, double height)
{
    Mat a = Mat::zeros(input.rows,input.cols,CV_8UC1);
    Vec3b rgb;
    double value;
    for(int i= 0;i<(int)input.rows;i++)
    {
        for(int j=0;j<(int)input.cols;j++)
        {
            rgb = input.at<Vec3b>(i,j);
            //value = 1*(-11.9309) + (-0.5242)*(double)rgb[0] + (0.9870)*(double)rgb[1] + (-0.4226)*(double)rgb[2];  // 2017.3.24
            //value = 1*(-30.75867) + (-0.18592)*(double)rgb[0] + (0.45722)*(double)rgb[1] + (-0.11345)*(double)rgb[2];  // 2017.3.25 10:00
            //value = 1*(-32.90805) + (-0.22581)*(double)rgb[0] + (0.61471)*(double)rgb[1] + (-0.21831)*(double)rgb[2];  // 2017.3.25 11:00
            //value = 1*(-23.51188) + (-0.25833)*(double)rgb[0] + (0.57213)*(double)rgb[1] + (-0.18396)*(double)rgb[2];  // 2017.3.27 11:00
            //value = 1*(-21.474544) + (-0.626870)*(double)rgb[0] + (1.019010)*(double)rgb[1] + (0.024270)*(double)rgb[2];  // 2017.4.18 13:20
            value = 1*(-4.28370) + (-0.39052)*(double)rgb[0] + (0.30496)*(double)rgb[1] + (0.25692)*(double)rgb[2];  // 2017.4.21 13:20

            if(value < 0)   a.at<char>(i,j) = 0;
            else          a.at<char>(i,j) = 255;
        }
    }
    
    //imshow("a",a);

    //Mat mat_open = a.clone();

    ///*使用开运算
    Mat mat_open;
    Mat open_kernel =getStructuringElement(MORPH_RECT, Size(17,17));
    morphologyEx(a, mat_open, MORPH_OPEN, open_kernel);
    //imshow("开运算",mat_open);
    //*/

    ///*使用闭运算
    Mat close_kernel =getStructuringElement(MORPH_RECT, Size(15,15));
    morphologyEx(mat_open, mat_open, MORPH_CLOSE, close_kernel);
    //imshow("闭运算",mat_open);

    //*/
    

    ///*细化
    Mat mat_feature = Mat::zeros(input.rows, input.cols, CV_8UC1);
    int line_width = P_FX/height/20;
    for(int x=0;x<input.cols;x+= 2)    //对横直线进行细化
    {
        bool is_beg = false;
        int beg = 0;
        for(int y=0;y<input.rows;y+= 2)
        {
            if(mat_open.at<char>(y,x) != 0 && !is_beg)  
            {
                is_beg = true;
                beg = y;
            }
            if(mat_open.at<char>(y,x) == 0 && is_beg)
            {
                is_beg = false; 
                if(y-beg > line_width*0.5)
                    mat_feature.at<char>((y+beg)/2,x) = 255;  //提取粗直线的中点，记为白色
            }
            //   因为把飞行场地外的瓷砖识别为白色
            if(mat_open.at<char>(y,x) != 0 && y > (input.rows-20) && is_beg)
            {
                is_beg = false; 
                if(y-beg > line_width*0.5)
                    mat_feature.at<char>(beg+5,x) = 255;  //提取粗直线的中点，记为白色
            }
        }
    }
    for(int y=0;y<input.rows;y+= 2)		//对竖直线进行细化
    {
        bool is_beg = false;
        int beg = 0;
        for(int x=0;x<input.cols;x+= 2)
        {
            if(mat_open.at<char>(y,x) != 0 && !is_beg)
            {
                is_beg = true;
                beg = x;
            }
            if(mat_open.at<char>(y,x) == 0 && is_beg)
            {
                is_beg = false;
                if(x-beg > line_width*0.5)
                    mat_feature.at<char>(y,(x+beg)/2) = 255;
            }
        }
    }
    //imshow("细化", mat_feature);
    Mat edge = mat_feature.clone();
    //*/

    


    vector<Vec2f> lines;
    HoughLines(edge, lines, RHO, THETA, HOUGH_THRESHOLD);
   // _DISPLINES(lines, "Hough", input.rows, input.cols);   //显示霍夫直线

    int num_lines = lines.size();
    sort(lines.begin(), lines.end(), cmp_theta);  //sort排列直线
    double rhov = 0;
    double rhoh = 0;
    double rhov2 = 0;
    LineGroup ret;
    ret.status = 0;
    ret.theta1 = 0;
    ret.theta2 = 0;
    ret.theta3 = 0;

    //获取ret.theta1，ret.theta2， ret.theta3 
    for(int i=0;i<num_lines;i++)
    {
        if(lines.at(i)[1] <= CV_PI/4.0 )  //说明是垂直线
        {
            ret.theta1 = lines.at(i)[1];
            if(!ret.pv.size()) 
            {
                ret.pv.push_back(lines.at(i)[0]);
                rhov = lines.at(i)[0];
            }
        }
    
    }
    for(int i=0;i<num_lines;i++)
    {
        if(lines.at(i)[1] >= (3.0*CV_PI/4.0) )  //说明是垂直线
        {
            ret.theta3 = lines.at(i)[1];
            if(!ret.pv2.size())
            {
                if(ret.pv.size() && abs( abs(lines.at(i)[0]) - abs(rhov) ) > rho_t) 
                {
                    ret.pv2.push_back(lines.at(i)[0]);
                    rhov2 = lines.at(i)[0];
                }
                else if(!ret.pv.size())
                {
                    ret.pv2.push_back(lines.at(i)[0]);
                    rhov2 = lines.at(i)[0];
                }
            }
        }
    
    }
    for(int i=0;i<num_lines;i++)
    {
        if(lines.at(i)[1] > CV_PI/4.0 )  //说明是水平直线
        if( lines.at(i)[1] < (3.0*CV_PI/4.0) )
        {
            ret.theta2 = lines.at(i)[1];
            if(!ret.ph.size()) 
            {
                ret.ph.push_back(lines.at(i)[0]);
                rhoh = lines.at(i)[0];
            }
        }
    
    }

    //根据ret的theta1, theta2, theta3划分弧长
    for(int i=0;i<num_lines;i++)
    {
        if(ret.ph.size() && abs(lines.at(i)[1] - ret.theta2) < theta_t)
        {
            bool flag = true;
            for(unsigned int j = 0;j<ret.ph.size();j++)
            {
                if(abs(abs(lines.at(i)[0]) - abs(ret.ph[j])) < rho_t )
                flag = false;
            }
            if(flag)  ret.ph.push_back(lines.at(i)[0]);  
        }
        if(ret.pv.size() && abs(lines.at(i)[1] - ret.theta1) < theta_t)
        {
            bool flag = true;
            for(unsigned int j = 0;j<ret.pv.size();j++)
            {
                if(abs(abs(lines.at(i)[0]) - abs(ret.pv[j])) < rho_t )
                flag = false;
            }
            for(unsigned int j = 0;j<ret.pv2.size();j++)
            {
                if(abs(abs(lines.at(i)[0]) - abs(ret.pv2[j])) < rho_t )
                flag = false;
            }
            if(flag)   ret.pv.push_back(lines.at(i)[0]);
        }
        if(ret.pv2.size() && abs(lines.at(i)[1] - ret.theta3) < theta_t)
        {
            bool flag = true;
            for(unsigned int j = 0;j<ret.pv.size();j++)
            {
                if(abs(abs(lines.at(i)[0]) - abs(ret.pv[j])) < rho_t )
                flag = false;
            }
            for(unsigned int j = 0;j<ret.pv2.size();j++)
            {
                if(abs(abs(lines.at(i)[0]) - abs(ret.pv2[j])) < rho_t )
                flag = false;
            }
            if(flag)   ret.pv2.push_back(lines.at(i)[0]);
        }
    }


    //取平均值
    double theta1_sum = 0.001;double count1 = 0.001;
    double theta2_sum = 0.001;double count2 = 0.001;
    double theta3_sum = 0.001;double count3 = 0.001;
    for(int i=0;i<num_lines;i++)
    {
        if(ret.ph.size() && abs(lines.at(i)[1] - ret.theta2) < theta_t)
        {
            theta2_sum += lines.at(i)[1];
            count2++;
        }
        if(ret.pv.size() && abs(lines.at(i)[1] - ret.theta1) < theta_t)
        {
            theta1_sum += lines.at(i)[1];
            count1++;
        }
        if(ret.pv2.size() && abs(lines.at(i)[1] - ret.theta3) < theta_t)
        {
            theta3_sum += lines.at(i)[1];
            count3++;
        }
    }
    if(ret.pv.size()) ret.theta1 = (double)(ret.theta1 + theta1_sum )/(count1+1);
    if(ret.ph.size()) ret.theta2 = (double)(ret.theta2 + theta2_sum )/(count2+1);
    if(ret.pv2.size()) ret.theta3 = (double)(ret.theta3 + theta3_sum )/(count3+1);
    

    for(int i=0;i<(int)ret.ph.size();i++)
    {
        double rho_sum = 0.001;
        double count = 0.001;
        for(int j=0;j<num_lines;j++)
        {
            if(abs(lines.at(j)[0] - ret.ph[i]) < rho_t) 
            {
                rho_sum += lines.at(j)[0];
                count++;
            }
        }
        if(rho_sum != 0.001 || count != 0.001)
        ret.ph[i] = (double)rho_sum/count;
    }
    for(int i=0;i<(int)ret.pv.size();i++)
    {
        double rho_sum = 0.001;
        double count = 0.001;
        if(ret.pv.size() == 1)
        {
            ret.pv[0] = rhov;
        }
        else
        {
            for(int j=0;j<num_lines;j++)
            {
                if(abs(lines.at(j)[0] - ret.pv[i]) < rho_t &&  abs(lines.at(i)[1] - ret.theta1) < theta_t) 
                {
                    rho_sum += lines.at(j)[0];
                    cout<<"lines "<<j<<" rho = "<<lines.at(j)[0]<<" "<<"theta = "<<lines.at(j)[1]<<endl;
                    count++;
                }
            }
            if(rho_sum != 0.001 || count != 0.001)
            ret.pv[i] = (double)rho_sum/count;
            cout<<"ret.pv["<<i<<"] = "<<ret.pv[i]<<endl;
        }
    }
    
    for(int i=0;i<(int)ret.pv2.size();i++)
    {
        double rho_sum = 0.001;
        double count = 0.001;
        for(int j=0;j<num_lines;j++)
        {
            if(abs(lines.at(j)[0] - ret.pv2[i]) < rho_t) 
            {
                rho_sum += lines.at(j)[0];
                count++;
            }
        }
        if(rho_sum != 0.001 || count != 0.001)
        ret.pv2[i] = (double)rho_sum/count;
    }


    //画直线---result
    Mat result = input.clone();
    for(int i=0;i<(int)ret.pv.size();i++)
    {
        float rho = ret.pv[i], theta = ret.theta1;
        if(rho==0)rho = 5;
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = x0+1000*(-b);
        pt1.y = y0+1000*(a);
        pt2.x = x0-1000*(-b);
        pt2.y = y0-1000*(a);
        line(result, pt1, pt2, Scalar(0,0,255), 2, CV_AA);  //垂直直线是红色
        //cout<<"垂直直线：rho = "<<rho<<" theta = "<<theta<<endl;
    }
    for(int i=0;i<(int)ret.pv2.size();i++)
    {
        float rho = ret.pv2[i], theta = ret.theta3;
        if(rho==0)rho = 5;
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = x0+1000*(-b);
        pt1.y = y0+1000*(a);
        pt2.x = x0-1000*(-b);
        pt2.y = y0-1000*(a);
        line(result, pt1, pt2, Scalar(0,0,255), 2, CV_AA);	//垂直直线是红色
        //cout<<"垂直直线：rho = "<<rho<<" theta = "<<theta<<endl;
    }
    for(int i=0;i<(int)ret.ph.size();i++)
    {
        float rho = ret.ph[i], theta = ret.theta2;
        if(rho==0)rho = 5;
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = x0+1000*(-b);
        pt1.y = y0+1000*(a);
        pt2.x = x0-1000*(-b);
        pt2.y = y0-1000*(a);
        line(result, pt1, pt2, Scalar(0,255,0), 2, CV_AA);	//水平直线是绿色
        //cout<<"水平直线：rho = "<<rho<<" theta = "<<theta<<endl;
    }
    
    

    //画出直线的交点
    vector<Point2f> inters; //用向量inters存储直线的交点
    if(ret.theta1 == 0) ret.theta1 = 0.01;
    if(ret.theta2 == 0) ret.theta2 = 0.01;
    if(ret.theta3 == 0) ret.theta3 = 0.01;
    if(ret.ph.size() || ret.pv.size() || ret.pv2.size())
    {
        double cos_th1 = cos(ret.theta1);  // pv
        double sin_th1 = sin(ret.theta1);
        double cos_th2 = cos(ret.theta2);  // ph
        double sin_th2 = sin(ret.theta2);
        double cos_th3 = cos(ret.theta3);  // pv2
        double sin_th3 = sin(ret.theta3);
        
        for(unsigned int i=0;i<ret.pv.size();i++)
        {
            double p1 = ret.pv[i];
            for(unsigned int j=0;j<ret.ph.size();j++)
            {
                double p2 = ret.ph[j];
                Point2f tmp;
                tmp.x = (p2*sin_th1-p1*sin_th2)/(sin_th1*cos_th2-sin_th2*cos_th1);
                tmp.y = (p1-tmp.x*cos_th1)/sin_th1; 
                inters.push_back(tmp);
            }
        }
        for(unsigned int i=0;i<ret.pv2.size();i++)
        {
            double p1 = ret.pv2.at(i);
            for(unsigned int j=0;j<ret.ph.size();j++)
            {
                double p2 = ret.ph.at(j);
                Point2f tmp;
                tmp.x = (p2*sin_th3-p1*sin_th2)/(sin_th3*cos_th2-sin_th2*cos_th3);
                tmp.y = (p1-tmp.x*cos_th3)/sin_th3; 
                inters.push_back(tmp);
            }
        }
        for(int i=0;i<(int)inters.size();i++)
        {
            circle(result, inters[i], 3, Scalar(255,0,0),-1,8);
        }

    }
    else
    cout<<"no lines, no points"<<endl;


    //输出交点坐标
    for(int i = 0;i<(int)inters.size();i++)
    {
        cout<<"x = "<<inters[i].x<<"  "<<"y = "<<inters[i].y<<endl;
    }
    imshow("result", result);

    
   
    return;
}
