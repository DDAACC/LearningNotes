### Cocos2dx 3.x-渲染-概览

[TOC]

#### 1.以渲染一个Sprite为例

##### 1.1流程

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

    matrix

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

#####1.2OpenGL调用

* 初始化

  applicationDidFinishLaunching->GLViewImpl::createWithRect

  glfw/glew初始化

* shader

  * 创建

    ```C++
    Camera::Camera()
    {
        _clearBrush = CameraBackgroundBrush::createDepthBrush(1.f);
    }
    
    CameraBackgroundDepthBrush* CameraBackgroundBrush::createDepthBrush(float depth)
    {
        return CameraBackgroundDepthBrush::create(depth);
    }
    
    bool CameraBackgroundDepthBrush::init()
    {
        auto shader = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_CAMERA_CLEAR);
        _glProgramState = GLProgramState::getOrCreateWithGLProgram(shader);
        _glProgramState->retain();
    }
    
    bool GLProgramCache::init()
    {
        loadDefaultGLPrograms();
    }
    //载入默认着色器程序loadDefaultGLPrograms()
    ```

  * 使用

    loadDefaultGLPrograms->updateUniforms->use

    drawBatchedTriangles->useMaterial->GLProgramState::apply

    ```C++
    void GLProgramState::apply(const Mat4& modelView)
    {
        applyGLProgram(modelView);
    
        applyAttributes();
    
        applyUniforms();
    }
    ```

* data

* drawcall

##### 1.3渲染一个sprite

* 添加sprite

  ```C++
  auto bird = Sprite::create("bird1.png");
  addChild(bird);
  bird->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  
  void Node::setPosition(float x, float y)
  {
      if (_position.x == x && _position.y == y)
          return;
      
      _position.x = x;
      _position.y = y;
      
      _transformUpdated = _transformDirty = _inverseDirty = true;
      _usingNormalizedPosition = false;
  }
  
  void Sprite::setTexture(Texture2D *texture)
  {       				       setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP, texture));
  }
  /*
  _globalZOrder 
  _texture 			Texture2D纹理
  getGLProgramState()
  _blendFunc			混合模式 _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
  _polyInfo.triangles
  transform
  flags
  */
  void Sprite::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
  {
      _trianglesCommand.init(_globalZOrder,
                             _texture,
                             getGLProgramState(),
                             _blendFunc,
                             _polyInfo.triangles,
                             transform,
                             flags);
  
      renderer->addCommand(&_trianglesCommand);
  }
  ```

* visit

  _modelViewTransform

  flags

  * processParentFlags

    _transformUpdated

    _contentSizeDirty

  * matrix **deprecated**

    push load

  ```C++
   _modelViewTransform = this->transform(parentTransform);
  Mat4 Node::transform(const Mat4& parentTransform)
  {
      return parentTransform * this->getNodeToParentTransform();
  }
  
  void Node::visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags)
  {
      // quick return if not visible. children won't be drawn.
      if (!_visible)
      {
          return;
      }
  
      uint32_t flags = processParentFlags(parentTransform, parentFlags);
  
      // IMPORTANT:
      // To ease the migration to v3.0, we still support the Mat4 stack,
      // but it is deprecated and your code should not rely on it
      _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
      _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
      
      bool visibleByCamera = isVisitableByVisitingCamera();
  
      int i = 0;
  
      if(!_children.empty())
      {
          sortAllChildren();
          // draw children zOrder < 0
          for(auto size = _children.size(); i < size; ++i)
          {
              auto node = _children.at(i);
  
              if (node && node->_localZOrder < 0)
                  node->visit(renderer, _modelViewTransform, flags);
              else
                  break;
          }
          // self draw
          if (visibleByCamera)
              this->draw(renderer, _modelViewTransform, flags);
  
          for(auto it=_children.cbegin()+i, itCend = _children.cend(); it != itCend; ++it)
              (*it)->visit(renderer, _modelViewTransform, flags);
      }
      else if (visibleByCamera)
      {
          this->draw(renderer, _modelViewTransform, flags);
      }
  
      _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
      
      // FIX ME: Why need to set _orderOfArrival to 0??
      // Please refer to https://github.com/cocos2d/cocos2d-x/pull/6920
      // reset for next frame
      // _orderOfArrival = 0;
  }
  ```

  

#### 2.关键类

##### 2.1GLProgram

* GLProgram

  着色器程序

  * struct VertexAttrib 顶点属性

    index

    size 

    type 数据类型

    name 顶点着色器中的名字

    预设顶点属性enum

  * struct Uniform

    location

    size

    type

    name

    预设uniform enum

    UniformFlags

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

  * createXXX->initXXX

    创建着色器程序

    创建、编译、附加、链接着色器

    * link

      -> bindPredefinedVertexAttribs->glBindAttribLocation(指定索引和变量直接的关系 layout(location = 0)

      -> parseVertexAttribs() 

      Query and store vertex attribute meta-data from the program.

      -> parseUniforms() _userUniforms

      Query and store uniforms from the program. 

      -> clearShader() 删除着色器

    * updateUniforms

      _builtInUniforms获得uniform地址

      _flags设置flags

      设置纹理单元

* GLProgramCache

  存放着色器程序的单例

   std::unordered_map<std::string, GLProgram*> _programs;

  init->

  ​	loadDefaultGLPrograms加载默认着色器程序

  ​	监听自定义事件Configuration::CONFIG_FILE_LOADED回调reloadDefaultGLProgramsRelativeToLights

* GLProgramState

  * class CC_DLL UniformValue

    * apply

      _glprogram->setUniformLocationWithXXX

  * class CC_DLL VertexAttribValue

    * apply

      glVertexAttribPointer

  * GLProgramState holds the 'state' (uniforms and attributes) of the GLProgram.

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

* 使用

##### 2.2render

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

##### 2.3camera

#####2.4其他

####1.概述

#####1.1游戏主循环 渲染

 \_openGLView->renderScene(_runningScene, _renderer);

scene->render(renderer, Mat4::IDENTITY, nullptr);

* 到父节点坐标系的变换矩阵

  const auto& transform = getNodeToParentTransform();

* camera

  ```C++
  //Scene::Scene()
  _defaultCamera = Camera::create();
  addChild(_defaultCamera);
  
  void Camera::onEnter()
  {
      if (_scene == nullptr)
      {
          auto scene = getScene();
          if (scene)
          {
              setScene(scene);
          }
      }
      Node::onEnter();
  }
  ```

  * 观察矩阵？
  * apply
  * clearBackground
  * visit遍历场景树
  * renderer->render()

* visit

  processParentFlags

  ```C++
  void Node::visit()
  {
      auto renderer = _director->getRenderer();
      auto& parentTransform = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
      visit(renderer, parentTransform, true);
  }
  
  void Node::visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags)
  {
      // quick return if not visible. children won't be drawn.
      if (!_visible)
      {
          return;
      }
  
      uint32_t flags = processParentFlags(parentTransform, parentFlags);
  
      // IMPORTANT:
      // To ease the migration to v3.0, we still support the Mat4 stack,
      // but it is deprecated and your code should not rely on it
      _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
      _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
      
      bool visibleByCamera = isVisitableByVisitingCamera();
  
      int i = 0;
  
      if(!_children.empty())
      {
          sortAllChildren();
          // draw children zOrder < 0
          for(auto size = _children.size(); i < size; ++i)
          {
              auto node = _children.at(i);
  
              if (node && node->_localZOrder < 0)
                  node->visit(renderer, _modelViewTransform, flags);
              else
                  break;
          }
          // self draw
          if (visibleByCamera)
              this->draw(renderer, _modelViewTransform, flags);
  
          for(auto it=_children.cbegin()+i, itCend = _children.cend(); it != itCend; ++it)
              (*it)->visit(renderer, _modelViewTransform, flags);
      }
      else if (visibleByCamera)
      {
          this->draw(renderer, _modelViewTransform, flags);
      }
  
      _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
      
      // FIX ME: Why need to set _orderOfArrival to 0??
      // Please refer to https://github.com/cocos2d/cocos2d-x/pull/6920
      // reset for next frame
      // _orderOfArrival = 0;
  }
  ```

  