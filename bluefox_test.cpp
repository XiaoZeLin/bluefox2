#include "bluefox2.h"
#include <iomanip>
#include <thread>
#include <chrono>
using namespace bluefox2;
using namespace std;

vector<string> splitCommand(string command)
{
	size_t cPos = 0;
	vector<string> result;
	while (cPos < command.size())
	{
		if (command[cPos] != ' ')
		{
			size_t spacePos = cPos;
			while (spacePos < command.size())
			{
				if (command[spacePos] == ' ')
					break;
				++spacePos;
			}
			result.push_back(command.substr(cPos, spacePos - cPos));
			cPos = spacePos;
		}
		++cPos;
	}
	return result;
}

vector<string> availableCommandList = {"open", "get", "set", "device", "show", "help"};
vector<string> availableGetProp = {"serial", "product", "timeout_ms", "exposeus"};
vector<string> availableSetProp = {"mm", "master", "slave", "aoi", "idpf", "cbm", "agc", "aec", "acs", "wbp", "hdr", "dcfm", "cpc", "ctm", "cts"};

vector<size_t> availableSetPropCount = {1, 0, 0, 2, 1, 1, 2, 2, 2, 4, 1, 1, 1, 1, 1};

map<string, map<string, int>> propValueMap = {
	{"mm", {	{"off", 0}, 
				{"topdown", 1}, 
				{"leftright", 2}, 
				{"topdown_and_leftright", 3}}},

	{"idpf", {	{"auto", 0}, 
				{"raw", 1}, 
				{"mono8", 2}, 
				{"rgb888_packed", 10}, 
				{"bgr888_packed", 22}}},

	{"cbm", {	{"off", 0}, 
				{"h", 0x1}, 
				{"v", 0x2}, 
				{"hv", 0x3}}},

	{"acs", {	{"unavailable", -1}, 
				{"slow", 0}, 
				{"medium", 1}, 
				{"fast", 2}}},

	{"wbp", {	{"unavailable", -1}, 
				{"tungsten", 0}, 
				{"halogen", 1}, 
				{"fluorescent", 2}, 
				{"daylight", 3}, 
				{"photolight", 4}, 
				{"bluesky", 5}, 
				{"user1", 6}, 
				{"calibrate", 10}}},

	{"dcfm", {	{"off", 0}, 
				{"on", 1}, 
				{"calibrate", 2}, 
				{"correction_image", 3}}},

	{"cpc", {	{"12000", 12000}, 
				{"20000", 20000}, 
				{"24000", 24000}, 
				{"27000", 27000}, 
				{"32000", 32000}, 
				{"40000", 40000}, 
				{"50000", 50000}}},

	{"ctm", {	{"continuous", 0}, 
				{"on_demand", 1}, 
				{"on_low_level", 2}, 
				{"on_high_level", 3}, 
				{"on_falling_edge", 4}, 
				{"on_rising_edge", 5}, 
				{"hard_sync", -1}}},

	{"cts", {	{"unavailable", -1}, 
				{"dig_in_0", 0}, 
				{"dig_in_1", 1}}}};

bool checkCommand(vector<string> &command)
{
	if (!command.size())
	{
		cout << "空命令！" << endl;
		return false;
	}
	if (command[0] == "open")
	{
		if (command.size() == 2)
			return true;
		cout << (command.size() < 2 ? "缺少序列号" : "参数过多") << endl;
		return false;
	}
	if (command[0] == "get")
	{
		if (command.size() == 2)
		{
			for (auto arg : availableGetProp)
				if (arg == command[1])
					return true;
			cout << "无效的参数： " << command[1] << endl;
			return false;
		}
		cout << "参数过多" << endl;
		return false;
	}
	if (command[0] == "set")
	{
		if (command.size() >= 2)
		{
			for (size_t i = 0; i < availableSetProp.size(); i++)
			{
				if (command[1] == availableSetProp[i])
				{
					if (command.size() - 2 == availableSetPropCount[i])
						return true;
					cout << (command.size() - 2 < availableSetPropCount[i] ? "缺少参数" : "参数过多") << endl;
					return false;
				}
			}
			cout << "无效的参数： " << command[1] << endl;
			return false;
		}
		cout << "参数过少" << endl;
		return false;
	}
	if (command[0] == "device")
	{
		if (command.size() == 1)
			return true;
		cout << "参数过多" << endl;
		return false;
	}
	if (command[0] == "show")
	{
		if (command.size() == 1)
			return true;
		cout << "参数过多" << endl;
		return false;
	}
	if (command[0] == "help")
	{
		if (command.size() >= 1 && command.size() <= 3)
			return true;
		cout << "参数过多" << endl;
		return false;
	}
	cout << "无效的命令： " << command[0] << endl;
	return false;
}

Bluefox2 *mBluefox2 = nullptr;
thread *showThread = nullptr;

void show()
{
	while (mBluefox2)
	{

		chrono::system_clock::time_point StartTime = chrono::system_clock::now();
		for(int i = 0;i<100;i++)
		{
		cv::Mat image;
		int n = 1;
		mBluefox2->FillCaptureQueue(n);
		mBluefox2->GrabImage(image);
		if (!image.empty())
			cv::imshow("image", image);
		else
			cout << "图像为空！" << endl;
		cv::waitKey(2);
		
		}
		cout << chrono::duration_cast<chrono::duration<double>>(chrono::system_clock::now() - StartTime).count() <<endl;
	}
}

int main()
{
	cout << Bluefox2::AvailableDevice() << endl;
	while (1)
	{
		string command;
		vector<string> args;
		getline(cin, command);
		args = splitCommand(command);
		if (checkCommand(args))
		{
			if (args[0] == "open")
			{
				if (mBluefox2)
					delete mBluefox2;
				mBluefox2 = new Bluefox2(args[1]);
				int t = 22;
				mBluefox2->SetIdpf(t);
			}
			if (args[0] == "get")
			{
				if (!mBluefox2)
				{
					cout << "请先打开摄像头！" << endl;
					continue;
				}
				if (args[1] == "serial")
					cout << mBluefox2->serial() << endl;
				if (args[1] == "product")
					cout << mBluefox2->product() << endl;
				if (args[1] == "timeout_ms")
					cout << mBluefox2->timeout_ms() << endl;
				if (args[1] == "exposeus")
					cout << mBluefox2->GetExposeUs() << endl;
			}
			if (args[0] == "set")
			{
				int tempInt;
				bool tempBool;
				double tempDouble1;
				double tempDouble2;
				double tempDouble3;
				if (!mBluefox2)
				{
					cout << "请先打开摄像头！" << endl;
					continue;
				}
				if (args[1] == "master")
				{
					mBluefox2->SetMaster();
					continue;
				}
				if (args[1] == "slave")
				{
					mBluefox2->SetSlave();
					continue;
				}
				if (args[1] == "aoi")
				{
					//mBluefox2->SetAoi(stoi(args[2]),stoi(args[3]));
					continue;
				}
				if (args[1] == "agc")
				{
					mBluefox2->SetAgc(tempBool = (args[2] == "t" || args[2] == "T"), tempDouble1 = stod(args[3]));
					continue;
				}
				if (args[1] == "aec")
				{
					mBluefox2->SetAec(tempBool = (args[2] == "t" || args[2] == "T"), tempInt = stoi(args[3]));
					continue;
				}
				if (args[1] == "hdr")
				{
					mBluefox2->SetHdr(tempBool = (args[2] == "t" || args[2] == "T"));
					continue;
				}
				if (propValueMap.find(args[1]) != propValueMap.end())
				{
					if (propValueMap.find(args[1])->second.find(args[2]) != propValueMap.find(args[1])->second.end())
					{
						int value = propValueMap.find(args[1])->second.find(args[2])->second;
						if (args[1] == "mm")
						{
							mBluefox2->SetMM(value);
							continue;
						}
						if (args[1] == "idpf")
						{
							mBluefox2->SetIdpf(value);
							continue;
						}
						if (args[1] == "cbm")
						{
							mBluefox2->SetCbm(value);
							continue;
						}
						if (args[1] == "acs")
						{
							mBluefox2->SetAcs(value, tempInt = stoi(args[3]));
							continue;
						}
						if (args[1] == "wbp")
						{
							if (value != propValueMap["wbp"]["calibrate"])
								mBluefox2->SetWbp(value, tempDouble1 = stod(args[3]), tempDouble2 = stod(args[4]), tempDouble3 = stod(args[5]));
							else
							{
								double c_r = 1.0, c_g = 1.0, c_b = 1.0;
								mBluefox2->SetWbp(value, c_r, c_g, c_b);
								cout << setprecision(5) << " 校正后的RGB增益："
									 << "  R: " << c_r << "  G: " << c_g << "  B: " << c_b << endl;
							}
							continue;
						}
						if (args[1] == "dcfm")
						{
							mBluefox2->SetDcfm(value);
							continue;
						}
						if (args[1] == "cpc")
						{
							mBluefox2->SetCpc(value);
							continue;
						}
						if (args[1] == "ctm")
						{
							mBluefox2->SetCtm(value);
							continue;
						}
						if (args[1] == "cts")
						{
							mBluefox2->SetCts(value);
							continue;
						}
					}
					else
					{
						cout << "无效的参数： " << args[2] << endl;
						continue;
					}
				}
				else
				{
					cout << "无效的参数： " << args[1] << endl;
					continue;
				}
			}
			if (args[0] == "device")
			{
				cout << Bluefox2::AvailableDevice() << endl;
			}
			if (args[0] == "show")
			{
				if (!mBluefox2)
				{
					cout << "请先打开摄像头！" << endl;
					continue;
				}
				if (!showThread)
				{
					showThread = new thread(show);
					showThread->detach();
				}
			}
			if (args[0] == "help")
			{
				if (args.size() == 1)
				{
					cout << "使用说明：\n"
							"可用命令：\n"
							"\tdevice\t设备列表\n"
							"\topen\t打开摄像头\n"
							"\tshow\t显示图像\n"
							"\tget\t获取参数\n"
							"\tset\t设置参数\n"
							"\thelp\t获取参数\n" << endl;
					continue;
				}
				if (args.size() == 2)
				{
					if (args[1] == "get")
					{
						cout << "可获取的参数：\n"
								"\tserial\t\t摄像头序列号\n"
								"\tproduct\t\t摄像头型号\n"
								"\ttimeout_ms\t超时时间\n"
								"\texposeus\t曝光时间\n" << endl;
						continue;
					}
					if (args[1] == "set")
					{
						cout << "可设置的参数：\n"
								"\tmm\tMirror mode 镜像模式\n"
								"\tmaster\t主设备模式（用于双摄像头）\n"
								"\tslave\t从设备模式（用于双摄像头）\n"
								"\tidpf\tThe pixel format of the resulting image 设置返回的图片格式\n"
								"\tcbm\tCamera binning mode 像素合并模式\n"
								"\tagc\tAuto gain control 自动增益控制\n"
								"\taec\tAuto expose control 自动曝光控制\n"
								"\tacs\tAuto control speed 自动控制速度\n"
								"\twbp\tWhite balance paramter 白平衡参数\n"
								"\thdr\tHigh Dynamic Range 高动态范围\n"
								"\tdcfm\tDark Current Filter Mode 暗电流滤波模式\n"
								"\tcpc\tPixel Clock 像素时钟速度\n"
								"\tctm\tTrigger Mode 触发模式\n"
								"\tcts\tTrigger Source 触发源\n" << endl;
						continue;
					}
					cout << "没有关于命令 " << args[1] << " 的帮助" << endl;
				}
				if (args.size() == 3)
				{
					if (args[1] == "set")
					{
						if (args[2] == "mm")
						{
							cout << "\n\tmm\tMirror mode 镜像模式\n"
									"命令格式： set mm 模式\n"
									"可选模式：\n"
									"\toff \t\t\t:关闭\n"
									"\ttopdown \t\t:上下镜像\n"
									"\tleftright \t\t:左右镜像\n"
									"\ttopdown_and_leftright \t:上下和左右镜像\n" << endl;
							continue;
						}
						if (args[2] == "master")
						{
							cout << "\n\tmaster\t主设备模式（用于双摄像头）\n"
									"命令格式： set master\n" << endl;
							continue;
						}
						if (args[2] == "slave")
						{
							cout << "\n\tslave\t从设备模式（用于双摄像头）\n"
									"命令格式： set slave\n" << endl;
							continue;
						}
						if (args[2] == "idpf")
						{
							cout << "\n\tidpf\tThe pixel format of the resulting image 设置返回的图片格式\n"
									"命令格式： set idpf 图片格式\n"
									"可选格式：\n"
									"\tauto\t\t:自动\n"
									"\traw\t\t:原始数据\n"
									"\tmono8\t\t:8位单色\n"
									"\trgb888_packed\t:每个像素RGB排列，RGB各8bit\n"
									"\tbgr888_packed\t:每个像素BGR排列，RGB各8bit\n"
									"备注： 默认为rgb888_packed\n" << endl;
							continue;
						}
						if (args[2] == "cbm")
						{
							cout << "\n\tcbm\tCamera binning mode 像素合并模式\n"
									"命令格式： set cbm 模式\n"
									"可选模式：\n"
									"\toff \t:不启用像素合并\n"
									"\th \t:水平每两个像素合并为1个像素\n"
									"\tv \t:竖直每两个像素合并为1个像素\n"
									"\thv \t:水平及竖直每4个像素合并为1个像素\n" << endl;
							continue;
						}
						if (args[2] == "agc")
						{
							cout << "\n\tagc\tAuto gain control 自动增益控制\n"
									"命令格式： set agc (T|F)  增益(dB)\n"
									"\t(T|F) \t\t:是否启用自动增益控制，T启用，F不启用\n"
									"\t增益(dB) \t:范围: 0.0 ~ 32.0\n" << endl;
							continue;
						}
						if (args[2] == "aec")
						{
							cout << "\n\taec\tAuto expose control 自动曝光控制\n"
									"命令格式： set aec (T|F)  曝光时间(us)\n"
									"\t(T|F) \t\t:是否启用自动曝光控制，T启用，F不启用\n"
									"\t曝光时间(us) \t:范围: 10 ~ 100000\n" << endl;
							continue;
						}
						if (args[2] == "acs")
						{
							cout << "\n\tacs\tAuto control speed 自动控制速度\n"
									"命令格式： set 控制速度 目标图像平均灰度\n"
									"可选控制速度：\n"
									"\tunavailable\t:不修改设置\n"
									"\tslow \t\t:慢速\n"
									"\tmedium \t\t:中等速度\n"
									"\tfast \t\t:快速\n\n"
									"目标图像平均灰度\t:范围: 0~255\n" << endl;
							continue;
						}
						if (args[2] == "wbp")
						{
							cout << "\n\twbp\tWhite balance paramter 白平衡参数\n"
									"命令格式： set wbp 模式 红色增益(0.1~10) 绿色增益(0.1~10) 蓝色增益(0.1~10)\n"
									"可选模式：\n"
									"\tunavailable\t:不修改白平衡参数\n"
									"\ttungsten\t:钨丝灯 2800K\n"
									"\thalogen\t\t:卤素灯 3000K\n"
									"\tfluorescent \t:日光灯 6500K\n"
									"\tdaylight\t:日光 5000K\n"
									"\tphotolight\t:闪光灯 5000-5500K\n"
									"\tbluesky\t\t:蓝天 9000K\n"
									"\tuser1\t\t:自定义\n"
									"\tcalibrate \t:校正，请把摄像头对着白色平面\n"
									"\t备注：增益参数仅当启用自定义模式(user1)时有效\n" << endl;
							continue;
						}
						if (args[2] == "hdr")
						{
							cout << "\n\thdr\tHigh Dynamic Range 高动态范围\n"
									"命令格式： set hdr (T|F)\n"
									"\t(T|F) \t:是否启用高动态范围，T启用，F不启用\n" << endl;
							continue;
						}
						if (args[2] == "dcfm")
						{
							cout << "\n\tdcfm\tDark Current Filter Mode 暗电流滤波模式\n"
									"命令格式： set dcfm 模式\n"
									"可选模式：\n"
									"\toff\t:关闭过滤\n"
									"\ton\t:打开过滤\n"
									"\tcalibrate\t:校正，请盖上镜头盖再执行该指令，执行后，将自动进入关闭过滤(off)模式\n"
									"\tcorrection_image\t:显示校正的图像(被过滤的噪点)\n"
									"备注：校正暗电流时，请先打开摄像头并让其工作5~6分钟，等其温度稳定后，再执行calibrate和on\n" << endl;
							continue;
						}
						if(args[2]=="cpc")
						{
							cout << "\n\tcpc\tPixel Clock 像素时钟速度(KHz)\n"
									"命令格式： set cpc 速度\n"
									"可选速度：\n"
									"\t12000\t:12MHz\n"
									"\t20000\t:20MHz\n"
									"\t27000\t:12MHz\n"
									"\t32000\t:32MHz\n"
									"\t40000\t:40MHz\n"
									"备注：建议设置为40MHz\n" << endl;
							continue;
						}
						if(args[2]=="ctm")
						{
							cout << "\n\tctm\tTrigger Mode 触发模式\n"
									"命令格式： set ctm 模式\n"
									"可选模式：\n"
									"\tcontinuous\t:不等待触发，持续曝光\n"
									"\ton_demand\t:当软件请求图像时，开始曝光\n"
									"\ton_low_level\t:外部输入引脚低电平触发\n"
									"\ton_high_level\t:外部输入引脚高电平触发\n"
									"\ton_falling_edge\t:外部输入引脚下降沿触发\n"
									"\ton_rising_edge\t:外部输入引脚上升沿触发\n"
									"\thard_sync\t:双摄像头时同步触发\n"
									"备注：外部输入引脚位于11，12引脚，具体请查阅 https://www.matrix-vision.com/manuals/mvBlueFOX/mvBF_page_tech.html#mvBF_subsection_single 中的mvBlueFOX-MLC2xx部分\n" << endl;
							continue;
						}
						if(args[2]=="cts")
						{
							cout << "\n\tcts\tTrigger Source 触发源\n"
									"命令格式： set cts 模式\n"
									"可选模式：\n"
									"\tunavailable\t:不使用外部触发\n"
									"\tdig_in_0\t:使用外部输入引脚0，位于12脚，作为触发源\n"
									"\tdig_in_1\t:使用外部输入引脚1，位于11脚，作为触发源\n" << endl;
									continue;
						}
						cout << "没有关于参数 " << args[2] << " 的帮助" <<endl;
					}
				}
			}
		}
	}
	return 0;
}