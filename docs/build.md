# build

## Linux

Ubuntu22.04 使用 gcc13 + cmake + makefile 编译示例：

```bash
mq-b@mqb-ubuntu:~/project$ git clone https://github.com/Mq-b/USBMonitor-cpp
正克隆到 'USBMonitor-cpp'...
remote: Enumerating objects: 56, done.
remote: Counting objects: 100% (56/56), done.
remote: Compressing objects: 100% (34/34), done.
remote: Total 56 (delta 17), reused 44 (delta 11), pack-reused 0 (from 0)
接收对象中: 100% (56/56), 13.07 KiB | 1.45 MiB/s, 完成.
处理 delta 中: 100% (17/17), 完成.
mq-b@mqb-ubuntu:~/project$ cd USBMonitor-cpp/
mq-b@mqb-ubuntu:~/project/USBMonitor-cpp$ mkdir build
mq-b@mqb-ubuntu:~/project/USBMonitor-cpp$ cd build
mq-b@mqb-ubuntu:~/project/USBMonitor-cpp/build$ cmake ..
-- The C compiler identification is GNU 13.1.0
-- The CXX compiler identification is GNU 13.1.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/gcc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/g++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/mq-b/project/USBMonitor-cpp/build
mq-b@mqb-ubuntu:~/project/USBMonitor-cpp/build$ cmake --build . -j
[ 12%] Automatic MOC for target example-usb-monitor
[ 25%] Automatic MOC for target example-Qt-usb-monitor
[ 25%] Built target example-usb-monitor_autogen
[ 37%] Building CXX object CMakeFiles/example-usb-monitor.dir/example-usb-monitor_autogen/mocs_compilation.cpp.o
[ 50%] Building CXX object CMakeFiles/example-usb-monitor.dir/examples/usb-monitor.cpp.o
[ 50%] Built target example-Qt-usb-monitor_autogen
[ 62%] Building CXX object CMakeFiles/example-Qt-usb-monitor.dir/examples/Qt-usb-monitor.cpp.o
[ 75%] Building CXX object CMakeFiles/example-Qt-usb-monitor.dir/example-Qt-usb-monitor_autogen/mocs_compilation.cpp.o
[ 87%] Linking CXX executable bin/example-usb-monitor
[ 87%] Built target example-usb-monitor
[100%] Linking CXX executable bin/example-Qt-usb-monitor
[100%] Built target example-Qt-usb-monitor
mq-b@mqb-ubuntu:~/project/USBMonitor-cpp/build$ cd bin/
mq-b@mqb-ubuntu:~/project/USBMonitor-cpp/build/bin$ ls
example-Qt-usb-monitor  example-usb-monitor
```

注意需要以 `sudo` 权限运行 `example`，否则无法访问 `/media` 目录。

![linux-qt-demo](./image/Linux-Qt-demo.png)

## Windows

在 Windows 中使用 Visual Studio 17 工具链测试无误。

## macOS

macOS 14中如下环境测试无误。


```
❯ clang++ -v
Homebrew clang version 20.1.8
Target: arm64-apple-darwin23.0.0
Thread model: posix
InstalledDir: /opt/homebrew/Cellar/llvm/20.1.8/bin
Configuration file: /opt/homebrew/etc/clang/arm64-apple-darwin23.cfg
System configuration file directory: /opt/homebrew/etc/clang
User configuration file directory: ~/.config/clang
❯ cmake --version
cmake version 4.1.0

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```
