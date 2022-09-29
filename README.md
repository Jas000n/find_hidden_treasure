# find_hidden_treasure

a multi-player client-server game using linux system call

## Installation
    git clone https://github.com/Jas000n/find_hidden_treasure.git
## Usage
    cd find_hidden_treasure
    //use this to init game, listen on the port you like, waiting for clients(gamers) to connect,  8080 for example
    ./server 127.0.0.1 8080 
    //use this to connect game server, use w, a, s, d to move the player(a ball) and hunt the hidden treasure
    ./client 1270.0.0.1 8080 	

## 1   概述

每个玩家可以通过键盘的W，A，S，D操控小球探索地图，即控制台，在过程中需要寻找一处隐藏的宝藏（具体位置由服务器指定），需要比其他连接至服务器的玩家更快找到宝藏以获胜（移动到宝藏坐标）。当其中一名玩家胜利后其他玩家客户端会被提示失败，显示失败界面。游戏随服务器性能影响支持不同数量的玩家同时游玩。

## 2   项目演示

这里只放几张游玩过程的截图，具体游玩视频会放在答辩视频中。

![img](https://tva1.sinaimg.cn/large/e6c9d24egy1h1drdbx647j20jy0b8t9c.jpg)

![img](https://tva1.sinaimg.cn/large/e6c9d24egy1h1drdcpvglj20jx0b73z4.jpg)

![img](https://tva1.sinaimg.cn/large/e6c9d24egy1h1drda87nij20jx0b7aaq.jpg)

## 3   系统设计和系统实现

因为linux下的线程并不比进程轻量许多，而且线程间通信较为复杂，所以客户端和服务器端都选择用多进程实现。下图为我设计的系统

![img](https://tva1.sinaimg.cn/large/e6c9d24egy1h1drdayqyoj20zb0ec3zj.jpg)

 

客户端：客户端fork出game子进程负责运行curses程序，父进程socket负责与服务器通讯。

 

子进程game相当于前端，从键盘getch，根据输入的w，a，s，d移动小球，并且将移动的操作通过管道写给socket父进程。并且在收到代表输赢的两种不同的信号量时，调用不同的函数展示游戏结果。

父进程socket负责连接服务器，先把客户端自身的情况告诉服务器（类似握手），之后从管道读子进程写入的小球的移动方向后，将小球的动作写给服务器，由服务器计算小球所在坐标以及是否探寻到宝藏，这样保证客户端不能作弊。

 

服务器端：用prefork的方法提前创建出子进程（接线员），这些接线员通过共享内存的方式确定其他接线员连接的客户端是否赢得了游戏。如果一个接线员连接的玩家赢得了游戏，共享内存会被该接线员写为“Y”，表明已经有人赢得了游戏，并且在写的过程会加锁，保证只有最先找到宝藏的玩家可以获胜。如果一个接线员自己连接的玩家没有赢得游戏而共享内存已被写为“Y”则表明有其他玩家已经赢得了游戏，这时玩家客户端会显示游戏失败。每个接线员在服务过一定次数过后会“死亡”，并且服务器会fork出新的进程补充上来，保持接线员的数量。

 

在这个系统中应用的技术主要有：

\1. 进程间通信：管道（客户端内父进程和子进程），socket（客户端父进程和服务器内的接线员），共享内存（服务器内的不同接线员之间），信号量（发送给客户端子进程）

\2. curses库，用于在客户端上显示游戏界面、游戏规则和胜利失败等游戏结果

3.进程同步：POSIX信号量（保证接线员们对共享内存的读写不出问题）

 

健壮性：在函数的关键位置都做了处理，考虑了诸如内存不够不能fork，对无效文件描述符读写失败等情况，不会出现内核错误。

## 4   学习体会和心得

 

整个完成大作业的过程中，在系统设计上花费的时间和遇到的困难最多。因为没有用高级编程语言，客户端和服务器都用c语言和系统调用实现，而c语言没有面向对象的特性，所以在设计系统架构的时候很难清晰的分包，理清关系。

这里说一下设计系统时候的一个小插曲，一开始用telnet当客户端，想实现任何人只要有一台电脑就能玩我这款游戏，不需要安装客户端，前后端所有逻辑都在服务器上实现。下图为当时的设计思路：

![img](https://tva1.sinaimg.cn/large/e6c9d24egy1h1drdd7suhj20z90b3jse.jpg)

先把服务器的1文件描述符关闭，再接受客户端telnet的tcp连接，根据最低可用原则此时socket连接的文件描述符会是1，之后在服务器上运行的curses就可以显示在客户端上了（因为curses像printf一样往标准输出，即1上写）。又因为server的标准输出被关闭了，不能打印系统日志方便调试，因此又打开一个log.txt文件，将server运行时状态，接受的客户端消息写入，方便查错。然而这么做的结果就是前后端都混杂在一起，逻辑极其混乱，遂放弃。所以后来又好好设计了系统，让前后端逻辑分离。
