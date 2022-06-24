# GBA_Raytracing
Raytracing in Gameboy Advance.

使用C代码在GBA上实现光线追踪。

#### 功能
目前实现的功能：
* 平面，圆球，长方体，三角面，网格求交
* 点光源，平行光，固定采样模糊阴影，多重反射渲染
* 从头文件数据导入贴图，网格，场景
* 简单动画控制

#### 截图
![screenshot_40](https://raw.githubusercontent.com/H5L0/GBA_Raytracing/master/images/GBA_RayTracing-40.png "screenshot_40")
![screenshot_53](https://raw.githubusercontent.com/H5L0/GBA_Raytracing/master/images/GBA_RayTracing-53.png "screenshot_53")


![screenshot_72](https://raw.githubusercontent.com/H5L0/GBA_Raytracing/master/images/GBA_RayTracing-72.png "screenshot_72")
![screenshot_78](https://raw.githubusercontent.com/H5L0/GBA_Raytracing/master/images/GBA_RayTracing-78.png "screenshot_78")

#### GBA硬件性能:
* 16.78MHz CPU
* 32KB内部工作内存
* 256KB外部内存

#### 程序性能：
* 简单场景：150s/frame
* 250面场景：1000-5000s/frame
* 500面场景：45min/frame
