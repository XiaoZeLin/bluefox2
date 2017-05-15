#include <iostream>    
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"  
#include "opencv2/features2d/features2d.hpp"
#include <chrono>
#include "bluefox2.h"
#include <time.h>
#include <string>
#include <vector>


using namespace cv;  
using namespace std;  
using namespace bluefox2;

    
Bluefox2 *mBluefox2 = nullptr;
void init();
void duan_OpticalFlow(Mat &frame, Mat & result);  
bool addNewPoints();  
bool acceptTrackedPoint(int i);  
    

Mat matSrc;  
Mat matRst; 
Mat curgray;    // 当前图片  
Mat pregray;    // 预测图片  
vector<Point2f> point[2]; // point0为特征点的原来位置，point1为特征点的新位置  
vector<Point2f> initPoint;    // 初始化跟踪点的位置  
vector<Point2f> features; // 检测的特征  
vector<KeyPoint> keypoints;
int maxCount = 200;         // 检测的最大特征数  
double qLevel = 0.01;   // 特征检测的等级  
double minDist = 10.0;  // 两特征点之间的最小距离  
vector<uchar> status; // 跟踪特征的状态，特征的流发现为1，否则为0  
vector<float> err;  


int main(int argc, char **argv)  
{  
    // perform the tracking process
    char key = 'a';  
    printf("Start the tracking process, press 's' to start or stop, press 'q' to quit.\n");  
    init();
    int flag = 1;


    while(1) 
    {  
        // get frame from the bluefox2
        mBluefox2->GrabImage(matSrc);
        mBluefox2->RequestSingle();
        if( matSrc.empty() )
        {
            cout << "***捕获到空帧***"<<endl;
            continue;
        }

        //set ROI  提取感兴趣的区域
        const double roi_x0 = matSrc.cols * 0.1;
        const double roi_y0 = matSrc.rows * 0.15;
        const double roi_width = matSrc.cols * 0.8;
        const double roi_height = matSrc.rows * 0.65;
        Mat matRoi(matSrc, Rect(roi_x0, roi_y0, roi_width, roi_height));
        rectangle(matSrc, Rect(roi_x0, roi_y0, roi_width, roi_height), Scalar(0, 255, 0), 2);
        imshow("ROI", matSrc);

        clock_t start, end;   // 计算光流时间
        start = clock();
        //duan_OpticalFlow(matSrc, matRst);  
        duan_OpticalFlow(matRoi, matRst);  
        end = clock();
        printf("time=%lfs\n", (double)(end - start) / CLOCKS_PER_SEC);

        
        key = cvWaitKey(flag);
        if(key == 's') flag = ~flag;
        if(key == 'q') break;
    }  
    
    return 0;  
    
}  
    
    
    
void duan_OpticalFlow(Mat &frame, Mat & result)  
{  
    cvtColor(frame, curgray, CV_BGR2GRAY);  
    result = frame.clone(); 

    
    if (addNewPoints())    //添加新的跟踪的特征点
    {  
        clock_t start1, end1;
        start1 = clock();
        
        //FastFeatureDetector fast(40);
        Ptr<FastFeatureDetector> fast = FastFeatureDetector::create();
        fast->detect(curgray,keypoints);

        goodFeaturesToTrack(curgray, features, maxCount, qLevel, minDist);  
        end1 = clock();
        printf("time1=%lfs     ", (double)(end1 - start1) / CLOCKS_PER_SEC);
        point[0].insert(point[0].end(), features.begin(), features.end());  
        initPoint.insert(initPoint.end(), features.begin(), features.end());  
    }  
    
    
    if (pregray.empty())  
    {  
        curgray.copyTo(pregray);  
    }  
    
    clock_t start2, end2;
    start2 = clock();
    calcOpticalFlowPyrLK(pregray, curgray, point[0], point[1], status, err);  //使用光流算法得到移动后的特征点point[1]
    end2 = clock();
    printf("time2=%lfs     ", (double)(end2 - start2) / CLOCKS_PER_SEC);
    
    
    int k = 0;  
    float dis_x = 0;
    float dis_y = 0;
    for (size_t i = 0; i<point[1].size(); i++)    //选择前后对应的特征点： point[0] 和 对应的 point[1]
    {  
        if (acceptTrackedPoint(i))  
        {  
            dis_x += point[1][i].x - point[0][i].x;
            dis_y += point[1][i].y - point[0][i].y;
            initPoint[k] = initPoint[i];  
            point[1][k++] = point[1][i];  
        }  
    }  
    
    
    point[1].resize(k);  
    initPoint.resize(k);  
    
    for (size_t i = 0; i<point[1].size(); i++)    // 画出点的移动方向
    {  
        line(result, initPoint[i], point[1][i], Scalar(0, 0, 255));  
        circle(result, point[1][i], 3, Scalar(0, 255, 0), -1);  
    }  

    
    if(k == 0) cout<<"没有检测到移动的点"<<"    ";
    else cout<<"dis_x = "<<dis_x/(float)k<<"   "<<"dis_y = "<<dis_y/(float)k<<"    ";
    
    swap(point[1], point[0]);  
    swap(pregray, curgray);  
    
    
    imshow("Optical Flow Result", result);  
}  
    
    



bool addNewPoints()  
{  
    return point[0].size() <= 10;  
}  
    
    
bool acceptTrackedPoint(int i)  
{  
    return status[i] && ((abs(point[0][i].x - point[1][i].x) + abs(point[0][i].y - point[1][i].y)) > 2);  
}  


void init()
{

    cout << Bluefox2::AvailableDevice() << endl;
    cout<<"请输入摄像头序列号:";
    int CameraID;
    cin>>CameraID;

    if (mBluefox2)
		delete mBluefox2;
	mBluefox2 = new Bluefox2(to_string(CameraID));
    int cpc = cpc_40000;
    int idpf = idpf_rgb888_packed;
    int cbm = cbm_off;
    bool agc = false;
    double gain_db = 32.0;    
    bool aec = false;
    int expose_us = 10000;
    int des_gray_val = 128;
    int acs = acs_fast;
    bool hdr = false;
    int wbp = wbp_user1;
    double r_gain = 0.9;
    double g_gain = 1.0;
    double b_gain = 1.4;
    int dcfm = dcfm_on;
    int mm = mm_off;
    int ctm = ctm_continuous;
    int cts = cts_unavailable;
	mBluefox2->SetMaster();
	mBluefox2->SetCpc(cpc);
	mBluefox2->SetIdpf(idpf);
	mBluefox2->SetCbm(cbm);
	mBluefox2->SetAgc(agc,gain_db);
	mBluefox2->SetAec(aec,expose_us);
    mBluefox2->SetAcs(acs,des_gray_val);
    mBluefox2->SetHdr(hdr);
	mBluefox2->SetWbp(wbp, r_gain, g_gain, b_gain);
	mBluefox2->SetDcfm(dcfm);
	mBluefox2->SetMM(mm);
	mBluefox2->SetCtm(ctm);
	mBluefox2->SetCts(cts);

	while(mBluefox2->RequestSingle()==DMR_NO_ERROR){}
    return;
}
