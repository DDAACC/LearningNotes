### LearnOpenGl学习笔记三

####模型加载

* Assimp

  **UV映射**

  模型的文件格式有很多种

  Assimp模型加载库

  * Scene

    整个模型加载进一个场景对象 

    RootNode

    Mesh

    Material

  * RootNode

    子节点 保存Scene中mesh数组的索引

  * Mesh

    **组合模型的每个单独的形状就叫做一个网格**

    一个网格是我们在OpenGL中绘制物体所需的**最小单位**（顶点数据、索引和材质属性）

    渲染所需要的所有相关数据

    顶点 法向量 纹理坐标 面 材质

    * 面代表渲染图元

  * Material

    构建Assimp

* 网格

  