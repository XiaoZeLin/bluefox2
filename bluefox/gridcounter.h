#ifndef __GRIDCOUNTER_H
#define __GRIDCOUNTER_H

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <vector>


using namespace std;
using namespace cv;

namespace grid
{
    typedef struct
    {
        bool status;
        double theta1;
        double theta2;
        double theta3;
        vector<double> pv;
        vector<double> pv2;
        vector<double> ph;
    }LineGroup;
    
    void gridcount( Mat &input, double height );
};

#endif
