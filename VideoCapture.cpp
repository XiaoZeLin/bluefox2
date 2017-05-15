#include <iostream>
#include <chrono>
#include "bluefox2.h"
#include "opencv2/opencv.hpp"
#include <time.h>
#include <string>

using namespace std;
using namespace cv;
using namespace bluefox2;

int o_x;
int o_y;
fstream output;
Mat frame;
void mouse_callback(int event, int x, int y, int flags, void* param)
{
	int c = 0;
	if(event == EVENT_LBUTTONDOWN)
	{
		o_x = x;
		o_y = y;
	}else if(event == EVENT_LBUTTONUP)
	{
		for(int i = o_x;i < x;i++)
		{
			for(int j = o_y;j<y;j++)
			{
				Vec3b rgb = frame.at<Vec3b>(j,i);
				output << (int)rgb[0] << " " << (int)rgb[1] << " " << (int)rgb[2] << endl;
				c++;
			}
		}
	}
	if(c) cout<<"鼠标点击的rgb值已写入文件"<<endl;
}


Bluefox2 *mBluefox2 = nullptr;
void init();


int main(int argc, char * argv[])
{
    if(argv[1] == NULL)
    {
        cout<<"没有输入样本文件名"<<endl;
        return 1;
    }
    string filename(argv[1]);
    output.open(filename, ios::out|ios::app);

    printf("Start the tracking process, press 's' to start or stop, press 'q' to quit.\n");  
    init();  // 初始化bluefox2
    Mat src;
    char key;
    int flag = 1;

    //VideoCapture cap(0);
    
    while(1)
    {
        mBluefox2->GrabImage(src);
        mBluefox2->RequestSingle();

        //cap>>src;
        if( src.empty() )
        {
            cout << "***捕获到空帧***"<<endl;
            continue;
        }
        Size dsize = Size(200,200);
        resize(src,src,dsize);


        frame = src.clone();
        imshow("image", frame);
        cout<<"rows = "<<frame.rows<<"  cols = "<<frame.cols<<endl;
        setMouseCallback("image", mouse_callback);
        key = waitKey(flag);

        if(key == 's')
        {
            flag = ~flag;
        }
        if(key == 'q')
        break;
        
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
