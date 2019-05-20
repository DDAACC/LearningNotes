### Cocos2dx 3.x-渲染-基础

[TOC]

#### 1.以渲染一个Sprite为例

目的：绘制一张图片到屏幕中间

##### 1.1使用OpenGL

* 准备

  * 着色器

    着色器程序创建

    (GLProgram为一个着色器程序，GLProgramCache对着色器程序进行缓存，内置了多个默认程序)

    (GLProgramState保存了uniform和顶点属性设置)

    (sprite使用内置的SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP着色器程序 setTexture->setGLProgramState)

    顶点着色器：根据顶点坐标以及MVP矩阵得到屏幕坐标

    片段着色器：根据纹理坐标以及采样器得到片段颜色

  * 纹理

  * 顶点属性

    顶点坐标、颜色、纹理坐标

    (V3F_C4B_T2F)

  * uniform

    模型、视图、投影矩阵

    采样器

* 渲染

  * 使用shader
  * 绑定顶点属性和uniform
  * draw call

##### 1.2sprite渲染实现

* texture

  sprite创建时进行设置

  initWithTexture->setTexture

* polyInfo

  setTextureRect->updatePoly->setQuad

  * _quad

    setTextureCoords 设置纹理坐标

    setVertexCoords 设置顶点坐标

* transform

  processParentFlags->transform->getNodeToParentTransform

* 生成TrianglsCommand

  **globalZOrder**用于排序

  **texture**

  **GLProgramState** setTexture->setGLProgramState->SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP

  **blendFunc**混合模式 initWithTexture->ALPHA_PREMULTIPLIED alpha预乘

  **polyInfo** 顶点属性

  **transform**模型视图矩阵

  **flags**

* render->drawBatchedTriangles

  使用V3F_C4B_T2F数据设置顶点属性

  useMaterial

  * set texture

  * set blend mode

  * _glProgramState->apply

    applyGLProgram 使用着色器 设置内置uniform

    applyAttributes

    applyUniforms

  glDrawElements 执行draw call

##### 1.3cocos中的渲染

cocos3.x渲染实现

* 遍历场景树时，对节点执行draw方法
* 在draw中添加renderCommand
* 遍历完统一进行渲染

cocos3.x源码渲染相关文件

* RenderCommand

  * CCBatchCommand
  * CCCustomCommand
  * CCGroupCommand
  * CCMeshCommand
  * CCPrimitive
  * CCPrimitiveCommand
  * CCQuadCommand
  * CCRenderCommand
  * CCRenderCommandPool
  * CCTrianglesCommand

* Render

  * CCMaterial
  * CCTechnique
  * CCPass
  * CCRender
  * CCRenderState

* GLProgram

  * CCGLProgram
  * CCGLProgramCache
  * CCGLProgramState
  * CCGLProgramStateCache

* Texture

  * CCTexture2D
  * CCTextureAtlas
  * CCTextureCache
  * CCTextureCube

* 其他

  * ccGLStateCache

  * CCFrameBuffer
  * CCVertexAttribBinding
  * CCVertexIndexBuffer
  * CCVertexIndexData

####2.源码分析

##### 2.1渲染流程

* main.cpp->run->applicationDidFinishLaunching->mainLoop

* applicationDidFinishLaunching->createScene

  ```C++
  bool AppDelegate::applicationDidFinishLaunching() {	
  	auto scene = MainScene::createScene();
      director->runWithScene(scene);
      return true;
  }
  
  Scene::Scene()
  {
      //create default camera
      _defaultCamera = Camera::create();
      addChild(_defaultCamera);
  }
  ```

* mainLoop->drawScene

  ```C++
  void Director::drawScene()
  {
      if (_runningScene)
      {
          //clear draw stats
          _renderer->clearDrawStats(); 
          //渲染当前场景
          _openGLView->renderScene(_runningScene, _renderer);   
          _eventDispatcher->dispatchEvent(_eventAfterVisit);
      }
  
      // draw the notifications node
      if (_notificationNode)
      {
          _notificationNode->visit(_renderer, Mat4::IDENTITY, 0);
      }
  
      if (_displayStats)
      {
          showStats();
      }
      _renderer->render();
  }
  ```

  * renderScene->render

    camera

    * visit

      遍历子节点，调用draw函数

      在draw函数中添加renderCommand

    * render

      根据globalZOrder处理renderCommand

      GLProgramState::apply执行一次drawcall

    ```C++
    void Scene::render(Renderer* renderer, const Mat4* eyeTransforms, const Mat4* eyeProjections, unsigned int multiViewCount)
    {
        auto director = Director::getInstance();
        Camera* defaultCamera = nullptr;
        //模型矩阵
        const auto& transform = getNodeToParentTransform();
    
        for (const auto& camera : getCameras())
        {
            if (!camera->isVisible())
                continue;
    
            Camera::_visitingCamera = camera;
            if (Camera::_visitingCamera->getCameraFlag() == CameraFlag::DEFAULT)
            {
                defaultCamera = Camera::_visitingCamera;
            }
    
            // There are two ways to modify the "default camera" with the eye Transform:
            // a) modify the "nodeToParentTransform" matrix
            // b) modify the "additional transform" matrix
            // both alternatives are correct, if the user manually modifies the camera with a camera->setPosition()
            // then the "nodeToParent transform" will be lost.
            // And it is important that the change is "permanent", because the matrix might be used for calculate
            // culling and other stuff.
            for (unsigned int i = 0; i < multiViewCount; ++i) {
                if (eyeProjections)
                    camera->setAdditionalProjection(eyeProjections[i] * camera->getProjectionMatrix().getInversed());
                if (eyeTransforms)
                    camera->setAdditionalTransform(eyeTransforms[i].getInversed());
                director->pushProjectionMatrix(i);
                director->loadProjectionMatrix(Camera::_visitingCamera->getViewProjectionMatrix(), i);
            }
    
            camera->apply();
            //clear background with max depth
            camera->clearBackground();
            //visit the scene
            visit(renderer, transform, 0);
    
            renderer->render();
            camera->restore();
    
            for (unsigned int i = 0; i < multiViewCount; ++i)
                director->popProjectionMatrix(i);
    
            // we shouldn't restore the transform matrix since it could be used
            // from "update" or other parts of the game to calculate culling or something else.
    //        camera->setNodeToParentTransform(eyeCopy);
        }
    
        Camera::_visitingCamera = nullptr;
    //    experimental::FrameBuffer::applyDefaultFBO();
    }
    ```

##### 2.2GLProgram

* **GLProgram**

  **一个GLProgram对象对应一个着色器程序，实现了顶点着色器、片段着色器的创建编译附加链接以及着色器程序的创建及使用，设置了内置的uniform和顶点属性，_builtInUniforms保存了内置uniform的位置，_flags保存了内置uniform的使用，_userUniforms保存了用户定义uniform的信息，_vertexAttribs保存了顶点属性的信息，可以给unifrom赋值，绑定新的顶点属性**

  着色器程序

  完成着色器的创建、编译、链接

  设置预设顶点属性和uniform

  缓存顶点属性，unifrom相关信息

  * **struct VertexAttrib** 顶点属性

    VertexAttrib is a structure to encapsulate data got from glGetActiveAttrib.

    用来抽象从glGetActiveAttrib获得的数据的结构体

  * **struct Uniform**

    Uniform is a structure to encapsulate data got from glGetActiveUniform and glGetUniformLocation.

    用来抽象从glGetActiveUniform和glGetUniformLocation获得的数据的结构体

  * **UniformFlags**

    用来标识使用了哪些uniform

  * **enum**

    * 预设顶点属性preallocated vertex attribute

      VERTEX_ATTRIB_POSITION = 0

    * 预设uniform Preallocated uniform handle.

      UNIFORM_AMBIENT_COLOR = 0

  * **string**

    * 内置shader类型

    * 内置uniform名 如CC_Texture0

    * 内置顶点属性名 如a_position

      通过bindPredefinedVertexAttribs()及preallocated vertex attribute进行绑定

  * 成员变量

    ```C++
    /**OpenGL handle for program.*/
    GLuint            _program;
    /**OpenGL handle for vertex shader.*/
    GLuint            _vertShader;
    /**OpenGL handle for fragment shader.*/
    GLuint            _fragShader;
    /**Built in uniforms.*/
    GLint             _builtInUniforms[UNIFORM_MAX];
    /**Indicate whether it has a offline shader compiler or not.*/
    bool              _hasShaderCompiler;
    
    /**User defined Uniforms.*/
    std::unordered_map<std::string, Uniform> _userUniforms;
    /**User defined vertex attributes.*/
    std::unordered_map<std::string, VertexAttrib> _vertexAttribs;
    /**Hash value of uniforms for quick access.*/
    std::unordered_map<GLint, std::pair<GLvoid*, unsigned int>> _hashForUniforms;
    //cached director pointer for calling
    Director* _director;
    
    /*needed uniforms*/
    UniformFlags _flags;
    ```

  * 函数

    ```C++
    //创建着色器程序
    createWithByteArrays();
    initWithByteArrays();
    createWithFilenames();
    initWithFilenames();
    
    link(); //链接
    use(); //使用
    compileShader(); //编译
    clearShader();
    reset();
    getProgram();
    
    parseUniforms();
    updateUniformLocation();
    getUniformFlags();
    getUniform();
    getUniformLocation();
    updateUniforms(); //更新内置uniform信息
    getUniformLocationForName();
    setUniformLocationWithXX();
    setUniformsForBuiltins();
    
    bindPredefinedVertexAttribs(); //绑定预设的顶点属性 即layout(location = 0)
    parseVertexAttribs(); //Query and store vertex attribute meta-data from the program 将当前程序中的顶点属性缓存至_vertexAttribs
    getVertexAttrib();
    bindAttribLocation();
    getAttribLocation();
    
    getVertexShaderLog();
    getFragmentShaderLog();
    getProgramLog();
    ```

* **GLProgramCache**

  存放着色器程序的单例

   std::unordered_map<std::string, GLProgram*> _programs;

  * init初始化
    * loadDefaultGLPrograms加载默认着色器程序
    * 监听自定义事件Configuration::CONFIG_FILE_LOADED回调reloadDefaultGLProgramsRelativeToLights

* **GLProgramState**

  保存着色器程序的状态（unifrom和顶点属性），对unifrom和顶点属性赋值并应用，

  缓存顶点属性和uniform设置

  * class CC_DLL **UniformValue**

    缓存将被glUniformXXX送至OpenGL管线的unifrom

    * apply

      _glprogram->setUniformLocationWithXXX

  * class CC_DLL **VertexAttribValue**

    顶点属性和数据指针的抽象

    * apply

      glVertexAttribPointer

  * GLProgramState holds the 'state' (uniforms and attributes) of the GLProgram.

    * 根据OpenGL程序或程序名（根据程序名获得程序）创建

    * 构建VertexAttribValue和UniformValue

    * update

      更新顶点属性uniform设置

    * apply

      * applyGLProgram

        使用着色器程序

        设置模型视图矩阵

      * applyAttributes

      * applyUniforms

    ```C++
    bool _uniformAttributeValueDirty;
    std::unordered_map<std::string, GLint> _uniformsByName;
    std::unordered_map<GLint, UniformValue> _uniforms;
    std::unordered_map<std::string, VertexAttribValue> _attributes;
    std::unordered_map<std::string, int> _boundTextureUnits;
    
    int _textureUnitIndex;
    uint32_t _vertexAttribsFlags;
    GLProgram* _glprogram;
    
    Node* _nodeBinding; // weak ref
    
    // contains uniform name and variable
    std::unordered_map<std::string, std::string> _autoBindings;
    
    // Map of custom auto binding resolvers.
    static std::vector<AutoBindingResolver*> _customAutoBindingResolvers;
    ```

* GLProgramStateCache

  缓存GLProgramState 

##### 2.3render

* render

  render->sort->visitRenderQueue

  visitRenderQueue->visitRenderQueue

* processRenderCommand

  * TRIANGLES_COMMAND
  * MESH_COMMAND 3D
  * GROUP_COMMAND
  * CUSTOM_COMMAND
  * BATCH_COMMAND
  * PRIMITIVE_COMMAND
  * QUAD_COMMAND processRenderCommand里没有

##### 2.4camera

#####2.5其他





