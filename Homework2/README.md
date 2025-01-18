# 4. 光照模型和阴影贴图

欢迎大家来到《实时三维图形基础》课程的第四次作业！

**请不要在目录中包含中文字符！**

## 作业说明

渲染（Rendering），是计算机图形学的重要内容。本次作业旨在让大家实现一些基础的实时渲染功能：
- 实现MVP变换，理解矩阵在图形学中的应用；
- 实现Phong Shading，理解光照模型的具体计算方法；
- 实现Shadow Mapping，在三维场景中实现较真实的阴影功能。

## 作业提交


## 作业要求

修改和完善 `src/maim.cpp ` 和 `src/shaders/light_shadow.frag`，实现 Phong 着色模型 和 阴影贴图 的功能。

## 项目结构

本项目目录包含以下若干子目录：

- `src/` - 作业项目的源代码：
    - `main.cpp` - 函数的入口。除了基本的显示功能外，通过键盘和鼠标事件，利用 camera 实现了视角和移动的功能。
    - `shaders/` - 本次作业中所用到的着色器
    - `utils/` - 作业项目中用到的工具函数和工具类，其中`gl/`文件夹下对opengl的一些功能进行了封装。
- `deps/` - 本次作业的依赖文件，请不要随意修改和移动这部分的文件。
- `resources/` - 本次作业需要小牛模型、平面模型，以及他们的材质。

## 作业指导

1. 在`utils/transform.cpp`中，完成`get_scale_matrix`、`get_trans_matrix`和`rotate_with`函数（2.1.1），实现物体在空间中的变换；
2. 在`utils/transform.cpp`中，完成`perspective`、`orthographic`和`look_at`函数（2.1.2），将物体变换到相机的局部坐标和裁剪空间中；
3. 在`shaders/light_shadow.frag`中，完成Phong Shading相关的代码（2.2.1）。
4. 在`main.cpp`和`shaders/light_shadow.frag`中，完成Shadow Mapping相关的代码（2.2.2）。

## 作业参考

1. Eigen库的[官方文档](http://eigen.tuxfamily.org/dox/modules.html)以及[中文文档](https://github.com/qixianyu-buaa/EigenChineseDocument)
2. [GAMES101课程](https://www.bilibili.com/video/BV1X7411F744)中的第三讲和第四讲