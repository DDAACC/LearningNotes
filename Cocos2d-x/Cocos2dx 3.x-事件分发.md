### Cocos2dx 3.x - 事件分发

[TOC]

#### 1.概述

* 在**Director中创建了一个EventDispatcher**对象，用来管理事件的分发

  ```C++
  Node::Node()
  {
      _director = Director::getInstance();
      _actionManager = _director->getActionManager();
      _actionManager->retain();
      _scheduler = _director->getScheduler();
      _scheduler->retain();
      _eventDispatcher = _director->getEventDispatcher();
      _eventDispatcher->retain();
  }
  ```

* 在需要的地方**创建一个EventListener监听器**，设置回调函数，监听器根据**ListenerID**来对事件进行监听

  创建时将设置**监听器的类型**、**ListenerID**以及**回调函数**

  不同类型的监听器拥有不同的ListenerID(一个字符串) **自定义事件监听器的ListenerID为自定义事件的名称**

* 将监听器注册到分发器，分发器**根据ListenerID将监听器保存到相应的队列**中

  每个ListenerID有两个队列

  * **优先级队列**

    注册时 设置优先级 

    ```C++
    void EventDispatcher::addEventListenerWithFixedPriority(EventListener* listener, int fixedPriority)
    ```

    分发时 根据优先级从低到高分发

  * **场景图队列**

    注册时 设置绑定的Node 场景图队列的优先级都为0 

    ```C++
    void EventDispatcher::addEventListenerWithSceneGraphPriority(EventListener* listener, Node* node)
    ```

    分发时 根据节点的层次从上向下分发

* 调用dispatchEvent来分发事件，分发器将根据优先级依次进行分发

#### 2.相关类简介

##### 2.1Event事件

包含了**事件类型**以及需要的**相关信息**

* 所有**事件类型**枚举

  ```C++
  enum class Type
  {
      TOUCH,
      KEYBOARD,
      ACCELERATION,
      MOUSE,
      FOCUS,
      GAME_CONTROLLER,
      CUSTOM
  };
  ```

* 主要成员变量

  ```C++
  Type _type;     ///< Event type 标记事件类型
  //是否停止继续分发
  bool _isStopped;       ///< whether the event has been stopped.
  //当前绑定的node
  Node* _currentTarget;  ///< Current targe
  ```

* 子类

  每一种事件类型派生出一个基类，如EventTouch、EventMouse、EventCustom

  **附加一些该类型事件需要的信息**

  ```C++
  //EventTouch
  EventCode _eventCode; //触摸类型 BEGAN MOVED ENDED CANCELLED
  std::vector<Touch*> _touches; //触摸点 
  
  //EventCustom
  void* _userData;       ///< User data
  std::string _eventName;
  ```

#####2.2EventListener监听器

负责监听事件触发回调 listenerID用来标识监听的事件

* **监听器类型**

  ```C++
  /** Type Event type.
  基本和事件类型一一对应 触摸事件分成了单点和多点两种
  */
  enum class Type
  {
      UNKNOWN,
      TOUCH_ONE_BY_ONE,
      TOUCH_ALL_AT_ONCE,
      KEYBOARD,
      MOUSE,
      ACCELERATION,
      FOCUS,
      GAME_CONTROLLER,
      CUSTOM
  };
  ```

* 成员变量

  ```C++
  bool EventListener::init(Type t, const ListenerID& listenerID, const std::function<void(Event*)>& callback)
  // 初始化的时候设置类型、id和回调
  
  std::function<void(Event*)> _onEvent;   /// Event callback function
  
  Type _type;                             /// Event listener type
  /*  除了自定义事件监听器外，其他监听器的listenerID都是一个固定的字符串
      如： __cc_touch_all_at_once __cc_touch_one_by_one
      自定义事件监听器listenerID是事件名
  */
  ListenerID _listenerID;                 /// Event listener ID
  //是否被添加到了dispatcher中
  bool _isRegistered;                     /// Whether the listener has been added to dispatcher.
  // 该监听器的优先级， 场景图监听器优先级为0
  int   _fixedPriority;   // The higher the number, the higher the priority, 0 is for scene graph base priority.
  // 场景图监听器绑定的node
  Node* _node;            // scene graph based priority
  //是否暂停接收事件 用于场景图监听器
  bool _paused;           // Whether the listener is paused
  //是否可用
  bool _isEnabled;        // Whether the listener is enabled
  ```

* 子类

  每一种监听器类型派生出一个基类，如EventListenerTouchAllAtOnce、EventListenerTouchOneByOne、EventListenerCustom

  根据监听器类型来调用init进行初始化，设置回调函数

  ```C++
  bool EventListenerTouchOneByOne::init()
  {
      if (EventListener::init(Type::TOUCH_ONE_BY_ONE, LISTENER_ID, nullptr))
      {
          return true;
      }
      
      return false;
  }
  
  //将事件作为参数传递给回调函数
  bool EventListenerCustom::init(const ListenerID& listenerId, const std::function<void(EventCustom*)>& callback)
  {
      bool ret = false;
      
      _onCustomEvent = callback;
      
      auto listener = [this](Event* event){
          if (_onCustomEvent != nullptr)
          {
              _onCustomEvent(static_cast<EventCustom*>(event));
          }
      };
      
      if (EventListener::init(EventListener::Type::CUSTOM, listenerId, listener))
      {
          ret = true;
      }
      return ret;
  }
  ```

#####2.3EventDispatcher事件分发

负责管理监听器以及分发事件

* 成员变量

  ```C++
  /** Listeners map 
  根据listenerID来区分事件，为每一个listenerID创建一个EventListenerVector存储两种队列
  */
  std::unordered_map<EventListener::ListenerID, EventListenerVector*> _listenerMap;
  
  /** The map of dirty flag 
  为每个listenerID设立一个脏标记，标记该ID的两个队列是否需要重新排序
  */
  std::unordered_map<EventListener::ListenerID, DirtyFlag> _priorityDirtyFlagMap;
  
  /** The map of node and event listeners 
  对场景图队列记录每个node上绑定的监听器
  */
  std::unordered_map<Node*, std::vector<EventListener*>*> _nodeListenersMap;
  
  //_nodePriorityMap _globalZOrderNodeMap _nodePriorityIndex用于对场景图队列排序
  /** The map of node and its event priority */
  std::unordered_map<Node*, int> _nodePriorityMap;
  
  /** key: Global Z Order, value: Sorted Nodes */
  std::unordered_map<float, std::vector<Node*>> _globalZOrderNodeMap;
  
  /** The listeners to be added after dispatching event */
  std::vector<EventListener*> _toAddedListeners;
  
  /** The listeners to be removed after dispatching event */
  std::vector<EventListener*> _toRemovedListeners;
  
  /** The nodes were associated with scene graph based priority listeners */
  std::set<Node*> _dirtyNodes;
  
  /** Whether the dispatcher is dispatching event 
  正在分发的事件数量
  */
  int _inDispatch;
  
  /** Whether to enable dispatching event 
  是否可以分发事件
  */
  bool _isEnabled;
  
  int _nodePriorityIndex;
  // 内部自定义监听事件
  std::set<std::string> _internalCustomListenerIDs;
  ```

* 优先级

  **优先级越低，越先响应事件**

  场景图监听器（优先级为0，根据场景图排序）

  优先级监听器（不能为0）

* EventListenerVector

  The vector to store event listeners with scene graph based priority and fixed priority.

  用来保存监听器的vector

  ```C++
  //优先级监听器
  std::vector<EventListener*>* _fixedListeners;
  //场景图监听器
  std::vector<EventListener*>* _sceneGraphListeners;
  //标记第一个优先级>=0的监听器的位置 在排序时进行设置
  ssize_t _gt0Index;
  //根据监听器的优先级放到相应的队列中去
  void EventDispatcher::EventListenerVector::push_back(EventListener* listener)
  ```

* 主要函数

  * 添加监听器

    * addEventListener

      根据_inDispatch判断当前是否在分发事件

      没有则直接添加，有则添加到_toAddedListeners中

    * forceAddEventListener

      根据listenerID找到对应的vector 并添加

      判断是否为场景图监听器，设置dirty flag

    * addEventListenerWithSceneGraphPriority

      ```C++
      listener->setAssociatedNode(node);
      listener->setFixedPriority(0);
      listener->setRegistered(true);
      ```

    * addEventListenerWithFixedPriority

      ```C++
      listener->setAssociatedNode(nullptr);
      listener->setFixedPriority(fixedPriority);
      listener->setRegistered(true);
      listener->setPaused(false);
      ```

    * addCustomEventListener

      ```C++
      EventListenerCustom *listener = EventListenerCustom::create(eventName, callback);
      addEventListenerWithFixedPriority(listener, 1);
      ```

  * 事件分发

    * dispatchEvent

      传入要分发的事件

      1.触摸事件 调用dispatchTouchEvent

      排序sortEventListeners

      2.否则,根据listenerID找到对应的监听器 触发回调

      ​	2.1dispatchEventToListeners

      ​        2.2鼠标事件dispatchTouchEventToListeners

      更新监听器updateListeners

    * dispatchTouchEvent

      排序sortEventListeners

    * dispatchEventToListeners

      根据优先级依次分发

      ```C++
      //停止继续分发
      if (l->isEnabled() && !l->isPaused() && l->isRegistered() && onEvent(l))
      {
          shouldStopPropagation = true;
          break;
      }
      
      //onEvent
      return event->isStopped();
      ```

    * dispatchTouchEventToListeners

      sceneGraphPriorityListeners处理不一样

    * dispatchCustomEvent

      ```C++
      EventCustom ev(eventName);
      ev.setUserData(optionalUserData);
      dispatchEvent(&ev);
      ```

  * 排序

    * 优先级队列根据优先级排序

    * 场景图队列中序遍历整个场景树

      根据global zorder将绑定了监听器的node存入_globalZOrderNodeMap

      按照global zorder从小到大将node存入\_nodePriorityMap并为每个node设置一个优先级(优先级为\_nodePriorityIndex的值，每次设置完后递增)

      最后对\_nodePriorityMap根据优先级排序    

####3.其他

##### 3.1Node

##### 3.2触摸

 ##### 3.3参考资料

<https://www.jianshu.com/p/d8b052a950d1>