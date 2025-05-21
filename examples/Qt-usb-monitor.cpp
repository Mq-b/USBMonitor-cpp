#include <USBMonitor.h>
#include <QApplication>
#include <QObject>
#include <QMessageBox>

class UsbWatcher : public QObject {
    Q_OBJECT
public:
    UsbWatcher() {
        monitor_ = std::make_unique<USBMonitor>(
            [this](UsbState s, std::string p) {
                emit usbEvent(s, QString::fromStdString(p));
            }
        );
        monitor_->startMonitoring();
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
