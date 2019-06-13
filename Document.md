##Document

[TOC]

### 1.打包发布到android平台

####1.1构建安卓工程

##### 1.1.1Cocos2d-x 3.x

项目的根目录下已有构建好的各平台的工程，其中**proj.android适用于Eclipse + ADT,proj.android-studio适用于Android Studio**

![pic](D:\Workspace\document\pics\s1.png)

#####1.1.2Cocos Creator

Cocos Creator的构建可以参考[官方文档](<https://docs.cocos.com/creator/manual/zh/publish/publish-native.html>)

* 首先，在设置中**配置NDK和android SDK**路径

* 然后选择**项目-构建发布**设置好参数后点击**构建**生成android工程

  生成的工程在build/jsb-link(jsb-default)/frameworks/runtime-src目录下

  ![pic](D:\Workspace\document\pics\s2.png)

#### 1.2使用Android Studio打包

##### 1.2.1Android Studio与Eclipse + ADT

有两种方案可以用来进行Android开发，**Android Studio**或是**Eclipse+ADT**。

- Android Studio

  由Google发布专门用于开发Android应用的IDE，基于IntelliJ IDEA

- Eclipse

  主要用于Java开发的IDE

- ADT

  Android Development Tools，为了在Eclipse中构建Android应用程序而安装的一个插件

目前的主流选择是**Android Studio**，原因可以参考[Android Studio 比 Eclipse 好用在哪里？](<https://www.zhihu.com/question/21534929/answers/updated>)

>从 v1.10 开始，我们不再支持 Eclipse 的 ANT 构建，需要使用 Android Studio 作为安卓平台的构建工具，并在 Android Studio 里下载所需的 SDK 和 NDK 包。——Cocos Creator官方文档

所以，我们只需要了解Android Studio的相关知识即可

#####1.2.2相关知识介绍

* Java

  在Android平台，通常使用Java来进行开发，想要发布游戏到Android平台，必须配置相关环境并对Java的相关概念有基础了解。

  * JRE

    > JRE是Java Runtime Environment缩写，指Java运行环境，是[Sun](https://baike.baidu.com/item/Sun)的产品。运行JAVA程序所必须的环境的集合，包含JVM标准实现及Java核心类库。

    JRE = JVM + Java核心类库

  * JDK

    > JDK是 [Java](https://baike.baidu.com/item/Java/85979) 语言的[软件开发工具包](https://baike.baidu.com/item/软件开发工具包/10418833)，主要用于移动设备、嵌入式设备上的java应用程序。JDK是整个java开发的核心，它包含了JAVA的运行环境（JVM+Java系统类库）和JAVA工具。

    JDK = JRE + JAVA工具

    如果只需要运行Java程序，安装JRE即可，而我们需要进行Java开发， 所以应该安装JDK。

    JDK的配置可以参考[Cocos Creator文档]([https://docs.cocos.com/creator/manual/zh/publish/setup-native-development.html#%E4%B8%8B%E8%BD%BD-java-sdk-%EF%BC%88jdk%EF%BC%89](https://docs.cocos.com/creator/manual/zh/publish/setup-native-development.html#下载-java-sdk-（jdk）))

  * JNI

    > JNI，全称为Java Native Interface，即Java本地接口，JNI是Java调用Native 语言的一种特性。通过JNI可以使得Java与C/C++机型交互。即可以在Java代码中调用C/C++等语言的代码或者在C/C++代码中调用Java代码。

    1. JNI是 Java调用 Native语言的一种特性
    2. JNI 是属于 Java 的，与 Android无直接关系

    * 使用步骤

      * 使用JDK中的工具javac(Java语言编译器)生成.class(运行于JVM的字节码)和.h(C/C++头文件)
      * 用C/C++实现方法后生成动态链接库

      具体过程参考：[示例](<https://baike.baidu.com/item/JNI#6>)

  * JAR

    >在[软件](https://zh.wikipedia.org/wiki/软件)领域，**JAR文件**（Java归档，英语：**J**ava **AR**chive）是一种[软件包](https://zh.wikipedia.org/wiki/软件包格式)[文件格式](https://zh.wikipedia.org/wiki/文件格式)，通常用于聚合大量的[Java类文件](https://zh.wikipedia.org/w/index.php?title=Java类文件&action=edit&redlink=1)、相关的[元数据](https://zh.wikipedia.org/wiki/元数据)和资源（文本、图片等）文件到一个文件，以便分发Java平台[应用软件](https://zh.wikipedia.org/wiki/应用软件)或[库](https://zh.wikipedia.org/wiki/函式庫)。
    >
    >JAR文件是一种[归档文件](https://zh.wikipedia.org/w/index.php?title=归档文件&action=edit&redlink=1)，以[ZIP格式](https://zh.wikipedia.org/wiki/ZIP格式)构建，以`.jar`为[文件扩展名](https://zh.wikipedia.org/wiki/文件扩展名)。用户可以使用[JDK](https://zh.wikipedia.org/wiki/JDK)自带的jar命令创建或提取JAR文件。也可以使用其他`zip`压缩工具，不过压缩时zip文件头里的条目顺序很重要，因为[Manifest文件](https://zh.wikipedia.org/wiki/Manifest文件)常需放在首位。JAR文件内的文件名是Unicode文本。

    Java编译好之后生成class文件，但如果直接发布这些class文件的话会很不方便，所以就把许多的class文件打包成一个jar，jar中除了class文件还可以包括一些资源和配置文件，通常一个jar包就是一个java程序或者一个java库。

    [简介](<https://blog.csdn.net/qq_22073849/article/details/77148847>)

* Android

  * Android SDK

    > **Android SDK(AndroidSoftware Development Kit)**，即Android软件开发工具包
    >
    > Android SDK提供了**开发测试所必须的Android API类库，开发工具**

    [SDK文档](<http://www.android-doc.com/sdk/>)

    - SDK版本参数设置

      在app模块下的build.gradle中需要设置`compileSdkVersion`、`minSdkVersion`、`targetSdkVersion`等参数，他们的含义可以参考<https://www.kaelli.com/26.html>

      `compileSdkVersion`即**编译时使用的SDK版本**

      `minSdkVersion`即**最低支持的SDK版本**（在该版本以下的系统上无法安装，android系统与SDK版本的对应关系可以参考SDK Manager）

      `targetSdkVersion`即**运行时使用的SDK版本**

      

      编译时经常会因为SDK版本问题报错，我们需要**根据实际安装的SDK版本来进行设置**。`如：compileSdkVersion设置了未安装的SDK版本时会报错：Failed to find target with hash string 'android-26' in: D:\android`

  * NDK

    > Android NDK 是一套允许您使用 C 和 C++ 等语言，以原生代码实现部分应用的工具集。在开发某些类型的应用时，这有助于您重复使用以这些语言编写的代码库。

    [NDK文档](<https://developer.android.google.cn/ndk/index.html>)

    - 可通过NDK在 Android中 使用 JNI**与本地代码（如C、C++）交互**

      使用NDK，可以将C++代码打包成.so文件供java调用，[示例](<https://blog.csdn.net/leilifengxingmw/article/details/71598687>)

    - .so文件

      Linux系统下,.so文件即**动态链接库**，.a文件即静态链接库，分别对应于Windows系统中的.dll和.lib

      参考资料：<https://www.cnblogs.com/scotth/p/3977928.html>

    - ABI

      >在软件开发中，**应用程序机器二元码接口**（英语：application binary interface，缩写为**ABI**）是指两程序模块间的接口；通常其中一个程序模块会是库或操作系统所提供的服务，而另一边的模块则是用户所运行的程序。
      >
      >一个ABI定义了机器代码如何访问数据结构与运算程序，此处所定义的界面相当低级并且相依于硬件。而类似概念的[*API*](https://zh.wikipedia.org/wiki/Application_programming_interface)则在源代码定义这些，则较为高端，并不直接相依于硬件，通常会是人类可阅读的代码。一个ABI常见的样貌即是[调用约定](https://zh.wikipedia.org/wiki/调用约定)：数据怎么成为计算程序的输入或者从中得到输出；[x86的调用约定](https://zh.wikipedia.org/wiki/X86调用约定)即是一个ABI的例子。

      不同的Android手机使用不同的CPU,因此需要提供对应的二进制接口交互规则(即对应的ABI文件)才能进行交互。

      C++代码必须根据Android 设备的CPU类型(通常称为”ABIs”)进行编译,常用的五种 ABI: 

      1.armeabiv-v7a: 第7代及以上的 ARM 处理器。2011年以后的生产的大部分Android设备都使用它。 
      2.arm64-v8a: 第8代、64位ARM处理器，很少设备，三星 Galaxy S6是其中之一。 
      3.armeabi: 第5代、第6代的ARM处理器，早期的手机用的比较多。 
      4.x86: 平板、模拟器用得比较多。 
      5.x86_64: 64位的平板。 

      参考：[Android ABI的浅析](<https://www.jianshu.com/p/d2119b3880d8>)

  * Gradle

    > **Gradle**是一个基于[Apache Ant](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/wiki/Apache_Ant)和[Apache Maven](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/wiki/Apache_Maven)概念的项目[自动化建构](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/wiki/%E8%87%AA%E5%8B%95%E5%8C%96%E5%BB%BA%E6%A7%8B)工具。它使用一种基于[Groovy](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/wiki/Groovy)的[特定领域语言](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/w/index.php%3Ftitle%3D%E7%89%B9%E5%AE%9A%E9%A2%86%E5%9F%9F%E8%AF%AD%E8%A8%80%26action%3Dedit%26redlink%3D1)来声明项目设置，而不是传统的[XML](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/wiki/XML)。当前其支持的语言限于[Java](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/wiki/Java)、[Groovy](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/wiki/Groovy)和[Scala](https://link.zhihu.com/?target=http%3A//zh.wikipedia.org/wiki/Scala)，计划未来将支持更多的语言。

    [如何通俗地理解 Gradle？](<https://www.zhihu.com/question/30432152>)

    [求比较好的gradle的入门指引？](<https://www.zhihu.com/question/21234585>)

    [Gradle的基本使用](<https://waynell.github.io/2015/04/03/gradle-use-01/>)

    - Gradle、Ant、Maven

      - Ant

        > **Apache Ant**，是一个将[软件](https://zh.wikipedia.org/wiki/软件)[编译](https://zh.wikipedia.org/wiki/编译)、[测试](https://zh.wikipedia.org/wiki/单元测试)、[部署](https://zh.wikipedia.org/wiki/软件部署)等步骤联系在一起加以自动化的一个[工具](https://zh.wikipedia.org/wiki/软件开发工具)，大多用于[Java](https://zh.wikipedia.org/wiki/Java)环境中的[软件开发](https://zh.wikipedia.org/wiki/软件开发)。

      - Maven

        > **Apache Maven**，是一个[软件](https://zh.wikipedia.org/wiki/软件)（特别是[Java](https://zh.wikipedia.org/wiki/Java_(编程语言))软件）[项目管理](https://zh.wikipedia.org/wiki/项目管理)及[自动构建](https://zh.wikipedia.org/wiki/自动构建)工具

      - Gradle的优势

        - 自动处理包相依关系 - 取自 Maven Repos 的概念
        - 自动处理布署问题 - 取自 Ant 的概念
        - 条件判断写法直觉 - 使用 Groovy 语言

      > 过去 Java 开发者常用 Maven 和 Ant 等工具进行封装布署的自动化，或是两者兼用，不过这两个包彼此有优缺点，如果频繁改变相依包版本，使用 Ant 相当麻烦，如果琐碎工作很多，Maven 功能不足，而且两者都使用 XML 描述，相当不利于设计 if、switch 等判断式，即使写了可读性也不佳，而 Gradle 改良了过去 Maven、Ant 带给开发者的问题，至今也成为 Android Studio 内置的封装布署工具。

    - Project&Task

      - 项目是指我们的构建产物（比如Jar包）或实施产物（将应用程序部署到生产环境）。
      - 任务是指不可分的最小工作单元，执行构建工作（比如编译项目或执行测试）。
      - 每一个构建都是由一个或多个 projects 构成的，每一个 project 是由一个或多个 tasks 构成的，一个 task是指不可分的最小工作单元，执行构建工作（比如编译项目或执行测试）

      每一个build.gradle都对应一个project，其中最外层的build.gradle对应根目录，即全局的构建配置，而app目录下的build.gradle则对应app这个module，是针对app这个project的配置。在`settings.gradle`中可以设置需要引入的module，通过命令`gradlew projects`可以显示所有的proejct，可以认为整个android工程对应根project，而module则对应子project。

    - Groovy

      [Groovy语法](http://groovy-lang.org/single-page-documentation.html)

    - Gradle DSL

      [文档](<https://docs.gradle.org/4.4/dsl/>)

    - Android DSL

      [文档](<https://google.github.io/android-gradle-dsl/3.1/>)

  * ADB

    > Android 调试桥 (adb) 是一个通用命令行工具，其允许您与模拟器实例或连接的 Android 设备进行通信。它可为各种设备操作提供便利，如安装和调试应用，并提供对 Unix shell（可用来在模拟器或连接的设备上运行各种命令）的访问

    [五一写的简介和使用示例](<https://www.teambition.com/project/5afd0cc9f843620018db2b10/posts/post/5c94d7354e87e2001842b090>)

    [官方文档](<https://developer.android.com/studio/command-line/adb?hl=zh-cn>)

#####1.2.3android工程结构分析

使用Android Studio新建一个空项目，文件结构如下：

![pic](D:\Workspace\document\pics\s3.png)

* .gradle					Android Studio自动生成的文件

  .idea					    Android Studio自动生成的文件

  app						 app模块

  gradle					 gradle wrapper配置文件

  .gitignore			   使用git时将指定文件排除在外

  build.gradle           全局的gradle构建脚本

  gradle.properties 全局的gradle配置文件

  gradlew                  在Linux或Mac系统中执行gradle命令

  gradlew.bat		   在Windows系统中执行gradle命令

  local.properties    指定Android SDK 和 NDK路径

  settings.gradle     指定项目中所有引入的模块（上面的app即一个模块）

  test.iml				  iml文件时所有IntelliJ IDEA项目都会自动生成的一个文件

  * app

    app模块，代码和资源都放在这里，在下面会详细分析

  * gradle gradlew gradlew.bat

  * build.gradle

    ```
    buildscript {
        
        repositories {
            google()
            jcenter()
        }
        dependencies {
            classpath 'com.android.tools.build:gradle:3.2.0'
            
    
            // NOTE: Do not place your application dependencies here; they belong
            // in the individual module build.gradle files
        }
    }
    
    allprojects {
        repositories {
            google()
            jcenter()
        }
    }
    
    task clean(type: Delete) {
        delete rootProject.buildDir
    }
    ```

    * buildscript(Gradle DSL)

      >Configures the build script classpath for this project.
      >
      >The given closure is executed against this project's [`ScriptHandler`](https://docs.gradle.org/4.4/javadoc/org/gradle/api/initialization/dsl/ScriptHandler.html). The [`ScriptHandler`](https://docs.gradle.org/4.4/javadoc/org/gradle/api/initialization/dsl/ScriptHandler.html) is passed to the closure as the closure's delegate.

      ```
      buildscript {
          
          repositories {
              google()
              jcenter()
          }
          dependencies {
              classpath 'com.android.tools.build:gradle:3.2.0'
              
      
              // NOTE: Do not place your application dependencies here; they belong
              // in the individual module build.gradle files
          }
      }
      ```

      buildscript中的声明是gradle脚本自身需要使用的资源

      * repositories

        代码仓库

        通过代码仓库，Gradle会为我们自动下载项目中所依赖的包

      * dependencies

        依赖

        参考 [谈一谈 Gradle 的仓库(Repositories)](<https://blog.csdn.net/csdnlijingran/article/details/80612363>)

    * allprojects(Gradle DSL)

      ```
      allprojects {
          repositories {
              google()
              jcenter()
          }
      }
      ```

      对所有project的统一配置

    * task clean

  * gradle.properties

    配置一些全局变量

  * local.properties

    ```
    ndk.dir=D\:\\android\\ndk-bundle
    sdk.dir=D\:\\android
    ```

    设置SDK和NDK的路径

  * settings.gradle

    ```
    include ':app'
    ```

    引入了app模块

* app文件夹

  主要的工作都在app文件夹下完成

  ![pic](D:\Workspace\document\pics\s4.png)

  build							编译时自动生成的文件

  libs								存放第三方jar包

  src								代码和资源，代码放在main/java下 资源放在main/res下

  .gitignore					使用git时将指定文件排除在外

  app.iml   					 iml文件时所有IntelliJ IDEA项目都会自动生成的一个文件

  build.gradle				app模块的gradle构建脚本

  proguard-rules.pro   用于指定项目代码的混淆规则

  * src

    AndroidManifest.xml

    > 每个应用的根目录中都必须包含一个 `AndroidManifest.xml` 文件（且文件名精确无误）。 清单文件向 Android 系统提供应用的必要信息，系统必须具有这些信息方可运行应用的任何代码。

    [官方文档](<https://developer.android.com/guide/topics/manifest/manifest-intro?hl=zh-cn>)

  * build.gradle

    ```
    apply plugin: 'com.android.application'
    
    android {
        compileSdkVersion 28
        defaultConfig {
            applicationId "com.example.wangtl.test"
            minSdkVersion 15
            targetSdkVersion 28
            versionCode 1
            versionName "1.0"
            testInstrumentationRunner "android.support.test.runner.AndroidJUnitRunner"
        }
        buildTypes {
            release {
                minifyEnabled false
                proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
            }
        }
    }
    
    dependencies {
        implementation fileTree(dir: 'libs', include: ['*.jar'])
        implementation 'com.android.support:appcompat-v7:28.0.0'
        implementation 'com.android.support.constraint:constraint-layout:1.1.3'
        testImplementation 'junit:junit:4.12'
        androidTestImplementation 'com.android.support.test:runner:1.0.2'
        androidTestImplementation 'com.android.support.test.espresso:espresso-core:3.0.2'
    }
    ```

    只针对app模块的配置

    * apply plugin

      使用Gradle android插件

    * android

      * defaultConfig(Android DSL)

        Specifies defaults for variant properties that the Android plugin applies to all build variants.

      * buildTypes(Android DSL)

        Encapsulates all build type configurations for this project.

    * dependencies

      app模块的依赖

##### 1.2.4打包

最终我们需要生成APK文件以在Android系统中运行。

* APK

  > **Android应用程序包**（英语：Android application package，**APK**）是[Android](https://zh.wikipedia.org/wiki/Android)[操作系统](https://zh.wikipedia.org/wiki/操作系统)使用的一种[应用程序](https://zh.wikipedia.org/wiki/应用程序)包文件格式，用于分发和安装[移动应用](https://zh.wikipedia.org/wiki/移动应用)及[中间件](https://zh.wikipedia.org/wiki/中间件)。一个Android应用程序的代码想要在Android设备上运行，必须先进行[编译](https://zh.wikipedia.org/wiki/编译)，然后被打包成为一个被Android系统所能识别的文件才可以被运行，而这种能被Android系统识别并运行的[文件格式](https://zh.wikipedia.org/wiki/文件格式)便是“APK”。 一个APK文件内包含被编译的代码文件(.dex 文件)，文件资源（resources）， assets，证书（certificates），和清单文件（manifest file）。

  APK 文件基于 [ZIP](https://zh.wikipedia.org/wiki/ZIP格式) 文件格式，它与[JAR](https://zh.wikipedia.org/wiki/JAR)文件的构造方式相似，因此APK和JAR都可以通过将后缀名改为.zip来用压缩文件打开。

  ![pic](D:\Workspace\document\pics\s6.png)

  - META-INF文件夹:

    - MANIFEST.MF: [清单文件](https://zh.wikipedia.org/wiki/清单文件)（Manifest file）

    - CERT.RSA: 保存着该应用程序的证书和授权信息。

    - CERT.SF: 保存着SHA-1信息资源列表，比如：

      ```
      Signature-Version: 1.0
      Created-By: 1.0 (Android)
      SHA1-Digest-Manifest: wxqnEAI0UA5nO5QJ8CGMwjkGGWE=
      ...
      Name: res/layout/exchange_component_back_bottom.xml
      SHA1-Digest: eACjMjESj7Zkf0cBFTZ0nqWrt7w=
      ...
      Name: res/drawable-hdpi/icon.png
      SHA1-Digest: DGEqylP8W0n0iV/ZzBx3MW0WGCA=
      ```

    [Android签名机制之---签名过程详解](<https://blog.csdn.net/jiangwei0910410003/article/details/50402000>)

    [Android APK 签名原理及方法](<https://juejin.im/entry/575ed0bb1532bc00609c3aa9>)

  - res: APK所需要的资源文件夹。

  - AndroidManifest.xml: 一个传统的Android清单文件，用于描述该应用程序的名字、版本号、所需权限、注册的服务、链接的其他应用程序。该文件使用XML文件格式，可以编译为二进制的XML，使用的工具为 [AXMLPrinter2](http://code.google.com/p/android4me/downloads/list) 或[apktool](http://code.google.com/p/android-apktool/).

  - classes.dex: classes文件通过DEX编译后的文件格式，用于在[Dalvik虚拟机](https://zh.wikipedia.org/wiki/Dalvik虚拟机)上运行的主要代码部分。

  - resources.arsc:程序的语言文件，可以通过这软件用(AndroidResEdit等工具)来进行翻译

* 打包流程

###2.SDK接入

#### 2.1Android工程SDK接入

以[微信分享](<https://open.weixin.qq.com/cgi-bin/showdocument?action=dir_list&t=resource/res_list&verify=1&id=1417751808&token=&lang=zh_CN>)为例

* 1.申请你的AppID

  要使你的程序启动后微信终端能响应你的程序，必须在代码中向微信终端注册你的AppID

* 2.下载SDK及API文档

  Android Studio下直接添加依赖即可自动下载

* 3.搭建开发环境

  在Android Studio中新建你的工程，并保证网络设置可以成功从jcenter下载微信SDK即可。

* 4.在代码中使用开发工具包

  根据文档一步步调用微信提供的API即可

  

一般来说第三方SDK都会提供详细的说明文档以及相应的demo

* SDK包可以手动放入libs文件夹下也可以通过添加依赖来自动下载
* 配置好SDK后 根据API文档和demo进行调用即可

####2.2Cocos Creator SDK接入

在Cocos Creator中我们编写的是JavaScript代码，JavaScript和Java交互的方法可以参考[官方文档](<https://docs.cocos.com/creator/manual/zh/advanced-topics/java-reflection.html>)

这样，只要将需要实现的功能用Java封装好，直接在Cocos Creator中使用JavaScript调用即可。

#### 2.3GVoiceSDK接入示例

* Cocos Creator

  * 新建一个Cocos Creator工程，添加start、stop、play3个按钮

    ![pic](D:\Workspace\document\pics\s7.png)

  * 构建android工程

    ![pic](D:\Workspace\document\pics\s8.png)

* GVoice接入

  - 参照[快速入门](https://gcloud.qq.com/document/5bf2a1fb7d34939a07883a7f)部分说明，在[管理控制台](https://console.gcloud.qq.com/)注册业务

    创建游戏后可以获得游戏ID和游戏Key，在SDK接入的时候会用到，这里我们用免费体验专区的ID和Key

  - 下载SDK

    [SDK下载](https://gcloud.qq.com/sdkdownload)

    下载完成后，将GVoice SDK的Jar包和SO文件导入到android工程的libs目录下

    ![pic](D:\Workspace\document\pics\s9.png)

    在APP模块的build.gradle里加入依赖`implementation 'com.qcloud:qcloud-image-sdk:2.3.6'`

    >在Android系统中使用某些功能需要先申请权限，比如这里录音就需要申请录音相关权限，具体哪些权限可以参考demo文件，将申请的权限添加到AndroidManifest.xml中

    ![pic](D:\Workspace\document\pics\s13.png)

  - 调用GVoiceAPI实现功能

    [API文档](<https://gcloud.qq.com/document/5b76964d24ba0b670dae820d>)

    * 初始化

      根据[接入指引](<https://gcloud.qq.com/document/5923b83582fb561c1b3024b5>)中的步骤完成初始化

      ![pic](D:\Workspace\document\pics\s10.png)

    * 录音&播放

      我们使用GVoice提供的录音和播放功能，调用流程参考[接口调用流程](<https://gcloud.qq.com/document/5bf2a849e974d4497c62f6cb#6c617606-8476-4764-9d75-d2b0fdbd8598>)中的离线消息

    * 按钮功能实现

      在Java中封装好开始、停止以及播放录音的接口，并在Cocos Creator的按钮回调中进行调用

      ![pic](D:\Workspace\document\pics\s11.png)

      ![pic](D:\Workspace\document\pics\s12.png)

* 测试

  编译完成后，连接真机，点击Android Studio右上角的Run按钮在真机上安装并测试

### 3.发布

#### 3.1GM后台

[GM后台](<http://gm.n.m.youzu.com/server/>)

* 运营商

  `运维相关>运营商管理`

  增加新渠道时需要添加相应运营商及其ID

* 运营平台

  `运维相关>运营平台管理`

  3个正式服平台 

  * AppStore ID2021
  * 安卓越狱混 ID2033
  * 腾讯平台 ID2072

  增加了新的运营商后，编辑对应的运营平台信息，在运营商列表中加入新运营商的ID

* 前端配置

  `运营相关>前端配置`

  配置新的运营商信息，复制同平台下其他渠道的内容即可

#### 3.2Jenkins

[Jenkins](<http://192.168.120.234:8080/>)

####3.3少三打整包流程

在Jenkins选择create_apk_4.0任务可以打整包

##### 3.3.1母包生成

根据选择的内更新版本，拉取最新的游戏代码，并生成母包工程。

```
svn up ${jenkins_publish_dir}/publish/projects
svn up ${jenkins_publish_dir}/publish/cfg

if [ "${jenkins_rebuild_origin}" == "true" ]
then
    perl ${jenkins_publish_dir}/publish/apk_supersdk.pl ${jenkins_publish_dir}/publish/cfg/apk.framework4.cfg
fi
```

##### 3.3.2渠道包参数配置

不同渠道的部分参数也不同，可以在SuperSDK工具中进行配置，如ICON等。

![pic](D:\Workspace\document\pics\s14.png)

#####3.3.3生成渠道包

```
for i in ${arr[@]}  
do  
   perl ${jenkins_publish_dir}/publish/supersdk.pl ${jenkins_publish_dir}/publish/cfg/$i
done  
```

[**SuperSDK一键打包分享**]([http://gitlab.wuxian.youzu.com/tech/docs/blob/master/misc/share/%E8%90%BD%E5%B0%8F%E9%B1%BC-SuperSDK%E4%B8%80%E9%94%AE%E6%89%93%E5%8C%85%E5%88%86%E4%BA%AB.ppt](http://gitlab.wuxian.youzu.com/tech/docs/blob/master/misc/share/落小鱼-SuperSDK一键打包分享.ppt))

### 4.其他

#### 4.1刘海屏适配

