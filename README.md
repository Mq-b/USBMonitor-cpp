# USBMonitor-cpp

`USBMonitor-cpp` 是一个用 C++17 编写的开源项目，提供了一个用于实时监控 USB 设备插拔状态的库。

它十分的简单，基于标准 C++，通过开启一个线程时刻监控 Linux 中外部存储设备默认挂载路径来发送信号。

在 Ubuntu22.04 中使用 GCC11、GCC12 测试无误。
