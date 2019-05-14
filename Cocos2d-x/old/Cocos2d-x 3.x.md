* Ref

  构造函数 引用计数为1

  retain release 引用计数为0则delete

  autorelease 加入内存池每帧结束自动release

  * clonable 用来clone

* PoolManager

  内存池管理类 单例

  ```C++
  void Director::mainLoop()
  {
      PoolManager::getInstance()->getCurrentPool()->clear();
  }
  ```
  * AutoreleasePool 自动释放池

    销毁时自动释放池中的对象

  https://blog.51cto.com/shahdza/1612540

* Node

  ```Node is the base element of the Scene Graph. Elements of the Scene Graph must be Node objects or subclasses of it.The most common Node objects are: Scene, Layer, Sprite, Menu, Label.```

  create 工厂函数

  init初始化

  属性变更时  \_transformUpdated = \_transformDirty = \_inverseDirty = true;

  ​	=>visit中调用processParentFlags

  ​	=>判断是否有变化 有则调用transform获得模型视图变化矩阵

  draw 子类override

  visit 递归访问并draw子节点

  

  事件派发

  \_eventDispatcher->setDirtyForNode(this);

  动作

  计时器

  

  color opacity涉及级联 cascade

  componet 组件？

  锚点 坐标 设置位置时 将锚点放到父节点坐标系对应的坐标上去 节点坐标系以左下角为(0, 0)点

  https://blog.51cto.com/shahdza/1550288

  * __NodeRGBA

    __NodeRGBA is a subclass of Node that implements the RGBAProtocol protocol

* Node子类

  * Scene

    https://blog.51cto.com/shahdza/1550375

    ```
    Scene is a subclass of Node that is used only as an abstract concept.
    
    Scene and Node are almost identical with the difference that Scene has its anchor point (by default) at the center of the screen.
    
    For the moment Scene has no other logic than that, but in future releases it might have
    additional logic.
    
    It is a good practice to use a Scene as the parent of all your nodes.
     
    Scene will create a default camera for you.
    ```

  * Layer

    ```
    Layer is a subclass of Node that implements the TouchEventsDelegate protocol.
    
    All features from Node are valid, plus the following new features:
    - It can receive iPhone Touches
    - It can receive Accelerometer input
    ```

    __LayerRGBA

    LayerColor

    LayerGradient

    LayerMultiplex

  * Sprite

    * Texture2D 纹理

      https://blog.csdn.net/xingxinmanong/article/details/75097815

    * TextureCache 纹理缓冲

      减少因生成纹理而多次对资源文件加载造成CPU和内存的开销。对于已经加载过的图形资源文件，TextureCache类不会再次加载，而是之间返回对资源文件的引用

    * SpriteFrame 精灵帧 texture + rect

      https://blog.51cto.com/kingzeus/1545746

    * SpriteFrameCache 精灵帧缓冲

  * Label

  * Widget

