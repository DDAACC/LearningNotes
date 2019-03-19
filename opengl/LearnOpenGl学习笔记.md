### LearnOpenGl学习笔记

#### 入门

* OpenGl

  * 核心模式与立即渲染模式（可编程管线 固定管线）

  * 扩展

  * 状态机 上下文 状态设置 状态使用

  * 对象 一些选项的集合 OpenGL状态的子集

  ​	创建对象 得到一个id保存它的引用

  ​	将对象绑定至上下文的目标位置

  ​	设置选项 保存到当前所绑定的对象中

  ​	解绑对象

* 创建窗口

  * GLFW 创建OpenGL上下文 定义窗口 处理输入

    http://www.glfw.org/download.html

    下载源代码包 使用CMake编译

  * GLAD GLEW

    查询OpenGL函数的位置

    http://glad.dav1d.de/

  * 创建新项目 

    Visual C++，然后选择Empty Project

    工程属性 

    VC++目录 包含目录include 库目录lib （或者添加到VS对应的目录里）

    链接器 输入增加glfw3.lib opengl32.lib

    并添加**glad.c**文件到你的工程中

* 你好，窗口

  * 实例化窗口

    ```C++
    glfwInit(); // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL版本3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //核心模式
    //创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    //初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //窗口变化回调 设置视口大小
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window); // 交换帧缓冲
        glfwPollEvents(); //检查事件触发 更新窗口状态
    }
    
    glfwTerminate(); //释放资源
    ```

  * 视口

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 注册回调函数监测窗口变化

    视口是指OpenGL坐标转换到屏幕上的位置

    **坐标范围(-1~1)**

    http://www.cnblogs.com/Sylla-Zhang/archive/2012/06/19/Sylla.html

  * 渲染循环

  * 输入

    ```C++
    glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS//ESC是否被按下
    ```

  * 颜色缓冲

    ```C++
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置清屏颜色
    glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲
    ```

    glClearColor **状态设置**

    glClear**状态使用**

* 三角形

  **VAO 顶点数组对象**

  **VBO 顶点缓冲对象**

  **EBO 索引缓冲对象**

  图形渲染管线 **3D坐标->2D坐标 2D坐标->屏幕上的像素**

  * 着色器

    GLSL

    * 顶点数据 以数组的形式传入顶点坐标、颜色、纹理坐标等

      **顶点属性**：位置、颜色

      **图元**：将数据渲染成点线还是三角形

      标准化设备坐标 -1 ~ 1

      `一旦你的顶点坐标已经在顶点着色器中处理过，它们就应该是标准化设备坐标了`

      3D坐标-》标准化设备坐标-》屏幕空间坐标（视口）

    * 顶点着色器

      以一个单独顶点为**输入**

      对顶点属性做基本处理

      **VBO**

      在GPU中创建内存存储顶点数据 并配置如何解释这些内存

      ```C++
      unsigned int VBO;
      glGenBuffers(1, &VBO); // 生成VBO对象
      //有多种缓冲类型 VBO的类型是GL_ARRAY_BUFFER
      glBindBuffer(GL_ARRAY_BUFFER, VBO);  //绑定对象类型 
      
      //将用户数据复制到当前缓冲
      //缓冲类型，数据大小，数据，如何管理（变化频率）
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      
      ```

      **着色器**

      ```C++
      unsigned int vertexShader;
      vertexShader = glCreateShader(GL_VERTEX_SHADER); //创建着色器
      //附加源码并编译
      glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
      glCompileShader(vertexShader);
      
      //检测是否编译成功
      int  success;
      char infoLog[512];
      glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
      if(!success)
      {
          glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
          std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
      }
      
      
      //分别编译顶点着色器 片段着色器 然后链接为一个着色器程序对象 输入输出应该相匹配
      unsigned int shaderProgram;
      shaderProgram = glCreateProgram();//创建程序
      //附加着色器并链接
      glAttachShader(shaderProgram, vertexShader);
      glAttachShader(shaderProgram, fragmentShader);
      glLinkProgram(shaderProgram);
      //链接完后删除着色器对象
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
      //激活着色器
      glUseProgram(shaderProgram);
      ```

      * 链接顶点属性

        ```C++
        // 0 对应顶点着色器中顶点属性的位置值(layout(position=0))
        // 3 顶点属性的大小 Vec3
        // 数据类型 是否标准化
        // 3 * sizeof(float) 步长 第二个顶点数据的位置
        // 数据在缓冲中起始位置的偏移量
        //根据调用glVertexAttribPointer时 绑定的VBO 来决定数据来源
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0); //以位置值来启用顶点属性
        ```

      * VAO

        存储顶点属性设置

        ```C++
        //一个顶点数组对象会储存以下这些内容：
        
        //glEnableVertexAttribArray和glDisableVertexAttribArray的调用。
        //通过glVertexAttribPointer设置的顶点属性配置。
        //通过glVertexAttribPointer调用与顶点属性关联的顶点缓冲对象。
        
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        
        // ..:: 初始化代码（只运行一次 (除非你的物体频繁改变)） :: ..
        // 1. 绑定VAO
        glBindVertexArray(VAO);
        // 2. 把顶点数组复制到缓冲中供OpenGL使用
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 3. 设置顶点属性指针
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        [...]
        
        // ..:: 绘制代码（渲染循环中） :: ..
        // 4. 绘制物体
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        someOpenGLFunctionThatDrawsOurTriangle();
        ```

      * 绘制函数

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) 配置如何绘制图元 GL_LINE线框模式

        https://blog.csdn.net/wang15061955806/article/details/48974157

      * EBO

    * 图元装配

      将顶点装配成指定图元形状

    * 几何着色器

      产生新顶点构造新图元

    * 光栅化

      把图元映射为屏幕上的像素 生成片段 并执行裁切

      **一个片段是渲染一个像素所需的所有数据**

    * 片段着色器

      计算出一个像素的最终颜色

    * 测试与混合

      深度测试 模板测试 混合

* 着色器

  GLSL

  ```
  #version version_number 版本声明
  in type in_variable_name; 输入变量/顶点属性
  in type in_variable_name;
  
  out type out_variable_name; 输出变量
  
  uniform type uniform_name; 
  
  int main()
  {
    // 处理输入并进行一些图形操作
    ...
    // 输出处理过的结果到输出变量
    out_variable_name = weird_stuff_we_processed;
  }
  ```

  * 数据类型

    基础类型 int float double uint bool

    向量

    矩阵

  * uniform

    从CPU中的应用想GPU中的着色器发送数据

    全局

    **更新一个uniform之前你必须先使用程序**

    ```C++
    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUseProgram(shaderProgram);
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    ```



* 纹理

  纹理坐标(0~1) 使用纹理坐标获取纹理颜色 = 采样

  * 纹理环绕方式

    定义纹理坐标超出(0~1)时的行为

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

  * 纹理过滤

    **纹理像素**Texture Pixek/Texel  即图片中的像素点

    通过纹理坐标去纹理图像上采样 提取纹理像素的颜色

    纹理过滤处理纹理像素到纹理坐标的转换

    GL_TEXTURE_MIN_FILTER纹理被缩小

    GL_TEXTURE_MAG_FILTER 纹理被放大

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    * 多级渐远纹理Mipmap

      glGenerateMipmaps

      **多级渐远纹理主要是使用在纹理被缩小的情况下**

  * 加载与创建纹理

    使用`stb_image.h`库加载图像

  * 生成纹理

    ```c++
    int width, height, nrChannels;
    unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data); // 释放图像内存
    ```

  * 应用纹理

    GLSL内建数据类型 采样器Sampler 以纹理类型为后缀

    内建texture函数采样纹理的颜色

    颜色 = texture(采样器， 纹理坐标)

    FragColor = texture(ourTexture, TexCoord);

    

    绑定纹理会自动把纹理赋值给片段着色器的采样器：

    * 纹理单元

      一个纹理的位置值称为一个纹理单元

      使用glUniform1i可以给纹理采样器分配一个位置值 这样就可以在片段着色器中设置多个纹理

      默认纹理单元为0 默认激活所以不用主动分配

      ```C++
      //纹理单元的主要目的是让我们在着色器中可以使用多于一个的纹理。通过把纹理单元赋值给采样器，我们可以一次绑定多个纹理，只要我们首先激活对应的纹理单元。就像glBindTexture一样，我们可以使用glActiveTexture激活纹理单元，传入我们需要使用的纹理单元：
      
      glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
      glBindTexture(GL_TEXTURE_2D, texture);
      ```

      内建函数mix 接受两个值作为参数 并根据第三个参数进行线性插值

* 变换

  * 向量

    向量运算 

    * 标准化 用每个分量分别除以长度 得到单位向量 长度为1
    * 点乘 计算夹角
    * 叉乘 产生3个互相正交的向量

  * 矩阵

    单位矩阵

    * 缩放

    * 位移

    * 旋转

      360 = 2pi

      1角度 = pi/180

      1弧度 = 180/pi

    * 齐次坐标

      向量的w分量即齐次坐标

      齐次向量-》3D向量 x/y/z分别除以w

    * 组合

      从右向左读 先进行的变化在右边

      应先缩放 再旋转 最后位移

  * glm

    ```C++
    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5)); //先缩放
    ```

    openGL 列主序 

    https://blog.csdn.net/oracleot/article/details/5378746

    但是GLM并不是把它们的矩阵储存为OpenGL所希望接受的那种，因此我们要先用GLM的自带的函数**value_ptr**来变换这些数据。

    ```c++
    unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    ```

* 坐标系统

  坐标=》标准化设备坐标=》屏幕坐标

  局部空间 =模型= 世界空间=观察= 观察空间=投影= 裁剪空间=视口变换= 屏幕空间

  * 概述

    模型 观察 投影 矩阵

    视口变换

  * 局部空间

  * 世界空间

  * 观察空间

    摄像机

  * 裁剪空间

    投影矩阵指定了一个范围的坐标 并将这个范围内的坐标转换为标准化设备坐标范围的(-1,1)

    `如果只是图元(Primitive)，例如三角形，的一部分超出了裁剪体积(Clipping Volume)，则OpenGL会重新构建这个三角形为一个或多个三角形让其能够适合这个裁剪范围。`

    三个轴范围内的坐标形成一个**平截头体** 只有平截头体内部的坐标才会最终出现在屏幕上

    透视除法 在顶点着色器最后一步执行

    * 正射投影

      `正射平截头体直接将平截头体内部的所有坐标映射为标准化设备坐标，因为每个向量的w分量都没有进行改变；如果w分量等于1.0，透视除法则不会改变这个坐标。`

      glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

      没有考虑透视

    * 透视投影

      `投影矩阵将给定的平截头体范围映射到裁剪空间，除此之外还修改了每个顶点坐标的w值，从而使得离观察者越远的顶点坐标w分量越大。`

      透视除法 每个分量分别除以w分量

      glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

  * 屏幕空间

  * Z缓冲 深度测试

* 摄像机

  * 摄像机/观察空间

    定义一个摄像机 需要

    * 在世界空间的位置

      glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

    * 观察的方向

      glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
      glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

      与实际方向相反

    * 指向上方的向量

      * 右轴 摄像机 正x轴

        用上向量叉乘方向向量

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

      * 上轴 摄像机 正y轴

        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

  * Look At

    ```C++
    glm::mat4 view;
    view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f), //位置
        glm::vec3(0.0f, 0.0f, 0.0f), //目标
        glm::vec3(0.0f, 1.0f, 0.0f)); //上向量
    ```

  * 视角移动

    欧拉角

    俯仰角 偏航角 滚转角

  * 鼠标输入

    ```C++
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //隐藏光标
    
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    glfwSetCursorPosCallback(window, mouse_callback); // 回调
    ```

* 资料

  齐次坐标 透视除法 https://www.jianshu.com/p/7e701d7bfd79