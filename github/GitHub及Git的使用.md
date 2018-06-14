### GitHub及Git的使用

##### 环境配置及安装

* git下载地址 https://git-scm.com/downloads
* github下载地址 https://desktop.github.com/
* 教程
  * https://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000/00137396287703354d8c6c01c904c7d9ff056ae23da865a000
  * https://www.zhihu.com/question/20070065

##### 使用

* 初始设置

  ```
  $ git config --global user.name "Your Name"
  $ git config --global user.email "email@example.com"
  ```

* 建立仓库

  `git init`

* 把文件添加到仓库

  `git add`

* 把文件提交到仓库

  `git commit`

* 查看文件状态

  `git status`

  `git diff`查看具体不同

* 查看提交日志

  `git log`

  与SVN不一样，版本号不是1,2,3···递增的数字，而是SHA1计算出来的一个非常大的数字，用十六进制表示。

  `HEAD`表示当前版本

  `HEAD^`表示上一个版本 

  `HEAD~100`往上100个版本

* 版本回退

  `git reset` 修改指针指向的位置

  `git reflog`记录每一次命令



##### 注意点

* 工作区与暂存区

  * 工作区 当前目录

  * 版本库 .git

     * stage 暂存区
     * master 分支

    add提交到暂存区 commit提交到分支

* 撤销

  `git checkout -- file `将工作区撤回到上一次使用`git add`或`git commit`时的状态

  `git reset HEAD`将暂存区回退到工作区的内容

##### github使用

* 创建SSH Key

  `$ ssh-keygen -t rsa -C "youremail@example.com"`

  在用户目录下的`.ssh`目录里生成两个文件 `id_rsa`私钥 `id_rsa.pub`公钥 

  在github上add ssh key 添加公钥

* 创建远程仓库

* 克隆远程仓库

  `git clone https://github.com/xxx/xxx`

  git:// 使用ssh `https`速度慢 每次推送需要输入口令

* 查看远程仓库信息

  `git remote -v`

* 添加远程仓库

  `git remote add origin xxx`

* 提交到远程仓库

  `git push origin master`

* `git checkout -b xxx`创建并切换分支

  `git branch`查看分支

  `git branch -d xxx`删除分支

  `git checkout xx`切换分支

  `git merge`合并

  加--no-ff 使用普通模式合并

* `git stash`储藏工作现场

------------------------------------------------------- TODO ----------------------------------------------------