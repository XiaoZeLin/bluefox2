#include "flow.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <iomanip>
#include <ctime>
#include "bluefox2.h"

using namespace std;
using namespace cv;
using namespace bluefox2;

Bluefox2 *mBluefox2 = nullptr;
void init();

int main()
{
    char key = 'a';  
    printf("Start the tracking process, press 's' to start or stop, press 'q' to quit.\n");  
    init();                    //初始化bluefox2
    int flag = 1;
    float flow_x;
	float flow_y;
    Mat pre,cur;

    //VideoCapture cap(0);   //使用笔记本电脑的摄像头
    //cap >> pre;
    mBluefox2->GrabImage(pre);   //使用bluefox2
    mBluefox2->RequestSingle();

    Size dsize = Size(IMG_WIDTH, IMG_HEIGHT);
    resize(pre, pre, dsize);
	clock_t pre_t = clock();
	waitKey(15);
    
	while(1)
	{
		
		mBluefox2->GrabImage(cur);
        mBluefox2->RequestSingle();
        //cap>>cur;
        
        resize(cur, cur, dsize);

		clock_t cur_t  = clock();
		cout << "time: " << (double)(cur_t - pre_t)/CLOCKS_PER_SEC << "s\t";

		compute_flow((uint8_t*)pre.data, (uint8_t*)cur.data, 0, 0, 0, &flow_x, &flow_y, (double)(cur_t-pre_t)*1e6/CLOCKS_PER_SEC);
		cout << fixed << setprecision(4) << setw(10) << left << flow_x << flow_y << endl;

        pre_t = cur_t;
		pre = cur;

		imshow("pic", cur);
        //cout<<"rows = "<<cur.rows<<"  cols = "<<cur.cols<<endl;

		key = cvWaitKey(flag);
        if(key == 's') flag = ~flag;
        if(key == 'q') break;
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
