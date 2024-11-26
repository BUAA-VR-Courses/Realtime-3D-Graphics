# 4. 光线追踪 Ray Tracing

欢迎大家来到《实时三维图形基础》课程的第四次作业！

**请不要在目录中包含中文字符！**

## 作业说明

本次作业旨在让大家了解光线追踪中不同材质散射和透射效果的计算方式，学习不同三维物体求交的检测和计算方法，学习光线追踪的加速算法和效果优化。

## 作业提交

- 提交内容：程序代码和实验报告PPT。程序代码仅需要提交修改过的内容。
- 提交位置：https://bhpan.buaa.edu.cn/link/AABD04E54287C84077B305DF7200EAA72F，提交至“第四次作业”子目录下
- 截止时间：2024-12-31 23:55

## 作业要求

修改和完善 `src/raytracing/camera.cpp` 和 `src/raytracing/hit.cpp`，实现基本的光线追踪。

## 项目结构

本项目目录包含以下若干子目录：

- src/ - 作业项目的源代码：
    - utils/ - 作业中用到的工具函数和工具类。
    - raytracing/ - 光线追踪的相关代码，包括光线追踪的主要逻辑。
    - main.cpp - 函数的入口，渲染循环的实现。
- deps/ - 本次作业的依赖文件，请不要随意修改和移动这部分的文件。

## 作业指导

见"第四次作业补充材料.pdf"或appendix.md。