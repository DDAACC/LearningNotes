### 基础字符串函数

```lua
--
a="sdfABC"

--string.len(a)返回字符串a的长度
t=string.len(a)     -- t=4

--string.rep(s,n)返回字符串s重复n次的结果
t=string.rep(a,3)   -- t="sdfABCsdfABCsdfABC"

--string.lower(a)返回将a中所有大写字符替换为小写字符的副本
t=string.lower(a)   -- t="sdfabc

--string.upper(a)返回将a中所有小写字符替换为大写字符的副本
t=string.upper(a)   -- t="SDFABC"

--string.sub(a,i,j)获取字符串a从第i位到第j位的字符串的副本
t=string.sub(a,2,4)   -- t="dfA"
t=string.sub(a,1,-2)  -- t="dfAB"  -i表示该字符串的倒数第i位

--string.char(...) 接受多个整数，将其转换为对应的字符并且返回这些字符的连接串
t=string.sub(214,208)  -- t="中"

--string.byte(a,i) 返回字符串a的第i个字符的内部数值表示,i可以省略，默认返回第一个字符数值
print(string.byte("abc"))   --97
print(string.byte("abc"),1) --97
print(string.byte("abc"),2) --98

--string.format(...) 格式化字符串，和c++里面的sprintf类似
print(string.format(("%s%d"),"sdf",123))
```

### 模式匹配

* string.find

  用于在一个给定的字符串中搜索一个模式

  string.find(x,y)  返回在字符串x中第一次出现的y模式，如果没有找到匹配，则返回nil

  ```lua
  a="12344123"
  i,j=string.find(a,"1%d+1")  -- 在字符串a中查找以1开头1结尾的数字串
  print(i,j)                  -- i=1,j=6
  i,j=string.find(a,"1%d+7")  -- 在字符串a中查找以1开头7结尾的数字串
  print(i,j)                  -- i=nil,j=nil
  ```

* string.match

  和string.find非常相似，只是string.match返回值为在目标串中匹配到的字符串

  ```lua
  a="12344123"
  i=string.match(a,"1%d+1")  -- 在字符串a中查找以1开头1结尾的数字串
  print(i)                   -- i="123331"
  i=string.match(a,"1%d+7")  -- 在字符串a中查找以1开头7结尾的数字串
  print(i)                   -- i=nil
  ```

* string.gsub

  string.gsub(a,p,v) 将目标串中所有和模式p匹配的字符串部分全部替换为v

  ```lua
  a="12344123"
  p="1%d"
  a=string.gsub(a,p,"hh")
  print(a)                    -- a="hh344hh3"
  ```

* string.gmatch

  string.gmatch返回一个函数，通过这个函数可以遍历到一个字符串中出现的所有指定的模式

  ```lua
  --遍历输出字符串a中所有的单个字母
  a="1a2b3c4d5f"
  p="%a"
  for v in string.gmatch(a,p) do
  	print(v)
  end
  ```

  * 迭代器tips

    泛型for的语法如下

    ```lua
    for <var-list> in <exp-list> do
    
    	<body>
    
    end
    
    ```

    具体的说，对于以下语句

    ```lua
    for var_1,...,var_n in <explist> do
    
    	<block>
    
    end
    
    ```

    执行过程中相当于进行了以下操作

    ```lua
    do
        --从<explist>中获取迭代器函数_f、恒定状态_s、控制变量初值_var
        local _f,_s,_var =<explist> 
        while true do
            local var_1,...var_n=_f(_s,_var)
            _var=var_1
            if _var==nil then break end
            <block>
            end
        end
    end
    ```

* 模式

  可以在模式串中使用字符类，来进行模糊搜索。Lua支持的字符类如下

  | .      任意字符     |
  | ------------------- |
  | %a    字母          |
  | %c     控制字符     |
  | %d     数字         |
  | %l     小写字母     |
  | %p     标点字符     |
  | %s     空白符       |
  | %u     大写字母     |
  | %w     字母和数字   |
  | %x     十六进制数字 |
  | %z     代表0的字符  |

  上面形式的大写形式表示小写形式的补集，例如"%A"表示非字母字符

  ```lua
  print(string.gsub("hello, up-down!", "%A", "."))   --> hello..up.down. 4
  ```

  gsub返回了替换值和替换的次数

  ---

  在模式中还有一些字符被称为"魔法字符"，他们具有特殊的含义

  ` ( ) . % + - * ? [ ] ^ $ `

  字符'%'作为这些魔法字符的转义符，因此**%.**表示匹配一个`.`,**%%**表示匹配字符`'%'`本身

  ```lua
  t="1231...12312^%#$"
  p="%.+"             --匹配字符串t中的多个连续的.连接的字符
  v=string.match(t,p)
  print(v)            --v="..."
  ```

  用户可以通过在一对方括号内将不同的字符分类或单个字符组合起来，创建属于自己定义的字符分类，这种新的字符分类被称为`字符集`,例如[%w_]表示同时匹配字母、数字和下划线，字符集[01]表示匹配二进制数字

  ```lua
  t="sdfsdf00011012313sdf"
  p="[01]+"                --匹配字符串t中的二进制串
  v.string.match(t,p)      --t="0001101"
  print(v)
  ```

  在Lua中可以通过修饰符来描述模式中的重复部分和可选部分

  在一个模式中，如果以`^`为开头，则表示只会匹配字符串开头部分，以`$`结尾，则表示只会匹配字符串结尾部分

  | + 重复一次或多次 |
  | ---------------- |
  | * 重复0次或多次  |
  | - 重复0次或多次  |
  | ？出现0次或1次   |

* 捕获

  捕获功能可以根据一个模式从目标字符串中抽出匹配于该模式的内容。在指定捕获的时候，应将模式中需要捕获的部分写到一对圆括号`()`内。

  对于具有捕获的模式，函数`string.match`会将所有捕获到的值作为单独的值返回

  ```lua
  t="sdfsdf00011012313sdf"
  p="(.+)01(.+)df"           -- '+"总是会试图匹配目标串中最长的匹配字符
  v1,v2=string.match(t,p)    -- v1="sdfsdf00011" v2="2313s"
  print(v1,v2)    
  ```

  匹配一段字符串中的时间格式

  ```lua
  date="Today is 2017/7/27"
  p="(%d+)/(%d+)/(%d+)"
  y,m,d=string.match(date,p)
  print(y,m,d)                 --2017    7       27
  ```

* 替换

  string.gsub的第三个参数不仅是一个字符串，还可以是一个函数或者table。当用一个函数来调用的时候，string.gsub会在每次找到匹配时调用该函数，调用的参数就是捕获到的内容，而该函数的返回值就是替换的值。当用table作为参数时，string.gsub会将每次捕获到的内容作为key值在table中进行查询并且替换，如果table中不包含这个key，那么不改变匹配值

  ```lua
  date="Today is 2017/7/27"
  p="%d+"
  function f(g)
  	print(g)
  	return "c"
  end
  date=string.gsub(date,p,f)
  print(date)
  
  ---运行结果
  ---2017
  ---7
  ---27
  ---Today is c/c/c
  
  ```

  



