### Cocos2dx 3.x-综述

[TOC]

#### 1.执行顺序

Application

Director

GLView

* win32/main.cpp

  ```C++
  AppDelegate app;
  return Application::getInstance()->run();
  ```

* Application::run()

  ```C++
  applicationDidFinishLaunching()
  director->mainLoop()
  ```

* AppDelegate::applicationDidFinishLaunching()

  ```C++
  director->runWithScene(scene);
  ```

  * Director::runWithScene(Scene *scene)

    ```C++
    pushScene(scene);
    startAnimation();
    ```

* Director::mainLoop()

  ```C++
  drawScene();
  ```

* Director::drawScene()

  ```C++
  void Director::drawScene()
  {
      // 1.计算_deltaTime
      calculateDeltaTime();
      
      // 2.glfwPollEvents
      if (_openGLView)
      {
          _openGLView->pollEvents();
      }
  
      // 3.计时器
      //tick before glClear: issue #533
      if (! _paused)
      {
          _eventDispatcher->dispatchEvent(_eventBeforeUpdate);
          _scheduler->update(_deltaTime);
          _eventDispatcher->dispatchEvent(_eventAfterUpdate);
      }
  	// 4.glClear
      _renderer->clear();
      experimental::FrameBuffer::clearAllFBOs();
      /* to avoid flickr, nextScene MUST be here: after tick and before draw.
       * FIXME: Which bug is this one. It seems that it can't be reproduced with v0.9
       */
      if (_nextScene)
      {
          setNextScene();
      }
  
      pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
      
      if (_runningScene)
      {
  #if (CC_USE_PHYSICS || (CC_USE_3D_PHYSICS && CC_ENABLE_BULLET_INTEGRATION) || CC_USE_NAVMESH)
          // 5.物理碰撞
          _runningScene->stepPhysicsAndNavigation(_deltaTime);
  #endif
          // 6.visit
          //clear draw stats
          _renderer->clearDrawStats();
          
          //render the scene
          _openGLView->renderScene(_runningScene, _renderer);
          
          _eventDispatcher->dispatchEvent(_eventAfterVisit);
      }
  
      // draw the notifications node
      // 通知节点 单独绘制 不随切换场景消除
      if (_notificationNode)
      {
          _notificationNode->visit(_renderer, Mat4::IDENTITY, 0);
      }
  
      if (_displayStats)
      {
          showStats();
      }
      // 7.渲染
      _renderer->render();
  
      _eventDispatcher->dispatchEvent(_eventAfterDraw);
  
      popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
  
      _totalFrames++;
  
      // 8.交换前后缓冲
      if (_openGLView)
      {
          _openGLView->swapBuffers();
      }
  
      if (_displayStats)
      {
          calculateMPF();
      }
  }
  ```

  