#include <usb_monitor.h>
#include <QApplication>
#include <QObject>
#include <QMessageBox>

using namespace usb_monitor;

class UsbWatcher : public QObject {
    Q_OBJECT
public:
    UsbWatcher() {
        monitor_ = std::make_unique<USBMonitor>(
            [this](UsbState s, std::string_view p) {
                emit usbEvent(static_cast<int>(s), QString::fromStdString(std::string(p)));
            }
        );
        monitor_->start();
    }

signals:
    void usbEvent(int state, const QString& path);

public slots:
    void onUsbEvent(int state, const QString& path) {
        static const char* name[] = { "USB Removed", "USB Inserted", "USB Update Ready" };
        QMessageBox::information(nullptr, name[state], path);
    }

private:
    std::unique_ptr<USBMonitor> monitor_;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    UsbWatcher watcher;
    QObject::connect(&watcher, &UsbWatcher::usbEvent, &watcher, &UsbWatcher::onUsbEvent, Qt::QueuedConnection);
    return app.exec();
}

#include "Qt-usb-monitor.moc"
