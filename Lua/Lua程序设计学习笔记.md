### Lua程序设计

以《Lua程序设计》为基础的Lua学习

---

书中对Lua的特性描述：

* 可扩展性
* 简易性
* 高效
* 可移植性



《Lua程序设计》结构：

* 介绍了语言本身，展示了语言所具有的所有能力。
* 深入介绍table 数据结构 持久化 包和面向对象编程
* Lua标准库
* Lua与C之间的API

---

#### 目录

[TOC]

---

遗留问题：

双精度浮点数

协同程序



####第一部分



#####开始

* 安装Lua

  * [下载Lua For Windows](https://code.google.com/archive/p/luaforwindows/downloads)
  * 下了安装不了 404
  * [GitHub下载](https://github.com/rjpcomputing/luaforwindows)
  * 勉强搞定
* 程序块chunk
* 词法规范

  * 不能以数字开头

  * _XXXX 特殊用途 _ 哑巴变量

  * 标识符 注意 repeat和until

* 全局变量
  * 删除全局变量 将其赋值为nil
  * Lua中直接赋值的变量均为全局变量
* 解释器程序

#####类型与值

8种基础类型 `nil` ` bool` ` number` ` string` `userdata` `function` `thread` ``table`

函数作为**第一类值**来看待

* nil

* boolean

  * false 和 nil 为**假** 其余为**真** 0和空字符串也为真

* number

  * Lua没有整数类型 采用**双精度浮点数**
  * Lua的numbers可以处理任何**长整数**不用担心误差

* string

  * Lua采用8位编码
  * 字符串是不可改变的值 只有通过新建一个字符串来修改
  * 匹配的双方括号来界定一个字母字符串`[[ ... ]]`
  * Lua提供了运行时 数字与字符串 的自动转换
  * 字符串前加`#`来获取长度
  * 使用 .. 作为字符串连接符

* table

  * table永远是匿名的，一个持有table的变量和table之间没有固定的关联性
  * 全局变量存储在table中
  * Lua以1作为索引的起始值
  * `#`操作符计算长度时从1开始 遇到nil时结束

* function

  * 第一类值

* userdata(自定义类型)

  * 只能进行赋值和相等性测试

* thread

  

##### 表达式

Lua表达式中可以包含函数定义和table构造式

* 算数操作符

  * `^` 指数操作符 `%`取模操作符
  * `a%b` == `a - floor(a/b)*b`
  * 整数取模 结果符号与第二个参数相同
  * `x%1`就是x的小数部分

* 关系操作符

  * 对于table userdata function lua是做引用比较的
  * 比较数字按照传统的数字大小进行，比较字符串按照字符顺序进行

* 逻辑操作符

  * `and`和 `or`都使用**短路求值**

* 字符串连接

* 优先级

* table构造式

  * 链表实现：

    ```lua
    list = nil
    for i, v in ipairs(data) do
    
    	list = {next = list, value = v}
    
    end
    ```


#####语句

Lua还支持一些不太常见的语句，例如 **多重赋值**和**局部变量声明**

* 赋值

  * 多重赋值时 先对右边的所有元素求值，然后才执行赋值

* 局部变量与块

  * do end 显示界定**块**

  * ```lua
    local a = a
    ```

    创建一个局部变量a 并用全局变量a的值来初始化它。可以加速在当前作用域中对a的访问

* 控制结构

  * repeat-until语句重复执行循环体直到条件为真时结束

    ```lua
    repeat
    	line = io.read()
    until line ~= ""
    ```

    在Lua中 声明在循环体中的局部变量的作用域包括了条件测试

  * 泛型for循环  中途改变table的值

    ![](1.png)

* break 与 return

  * break 和 return 只能是一个块的最后一条语句

##### 函数

一个函数若只有一个参数 并且此参数是一个字面字符串或table构造式 那么圆括号便可有可无

* 多重返回值

  * 只有当一个函数调用是一系列表达式中的最后一个元素时，才能获得它的所有返回值
  * unpack() 泛型调用 从下标1开始返回数组的所有元素

* 变长参数

  * select()
  * 局部变量`arg`来接受所有的变长参数 `arg.n`来记录变长参数的总数

* 具名实参

  

#####深入函数

**第一类值**表示Lua中函数和其他传统类型的值具有相同的权利

**词法域**表示一个函数可以嵌套在另一个函数中，内部的函数可以访问外部函数的变量

函数与所有其他值一样都是匿名的

接受另一个函数作为实参的 被称为**高阶函数**

* closure闭包

  * 一个closure就是一个函数加上该函数所需访问的所有**非局部的变量**

  * > 从技术上讲，Lua中只有closure，而不存在“函数”。因为，函数本身就是一种特殊的closure

* 非全局的函数

  * Lua提供了一种语法糖来定义局部函数

    ```lua
    local function foo()
    ...
    end
    
    local foo
    foo = function()
    ...
    end
    ```

    在递归调用时，函数需要定义完成才可调用，如下的代码是错误的：

    ```lua
    local fact = function(n)
    	if n == 0 then
    		return 1 
    	else
    		return fact(n - 1)
    	end
    end
    ```

    函数体中的fact()并非此函数本身，使用语法糖形式来定义可以解决此问题。

  * 对于间接递归调用，必须使用明确的前向声明

    ```lua
    local f, g -- 前向声明
    function g()
    	f()
    end
    
    function f()
    	g()
    end
    ```

* 正确的尾调用

   * 当一个函数调用是另一个函数的最后一个动作时，该调用是一条尾调用

  * 在尾调用之后，程序不需要保存任何关于该函数的栈信息。这种实现即“尾调用消除”

  * 判断的准则是 调用完之后，没有任何其他事可做。

    以下几种情况不是**尾调用**

    ```lua
    -- 还需要丢弃g(X)返回的结果
    function f(x)
    	g(x)
    end
    
    -- 必须做一次加法
    function f(x)
    	return g(x) + 1
    end
    
    ```

  * 一条尾调用就好像是一条`goto`语句

  * 尾调用的一大应用是编写状态机

##### 迭代器与泛型for

* 迭代器与closure
  * 每调用一次函数，就返回集合中的下一个元素。迭代器需要在每次调用之间保持一些状态。
  * 一个closure结构涉及两个函数 closure本身和一个用于创建closure的工厂函数
* 泛型for的语义
  * 泛型for在循环过程内部保存了迭代器函数。实际上它保存了三个值：一个迭代器函数，一个恒定状态和一个控制变量。
* 无状态的迭代器
  * 自身不保存任何状态的迭代器，可以再多个循环中使用同一个无状态的迭代器。
* 具有复杂状态的迭代器
  * 在循环过程中，恒定状态总是同一个table，但table的内容可以改变。
  * 基于closure的迭代器比使用一个table的迭代器更高效：
    * 创建一个closure比创建一个table更廉价
    * 访问非局部的变量也比访问table字段快
  * 使用协程编写迭代器是功能最强的。
* 真正的迭代器
  * 生成器风格的迭代器更加灵活

##### 编译 执行 与错误

* 编译
  * loadfile与dofile
  * loadstring 在编译时不涉及词法域，总是在全局环境中编译它的字符串
  * 在Lua中，函数定义是一种赋值操作，也就是说，它们是在运行时才完成的操作
* C代码
  * Lua为几种平台实现了一套动态链接机制
  * package.loadlib
  * require()会搜索指定的库，然后使用package.loadlib来加载库，并返回初始化函数
* 错误
  * error()
  * assert()
* 错误处理与异常
  * pcall()
  * 使用error来抛出异常 使用pcall来捕获异常
* 错误消息与追溯(traceback)
  * error() 第二个参数用来标识错误发生在哪个层级
  * xpcall()
  * debug.debug() debug.trackback()

#####协同程序（coroutine)(*)

协同程序与线程差不多，也就是一条执行序列，拥有自己独立的栈、局部变量和指令指针，同时又与其他协同程序共享全局变量和其他大部分东西。

线程与协程的区别：在任意时刻只能同时运行一个协同程序，正在执行的协程只有在显示要求挂起时，执行才会暂停。

* 协同程序基础

  * Lua将所有关于协同程序的函数都放在一个名为"coroutine"的table中

  * create(function) 创建一个协程 function即该协程需要执行的内容 返回一个thread类型的值

  * 4种状态 挂起suspended 运行running 死亡dead 正常normal

  * status(thread) 检查状态

  * resume(thread) 将状态由挂起改为运行

  * yield() 挂起

  * 对已死亡的协程 调用resume将返回false及一条错误消息

  * 保护模式？（回看下第八章）

  * A唤醒B时 A处于正常状态(无法运行也不在运行)

  * resume-yield交换数据

    ```lua
    co = coroutine.create(function(a, b, c)
    	print("co", a, b, c)
    end)
    coroutine.resume(co, 1, 2, 3) --> co 1 2 3
    
    co = coroutine.create(function(a, b)
    	coroutine.yield(a + b, a - b)
    end)
    print(coroutine.resume(co, 20, 10)) --> true 30 10
    -- resume返回的内容中 第一个值为true表示没有错误 额外值为yield()的参数
    -- 协程结束时，主函数返回的值将作为resume的返回值
    ```

    

  * resume() yield() **非对称的协同程序**

  * Lua提供了**完整的** **非对称**的协同程序

* 管道pipe和过滤器filter

  * 使用协程解决**生产者消费者问题**  **消费者驱动**
  * 实现**过滤器** 既是生产者又是消费者

---

**TODO**:

- 以协同程序实现迭代器
- 非抢先式的多线程

---

##### 完整的示例

* 数据描述

* 马尔科夫链

  根据原始文本中n个单词的序列来确定后面的单词

---



####第二部分



#####数据结构

使用table来实现一些传统的数据结构

* 数组

  * 使用整数索引table即可实现数组

* 矩阵与多维数组

  * 矩阵 两种实现方式
     * 数组的数组 `a[m][n]`
     * 合并索引`a[(i - 1)*M + j]`
  * 必须显示地创建每一行
  * 更加灵活
    * 实现稀疏矩阵
    * 稀疏矩阵不能使用`#`操作符 使用`pairs()`来遍历 是无序的

* 链表

  * 实现

    ```lua
    local list = nil
    for i = 1, 10 do
    	list = {next = list, value = i}
    end
    ```

  * Lua中很少使用链表

* 队列与双向队列

  * `table.insert()` `table.remove()` 对于大结构 开销大
  * 使用两个索引值来标记首尾元素

* 集合与无序组

  * 包 也称为 多重集合 每个元素可以出现多次

    ```lua
    bag[element] = num -- 计数器
    ```

* 字符串缓冲

  * table.concat()

  * 内部使用了算法来创建较大的字符串

    使用栈来存放字符串，当入栈的字符串比栈顶字符串长时 就进行连接操作 并继续向下比较 遇到更长的字符串或到达栈底 否则直接进行入栈操作

* 图

  * 以结点为对象（table) 用name字段表示结点的名字 adj字段表示与该结点邻接的结点的集合

##### 数据文件与持久性

使用Lua来避免程序中所有有关数据读取的代码

* 数据文件
  * CSV XML
* 串行化
  * 将数据转换为字节流或字符流

##### 元表(metatable)与元方法(metamethod)

通过元表来修改一个值的行为，使其在面对一个非预定义的操作时执行一个指定的操作。

table和userdata可以有各自独立的表 其他类型的值共享类型所属的单一元表

创建table时不会创建元表

`getmetatable(table)` `setmetatable(table, t1)`

在Lua代码中 只能设置table的元表 其他类型需要通过C代码来完成 

默认没有元表 字符串程序库给字符串类型设置了一个元表

- 算数类的元方法

  __add 加法

  __mul

  __sub

  __div

  __unm

  __mod

  __pow

  __concat

  t1.__add = func

  如果符号两边具有不同元表 先查找第一个值得元表 再查找第二个值

- 关系类的元方法

  __eq 等于

  __lt  小于

  __le 小于等于

  - **部分有序**
  - 关系类元方法不能应用于混合的类型(不同类型/拥有不同元方法) 可以是不同元表

- 库定义的元方法

  __tostring

  __metatable 保护元表 不能看和修改元表

- table访问的元方法

  算数类和关系类是为原本错误情况定义行为 也可以修改table的行为

  查询table 和 修改table中不存在的字段

  - __index

    访问不存在的字段时 会查找__index元方法

    __index元方法不必一定是函数 也可以是table

    使用rawget 在访问table时 不涉及元方法

  - __newindex

    __newindex用于更新

    __index用于查询

    赋值时查找__newindex 如果元方法是table 就对该table赋值 而不是原来的table

    rawset()

  - 具有默认值的table

    - 弱引用table  备忘录元表 见17章

  - 跟踪table的访问

  - 只读的table

- 1

##### 环境

Lua中所有全局变量存在一个常规的table中，即"环境" _G

- 具有动态名字的全局变量

- 全局变量声明

- 非全局的环境

  `setfenv`改变一个函数的环境

  函数继承创建它的函数的环境，若父函数环境改变，子函数的环境也会改变

##### 模块与包

`require`使用慕课 `module`创建模块

- require函数

  对于require而言 模块就是一段定义了一些值得代码

  返回一个table  包含模块内的函数和table的全局变量(返回的值由模块决定)

  package.loaded 保存已加载过的模块

  findloader寻找加载器 `loadfile`加载lua文件 `loadlib`加载C程序库

  **package.preload**

  LUA_PATH package.path保存搜索Lua文件的路径 用分号隔开不同路径

  package.cpath搜索C程序库的路径

  luaopen_模块名 以a-b来命名模块 避免名称冲突

- 编写模块的基本方法

  最简单的方法 创建一个table 将需要的函数放入其中 最后返回这个table

- 使用环境

- module函数

  `module()`会创建一个新的table 并将其赋予适当的全局变量和loaded table 并将该table设为主程序块的环境

- 子模块与包

  Lua支持具有层级性的模块吗 `mod.sub`

  包是一个完整的模块树 Lua中发行的单位

##### 面向对象编程

拥有标识 状态 和状态上的操作 缺少类 继承和私密性

- 类

  类 创建对象的模具

- 继承

- 多重继承

- 私密性

- 单一方法的做法

##### 弱引用table

Lua采用自动内存管理 通过使用垃圾收集机制 会自动删除那些已经成为垃圾的对象

如果一个对象的所有引用都是弱引用 那么Lua就可以回收这个对象了 并且还可以以某种形式来删除这些弱引用本身

弱引用table即具有弱引用条目(key 或 value)的table

__mode元方法来设置弱引用 包含"k"或包含"v"

key是数字 布尔值 字符串时 不会被回收



- 备忘录函数
- 对象属性
- 回顾table的默认值



####第三部分

#####数学库



##### table库

* 插入和删除

  `table.insert()` 没有指定位置 则默认添加到末尾

  `table.remove()` 

* 排序

  `table.sort()`

* 连接

  `table.concat()` 

##### 字符串库

在Lua5.1中 还将sting库中的函数作为string类型的方法(通过元表)

`string.upper(s)` or `s:upper()`

* 基础字符串函数

  `string.len(s)`返回长度

  `string.rep(s, n)`返回s重复n次

  `string.upper()`

  `string.lower()`

  `string.sub(s, i, j)`提取第i到第j个字符 第一个字符索引为1 -1表示最后一个

  `string.char()` `string.byte()` 转换字符及其内部表示

  `string.format()`格式化字符串 % + d十进制 x十六进制数 o八进制 f浮点数 s字符串(%.4f表示小数点后有4个数字 %02d 2个十进制数 不足用0补 %2d用空格补)

* 模式匹配函数

  find match gsub gmatch

  * string.find(s, "xxx", j) 在字符串s的第j位开始查找子串"xxx"

  * match

  * gsub 参数 目标字符串 模式 替换字符串 第四个可选参数 替换的次数

    返回 匹配后的字符串 实际替换的次数

    (select(index, func) 返回函数的第index个返回值)

  * gmatch

* 模式

  %a 字母

  %c 控制字符

  %d 数字

  %l 小写字母

  %p 标点符号

  %s 空白字符

  %u 大写字母

  %w 字母和数字字符

  %x 十六进制数字

  %z 内部表示为0的字符

  % + 大写 表示补集 即非XXX

  

  魔法字符

  ( ) . % + - * ? [ ] ^ $ 

  转义 % + XX

  *  只有模式函数才会把%当做转义字符

  * [ ]创建字符分类

  * 横线表示范围[0-9]

  * 在字符集前面加^ 取补集

  * `+ - * ? `修饰符 表示模式中的重复部分和可选部分

    `+`重复一次或多次

    ` - *`重复0次或多次

    最短子串和最长子串

    `?`可选0次或1次

  * 模式以^开头 只匹配目标字符串的开头部分

    以$结尾 只匹配目标字符串的结尾部分

  * %b用于匹配成对的字符

* 捕获

  根据一个模式从目标字符串中抽出匹配于该模式的内容，在指定捕获时，应将模式中需要捕获的部分写到一对圆括号内。

  在捕获内使用%d表示第d个被捕获的值

  使用gsub 也可以使用%d 其中 %0表示整个匹配

* 替换

  gsub的第三个参数可以是函数或table

  以捕获到的内容为参数调用函数 返回值为要替换的值

  以捕获到的内容为key  value为要替换的值

  * URL编码

  * tab扩展

    ()空白捕获不代表捕获空内容 而是捕获它在目标字符串中的位置

* 技巧

  ：TODO 回看

##### I/O库

I/O库为文件操作提供了两种不同的模型，简单模型和完整模型

* 简单I/O模型

  所有操作都作用于两个当前文件 stdin stdout

  io.read()从标准输入中读取一行

  io.input io.output 可以改变两个当前文件

  `io.input`以只读模式打开指定文件，并将其设为当前输入文件

  io.write()写入当前输出文件

  io.write(a, b, c) > io.write(a .. b .. c)

  * write 与 print的不同

    write在输出时不会添加制表符或回车这样的额外字符

    write使用当前输出文件 print使用标准输出

    print能对参数自动调用tostring()方法，所以可以显示table 函数 和 nil

  io.read(0)用于检查是否到达文件末尾，如果还有数据可以读取，会返回一个空字符串，否则返回nil

* 完整I/O模型

  io.open() 参数 要打开的文件名 模式字符串

  ```lua
  local f = assert(io.open(filename, "r"))
  local t = f:read("*all")
  f:close()
  ```

  io.stdin io.stdout io.stderr 预定义C语言流的句柄

  * 性能小诀窍

  * 二进制文件

    io.open()参数 模式字符串为"b"表示打开二进制文件

  * 其他文件操作

    tmpfile()返回一个临时文件的句柄 

    io.flush f:flush()

    seek

#####操作系统库

操作系统库定义在table os中 包含了文件操作函数 获取当前日期和时间的函数 以及其他一些与操作系统相关的功能。为了保证Lua的可移植性，只使用了ANSI标准中定义的函数。

文件操作：os.rename os.remove

* 日期和时间

  time() date()

  date是time的反函数

  os.clock()会返回当前CPU时间的秒数

* 其他系统调用

  os.exit可以中止当前程序的执行

  os.getenv可获取一个环境变量的值

  os.execute可运行一条系统命令

  os.setlocale设置当前lua程序所使用的区域

##### 调试库

调试库由两类函数组成：自省函数 和 钩子

自省函数允许检查一个正在运行中程序的各个方面

钩子允许跟踪一个程序的执行

栈层 调用调试库的函数是层1 调用这个函数的函数是层2

* 自省机制

  主要的自省函数是debug.getinfo函数

  第一个参数可以是一个函数或一个栈层 返回一个包含了与该函数相关信息的table

  debug.traceback返回一个表示追溯结果的字符串

  * 访问局部变量

    debug.getlocal检查任意活动函数的局部变量

    debug.setlocal改变局部变量的值

  * 访问非局部的变量

    debug.getupvalue

    debug.setupvalue    getfenv(f)返回参数f所指定函数所使用的当前环境

  * 访问其他协同程序

    调试库中的所有自省函数都接受一个可选的协同程序参数作为第一个参数 这样就可以从外部来检查这个协同程序

* 钩子

  使用户可以注册一个钩子函数，这个函数会在程序运行中某个特定事件发生时被调用

  四种事件会触发一个钩子：

  1.调用一个函数时产生的call事件

  2.函数返回时产生的return事件

  3.开始执行一行新代码时产生的line事件

  4.执行完指定数量的指令后产生的count事件

  注册钩子 debug.sethook

* 性能剖析



####第四部分

##### C API概述

