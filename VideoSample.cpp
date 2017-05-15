#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

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

int main(int argc, char *argv[])
{
    Mat src;
	if(argv[1] == NULL)
	{
		cout<<"no video name"<<endl;
		return 1;
	}
	string filename(argv[1]);
    VideoCapture cap(filename);
	output.open("negative.txt", ios::out|ios::app);

    char key = 'a';  
    printf("Start the tracking process, press 's' to start or stop, press 'q' to quit.\n");  
    int flag = 1;
    while(1)
    {
        cap >> src;
        frame = src.clone();
        imshow("Video", src);
		setMouseCallback("Video", mouse_callback);
        key = cvWaitKey(flag);

        if(key == 's') flag = ~flag;
        if(key == 'q') break;
    }

    return 0;
}