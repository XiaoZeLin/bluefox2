/**
* @brief 打开摄像头并录制
*
* 用法:
* ./bluefox_capture
* 快捷键:
*     r - 开始/停止 录制
*     p - 保存图片
*     f - 显示FPS
* 保存文件名为： 时间.avi
*/

#include <iostream>
#include <chrono>
#include "bluefox2.h"
#include "opencv2/opencv.hpp"
#include <time.h>
using namespace std;
using namespace cv;
using namespace bluefox2;

int main(int argc, char **argv)
{
	Bluefox2 *mBluefox2 = nullptr;
	cout << Bluefox2::AvailableDevice() << endl;
	//VideoCapture Cap;
	VideoWriter Recorder;
	Mat Frame;
	chrono::system_clock::time_point StartTime;
	bool showFPS = false;
	double FPS = 1.0;
	time_t timep;
	bool Recording = false;
	char key;
	int CameraID;
	cout << "快捷键: \t r 开始/停止录制\n"
			"\t\t p 保存图片\n"
			"\t\t f 显示FPS\n" << endl;
	cout << "请输入摄像头序列号:";
	cin >> CameraID;

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
	double r_gain = 1.1;
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
	mBluefox2->SetAgc(agc, gain_db);
	mBluefox2->SetAec(aec, expose_us);
	mBluefox2->SetAcs(acs, des_gray_val);
	mBluefox2->SetHdr(hdr);
	mBluefox2->SetWbp(wbp, r_gain, g_gain, b_gain);
	mBluefox2->SetDcfm(dcfm);
	mBluefox2->SetMM(mm);
	mBluefox2->SetCtm(ctm);
	mBluefox2->SetCts(cts);

	while (mBluefox2->RequestSingle() == DMR_NO_ERROR) {
	}
	mBluefox2->manuallyStartAcquisitionIfNeeded();
	StartTime = chrono::system_clock::now();
	int count = 0;

	int pictureNum = 0;
	while (1)
	{
		//int n = 1;
		//mBluefox2->FillCaptureQueue(n);
		mBluefox2->GrabImage(Frame);
		mBluefox2->RequestSingle();
		if (Frame.empty())
		{
			cout << "***捕获到空帧***" << endl;
			continue;
		}
		count++;
		if (count >= 100)
		{
			count = 0;
			FPS = 100.0 / chrono::duration_cast<chrono::duration<double>>(chrono::system_clock::now() - StartTime).count();
			if (showFPS)
				cout << "FPS: " << FPS << endl;
			StartTime = chrono::system_clock::now();
		}

		if (Recording)
			Recorder << Frame;

		//cout << "Capture Time: " << chrono::duration_cast<chrono::duration<double>>(chrono::system_clock::now() - StartTime).count() << " " << Frame.cols <<" " <<Frame.rows<< endl;
		imshow("Raw", Frame);
		key = cvWaitKey(1);
		if (key == 'r')
		{
			if (!Recording)
			{
				time(&timep);
				string s(asctime(gmtime(&timep)));
				if (s[s.length() - 1] == '\n')
					s = s.substr(0, s.length() - 1);
				s = s + ".avi";
				for (size_t i = 0; i < s.length(); i++)
					if (s[i] == ' ')
						s[i] = '-';
				cout << "正在录制 " << s << endl;
				Recorder.open(s, CV_FOURCC('M', 'P', '4', '2'), FPS, Size(752, 480), true);
				if (!Recorder.isOpened())
					cout << "***无法初始化录制***" << endl;
				else
					Recording = true;
			}
			else
			{
				Recording = false;
				Recorder.release();
				cout << "录制结束" << endl;
			}
		}
		if (key == 'p')
		{
			pictureNum++;
			time(&timep);
			string s(asctime(gmtime(&timep)));
			if (s[s.length() - 1] == '\n')
				s = s.substr(0, s.length() - 1);
			s = s + ".png";
			for (size_t i = 0; i < s.length(); i++)
				if (s[i] == ' ')
					s[i] = '-';
			//string s;
			//s = "BF"+to_string(pictureNum)+".jpg";
			cout<< "保存图片: " << s <<endl;
        	imwrite("./"+s,Frame);
        }
        if(key == 'f')
        	showFPS = !showFPS;
    }
    return 0;
}