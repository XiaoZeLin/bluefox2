# bluefox2
使用bluefox2摄像头进行数格子，光流，视频采样，和摄像头参数调整等等。bluefox2项目现在包括bluefox2_capture, bluefox2_test, blueflow, px4flow, videocapture, videosample 和 grid这几个部分。

其中：
1. bluefox2_capture 和 bluefox2_test 是可以进行bluefox2摄像头的参数调整，主要是init函数。
2. blueflow 是使用了opencv的快速特征点检测 和 计算光流方法。 
3. px4flow使用的是px4flow 摄像头自带的计算光方法。
4. videocapture 是使用bluefox2摄像头进行视频拍摄。
5. videosample 是对视频进行逻辑回归的采样。

bluefox2摄像头驱动搭建和编译代码： cd intall  ->  sudo ./install_mvBlueFOX.sh  ->  cd ..  ->  cmake CMakeLists.txt  ->  make