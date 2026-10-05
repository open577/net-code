# net-code
Computer Network Learning Code Repository

## 简介
本仓库存放计算机网络学习相关代码、实验Demo以及学习笔记。
包含 Socket 网络编程、TCP/UDP、简易HTTP、报文抓包等相关示例代码。

## 目录结构
~~~
net-code/
├── socket/         # Socket 基础编程示例 (C/C++)
├── tcp-udp/        # TCP、UDP 通信 demo
├── http/           # 简易 HTTP 客户端与服务端
├── packet/         # 抓包脚本、Wireshark 分析笔记
├── docs/           # 学习笔记、实验文档
└── tools/          # 网络测试辅助小工具
~~~


## 环境依赖
- 推荐系统：Linux
- 编程语言：C / C++
- 工具：gcc、g++、Wireshark

## 编译运行示例
```bash
# 编译源码
gcc server.c -o server
gcc client.c -o client

# 启动服务端
./server

# 新开终端启动客户端
./client

## 学习清单

- Socket 套接字基础编程
- TCP 面向连接通信
- UDP 无连接通信
- 简易 HTTP 服务实现
- 网络数据包抓包与协议解析
- IO 多路复用、多线程网络模型

## 声明

仓库内所有代码仅用于个人学习实验，不可用于生产环境。
代码存在不足，欢迎交流指正。