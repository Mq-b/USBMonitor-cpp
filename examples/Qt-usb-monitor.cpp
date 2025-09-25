#include <USBMonitor.h>
#include <QApplication>
#include <QObject>
#include <QMessageBox>

#if defined(__APPLE__) && defined(__MACH__)
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

// macOS原生 QMessageBox 不显示 title，于是自定义对话框
class UsbEventDialog : public QDialog {
    Q_OBJECT
public:
    UsbEventDialog(const QString& title, const QString& message, QWidget* parent = nullptr)
        : QDialog(parent) {
        setWindowTitle(title);
        setModal(true);
        QVBoxLayout* layout = new QVBoxLayout(this);
        QLabel* label = new QLabel(message, this);
        layout->addWidget(label);
        QPushButton* btn = new QPushButton("OK", this);
        layout->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, &QDialog::accept);
        setLayout(layout);
        resize(320, 120);
    }
};
#endif

class UsbWatcher : public QObject {
    Q_OBJECT
public:
    UsbWatcher() {
        monitor_ = std::make_unique<USBMonitor>(
            [this](UsbState s, std::string p) {
                emit usbEvent(static_cast<int>(s), QString::fromStdString(p));
            }
        );
        monitor_->startMonitoring();
    }

signals:
    void usbEvent(int state, const QString& path);

public slots:
    void onUsbEvent(int state, const QString& path) {
        static const char* name[] = { "USB Removed", "USB Inserted", "USB Update Ready" };
#if defined(__APPLE__) && defined(__MACH__)
        UsbEventDialog dlg(QString::fromUtf8(name[state]), path);
        dlg.exec();
#else
        QMessageBox::information(nullptr, name[state], path);
#endif
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
