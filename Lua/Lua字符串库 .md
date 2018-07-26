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

    for <var-list> in <exp-list> do

    ​	<body>

    end

    具体的说，对于以下语句

    for var_1,...,var_n in <explist> do

    ​	<block>

    end

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

  * 



