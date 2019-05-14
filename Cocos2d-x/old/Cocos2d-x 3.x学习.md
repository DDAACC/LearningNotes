### Cocos2d-x 3.x

[TOC]

#### 一、开始

[参考](http://docs.cocos.com/cocos2d-x/manual/zh/editors_and_tools/cocosCLTool.html)

* 项目创建

  ```sh
  cocos new <game name> -p <package identifier> -l <language> -d <location>
  ```

* 打开项目

  proj.win32/proName.sln

* 文档

  [文档地址](http://docs.cocos2d-x.org/api-ref/cplusplus/v3x/index.html#sec1)

#### 二、源码

##### Scheduler

CCScheduler.h

* Director持有一个Scheduler对象

  在游戏循环中每帧调用Scheduler::update函数(drawScene)

  ```
  Director::drawScene() => _scheduler->update(_deltaTime);
  ```

* 提供两种定时器

  update定时器 每帧调用注册对象的update函数，可以设置优先级

  ​	使用三个_listEntry队列来存储

  ​		_updatesNegList 优先级小于0

  ​		_updates0List 优先级等于0

  ​		_updatesPosList 优先级大于0

  ​	scheduleUpdate()

  自定义定时器 可以设置回调函数以及调用间隔 使用Timer类来计时

  ​	存储在_hashForTimers中 通过对象分类 每个对象可以设置多个timer

  ​	分为callback和selector两种，分别通过key和getSelector()来区分

  ​		分别有两种schedule()函数(有延迟和无延迟)

* Node中封装了相关定时器函数 可以直接调用

数据结构

_listEntry

​	双向链表 绑定target callback priority 状态变量 paused markedForDeletion

_hashUpdateEntry

​	_hashForUpdates

_hashSelectorEntry

​	_hashForTimers

class Timer

```C++
class CC_DLL Timer : public Ref
{
protected:
    Timer();
public:
    /** get interval in seconds */
    float getInterval() const { return _interval; }
    /** set interval in seconds */
    void setInterval(float interval) { _interval = interval; }
    
    void setupTimerWithInterval(float seconds, unsigned int repeat, float delay);
    
    virtual void trigger(float dt) = 0;  // 执行
    virtual void cancel() = 0; // 停止
    
    /** triggers the timer */
    void update(float dt);
    
protected:
    Scheduler* _scheduler; // weak ref
    float _elapsed; //距离上一次执行的时间 用于与interval比较
    bool _runForever; //是否循环
    bool _useDelay; //是否延迟
    unsigned int _timesExecuted; //执行次数
    unsigned int _repeat; //0 = once, 1 is 2 x executed 最大执行次数 0执行1次 1是执行2次
    float _delay; //延迟时间
    float _interval; //间隔
};

```

​	class TimerTargetSelector: public Timer

​		target selector

​	class TimerTargetCallback: public Timer

​		callback key

​	**class TimerScriptHandler : public Timer**

​		**scriptHandler**



class Scheduler

```C++
class CC_DLL Scheduler : public Ref
{
public:
    //系统服务的优先级
    static const int PRIORITY_SYSTEM;
    
    // 用户定时器的最小优先级
    static const int PRIORITY_NON_SYSTEM_MIN;
    
    Scheduler();
    virtual ~Scheduler();


    float getTimeScale() { return _timeScale; }
    /** Modifies the time of all scheduled callbacks.
    You can use this property to create a 'slow motion' or 'fast forward' effect.
    Default is 1.0. To create a 'slow motion' effect, use values below 1.0.
    To create a 'fast forward' effect, use values higher than 1.0.
    @since v0.8
    @warning It will affect EVERY scheduled selector / action.
    */
    void setTimeScale(float timeScale) { _timeScale = timeScale; }

    /** 'update' the scheduler.
     * You should NEVER call this method, unless you know what you are doing.
     * @lua NA
     */
    void update(float dt);

    /////////////////////////////////////
    
    // schedule
    
    /** The scheduled method will be called every 'interval' seconds.
     If paused is true, then it won't be called until it is resumed.
     If 'interval' is 0, it will be called every frame, but if so, it's recommended to use 'scheduleUpdate' instead.
     If the 'callback' is already scheduled, then only the interval parameter will be updated without re-scheduling it again.
     repeat let the action be repeated repeat + 1 times, use CC_REPEAT_FOREVER to let the action run continuously
     delay is the amount of time the action will wait before it'll start.
     @param callback The callback function.
     @param target The target of the callback function.
     @param interval The interval to schedule the callback. If the value is 0, then the callback will be scheduled every frame.
     @param repeat repeat+1 times to schedule the callback.
     @param delay Schedule call back after `delay` seconds. If the value is not 0, the first schedule will happen after `delay` seconds.
            But it will only affect first schedule. After first schedule, the delay time is determined by `interval`.
     @param paused Whether or not to pause the schedule.
     @param key The key to identify the callback function, because there is not way to identify a std::function<>.
     @since v3.0
     */
    void schedule(const ccSchedulerFunc& callback, void *target, float interval, unsigned int repeat, float delay, bool paused, const std::string& key);

    /** The scheduled method will be called every 'interval' seconds for ever.
     @param callback The callback function.
     @param target The target of the callback function.
     @param interval The interval to schedule the callback. If the value is 0, then the callback will be scheduled every frame.
     @param paused Whether or not to pause the schedule.
     @param key The key to identify the callback function, because there is not way to identify a std::function<>.
     @since v3.0
     */
    void schedule(const ccSchedulerFunc& callback, void *target, float interval, bool paused, const std::string& key);
    
    void schedule(SEL_SCHEDULE selector, Ref *target, float interval, unsigned int repeat, float delay, bool paused);
    
    void schedule(SEL_SCHEDULE selector, Ref *target, float interval, bool paused);
    
    /** Schedules the 'update' selector for a given target with a given priority.
     The 'update' selector will be called every frame.
     The lower the priority, the earlier it is called.
     @since v3.0
     @lua NA
     */
    template <class T>
    void scheduleUpdate(T *target, int priority, bool paused)
    {
        this->schedulePerFrame([target](float dt){
            target->update(dt);
        }, target, priority, paused);
    }

#if CC_ENABLE_SCRIPT_BINDING
    // Schedule for script bindings.
    /** The scheduled script callback will be called every 'interval' seconds.
     If paused is true, then it won't be called until it is resumed.
     If 'interval' is 0, it will be called every frame.
     return schedule script entry ID, used for unscheduleScriptFunc().
     
     @warn Don't invoke this function unless you know what you are doing.
     @js NA
     @lua NA
     */
    unsigned int scheduleScriptFunc(unsigned int handler, float interval, bool paused);
#endif
    /////////////////////////////////////
    
    // unschedule

    /** Unschedules a callback for a key and a given target.
     If you want to unschedule the 'callbackPerFrame', use unscheduleUpdate.
     @param key The key to identify the callback function, because there is not way to identify a std::function<>.
     @param target The target to be unscheduled.
     @since v3.0
     */
    void unschedule(const std::string& key, void *target);

    /** Unschedules a selector for a given target.
     If you want to unschedule the "update", use `unscheduleUpdate()`.
     @param selector The selector that is unscheduled.
     @param target The target of the unscheduled selector.
     @since v3.0
     */
    void unschedule(SEL_SCHEDULE selector, Ref *target);
    
    /** Unschedules the update selector for a given target
     @param target The target to be unscheduled.
     @since v0.99.3
     */
    void unscheduleUpdate(void *target);
    
    /** Unschedules all selectors for a given target.
     This also includes the "update" selector.
     @param target The target to be unscheduled.
     @since v0.99.3
     @lua NA
     */
    void unscheduleAllForTarget(void *target);
    
    /** Unschedules all selectors from all targets.
     You should NEVER call this method, unless you know what you are doing.
     @since v0.99.3
     */
    void unscheduleAll();
    
    /** Unschedules all selectors from all targets with a minimum priority.
     You should only call this with `PRIORITY_NON_SYSTEM_MIN` or higher.
     @param minPriority The minimum priority of selector to be unscheduled. Which means, all selectors which
            priority is higher than minPriority will be unscheduled.
     @since v2.0.0
     */
    void unscheduleAllWithMinPriority(int minPriority);
    
#if CC_ENABLE_SCRIPT_BINDING
    /** Unschedule a script entry. 
     * @warning Don't invoke this function unless you know what you are doing.
     * @js NA
     * @lua NA
     */
    void unscheduleScriptEntry(unsigned int scheduleScriptEntryID);
#endif
    
    /////////////////////////////////////
    
    // isScheduled
    
    /** Checks whether a callback associated with 'key' and 'target' is scheduled.
     @param key The key to identify the callback function, because there is not way to identify a std::function<>.
     @param target The target of the callback.
     @return True if the specified callback is invoked, false if not.
     @since v3.0.0
     */
    bool isScheduled(const std::string& key, const void *target) const;
    
    /** Checks whether a selector for a given target is scheduled.
     @param selector The selector to be checked.
     @param target The target of the callback.
     @return True if the specified selector is invoked, false if not.
     @since v3.0
     */
    bool isScheduled(SEL_SCHEDULE selector, const Ref *target) const;
    
    /////////////////////////////////////
    
    /** Pauses the target.
     All scheduled selectors/update for a given target won't be 'ticked' until the target is resumed.
     If the target is not present, nothing happens.
     @param target The target to be paused.
     @since v0.99.3
     */
    void pauseTarget(void *target);

    /** Resumes the target.
     The 'target' will be unpaused, so all schedule selectors/update will be 'ticked' again.
     If the target is not present, nothing happens.
     @param target The target to be resumed.
     @since v0.99.3
     */
    void resumeTarget(void *target);

    /** Returns whether or not the target is paused.
     * @param target The target to be checked.
     * @return True if the target is paused, false if not.
     * @since v1.0.0
     * @lua NA
     */
    bool isTargetPaused(void *target);

    /** Pause all selectors from all targets.
      You should NEVER call this method, unless you know what you are doing.
     @since v2.0.0
      */
    std::set<void*> pauseAllTargets();

    /** Pause all selectors from all targets with a minimum priority.
      You should only call this with PRIORITY_NON_SYSTEM_MIN or higher.
      @param minPriority The minimum priority of selector to be paused. Which means, all selectors which
            priority is higher than minPriority will be paused.
      @since v2.0.0
      */
    std::set<void*> pauseAllTargetsWithMinPriority(int minPriority);

    /** Resume selectors on a set of targets.
     This can be useful for undoing a call to pauseAllSelectors.
     @param targetsToResume The set of targets to be resumed.
     @since v2.0.0
      */
    void resumeTargets(const std::set<void*>& targetsToResume);

    /** Calls a function on the cocos2d thread. Useful when you need to call a cocos2d function from another thread.
     This function is thread safe.
     @param function The function to be run in cocos2d thread.
     @since v3.0
     @js NA
     */
    void performFunctionInCocosThread(std::function<void()> function);
    
    /**
     * Remove all pending functions queued to be performed with Scheduler::performFunctionInCocosThread
     * Functions unscheduled in this manner will not be executed
     * This function is thread safe
     * @since v3.14
     * @js NA
     */
    void removeAllFunctionsToBePerformedInCocosThread();
    
    /////////////////////////////////////
    
    // Deprecated methods:
    
    /** The scheduled method will be called every 'interval' seconds.
     If paused is true, then it won't be called until it is resumed.
     If 'interval' is 0, it will be called every frame, but if so, it's recommended to use 'scheduleUpdateForTarget:' instead.
     If the selector is already scheduled, then only the interval parameter will be updated without re-scheduling it again.
     repeat let the action be repeated repeat + 1 times, use CC_REPEAT_FOREVER to let the action run continuously
     delay is the amount of time the action will wait before it'll start
     @deprecated Please use `Scheduler::schedule` instead.
     @since v0.99.3, repeat and delay added in v1.1
     @js NA
     */
    CC_DEPRECATED_ATTRIBUTE void scheduleSelector(SEL_SCHEDULE selector, Ref *target, float interval, unsigned int repeat, float delay, bool paused)
    {
        schedule(selector, target, interval, repeat, delay, paused);
    }
    
    /** Calls scheduleSelector with CC_REPEAT_FOREVER and a 0 delay.
     *  @deprecated Please use `Scheduler::schedule` instead.
     *  @js NA
     */
    CC_DEPRECATED_ATTRIBUTE void scheduleSelector(SEL_SCHEDULE selector, Ref *target, float interval, bool paused)
    {
        schedule(selector, target, interval, paused);
    }
    
    /** Schedules the 'update' selector for a given target with a given priority.
     The 'update' selector will be called every frame.
     The lower the priority, the earlier it is called.
     @deprecated Please use 'Scheduler::scheduleUpdate' instead.
     @since v0.99.3
     */
    template <class T>
    CC_DEPRECATED_ATTRIBUTE void scheduleUpdateForTarget(T* target, int priority, bool paused) { scheduleUpdate(target, priority, paused); };
    
    /** Unschedule a selector for a given target.
     If you want to unschedule the "update", use unscheduleUpdateForTarget.
     @deprecated Please use 'Scheduler::unschedule' instead.
     @since v0.99.3
     @js NA
     */
    CC_DEPRECATED_ATTRIBUTE void unscheduleSelector(SEL_SCHEDULE selector, Ref *target) { unschedule(selector, target); };
    
    /** Checks whether a selector for a given target is scheduled.
     @deprecated Please use 'Scheduler::isScheduled' instead.
     @since v0.99.3
     @js NA
     */
    CC_DEPRECATED_ATTRIBUTE bool isScheduledForTarget(Ref *target, SEL_SCHEDULE selector) { return isScheduled(selector, target); };
    
    /** Unschedules the update selector for a given target
     @deprecated Please use 'Scheduler::unscheduleUpdate' instead.
     @since v0.99.3
     */
    CC_DEPRECATED_ATTRIBUTE void unscheduleUpdateForTarget(Ref *target) { return unscheduleUpdate(target); };
    
protected:
    
    /** Schedules the 'callback' function for a given target with a given priority.
     The 'callback' selector will be called every frame.
     The lower the priority, the earlier it is called.
     @note This method is only for internal use.
     @since v3.0
     @js _schedulePerFrame
     */
    void schedulePerFrame(const ccSchedulerFunc& callback, void *target, int priority, bool paused);
    
    void removeHashElement(struct _hashSelectorEntry *element);
    void removeUpdateFromHash(struct _listEntry *entry);

    // update specific

    void priorityIn(struct _listEntry **list, const ccSchedulerFunc& callback, void *target, int priority, bool paused);
    void appendIn(struct _listEntry **list, const ccSchedulerFunc& callback, void *target, bool paused);


    float _timeScale;

    //
    // "updates with priority" stuff
    //
    struct _listEntry *_updatesNegList;        // list of priority < 0
    struct _listEntry *_updates0List;            // list priority == 0
    struct _listEntry *_updatesPosList;        // list priority > 0
    struct _hashUpdateEntry *_hashForUpdates; // hash used to fetch quickly the list entries for pause,delete,etc
    std::vector<struct _listEntry *> _updateDeleteVector; // the vector holds list entries that needs to be deleted after update

    // Used for "selectors with interval"
    struct _hashSelectorEntry *_hashForTimers;
    struct _hashSelectorEntry *_currentTarget;
    bool _currentTargetSalvaged;
    // If true unschedule will not remove anything from a hash. Elements will only be marked for deletion.
    bool _updateHashLocked;
    
#if CC_ENABLE_SCRIPT_BINDING
    Vector<SchedulerScriptHandlerEntry*> _scriptHandlerEntries;
#endif
    
    // Used for "perform Function"
    std::vector<std::function<void()>> _functionsToPerform;
    std::mutex _performMutex;
};
```





#####事件

* Event

  * 子类

    EventAcceleration

    EventCustom

    EventFocus

    EventKeyboard

    EventMouse

    EventTouch

​	

```C++
// 事件类型
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

//属性：
	Type _type;     ///< Event type
    bool _isStopped;       ///< whether the event has been stopped.
    Node* _currentTarget;  ///< Current target
```

* EventListener

  * 子类

  ​	EventListenerAcceleration

  ​	EventListenerCustom

  ​	EventListenerFocus

  ​	EventListenerKeyboard

  ​	EventListenerMouse

  ​	EventListenerTouchOneByOne

  ​	EventListenerTouchAllAtOnce

​	事件监听器

```C++
//类型：
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

//属性：
    std::function<void(Event*)> _onEvent;   /// 回调函数

    Type _type;                             /// 监听器类型
    ListenerID _listenerID;                 /// 监听事件ID
    bool _isRegistered;                     /// 是否注册到分发器中

    int   _fixedPriority;   // The higher the number, the higher the priority, 0 is for scene graph base priority.
    Node* _node;            // scene graph based priority
    bool _paused;           // Whether the listener is paused
    bool _isEnabled;        // Whether the listener is enabled
```

* EventDispatcher

  事件分发器

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
  
  std::set<std::string> _internalCustomListenerIDs;
  
  // Adds event listener.
      void addEventListener(EventListener* listener);
      void forceAddEventListener(EventListener* listener);
  
      void addEventListenerWithSceneGraphPriority(EventListener* listener, Node* node);
      void addEventListenerWithFixedPriority(EventListener* listener, int fixedPriority);
      EventListenerCustom* addCustomEventListener(const std::string &eventName, const std::function<void(EventCustom*)>& callback);
  
  // Removes event listener
      void removeEventListener(EventListener* listener);
      void removeEventListenersForTarget(Node* target, bool recursive = false);
  
      void removeAllEventListeners();
  
      void removeEventListenersForType(EventListener::Type listenerType);
      void removeCustomEventListeners(const std::string& customEventName);
      void removeEventListenersForListenerID(const EventListener::ListenerID& listenerID);
  
   // Pauses / Resumes event listener
      void pauseEventListenersForTarget(Node* target, bool recursive = false);
      void resumeEventListenersForTarget(Node* target, bool recursive = false);
      
      void setPriority(EventListener* listener, int fixedPriority);
      void setEnabled(bool isEnabled);
      bool isEnabled() const;
  
   /** Dispatches the event.**/
      void dispatchEvent(Event* event);
      void dispatchCustomEvent(const std::string &eventName, void *optionalUserData = nullptr);
      void dispatchTouchEvent(EventTouch* event);
      void dispatchEventToListeners(EventListenerVector* listeners, const std::function<bool(EventListener*)>& onEvent);
      void dispatchTouchEventToListeners(EventListenerVector* listeners, const std::function<bool(EventListener*)>& onEvent);
  
  
      bool hasEventListener(const EventListener::ListenerID& listenerID) const;
  	void setDirtyForNode(Node* node);
      EventListenerVector* getListeners(const EventListener::ListenerID& listenerID) const;
      void updateDirtyFlagForSceneGraph();
      
      void sortEventListeners(const EventListener::ListenerID& listenerID);
      void sortEventListenersOfSceneGraphPriority(const EventListener::ListenerID& listenerID, Node* rootNode);
      void sortEventListenersOfFixedPriority(const EventListener::ListenerID& listenerID);
      
      void updateListeners(Event* event);
      void associateNodeAndEventListener(Node* node, EventListener* listener);
      void dissociateNodeAndEventListener(Node* node, EventListener* listener); 
      void releaseListener(EventListener* listener);
      /// Priority dirty flag
      enum class DirtyFlag
      {
          NONE = 0,
          FIXED_PRIORITY = 1 << 0,
          SCENE_GRAPH_PRIORITY = 1 << 1,
          ALL = FIXED_PRIORITY | SCENE_GRAPH_PRIORITY
      };
      void setDirty(const EventListener::ListenerID& listenerID, DirtyFlag flag);
      void visitTarget(Node* node, bool isRootNode);
      void cleanToRemovedListeners();
  ```

* 鼠标事件

  ```C++
  auto* mouseListener = EventListenerMouse::create();
  mouseListener->onMouseDown = CC_CALLBACK_1(GameMainLayer::onMouseDown, this);
  dispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
  ```

* 按钮点击

  ```C++
  startButton->addClickEventListener(CC_CALLBACK_1(GameMainLayer::onClickStartButton, this));
  ```

* 键盘事件

  ```C++
  auto *dispatcher = Director::getInstance()->getEventDispatcher();
  auto* keyListener = EventListenerKeyboard::create();
  keyListener->onKeyPressed = CC_CALLBACK_2(GameMainLayer::onKeyPressed, this);
  keyListener->onKeyReleased = CC_CALLBACK_2(GameMainLayer::onKeyReleased, this);
  dispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
  
  
  bool EventListenerKeyboard::init()
  {
      auto listener = [this](Event* event){
          auto keyboardEvent = static_cast<EventKeyboard*>(event);
          if (keyboardEvent->_isPressed)
          {
              if (onKeyPressed != nullptr)
                  onKeyPressed(keyboardEvent->_keyCode, event);
          }
          else
          {
              if (onKeyReleased != nullptr)
                  onKeyReleased(keyboardEvent->_keyCode, event);
          }
      };
      
      if (EventListener::init(Type::KEYBOARD, LISTENER_ID, listener))
      {
          return true;
      }
      
      return false;
  }
  // 和触摸事件绑定
  void Widget::onTouchEnded(Touch *touch, Event* /*unusedEvent*/)
  {
      _touchEndPosition = touch->getLocation();
  
      /*
       * Propagate touch events to its parents
       */
      if (_propagateTouchEvents)
      {
          this->propagateTouchEvent(TouchEventType::ENDED, this, touch);
      }
  
      bool highlight = _highlight;
      setHighlighted(false);
  
      if (highlight)
      {
          releaseUpEvent();
      }
      else
      {
          cancelUpEvent();
      }
  }
  ```

* 触摸事件

  ```C++
  // Touch:
  int _id;
  bool _startPointCaptured;
  Vec2 _startPoint;
  Vec2 _point;
  Vec2 _prevPoint;
  float _curForce;
  float _maxForce;
  
  
  setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
  setTouchEnabled(true);
  ```

* 自定义事件

  ```C++
  auto* customListener = EventListenerCustom::create("TEST_CUSTOM", CC_CALLBACK_1(GameMainLayer::onGetCustomEvent, this));
  	dispatcher->addEventListenerWithSceneGraphPriority(customListener, this);
  
  
  auto *dispatcher = Director::getInstance()->getEventDispatcher();
  dispatcher->dispatchCustomEvent("TEST_CUSTOM", nullptr);
  ```

##### 数据结构

* Vector
* Value
* Map

---

**libcocos2d 结构**

2d、base、renderer、ui

audioengine、cocosdension 音效

cocosbuilder、cocostudio 编辑器相关

deprecated 不推荐使用

extension、external、math、navmesh、network、physics、physics3d、3d、platform、storage、vr



---

##### 2d

* action

  ```C++
  // action manager
  _actionManager = new (std::nothrow) ActionManager();
  _scheduler->scheduleUpdate(_actionManager, Scheduler::PRIORITY_SYSTEM, false);
  ```

  => Action

  ​	=> FiniteTimeAction     float _duration;

  ​		=> ActionInterval 延时动作

  ```C++
      // FiniteTimeAction     
  	float _duration;
      
  	// ActionInterval      
      float _elapsed;
      bool _firstTick;
      bool _done;
  
      void ActionInterval::step(float dt)
      {
          if (_firstTick)
          {
              _firstTick = false;
              _elapsed = 0;
          }
          else
          {
              _elapsed += dt;
          }
  
  
          float updateDt = MAX (0,                                  // needed for rewind. elapsed could be negative
                                 MIN(1, _elapsed / _duration)
                                 );
  
          if (sendUpdateEventToScript(updateDt, this)) return;
  
          this->update(updateDt);
  
          _done = _elapsed >= _duration;
      }
  ```



  ​			组合动作

  ​			=> Sequence

  ​			=> Repeat

  ​			=> RepeatForever

  ​			=> Spawn



  ​			=> CCActionInterval.h

  ​				RotateTo/By 旋转

  ​				MoveTo/By 移动

  ​				SkewTo/By 偏移

  ​				ResizeTo/By 改变大小

  ​				JumpTo/By 跳跃

  ​				BezierTo/By 贝塞尔曲线

  ​				ScaleTo/By 缩放

  ​				Blink  闪烁(setVisible)

  ​				FadeTo/In/Out 褪色(setOpacity)

  ​				TintTo/By 变色

  ​				DelayTime 延时

  ​				ReverseTime 倒放action

  ​				**Animate 动画**

  ​				TargetedAction 将target和action绑定

  ​				ActionFloat

  ​			=> **CCActionCamera.h**

  ​				ActionCamera

  ​					OrbitCamera

  ​			=> **CCActionCatmullRom.h**

  ​				CardinalSplineTo/By 沿线运动

  ​					CatmullRomTo/By

  ​			=> CCActionEase.h

  ​			=> CCActionGrid.h

  ​			     CCActionGrid3D.h

  ​			     CCActionPageTurn3D.h

  ​                             CCActionTiledGrid.h

  ​			=> CCActionProgressTimer.h

  ​			=> CCActionTween.h

  ​		=> ActionInstance 瞬时动作

  ​			=> Show

  ​			=> Hide

  ​			=> ToggleVisibility

  ​			=> RemoveSelf

  ​			=> FlipX/Y

  ​			=> Place

  ​			=> CallFunc

  ​	=> Speed 变速动作_speed _innerAction

  ```C++
  void Speed::step(float dt)
  {
      _innerAction->step(dt * _speed);
  }
  ```

  ​	=> Follow 跟随动作

  ```C++
  void Follow::step(float /*dt*/)
  {
      if(_boundarySet)
      {
          // whole map fits inside a single screen, no need to modify the position - unless map boundaries are increased
          if(_boundaryFullyCovered)
          {
              return;
          }
  
          Vec2 tempPos = _halfScreenSize - _followedNode->getPosition();
  
          _target->setPosition(clampf(tempPos.x, _leftBoundary, _rightBoundary),
                                     clampf(tempPos.y, _bottomBoundary, _topBoundary));
      }
      else
      {
          _target->setPosition(_halfScreenSize - _followedNode->getPosition());
      }
  }
  ```

  ​	=> ExtraAction Extra action for making a Sequence or Spawn when only adding one action to it.

* 

