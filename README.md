# USBMonitor-cpp

`USBMonitor-cpp` 是一个用 C++17 编写的开源项目，提供了一个用于实时监控 USB 设备插拔状态的库。

它十分的简单，通过开启一个线程时刻监控 Linux 中外部存储设备默认挂载路径来发送信号。

而 windows 中，它使用了一个 Windows API `GetDriveTypeA` 来获取当前所有驱动器进行监控。

macOS 下，U 盘等外部存储设备插入后会自动挂载到 `/Volumes/<设备名>` 目录。这里通过定时轮询 `/Volumes` 目录，记录当前所有挂载点。每次轮询时，检测新出现的挂载点（即插入事件），和消失的挂载点（即拔出事件）。插入时会进一步判断 `/Volumes/<设备名>/update` 文件夹是否存在且不为空，若满足则判定为“更新盘”。

这种方式无需依赖底层 IOKit 或 BSD 设备名。


> [!NOTE]
> `USBMonitor-cpp` 是一个**纯头文件库**，你只需要将 include 文件夹的内容复制到自己项目目录即可使用。

## 构建

```shell
git clone https://github.com/Mq-b/USBMonitor-cpp
cd USBMonitor-cpp
mkdir build
cd build
cmake ..
cmake --build . -j
```

会在 `build` 目录下生成编译好的 `example` 可执行文件。

在 Ubuntu22.04 中使用 GCC11、GCC12、GCC13 测试无误。

![linux-qt-demo](./image/Linux-Qt-demo.png)

在 windows 使用 visual studio 17 测试无误。

![win-qt-demo](./image/win-Qt-demo.png)


macOS 14 环境：

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


macOS 监控逻辑采用轮询 `/Volumes` 目录，插拔 U 盘时会自动提示挂载点变化。

![macOS-Qt-demo](./image/macOS-Qt-demo.png)

---

如果需要如 wsl 之类的无桌面的 Linux 系统需要显式挂载外部存储设备进行测试，则可以这样：

```shell
# 挂载外部存储这个 E 是 windows 中盘符
mount -t drvfs E: /media/A/E

# 手动拔出设备后解除挂载点
sudo umount /media/A/E
# 删除文件夹
sudo rm -rf /media/A/E
```

```txt
root@Mq-B:~/test/USBMonitor-cpp/build/bin# sudo ./example-usb-monitor
插入更新U盘：/media/A/E
USB Update Ready: /media/A/E
拔出U盘：/media/A/E
USB Removed: /media/A/E
```

如果是完整的带桌面的系统会自动处理挂载和卸载。
