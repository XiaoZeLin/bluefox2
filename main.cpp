#include <iostream>
#include <chrono>
#include "bluefox2.h"
#include "opencv2/opencv.hpp"
#include <time.h>
#include "gridcounter.h"
#include <string>
#include <time.h>

using namespace std;
using namespace cv;
using namespace bluefox2;
using namespace grid;


Bluefox2 *mBluefox2 = nullptr;
void init();


int main(int argc, char **argv)
{
	char key = 'a';
    printf("Start the tracking process, press 's' to start or stop, press 'q' to quit.\n");  
    int flag = 1;
    if(argv[1] == NULL)   //使用bluefox2摄像头
    {
        init();    //初始化bluefox2
        Mat Frame;
        while(1)
        {        
            mBluefox2->GrabImage(Frame);
            mBluefox2->RequestSingle();
            if( Frame.empty() )
            {
                cout << "***捕获到空帧***"<<endl;
                continue;
            }
            imshow("Raw",Frame);
            double height = 1.0;   // 需要获得飞行器高度
            gridcount(Frame, height);
            key = cvWaitKey(flag);

            if(key == 's') flag = ~flag;
            if(key == 'q') break;
        }

    }
	else    //读取录制好的视频
    {
        string filename(argv[1]);         
        Mat src;
        VideoCapture cap(filename);
        int frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
        for(int i = 0;i < frames;++i)
        {
            cap >> src;
            if (src.empty()) continue;
            imshow("Video", src);
            double height = 1.2;
            clock_t start, end;
            start = clock();
            gridcount(src, height);
            end = clock();
            printf("time=%lfs\n", (double)(end - start) / CLOCKS_PER_SEC);
            key = cvWaitKey(flag);

            if(key == 's') flag = ~flag;
            if(key == 'q') break;
        }
        cout<<"程序运行结束"<<endl;
    }
    

    return 0;
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
