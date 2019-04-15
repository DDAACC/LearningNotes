### Cocos2d-x 3.x - 计时器

[TOC]

#### 1.概述

* Timer类

  Timer类实现计时功能

  设置间隔时间，循环次数，延迟，每隔一段时间调用一次回调

  * TimerTargetSelector

    设置 selector target

    回调 target->selector

    查找 target + selector

  * TimerTargetCallback

    设置callback target key

    回调 callback

    查找 target + key

* 两种回调

  * update selector

    每帧调用 可以设置优先级

    存入_hashForUpdates

    * _listEntry

      双向链表 

       A list double-linked list used for "updates with priority"

    * _hashUpdateEntry

      哈希表

  * custom selector

    可以自定义间隔时间

    存入_hashForTimers

    * _hashSelectorEntry

      哈希表

* schedule

  * 优先级

    scheduleUpdate(T *target, int priority, bool paused)

    Scheduler::schedulePerFrame(const ccSchedulerFunc& callback, void *target, int priority, bool paused)

    回调默认为update()函数

    * 在_hashForUpdates查找

    * 根据优先级添加

      < 0 && > 0_updatesNegList _updatesPosList

      == 0 _updates0List

  * 自定义

    同一个target上可以绑定多个timer

    * callback

      schedule(const ccSchedulerFunc& callback, void *target, float interval, unsigned int repeat, float delay, bool paused, const std::string& key)

      根据key来索引timer

    * selector

      schedule(SEL_SCHEDULE selector, Ref *target, float interval, unsigned int repeat, float delay, bool paused)

      根据selector来索引timer

* update

  主循环中每帧调用\_scheduler->update(_deltaTime);

#### 2.源码分析

