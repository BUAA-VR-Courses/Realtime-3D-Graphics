# 3. 网格简化 Mesh Simplification

欢迎大家来到《实时三维图形基础》课程的第三次作业！

**请不要在目录中包含中文字符！**

## 作业说明

本次作业旨在让大家了解三维模型网格的基本结构，并通过阅读论文，实现网格简化中的经典算法 Quadric Error Metrics (QEM) 。

## 作业提交


## 作业要求

修改和完善 `src/mesh_simplification.h` 和 `src/mesh_simplification.cpp`，实现网格简化的功能。

## 项目结构

本项目目录包含以下若干子目录：

- src/ - 作业项目的源代码：
    - utils/ - 作业中用到的工具函数和工具类。
    - main.cpp - 函数的入口，除了基本的显示功能外，通过键盘和鼠标事件，利用 camera 实现了视角和移动的功能，大家可以自行修改控制逻辑。作业中使用 Imgui 显示了当前的一些属性，如果想要显示更多的内容可以自行添加。
    - **mesh_simplification.h** 和 **mesh_simplification.cpp** 是你这次需要修改和完善的部分，需要在其中完成网格简化的功能。
- deps/ - 本次作业的依赖文件，请不要随意修改和移动这部分的文件。
- resources/ - 本次作业需要简化的网格文件，你也可以自行从网络收集其它网格，将其放在此文件夹下，并修改 main.cpp 中加载 mesh 的文件路径。

## 作业指导

1. 首先，请阅读 quadrics.pdf 文件，这是1997年发表在siggraph上的一篇文章，文章中提出了经典的 QEM 网格简化算法。文章中为你总结了**边的误差的计算方法**和**实现算法的整体流程**，这两块内容需要你在这次作业中实现。

2. 对于一个三维模型而言，我们常常采用三角网格的形式：

   ```
   1----2
   |\   |
   | \  |
   |  \ |	
   |   \|
   3----4
   ```

   例如上面有4个顶点（vertex），编号为1，2，3，4，我们拥有它们的空间坐标；上面有2个面（face），我们通常使用**顶点编号**来表示一个面，比如 (1, 2, 4)，利用面包含的顶点编号去索引顶点的空间坐标即可计算这个面的相关信息。

3. 打开 model.h 和 model.cpp，可以看到 Model 类封装了顶点（positions）和面（indices）的属性。Model 类实现了从 .obj 文件中和从一系列对应的顶点和面中加载新的网格的方法，我们在网格简化后会得到新的顶点和面元，通过 load 函数可以得到新的模型。

4. 打开 mesh_simplification.h 和 mesh_simplification.cpp，我们的网格简化函数传入当前的顶点和面元，以及简化比例，在1次简化操作中，你需要将面的数量按照 QEM 的算法降到这个比例之下。

   作业中已经写好了如下代码，`vertices_deleted` 用于纪录该点是否被删除，`faces_of_vertices` 提供了从点反向索引面的数组。并非必须使用这两个数组，如果你在作业中使用了它们，请注意在增删点和面的过程中同时维护它们。

   ```cpp
   // record whether the vertex is deleted
   std::deque<bool> vertices_deleted(vertices.size(), false);
   
   // record the face index of each vertex,
   std::vector<std::vector<int>> faces_of_vertices(vertices.size());
   for (int i = 0; i < faces.size(); ++i) {
       for (int j = 0; j < 3; ++j) {
           faces_of_vertices[faces[i][j]].push_back(i);
       }
   }
   for (int i = 0; i < vertices.size(); ++i) {
       if (faces_of_vertices[i].empty()) {
           vertices_deleted[i] = true;
       }
   }
   ```

   函数的最后写了这样一段代码，它的作用是从 vertices 和 faces 数组中构建新的网格。在实现网格简化的过程中，你需要对相应的顶点标记是否删除，并且通过将被删除的面标记为 (-1, -1, -1) 的方式表示面是否删除。无论你是新增点和面还是直接在原本的点和面上进行修改，只要你按照上述步骤维护了点和面的删除情况，都可以正确的构建出新的网格模型。

   你也可以修改这一部分，从而与你自己的代码相匹配。

   ```cpp
   // create the new mesh
   int new_vert_cnt = 0;
   int new_face_cnt = 0;
   for (auto i = 0; i < vertices.size(); ++i) {
       if (!vertices_deleted[i]) {
           vertices[new_vert_cnt] = vertices[i];
           for (auto face : faces_of_vertices[i]) {
               assert(face != -1);
               for (int j = 0; j < 3; ++j) {
                   if (faces[face][j] == i) {
                       faces[face][j] = new_vert_cnt;
                   }
               }
           }
           new_vert_cnt += 1;
       }
   }
   for (int i = 0; i < faces.size(); ++i) {
       if (faces[i][0] != faces[i][1] && faces[i][1] != faces[i][2] && faces[i][2] != faces[i][0]) {
           faces[new_face_cnt] = faces[i];
           new_face_cnt += 1;
       }
   }
   vertices.resize(new_vert_cnt);
   faces.resize(new_face_cnt);
   ```

5. 通过论文阅读，我们可以按照以下几个步骤完成网格简化：

   1. 计算所有原始点的Q矩阵；

   2. 选择所有的有效点对并计算它们的合并代价。

   3. 迭代进行合并，选择 cost 最小的边，将两个点合并为1个。你可以选择在原本的两个点上操作，将一个点标为delete，另一个点作为合并后的点；也可以新造一个点，将其加入 vertices 中。注意在迭代的同时维护面的情况，将不需要的面标为 (-1, -1, -1)。还要注意及时**更新相关的边及边的 cost**，**思考顶点的Q矩阵是否需要变化**。

6. 在作业中，提供了 Edge 类，如果你想使用标准库的有序容器，你需要提供排序的方法，一个比较简单的方法是重载 Edge 的<运算符，即在 Edge 类内添加以下代码：

   ```cpp
   struct Edge {
       ...
       
       bool operator<(const Edge& other) const {
           return cost < other.cost;    
       }
   }
   ```

7. 本次作业有一定的难度，在性能上会有一定的区分度，但是最重要的是功能的实现。

   1. 可以先在纸上进行推导，通过自定义函数封装最优点和 cost 的计算；
   2. 根据需求选择合适的数据结构，或者自己实现一定的数据结构，简化代码的实现。
   3. 推荐使用**堆**（`std::priority_queue<>`），注意堆的方向。

   