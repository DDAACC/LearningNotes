### Cocos2dx 3.x-渲染

[TOC]

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

  