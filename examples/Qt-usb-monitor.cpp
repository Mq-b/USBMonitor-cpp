#include <USBMonitor.h>
#include <QMessageBox>
#include <QApplication>

/*
* 用于测试 USBMonitor 类的 Qt 示例
* 该示例演示了如何使用 USBMonitor 类来监视 USB 设备的插入和拔出
* 并在设备插入和拔出时显示消息框
*
* 你也可以选择 emit 发送信号来与 Qt 的 UI 进行通信。
* 这对 QML 程序来说也是有效的。
*/


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    USBMonitor monitor([](UsbState state, std::string path){
        switch (state) {
            case UsbState::Removed:
                QMessageBox::information(nullptr, "USB Removed", QString::fromStdString(path));
                break;
            case UsbState::Inserted:
                QMessageBox::information(nullptr, "USB Inserted", QString::fromStdString(path));
                break;
            case UsbState::UpdateReady:
                QMessageBox::information(nullptr, "USB UpdateReady", QString::fromStdString(path));
                break;
        }
    });

    monitor.startMonitoring();

    while (true) {
        app.processEvents();
    }
}
