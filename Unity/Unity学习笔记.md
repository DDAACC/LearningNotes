[TOC]

##Unity学习笔记

###Unity的使用

####界面

主编辑器窗口如下所示

编辑器里面的所有窗口都是可以重新排列、组合的

![](https://docs.unity3d.com/uploads/Main/Editor-Breakdown.jpg)

下面对各个窗口作简介

* Project Window

  ![](https://docs.unity3d.com/uploads/Main/ProjectWindowCallout.jpg)

  在Project窗口中显示了当前项目中可用的资源列表(Assets文件夹)，当你导入了新的资源文件时，它们将出现在这个窗口

* Scene View

  ![](https://docs.unity3d.com/uploads/Main/SceneViewCallout.jpg)



* Hierarchy Window

  ![](https://docs.unity3d.com/uploads/Main/HierarchyWindowCallout.jpg)

  场景中所有的物体都会被分层显示在这个窗口中

* Inspector Window

  ![](https://docs.unity3d.com/uploads/Main/InspectorWindowCallout.jpg)

  在这个窗口中，你可以查看并且编辑你选中的场景中物体的属性

* Toolbar

  ![](https://docs.unity3d.com/uploads/Main/ToolbarCallout.png)

  提供了一系列的Unity中基本操作的快捷键

#### 基础概念

##### 场景(Scene)

在一个场景中包含了游戏中的环境和菜单等。当你创建了一个新的Unity项目后，你将在**Scene View**中看到一个新的场景。这个与创建的场景是**未命名**以及**未保存**的。这个场景中只包含了一个**Camera（相机）**和**Light(灯光)**

![](https://docs.unity3d.com/2017.4/Documentation/uploads/Main/NewEmptyScene.png)

* 保存场景

  通过选择**File** > **Save Scene**或者按下Ctrl+S来保存当前正在使用的场景。被保存的场景将存放在Assets文件夹中，并且显示在Project Window里面

* 打开场景

  双击Project window里面的场景资源即可打开一个已保存的场景。

##### 游戏物体(Game Object)

Game Object是Unity编辑器中最为重要的概念。所有出现在游戏中的物体都是一个Game Object，游戏里面的游戏角色、游戏道具乃至游戏光照、相机、特效都是Game Object，尽管如此，单独的一个Game Object不能发挥任何效果，你需要为它配置属性。

你需要为一个Game Object添加**组件(componets)**让其发挥作用。通过不同的**组件**的组合为Game Object添加了不同的属性让其发挥了应有的效果

* 组件(compents)介绍

  一个GameObject可以包含很多个组件

  下图所示是一个GameObject的**Transform Component**，你可以点击一个GameObject然后在**Inspector Window**里面查看到这个组件

  ![](https://docs.unity3d.com/2017.4/Documentation/uploads/Main/EmptyGO.png)

  在Unity里，所有的GameObject都包含一个**Transform Component**,这个组件定义了一个GameObject的位置、旋转角度、缩放等，当你修改了组件的数值时，场景中对应的GameObject的相应属性就会发生对应改变。

  一个GameObject还可以包含很多其他的组件，他们都影响了这个GameObject的各方面的属性

  ![](https://docs.unity3d.com/2017.4/Documentation/uploads/Main/GameObject-maincamera.png)

* 组件的使用

  * 为GameObject添加组件

    







