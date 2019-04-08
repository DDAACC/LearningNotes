### LearnOpenGl学习笔记四

#### 高级OpenGL

* 深度测试

  深度缓冲是在片段着色器运行之后（以及模板测试(Stencil Testing)运行之后，我们将在[下一节](https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/02%20Stencil%20testing/)中讨论）在屏幕空间中运行的

* 模板测试

* 混合

  实现物体透明度(Transparency)的一种技术

  * 丢弃片段 用于完全透明的物体

    丢弃(Discard)显示纹理中透明部分的片段

    GLSL给了我们`discard`命令，一旦被调用，它就会保证片段不会被进一步处理

    ```
    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
    discard;
    FragColor = texColor;
    ```

  * 混合

    glEnable(GL_BLEND); 启用混合
    $$
    C¯result=C¯source∗Fsource+C¯destination∗Fdestination
    $$
    片段着色器运行完成后，并且所有的测试都通过之后，这个混合方程(Blend Equation)才会应用到片段颜色输出与当前颜色缓冲中的值（当前片段之前储存的之前片段的颜色）上

    glBlendFunc(GLenum sfactor, GLenum dfactor)函数接受两个参数，来设置源和目标因子

    glBlendFunc

    glBlendColor

    glBlendFuncSeparate

    glBlendEquation

  * 渲染半透明纹理

    在绘制的时候，我们必须先手动将窗户按照最远到最近来排序，再按照顺序渲染。（深度测试）

    1. 先绘制所有不透明的物体。
    2. 对所有透明的物体排序。
    3. 按顺序绘制所有透明的物体。

  * 不要打乱顺序

    **次序无关透明度**

* 面剔除

  检查所有面向(Front Facing)观察者的面，并渲染它们，而丢弃那些背向(Back Facing)的面

  **逆时针**顶点所定义的三角形将会被处理为**正向三角形**

  glEnable(GL_CULL_FACE); 开启面剔除

  glCullFace(GL_FRONT); 改变剔除的面

  glFrontFace(GL_CCW); 定义正向面GL_CCW逆时针 GL_CW顺时针

* 帧缓冲

  颜色缓冲+深度缓冲+模板缓冲 = 帧缓冲

  默认帧缓冲在创建窗口的时候生成和配置

  * 创建帧缓冲

    ```c++
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    
    glDeleteFramebuffers(1, &fbo);
    ```

    一个完整的帧缓冲需要满足以下的条件：

    - 附加至少一个缓冲（颜色、深度或模板缓冲）。
    - 至少有一个颜色附件(Attachment)。
    - 所有的附件都必须是完整的（保留了内存）。
    - 每个缓冲都应该有相同的样本数。

    glCheckFramebufferStatus 检查是否完整

    **附件**是一个内存位置，它能够作为帧缓冲的一个缓冲

    * 纹理附件

      当把一个纹理附加到帧缓冲的时候，所有的渲染指令将会写入到这个纹理中

      ```C++
      unsigned int texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
      ```

    * 渲染缓冲对象附件

      它会将数据储存为OpenGL原生的渲染格式，它是为离屏渲染到帧缓冲优化过的

      只写 通过glReadPixels读取

      ```C++
      unsigned int rbo;
      glGenRenderbuffers(1, &rbo);
      
      glBindRenderbuffer(GL_RENDERBUFFER, rbo);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
      
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
      ```

      由于渲染缓冲对象通常都是只写的，它们会经常用于深度和模板附件，因为大部分时间我们都不需要从深度和模板缓冲中读取值，只关心深度和模板测试。我们**需要**深度和模板值用于测试，但不需要对它们进行**采样**，所以渲染缓冲对象非常适合它们。

  * 渲染到纹理

  * 后期处理

    * 反相

    * 灰度

      RGB平均化

      ```C++
      float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
      FragColor = vec4(average, average, average, 1.0);
      
      //这已经能创造很好的结果了，但人眼会对绿色更加敏感一些，而对蓝色不那么敏感，所以为了获取物理上更精确的效果，我们需要使用加权的(Weighted)通道：
      
      void main()
      {
          FragColor = texture(screenTexture, TexCoords);
          float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
          FragColor = vec4(average, average, average, 1.0);
      }
      ```

    * 核效果

      锐化

      模糊

      边缘检测

* 立方体贴图

  将多个纹理组合起来映射到一张纹理上的一种纹理类型

  可以通过一个方向向量来进行索引/采样

  只要立方体的中心位于原点，我们就能使用立方体的实际位置向量来对立方体贴图进行采样了

  * 创建立方体贴图

  * 天空盒

  * 优化

    提前深度测试 

  * 环境映射
    * 发射
    * 折射

  * 动态环境贴图

    通过使用帧缓冲，我们能够为物体的6个不同角度创建出场景的纹理，并在每个渲染迭代中将它们储存到一个立方体贴图中。之后我们就可以使用这个（动态生成的）立方体贴图来创建出更真实的，包含其它物体的，反射和折射表面了。这就叫做动态环境映射(Dynamic Environment Mapping)，因为我们动态创建了物体周围的立方体贴图，并将其用作环境贴图。

* 高级数据

  * 分批顶点属性
  * 复制缓冲

* 高级GLSL

  