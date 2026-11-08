#include <QGuiApplication>
#include <QSignalSpy>
#include <QStyleHints>
#include <QTest>

#include "ui/controllers/themeController.h"

#ifdef Q_OS_WINDOWS
    #include <QAction>
    #include <QImage>
    #include <QPixmap>
    #include <QSystemTrayIcon>
    #include <qt_windows.h>

    #include "platforms/windows/windowsThemeListener.h"
    #include "ui/utils/systemTrayNotificationHandler.h"

class SilentTrayHandler : public SystemTrayNotificationHandler
{
public:
    explicit SilentTrayHandler(QObject *parent) : SystemTrayNotificationHandler(parent)
    {
    }

protected:
    void notify(Message, const QString &, const QString &, int) override
    {
    }
};
#endif

class TestThemeController : public QObject
{
    Q_OBJECT

private slots:
    void initialApplicationTheme()
    {
        ThemeController controller;
        QCOMPARE(controller.applicationColorScheme(), QGuiApplication::styleHints()->colorScheme());
#ifdef Q_OS_WINDOWS
        auto *listener = controller.findChild<WindowsThemeListener *>();
        QVERIFY(listener);
        const auto expected = listener->colorScheme() == Qt::ColorScheme::Unknown ? controller.applicationColorScheme()
                                                                                  : listener->colorScheme();
        QCOMPARE(controller.systemColorScheme(), expected);
#else
        QCOMPARE(controller.systemColorScheme(), controller.applicationColorScheme());
#endif
    }

    void applicationThemeChanges()
    {
        ThemeController controller;
        QSignalSpy changes(&controller, &ThemeController::applicationColorSchemeChanged);
        auto *styleHints = QGuiApplication::styleHints();
        const auto first = controller.applicationColorScheme() == Qt::ColorScheme::Light ? Qt::ColorScheme::Dark
                                                                                         : Qt::ColorScheme::Light;
        styleHints->colorSchemeChanged(first);
        QCOMPARE(controller.applicationColorScheme(), first);
        QCOMPARE(changes.count(), 1);
        styleHints->colorSchemeChanged(first);
        QCOMPARE(changes.count(), 1);
        styleHints->colorSchemeChanged(Qt::ColorScheme::Unknown);
        QCOMPARE(controller.applicationColorScheme(), Qt::ColorScheme::Unknown);
        QCOMPARE(changes.count(), 2);
#ifndef Q_OS_WINDOWS
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Unknown);
#endif
    }

#ifdef Q_OS_WINDOWS
    void independentWindowsThemes()
    {
        ThemeController controller;
        auto *listener = controller.findChild<WindowsThemeListener *>();
        QVERIFY(listener);
        QSignalSpy changes(&controller, &ThemeController::systemColorSchemeChanged);
        auto *styleHints = QGuiApplication::styleHints();

        listener->colorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Dark);
        changes.clear();
        styleHints->colorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(controller.applicationColorScheme(), Qt::ColorScheme::Light);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Dark);
        QCOMPARE(changes.count(), 0);
        listener->colorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(changes.count(), 0);
        listener->colorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Light);
        QCOMPARE(changes.count(), 1);
        listener->colorSchemeChanged(Qt::ColorScheme::Unknown);
        styleHints->colorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Dark);
        QCOMPARE(changes.count(), 2);
    }

    void unchangedWindowsSettings()
    {
        WindowsThemeListener listener;
        QSignalSpy changes(&listener, &WindowsThemeListener::colorSchemeChanged);
        MSG message { };
        message.message = WM_SETTINGCHANGE;
        QVERIFY(!listener.nativeEventFilter("windows_generic_MSG", &message, nullptr));
        QCoreApplication::processEvents();
        QCOMPARE(changes.count(), 0);
    }

    void trayThemePreservesConnectionState_data()
    {
        QTest::addColumn<int>("state");
        QTest::addColumn<bool>("connectEnabled");
        QTest::addColumn<bool>("disconnectEnabled");
        QTest::newRow("connected") << int(Vpn::Connected) << false << true;
        QTest::newRow("disconnected") << int(Vpn::Disconnected) << true << false;
        QTest::newRow("preparing") << int(Vpn::Preparing) << false << true;
        QTest::newRow("connecting") << int(Vpn::Connecting) << false << true;
        QTest::newRow("disconnecting") << int(Vpn::Disconnecting) << false << true;
        QTest::newRow("reconnecting") << int(Vpn::Reconnecting) << false << true;
        QTest::newRow("error") << int(Vpn::Error) << true << false;
        QTest::newRow("unknown") << int(Vpn::Unknown) << false << true;
    }

    void trayThemePreservesConnectionState()
    {
        QFETCH(int, state);
        QFETCH(bool, connectEnabled);
        QFETCH(bool, disconnectEnabled);
        QObject owner;
        SilentTrayHandler handler(&owner);
        handler.setConnectionState(static_cast<Vpn::ConnectionState>(state));
        QSignalSpy notifications(&handler, &NotificationHandler::notificationShown);
        auto *tray = owner.findChild<QSystemTrayIcon *>();
        QVERIFY(tray);
        QVERIFY(!tray->icon().isNull());
        const auto darkIcon = tray->icon().cacheKey();
        handler.setSystemColorScheme(Qt::ColorScheme::Light);
        const auto lightIcon = tray->icon().cacheKey();
        QVERIFY(lightIcon != darkIcon);
        const QString lightIconName = state == Vpn::Connected ? "default.png"
                : state == Vpn::Error                         ? "error.png"
                                                              : "active.png";
        QImage expectedLightIcon = QPixmap(":/images/tray/" + lightIconName).scaled(128, 128).toImage();
        if (lightIconName == "active.png") {
            for (int y = 0; y < expectedLightIcon.height(); ++y) {
                for (int x = 0; x < expectedLightIcon.width(); ++x) {
                    expectedLightIcon.setPixelColor(x, y, QColor(0, 0, 0, expectedLightIcon.pixelColor(x, y).alpha()));
                }
            }
        }
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(), expectedLightIcon);
        handler.setSystemColorScheme(Qt::ColorScheme::Light);
        QCOMPARE(tray->icon().cacheKey(), lightIcon);
        handler.setSystemColorScheme(Qt::ColorScheme::Dark);
        const QString darkIconName = state == Vpn::Connected ? "active.png"
                : state == Vpn::Error                        ? "error.png"
                                                             : "active.png";
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(),
                 QPixmap(":/images/tray/" + darkIconName).scaled(128, 128).toImage());
        const auto actions = tray->contextMenu()->actions();
        QCOMPARE(actions.at(2)->isEnabled(), connectEnabled);
        QCOMPARE(actions.at(3)->isEnabled(), disconnectEnabled);
        QCOMPARE(notifications.count(), 0);
    }
#endif
};

QTEST_MAIN(TestThemeController)
#include "testThemeController.moc"
