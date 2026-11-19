#include <QColor>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QImage>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QStyleHints>
#include <QTest>
#include <QUuid>

#include "amneziaApplication.h"
#include "core/controllers/themeController.h"
#include "core/repositories/secureAppSettingsRepository.h"
#include "platforms/systemThemeListener.h"
#include "secureQSettings.h"
#include "utils/testCoreController.h"

#if !defined(Q_OS_MAC) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    #include "ui/utils/systemTrayNotificationHandler.h"
    #include <QAction>
    #include <QPixmap>
    #include <QSystemTrayIcon>

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

#ifdef Q_OS_WINDOWS
    #include "platforms/windows/windowsThemeListener.h"
    #include <qt_windows.h>
#endif

class TestThemeController : public QObject
{
    Q_OBJECT

private slots:
    void modeResolution_data()
    {
        QTest::addColumn<int>("mode");
        QTest::addColumn<Qt::ColorScheme>("source");
        QTest::addColumn<Qt::ColorScheme>("expected");
        QTest::newRow("dark/unknown") << int(ThemeController::Dark) << Qt::ColorScheme::Unknown << Qt::ColorScheme::Dark;
        QTest::newRow("dark/light") << int(ThemeController::Dark) << Qt::ColorScheme::Light << Qt::ColorScheme::Dark;
        QTest::newRow("dark/dark") << int(ThemeController::Dark) << Qt::ColorScheme::Dark << Qt::ColorScheme::Dark;
        QTest::newRow("light/unknown") << int(ThemeController::Light) << Qt::ColorScheme::Unknown
                                       << Qt::ColorScheme::Light;
        QTest::newRow("light/light") << int(ThemeController::Light) << Qt::ColorScheme::Light << Qt::ColorScheme::Light;
        QTest::newRow("light/dark") << int(ThemeController::Light) << Qt::ColorScheme::Dark << Qt::ColorScheme::Light;
        QTest::newRow("system/unknown") << int(ThemeController::System) << Qt::ColorScheme::Unknown
                                        << Qt::ColorScheme::Dark;
        QTest::newRow("system/light") << int(ThemeController::System) << Qt::ColorScheme::Light << Qt::ColorScheme::Light;
        QTest::newRow("system/dark") << int(ThemeController::System) << Qt::ColorScheme::Dark << Qt::ColorScheme::Dark;
    }

    void modeResolution()
    {
        QFETCH(int, mode);
        QFETCH(Qt::ColorScheme, source);
        QFETCH(Qt::ColorScheme, expected);
        ThemeController controller;
        auto *listener = controller.findChild<SystemThemeListener *>();
        QVERIFY(listener);
        listener->applicationColorSchemeChanged(source);
        controller.setThemeMode(static_cast<ThemeController::ThemeMode>(mode));
        QCOMPARE(controller.effectiveColorScheme(), expected);
    }

    void modeTransitionsAndNotifications()
    {
        ThemeController controller;
        auto *listener = controller.findChild<SystemThemeListener *>();
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Dark);
        QSignalSpy modeChanges(&controller, &ThemeController::themeModeChanged);
        QSignalSpy effectiveChanges(&controller, &ThemeController::effectiveColorSchemeChanged);
        controller.setThemeMode(ThemeController::System);
        QCOMPARE(modeChanges.count(), 1);
        QCOMPARE(effectiveChanges.count(), 0);
        controller.setThemeMode(ThemeController::System);
        controller.setThemeMode(static_cast<ThemeController::ThemeMode>(99));
        QCOMPARE(modeChanges.count(), 1);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(controller.effectiveColorScheme(), Qt::ColorScheme::Light);
        QCOMPARE(effectiveChanges.count(), 1);
        controller.setThemeMode(ThemeController::Light);
        QCOMPARE(modeChanges.count(), 2);
        QCOMPARE(effectiveChanges.count(), 1);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(controller.effectiveColorScheme(), Qt::ColorScheme::Light);
        controller.setThemeMode(ThemeController::System);
        QCOMPARE(controller.effectiveColorScheme(), Qt::ColorScheme::Dark);
        QCOMPARE(effectiveChanges.count(), 2);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Unknown);
        QCOMPARE(effectiveChanges.count(), 2);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Light);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(effectiveChanges.count(), 3);
        controller.setThemeMode(ThemeController::Dark);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(controller.effectiveColorScheme(), Qt::ColorScheme::Dark);
        QCOMPARE(effectiveChanges.count(), 4);
    }

    void persistedModes_data()
    {
        QTest::addColumn<QVariant>("stored");
        QTest::addColumn<int>("expected");
        QTest::newRow("missing") << QVariant() << int(ThemeController::Dark);
        QTest::newRow("dark") << QVariant(0) << int(ThemeController::Dark);
        QTest::newRow("light") << QVariant(1) << int(ThemeController::Light);
        QTest::newRow("system") << QVariant(2) << int(ThemeController::System);
        QTest::newRow("negative") << QVariant(-1) << int(ThemeController::Dark);
        QTest::newRow("out-of-range") << QVariant(3) << int(ThemeController::Dark);
        QTest::newRow("invalid") << QVariant("invalid") << int(ThemeController::Dark);
    }

    void persistedModes()
    {
        QFETCH(QVariant, stored);
        QFETCH(int, expected);
        const QString organization = "AmneziaVPN-ThemeTest-" + QUuid::createUuid().toString(QUuid::WithoutBraces);
        {
            SecureQSettings settings(organization, "theme", nullptr, false);
            if (stored.isValid()) {
                settings.setValue("Conf/themeMode", stored);
            }
        }
        {
            SecureQSettings settings(organization, "theme", nullptr, false);
            SecureAppSettingsRepository repository(&settings);
            ThemeController controller(nullptr, &repository);
            QCOMPARE(int(controller.themeMode()), expected);
            for (const auto mode : { ThemeController::Light, ThemeController::System, ThemeController::Dark }) {
                controller.setThemeMode(mode);
                ThemeController restored(nullptr, &repository);
                QCOMPARE(restored.themeMode(), mode);
            }
            controller.setThemeMode(ThemeController::Light);
            const auto lightBackup = repository.backupAppConfig();
            controller.setThemeMode(ThemeController::Dark);
            QVERIFY(repository.restoreAppConfig(lightBackup));
            QCOMPARE(controller.themeMode(), ThemeController::Light);
            QCOMPARE(controller.effectiveColorScheme(), Qt::ColorScheme::Light);
            QVERIFY(!repository.restoreAppConfig("invalid"));
            QCOMPARE(controller.themeMode(), ThemeController::Light);
            repository.clearSettings();
            QCOMPARE(controller.themeMode(), ThemeController::Dark);
            QCOMPARE(controller.effectiveColorScheme(), Qt::ColorScheme::Dark);
            settings.clearSettings();
        }
    }

    void qmlUsesControllerContract()
    {
        ThemeController controller;
        QQmlEngine engine;
        engine.addImportPath("qrc:/ui/qml/Modules");
        engine.rootContext()->setContextProperty("ThemeController", &controller);
        QQuickWindow window;
        window.resize(380, 680);
        QQmlComponent component(&engine);
        component.setData(R"(
            import QtQuick
            import Style 1.0
            import ThemeMode 1.0
            Rectangle {
                color: AmneziaStyle.color.bgBase
                function chooseLight() { ThemeController.themeMode = ThemeMode.Light }
                function chooseSystem() { ThemeController.themeMode = ThemeMode.System }
            }
        )",
                          QUrl());
        QScopedPointer<QObject> item(component.create());
        QVERIFY2(item, qPrintable(component.errorString()));
        auto *background = qobject_cast<QQuickItem *>(item.data());
        QVERIFY(background);
        background->setParentItem(window.contentItem());
        background->setSize(QSizeF(380, 680));
        QCOMPARE(item->property("color").value<QColor>(), QColor("#101012"));
        QVERIFY(QMetaObject::invokeMethod(item.data(), "chooseLight"));
        QCOMPARE(controller.themeMode(), ThemeController::Light);
        QCOMPARE(item->property("color").value<QColor>(), QColor("#fafafa"));
        auto *listener = controller.findChild<SystemThemeListener *>();
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(item->property("color").value<QColor>(), QColor("#fafafa"));
        QVERIFY(QMetaObject::invokeMethod(item.data(), "chooseSystem"));
        QCOMPARE(item->property("color").value<QColor>(), QColor("#101012"));
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(item->property("color").value<QColor>(), QColor("#fafafa"));

        QQmlComponent contextComponent(&engine);
        contextComponent.setData(R"(
            import QtQuick
            QtObject {
                property int safeAreaBottomMargin: 0
                function getLineHeightAppend() { return 0 }
                function getInitialPageNavigationBarColor() { return 0 }
                function updateNavigationBarColor(color) {}
            }
        )",
                                 QUrl());
        QScopedPointer<QObject> contextObject(contextComponent.create());
        QVERIFY2(contextObject, qPrintable(contextComponent.errorString()));
        engine.rootContext()->setContextProperty("PageController", contextObject.data());
        engine.rootContext()->setContextProperty("LanguageUiController", contextObject.data());
        QQmlComponent drawerComponent(&engine, QUrl("qrc:/ui/qml/Components/SelectThemeDrawer.qml"));
        QScopedPointer<QObject> drawer(drawerComponent.create());
        QVERIFY2(drawer, qPrintable(drawerComponent.errorString()));
        QVERIFY(drawer->setProperty("parent", QVariant::fromValue(window.contentItem())));
        QCOMPARE(drawer->property("currentThemeName").toString(), QString("Automatic (system)"));
        QObject *lightOption = nullptr;
        QList<QQuickItem *> pending { drawer->property("contentItem").value<QQuickItem *>() };
        while (!pending.isEmpty()) {
            auto *child = pending.takeLast();
            QVERIFY(child);
            if (child->property("text").toString() == "Light" && child->property("checked").isValid()) {
                lightOption = child;
                break;
            }
            pending.append(child->childItems());
        }
        QVERIFY(lightOption);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        QVERIFY(QMetaObject::invokeMethod(drawer.data(), "open"));
        QTRY_VERIFY(drawer->property("opened").toBool());
        auto *lightItem = qobject_cast<QQuickItem *>(lightOption);
        QVERIFY(lightItem);
        const QPointF position = lightItem->mapToScene(QPointF(lightItem->width() / 2, lightItem->height() / 2));
        QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier, position.toPoint());
        QCOMPARE(controller.themeMode(), ThemeController::Light);
        QCOMPARE(drawer->property("currentThemeName").toString(), QString("Light"));
        QCOMPARE(lightOption->property("checked").toBool(), true);
        QTRY_VERIFY(!drawer->property("visible").toBool());
        const QString captureDirectory = qEnvironmentVariable("AMNEZIA_THEME_CAPTURE_DIR");
        if (!captureDirectory.isEmpty()) {
            for (const auto mode : { ThemeController::Light, ThemeController::Dark }) {
                controller.setThemeMode(mode);
                QVERIFY(QMetaObject::invokeMethod(drawer.data(), "open"));
                QTRY_VERIFY(drawer->property("opened").toBool());
                QTest::qWait(300);
                const QString name = mode == ThemeController::Light ? "theme-drawer-light.png" : "theme-drawer-dark.png";
                QVERIFY(window.grabWindow().save(QDir(captureDirectory).filePath(name)));
                QVERIFY(QMetaObject::invokeMethod(drawer.data(), "close"));
                QTRY_VERIFY(!drawer->property("visible").toBool());
            }
        }
        controller.setThemeMode(ThemeController::Dark);
        QCOMPARE(drawer->property("currentThemeName").toString(), QString("Dark"));
        QCOMPARE(lightOption->property("checked").toBool(), false);
    }

#ifdef Q_OS_WINDOWS
    void changedPagesLoad_data()
    {
        QTest::addColumn<QString>("page");
        QTest::newRow("application") << QString("PageSettingsApplication.qml");
        QTest::newRow("qr-reader") << QString("PageSetupWizardQrReader.qml");
        QTest::newRow("mtproxy") << QString("PageServiceMtProxySettings.qml");
        QTest::newRow("telemt") << QString("PageServiceTelemtSettings.qml");
        QTest::newRow("controls") << QFINDTESTDATA("qml/ThemeControlsPreview.qml");
    }

    void changedPagesLoad()
    {
        QFETCH(QString, page);
        SecureQSettings settings("AmneziaVPN-ThemePages-" + QUuid::createUuid().toString(QUuid::WithoutBraces), page,
                                 nullptr, false);
        QScopedPointer<TestCoreController> core;
        QQmlApplicationEngine engine;
        engine.addImportPath("qrc:/ui/qml/Modules");
        auto connection = QSharedPointer<VpnConnection>::create(nullptr, nullptr);
        core.reset(new TestCoreController(connection, &settings, &engine));
        QQuickWindow window;
        const bool preview = QFileInfo(page).isAbsolute();
        const QUrl url = preview ? QUrl::fromLocalFile(page) : QUrl("qrc:/ui/qml/Pages2/" + page);
        QQmlComponent component(&engine, url);
        QScopedPointer<QObject> object(component.create());
        QVERIFY2(object, qPrintable(component.errorString()));
        auto *item = qobject_cast<QQuickItem *>(object.data());
        QVERIFY(item);
        const QSize size = preview ? QSize(qCeil(item->implicitWidth()), qCeil(item->implicitHeight())) : QSize(380, 680);
        window.resize(size);
        item->setParentItem(window.contentItem());
        item->setSize(size);
        auto *controller = core->findChild<ThemeController *>();
        QVERIFY(controller);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        if (preview) {
            QVERIFY(QMetaObject::invokeMethod(object.data(), "preparePreview"));
            auto *hoverTarget = object->property("hoverTarget").value<QQuickItem *>();
            QVERIFY(hoverTarget);
            const QPointF position =
                    hoverTarget->mapToScene(QPointF(hoverTarget->width() / 2, hoverTarget->height() / 2));
            QTest::mouseMove(&window, position.toPoint());
            QTRY_VERIFY(hoverTarget->property("hovered").toBool());
        }
        for (const auto mode : { ThemeController::Light, ThemeController::Dark }) {
            controller->setThemeMode(mode);
            window.update();
            QTest::qWait(300);
            const QImage image = window.grabWindow();
            QVERIFY(!image.isNull());
            const QString captureDirectory = qEnvironmentVariable("AMNEZIA_THEME_CAPTURE_DIR");
            if (!captureDirectory.isEmpty()) {
                QDir directory(captureDirectory);
                QVERIFY(directory.mkpath("."));
                const QString name =
                        QFileInfo(page).fileName() + (mode == ThemeController::Light ? "-light.png" : "-dark.png");
                QVERIFY(image.save(directory.filePath(name)));
                if (preview) {
                    item->setHeight(item->implicitHeight());
                    item->setY(-qMax(0.0, item->height() - window.height()));
                    window.update();
                    QTest::qWait(100);
                    QVERIFY(window.grabWindow().save(directory.filePath("bottom-" + name)));
                    item->setY(0);
                }
            }
        }
        settings.clearSettings();
    }
#endif

    void darkIconPreservesShape()
    {
        const QImage original(":/images/tray/active.png");
        const QImage dark(":/images/tray/default-dark.png");
        QVERIFY(!original.isNull());
        QVERIFY(!dark.isNull());
        QCOMPARE(dark.size(), original.size());
        for (int y = 0; y < original.height(); ++y) {
            for (int x = 0; x < original.width(); ++x) {
                const QColor pixel = dark.pixelColor(x, y);
                QCOMPARE(pixel.alpha(), original.pixelColor(x, y).alpha());
                QCOMPARE(pixel.red(), 0);
                QCOMPARE(pixel.green(), 0);
                QCOMPARE(pixel.blue(), 0);
            }
        }
    }

    void controllerTracksListener()
    {
        ThemeController controller;
        auto *listener = controller.findChild<SystemThemeListener *>();
        QVERIFY(listener);
        QSignalSpy applicationChanges(&controller, &ThemeController::applicationColorSchemeChanged);
        QSignalSpy systemChanges(&controller, &ThemeController::systemColorSchemeChanged);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Dark);
        listener->systemColorSchemeChanged(Qt::ColorScheme::Light);
        applicationChanges.clear();
        systemChanges.clear();
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Dark);
        listener->systemColorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(applicationChanges.count(), 0);
        QCOMPARE(systemChanges.count(), 0);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(controller.applicationColorScheme(), Qt::ColorScheme::Light);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Light);
        QCOMPARE(applicationChanges.count(), 1);
        QCOMPARE(systemChanges.count(), 0);
        listener->systemColorSchemeChanged(Qt::ColorScheme::Unknown);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Unknown);
        QCOMPARE(systemChanges.count(), 1);
    }

    void initialApplicationTheme()
    {
        ThemeController controller;
#ifdef Q_OS_WINDOWS
        auto *listener = controller.findChild<WindowsThemeListener *>();
        QVERIFY(listener);
        QCOMPARE(controller.applicationColorScheme(), listener->applicationColorScheme());
        const auto expected = listener->colorScheme() == Qt::ColorScheme::Unknown ? controller.applicationColorScheme()
                                                                                  : listener->colorScheme();
        QCOMPARE(controller.systemColorScheme(), expected);
#else
        QCOMPARE(controller.applicationColorScheme(), QGuiApplication::styleHints()->colorScheme());
        QCOMPARE(controller.systemColorScheme(), controller.applicationColorScheme());
#endif
    }

    void applicationThemeChanges()
    {
        ThemeController controller;
        QSignalSpy changes(&controller, &ThemeController::applicationColorSchemeChanged);
#ifdef Q_OS_WINDOWS
        auto *source = controller.findChild<WindowsThemeListener *>();
#else
        auto *source = QGuiApplication::styleHints();
#endif
        const auto first = controller.applicationColorScheme() == Qt::ColorScheme::Light ? Qt::ColorScheme::Dark
                                                                                         : Qt::ColorScheme::Light;
#ifdef Q_OS_WINDOWS
        source->applicationColorSchemeChanged(first);
#else
        source->colorSchemeChanged(first);
#endif
        QCOMPARE(controller.applicationColorScheme(), first);
        QCOMPARE(changes.count(), 1);
#ifdef Q_OS_WINDOWS
        source->applicationColorSchemeChanged(first);
#else
        source->colorSchemeChanged(first);
#endif
        QCOMPARE(changes.count(), 1);
#ifdef Q_OS_WINDOWS
        source->applicationColorSchemeChanged(Qt::ColorScheme::Unknown);
#else
        source->colorSchemeChanged(Qt::ColorScheme::Unknown);
#endif
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

        listener->colorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Dark);
        changes.clear();
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Light);
        QGuiApplication::styleHints()->colorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(controller.applicationColorScheme(), Qt::ColorScheme::Light);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Dark);
        QCOMPARE(changes.count(), 0);
        listener->colorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(changes.count(), 0);
        listener->colorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Light);
        QCOMPARE(changes.count(), 1);
        listener->colorSchemeChanged(Qt::ColorScheme::Unknown);
        listener->applicationColorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(controller.systemColorScheme(), Qt::ColorScheme::Dark);
        QCOMPARE(changes.count(), 2);
    }

    void unchangedWindowsSettings()
    {
        WindowsThemeListener listener;
        QSignalSpy changes(&listener, &WindowsThemeListener::colorSchemeChanged);
        QSignalSpy applicationChanges(&listener, &WindowsThemeListener::applicationColorSchemeChanged);
        MSG message { };
        message.message = WM_SETTINGCHANGE;
        QVERIFY(!listener.nativeEventFilter("windows_generic_MSG", &message, nullptr));
        QCoreApplication::processEvents();
        QCOMPARE(changes.count(), 0);
        QCOMPARE(applicationChanges.count(), 0);
    }
#endif

#if !defined(Q_OS_MAC) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
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
        const auto initialIcon = tray->icon().cacheKey();
        handler.setSystemColorScheme(Qt::ColorScheme::Light);
        const auto lightIcon = tray->icon().cacheKey();
        QVERIFY(lightIcon != initialIcon);
        const QString lightIconName = state == Vpn::Connected ? "default.png"
                : state == Vpn::Error                         ? "error.png"
                                                              : "default-dark.png";
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(),
                 QPixmap(":/images/tray/" + lightIconName).scaled(128, 128).toImage());
        handler.setSystemColorScheme(Qt::ColorScheme::Light);
        QCOMPARE(tray->icon().cacheKey(), lightIcon);
        handler.setSystemColorScheme(Qt::ColorScheme::Dark);
        const QString darkIconName = state == Vpn::Connected ? "default.png"
                : state == Vpn::Error                        ? "error.png"
                                                             : "active.png";
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(),
                 QPixmap(":/images/tray/" + darkIconName).scaled(128, 128).toImage());
        handler.setSystemColorScheme(Qt::ColorScheme::Unknown);
        const QString unknownIconName = state == Vpn::Connected ? "active.png"
                : state == Vpn::Error                           ? "error.png"
                                                                : "default.png";
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(),
                 QPixmap(":/images/tray/" + unknownIconName).scaled(128, 128).toImage());
        const auto actions = tray->contextMenu()->actions();
        QCOMPARE(actions.at(2)->isEnabled(), connectEnabled);
        QCOMPARE(actions.at(3)->isEnabled(), disconnectEnabled);
        QCOMPARE(notifications.count(), 0);
    }

    void themeControllerUpdatesTray()
    {
        QObject owner;
        SilentTrayHandler handler(&owner);
        ThemeController controller;
        auto *listener = controller.findChild<SystemThemeListener *>();
        QVERIFY(listener);
        connect(&controller, &ThemeController::systemColorSchemeChanged, &handler,
                &SystemTrayNotificationHandler::setSystemColorScheme);
        handler.setSystemColorScheme(controller.systemColorScheme());
        auto *tray = owner.findChild<QSystemTrayIcon *>();
        QVERIFY(tray);
        listener->systemColorSchemeChanged(Qt::ColorScheme::Dark);
        listener->systemColorSchemeChanged(Qt::ColorScheme::Light);
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(),
                 QPixmap(":/images/tray/default-dark.png").scaled(128, 128).toImage());
        handler.setConnectionState(Vpn::Connected);
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(),
                 QPixmap(":/images/tray/default.png").scaled(128, 128).toImage());
        listener->systemColorSchemeChanged(Qt::ColorScheme::Dark);
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(),
                 QPixmap(":/images/tray/default.png").scaled(128, 128).toImage());
        handler.setConnectionState(Vpn::Disconnected);
        QCOMPARE(tray->icon().pixmap(QSize(128, 128), 1.0).toImage(),
                 QPixmap(":/images/tray/active.png").scaled(128, 128).toImage());
    }
#endif
};

int main(int argc, char *argv[])
{
#ifdef Q_OS_WINDOWS
    SecureQSettings settings("AmneziaVPN-ThemeApplication-" + QUuid::createUuid().toString(QUuid::WithoutBraces),
                             "theme", nullptr, false);
    AmneziaApplication application(argc, argv, &settings);
    application.registerTypes();
    application.loadFonts();
#else
    AMNEZIA_BASE_CLASS application(argc, argv);
    QQuickStyle::setStyle("Basic");
    qmlRegisterUncreatableType<ThemeController>("ThemeMode", 1, 0, "ThemeMode", "Use ThemeController");
#endif
    TestThemeController test;
    const int result = QTest::qExec(&test, argc, argv);
#ifdef Q_OS_WINDOWS
    settings.clearSettings();
#endif
    return result;
}

#include "testThemeController.moc"
