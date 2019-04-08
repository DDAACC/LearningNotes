# Lua与C交互

在c与Lua之间通信关键内容在于一个虚拟的栈，几乎所有的API调用都是对栈上的值进行操作，所有的c与Lua之间的数据交换也是通过这个栈来完成。

---

#### 压栈操作

在API中有一系列压栈的操作

````c++
void lua_pushnil(lua_State* L); \\压入nil类型变量
void lua_pushbollean(lua_State* L, int bool) \\压入bool类型变量
void lua_pushnumber(lua_State* L, double n);
void lua_pushlstring(lua_State* L, const char* s, size_t length);
void lua_pushstring(lua_State* L, const char* s);


````

Lua中的字符串不是以**\0**作为结束符的,它们依赖一个明确的长度,将字符串压入串的正式函数是**lua_pushlstring**

,使用**lua_pushstring**将会自动调用**strlen**来计算字符串的长度。

栈上的空间是有限的，在有些情况下，栈的空间可能是不够的，这时候需要依赖这个函数

````c++
int lua_checkstack(lua_State* L, int sz); // 检测栈上是否有足够的空间
````

---

#### 查询元素

* 栈中元素索引

  第一个为1,第二个为2，以此类推

  -1表示栈顶，-2指前一个，以此类推

* 检测一个元素是否为某个类型

  ````c++
  int lua_is...(lua_State* L, int index); \\...可以是 number string table等， index表示栈中元素索引
  ````

* 获取元素类型

  ````c++
  #define LUA_TNIL 			0
  #define LUA_TBOLLEAN		1
  #define LUA_TLIGHTUSERDATA	2
  //...
  
  int lua_type(lua_State* L, int index); \\ 返回值类似上述宏
  ````

* 获取元素值

  ````c++
  int				lua_tobollean(lua_State* L, int index);
  double			lua_tonumber(lua_State* L, int index);
  const char *	lua_tostring(lua_State* L, int index);
  \\...
  ````

#### 其他堆栈操作

````c++
int  lua_gettop (lua_State *L); \\获取栈顶索引
void lua_settop (lua_State *L, int index); \\设置栈顶,高于新栈顶的值会被丢弃
void lua_pushvalue (lua_State *L, int index); \\压入堆栈上指定索引的元素的拷贝到栈顶
void lua_remove (lua_State *L, int index); \\移除指定索引的元素,其上方所有元素会下移一位来填补该处空白
void lua_insert (lua_State *L, int index); \\移动栈顶元素到指定索引
void lua_replace (lua_State *L, int index); \\从栈顶弹出元素,并且将指定位置元素替换为刚才弹出的栈顶元素
````

---

#### 通过Lua扩展应用

* 获取lua里table的元素

  ````c++
  #define LUA_API __declspec(dllexport)
  LUA_API void lua_getfield(lua_State* L, int idx, const char * k);
  ````

  这个函数将会查找栈中索引为idx的table,并且将这个table的key为k的元素压入栈顶

  ```c++
  #define LUA_GLOBALSINDEX	(-10002)
  #define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))
  ```

  这个函数将会从_G中取key，从而可以查找到全局的变量

  

  **使用例子**

  ````lua
  //main.lua
  BLUE = {r = 0, g = 0, b = 0}
  ````

  从main.lua中取到BLUE和BLUE的元素的c代码如下

  ````c++
  lua_getglobal(L, "BLUE");
  const char * pKey = "r";
  lua_getfield(L, -1, pKey);
  int r = lua_tonumber(L, -1);
  lua_pop(L, 1);
  ````

  也可以对取值的操作进行封装

  ````c++
  int GetField(cont char* pKey)
  {
      int nRet;
      lua_getfield(L, -1, pKey);
      nRet = lua_tonumber(L, -1);
      lua_pop(L, 1);
      return nRet;
  }
  ````

* 调用lua中函数

  ````c++
  LUA_API void  (lua_callk) (lua_State *L, int nargs, int nresults,
                             lua_KContext ctx, lua_KFunction k);
  #define lua_call(L,n,r)		lua_callk(L, (n), (r), 0, NULL)
  
  LUA_API int   (lua_pcallk) (lua_State *L, int nargs, int nresults, int errfunc,
                              lua_KContext ctx, lua_KFunction k);
  #define lua_pcall(L,n,r,f)	lua_pcallk(L, (n), (r), (f), 0, NULL)
  
  \\ n为参数个数, r为返回值个数, lua_kFunction为错误处理函数
  \\ 调用lua_pcall后，会按照r的值来返回结果的个数,多余的丢弃,少的使用nil填充,返回值将会被插入到栈顶,在结果入栈前,lua_pcall会将栈内的函数和参数移除
  ````

  ````c++
  // 假设lua中有这样一个函数 function f(x, y)
  //							return x + y;
  //						end
  
  lua_getglobal(L, "f");\\压入函数
  lua_pushnumber(L, 1);\\压入参数1
  lua_pushnumber(L, 2);\\压入参数2
  lua_pcall(L, 2, 1, 0);
  int nRet = lua_tonumber(L, -1);
  lua_pop(L, 1);
  // nRet为函数调用返回值
  ````

----

#### Lua中调用C函数

任何在c中注册的函数必需有同样的原型,这个原型在lua.h中定义

````c++
typedef int (*lua_CFunciton) (lua_State* L);
````

一个C函数接受一个单一的参数**lua_State***,返回的数值代表该函数在lua中返回值个数.

一个函数的调用方式可能是这样的

````c++
int Lua_Test(lua_State* L)
{
    int nX = lua_tonumber(L, -1);
    int ny = lua_tonumber(L, -2);
    printf("%d %d", nX, nY);
    lua_pushnumber(L, nX);
    lua_pushnumber(L, ny);
}
...
lua_pushcfunction(L, Lua_Test);
lua_setglobal(L, "T");

\\main.lua
\\local a, b = T(2, 3);
\\print(a,b);
````

c语言中通过**lua_pushcfunction** 以及 **lua_setglobal**将Lua_Test函数压入栈中并且设置为全局Table **_G**的名字为"T"的元素，在Lua里就可以使用T来调用Lua_Test函数

main.lua中,T(2, 3)会将2和3按顺序压入栈内，然后可以通过lua_tonumber在CFunction Lua_Test中取到这两个值,运算结束后，在C里再将返回值压回栈内，就可以在Lua里取到返回值, 返回值a，b依次按照压栈的顺序获取

在上面的例子中, nX=3, nY = 2, a = 2, b = 3;

对于稍微专业点的注册函数，我们需要检测lua中所给的参数的类型

````c++
luaL_check...(luaState* L, int Index);
//检查栈中指定索引元素类型, lua_checknumber, lua_checkstring ...
````

---

### UserData

