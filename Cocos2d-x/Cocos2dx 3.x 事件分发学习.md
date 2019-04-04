### Cocos2dx 3.x 事件分发学习

引擎版本3.14.1

[TOC]

#### 1.概览

* 相关类简介

  * Event类

    Base class of all kinds of events

    所有事件类的基类

    _type

    _isStopped

    _currentTarget

    枚举类型

    ```C++
        enum class Type
        {
            TOUCH, //触摸
            KEYBOARD, //键盘
            ACCELERATION, //加速度
            MOUSE, //鼠标
            FOCUS,
            GAME_CONTROLLER,
            CUSTOM // 自定义
        };
    ```

    EventAcceleration

    EventCustom

    EventFocus

    EventKeyboard

    EventMouse

    EventTouch

  * EventDispatcher类

    This class manages event listener subscriptions
    and event dispatching.

    The EventListener list is managed in such a way that
    event listeners can be added and removed even
    from within an EventListener, while events are being
    dispatched.

    控制监听器的注册和事件的派发

    addEventListenerWithSceneGraphPriority(EventListener* listener, Node* node);

    addEventListenerWithFixedPriority(EventListener* listener, int fixedPriority);

    addCustomEventListener(const std::string &eventName, const std::function<void(EventCustom*)>& callback);

  * EventListener类

    The base class of event listener.

    If you need custom listener which with different callback, you need to inherit this class.

    For instance, you could refer to EventListenerAcceleration, EventListenerKeyboard, EventListenerTouchOneByOne, EventListenerCustom.

    事件监听器的基类

    _onEvent

    _type

    _listenerID

    _isRegistered

    _fixedPriority

    _node

    _paused

    _isEnabled

    子类设置不同的type listenerId 回调

    枚举类型

    ```C++
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

  

    - 触摸事件     : EventListenerTouchOneByOne、EventListenerTouchAllAtOnce
    - 鼠标响应事件 : EventListenerMouse
    - 键盘响应事件 : EventListenerKeyboard
    - 加速计事件   : EventListenerAcceleration
    - 自定义事件   : EventListenerCustom
    - 物理碰撞事件 : EventListenerPhysicsContact
    - 游戏手柄事件 : EventListenerController


#### 2.源码分析

* 事件

  Event类

  ```C++
  class CC_DLL Event : public Ref
  {
  public:
      /** Type Event type.*/
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
      
  CC_CONSTRUCTOR_ACCESS:
      /** Constructor */
      Event(Type type);
  public:
      /** Destructor.
       */
      virtual ~Event();
  
      /** Gets the event type.
       *
       * @return The event type.
       */
      Type getType() const { return _type; }
      
      /** Stops propagation for current event.
       */
      void stopPropagation() { _isStopped = true; }
      
      /** Checks whether the event has been stopped.
       *
       * @return True if the event has been stopped.
       */
      bool isStopped() const { return _isStopped; }
      
      /** Gets current target of the event.
       * @return The target with which the event associates.
       * @note It's only available when the event listener is associated with node.
       *        It returns 0 when the listener is associated with fixed priority.
       */
      Node* getCurrentTarget() { return _currentTarget; }
      
  protected:
      /** Sets current target */
      void setCurrentTarget(Node* target) { _currentTarget = target; }
      
  	Type _type;     ///< Event type 事件类型
      bool _isStopped;       ///< whether the event has been stopped. 事件是否停止
      Node* _currentTarget;  ///< Current target 当前对象
      
      friend class EventDispatcher;
  };
  ```

* 事件监听器

  EventListener类

  fixed priority

  scene graph priority 优先级为0

  设置监听器类型 事件ID 是否注册、暂停、可用 相关联节点

  ```C++
  class CC_DLL EventListener : public Ref
  {
  public:
      /** Type Event type.*/
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
  
      typedef std::string ListenerID;
  
  CC_CONSTRUCTOR_ACCESS:
      /**
       * Constructor
       * @js ctor
       */
      EventListener();
  
      /** 
       * Initializes event with type and callback function
       * @js NA
       */
      bool init(Type t, const ListenerID& listenerID, const std::function<void(Event*)>& callback);
  public:
      /** Destructor.
       * @js NA 
       */
      virtual ~EventListener();
  
      /** Checks whether the listener is available.
       * 
       * @return True if the listener is available.
       */
      virtual bool checkAvailable() = 0;
  
      /** Clones the listener, its subclasses have to override this method.
       */
      virtual EventListener* clone() = 0;
  
      /** Enables or disables the listener.
       * @note Only listeners with `enabled` state will be able to receive events.
       *        When an listener was initialized, it's enabled by default.
       *        An event listener can receive events when it is enabled and is not paused.
       *        paused state is always false when it is a fixed priority listener.
       *
       * @param enabled True if enables the listener.
       */
      void setEnabled(bool enabled) { _isEnabled = enabled; }
  
      /** Checks whether the listener is enabled.
       *
       * @return True if the listener is enabled.
       */
      bool isEnabled() const { return _isEnabled; }
  
  protected:
  
      /** Sets paused state for the listener
       *  The paused state is only used for scene graph priority listeners.
       *  `EventDispatcher::resumeAllEventListenersForTarget(node)` will set the paused state to `true`,
       *  while `EventDispatcher::pauseAllEventListenersForTarget(node)` will set it to `false`.
       *  @note 1) Fixed priority listeners will never get paused. If a fixed priority doesn't want to receive events,
       *           call `setEnabled(false)` instead.
       *        2) In `Node`'s onEnter and onExit, the `paused state` of the listeners which associated with that node will be automatically updated.
       */
      void setPaused(bool paused) { _paused = paused; }
  
      /** Checks whether the listener is paused */
      bool isPaused() const { return _paused; }
  
      /** Marks the listener was registered by EventDispatcher */
      void setRegistered(bool registered) { _isRegistered = registered; }
  
      /** Checks whether the listener was registered by EventDispatcher */
      bool isRegistered() const { return _isRegistered; }
  
      /** Gets the type of this listener
       *  @note It's different from `EventType`, e.g. TouchEvent has two kinds of event listeners - EventListenerOneByOne, EventListenerAllAtOnce
       */
      Type getType() const { return _type; }
  
      /** Gets the listener ID of this listener
       *  When event is being dispatched, listener ID is used as key for searching listeners according to event type.
       */
      const ListenerID& getListenerID() const { return _listenerID; }
  
      /** Sets the fixed priority for this listener
       *  @note This method is only used for `fixed priority listeners`, it needs to access a non-zero value.
       *  0 is reserved for scene graph priority listeners
       */
      void setFixedPriority(int fixedPriority) { _fixedPriority = fixedPriority; }
  
      /** Gets the fixed priority of this listener
       *  @return 0 if it's a scene graph priority listener, non-zero for fixed priority listener
       */
      int getFixedPriority() const { return _fixedPriority; }
  
      /** Sets the node associated with this listener */
      void setAssociatedNode(Node* node) { _node = node; }
  
      /** Gets the node associated with this listener
       *  @return nullptr if it's a fixed priority listener, otherwise return non-nullptr
       */
      Node* getAssociatedNode() const { return _node; }
  
      ///////////////
      // Properties
      //////////////
      std::function<void(Event*)> _onEvent;   /// Event callback function
  
      Type _type;                             /// Event listener type
      ListenerID _listenerID;                 /// Event listener ID
      bool _isRegistered;                     /// Whether the listener has been added to dispatcher.
  
      int   _fixedPriority;   // The higher the number, the higher the priority, 0 is for scene graph base priority.
      Node* _node;            // scene graph based priority
      bool _paused;           // Whether the listener is paused
      bool _isEnabled;        // Whether the listener is enabled
      friend class EventDispatcher;
  };
  ```

* 事件分发器

  EventDispatcher类

  A lower priority will be called before the ones that have a higher value.

  ```C++
  /** Listeners map */
  std::unordered_map<EventListener::ListenerID, EventListenerVector*> _listenerMap;
  
  /** The map of dirty flag */
  std::unordered_map<EventListener::ListenerID, DirtyFlag> _priorityDirtyFlagMap;
  
  /** The map of node and event listeners */
  std::unordered_map<Node*, std::vector<EventListener*>*> _nodeListenersMap;
  
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
  
  /** Whether the dispatcher is dispatching event */
  int _inDispatch;
  
  /** Whether to enable dispatching event */
  bool _isEnabled;
  
  int _nodePriorityIndex;
  ```

  * visitTarget 
  * pauseEventListenersForTarget

#### 3.使用

* 触摸

  单点触摸

  ```C++
  //1.创建监听器对象
  
  auto touchlistener = EventListenerTouchOneByOne::create();
  
  //2.设置回调
  
  touchlistener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
  touchlistener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
  touchlistener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
  
  //3.注册监听器
  _eventDispatcher->addEventListenerWithFixedPriority(touchlistener, 1.0);
  
  ```

  多点触摸

  ```c++
  auto listener = EventListenerTouchAllAtOnce::create();
  listener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
  listener->onTouchesMoved = CC_CALLBACK_2(HelloWorld::onTouchesMoved, this);
  listener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
  listener->onTouchesCancelled = CC_CALLBACK_2(HelloWorld::onTouchesCancelled, this);
  
  _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
  ```


* Button

  ```C++
  attakcButton->addClickEventListener(CC_CALLBACK_1(HelloWorld::btnAttackClick, this));
  ```


#### 4.参考资料

https://www.cnblogs.com/liugangBlog/p/6285852.html

https://www.jianshu.com/p/d8b052a950d1