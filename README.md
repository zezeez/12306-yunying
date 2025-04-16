# ![](images/ticket.ico) 云映12306客户端

*由Qt构建的 12306 GUI购票工具*

![](https://img.shields.io/badge/GPL-v3-blue.svg)
![](https://img.shields.io/badge/Qt-6.0.0-blue.svg)
![](https://img.shields.io/badge/C++-11-deeppink.svg)
![](https://img.shields.io/badge/12306-App-red.svg)

![](https://img.shields.io/badge/-Windows-blue?logo=windows)
![](https://img.shields.io/badge/-MacOS-black?logo=apple)
![](https://img.shields.io/badge/-Linux-333?logo=ubuntu)

### 支持功能
- **即时下单**
- **候补**
- **定时抢票**
- **捡漏**
- **学生票**
- **车次/席别优先规则**
- **选座**
- **自动打开支付页面**
- **持邮件通知**
- **Server酱通知**

### 测试平台
![](https://img.shields.io/badge/Windows-10-pass.svg) ![](https://img.shields.io/badge/MACOS-12.5.3-pass.svg) ![](https://img.shields.io/badge/Ubuntu-20.04-pass.svg)

### 软件截图
![](https://op9.top/img/running_snapshot.png)

**Qt版本要求**

``支持Qt 6.0.0之后的版本``

**操作系统要求**

``支持Windows/MacOS/Linux``

### 安装

##### 方法1 直接下载
2. 目前有两个网站用于发布已编译的安装包，其中Windows为便携版本，无需安装
   * [https://zezeez.github.io](https://zezeez.github.io)
   * [https://op9.top](https://op9.top)

从上述网站中下载安装包，优点是简单、开箱即用

##### 方法2 源代码编译
适合有代码基础的用户

#### 1. 安装Qt编译环境
编译器可选Virtual Studio或MinGW

#### 2. 导入项目
下载项目仓库代码，在QtCreator中`文件->打开文件或项目`打开项目中`yunying.pro`导入项目

#### 3. 注释HAS_CDN
由于Qt Network库不支持主动设置域名对应的ip，而是要走一遍域名解析流程，如果不注释HAS_CDN，会报无法解析setIpAddress符号错误，所以在编译之前请先注释`yunying.pro`文件中的宏`HAS_CDN`

前面加上``#``号注释

``# DEFINES += HAS_CDN``

否则编译会出错

#### 4. 构建并运行
点击左下角的绿色三角形即自动构建并运行，完成后会弹出软件登陆界面及主界面

### 使用方法
如需了解操作步骤、使用方法，参照 [使用教程](https://op9.top/help.html) 提供的使用教程

## 声明
### 本软件仅用于学习交流，禁止用于任何商业行为，包括但不限于车票代购、倒卖、囤票、加价等行为，违者自行承担相关责任

## 许可证
### [GPL V3](LICENSE)
如果您自行发布了本软件、发布了本软件的修改版本或您的软件中包含了本软件的组件，请确保您的许可证是与`GPL V3`是相兼容的

#### [关于抢票/候补的一点思考](LittleSummary.md)

