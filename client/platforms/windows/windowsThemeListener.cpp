#include "windowsThemeListener.h"

#include <QCoreApplication>
#include <QSettings>
#include <QTimer>
#include <qt_windows.h>

WindowsThemeListener::WindowsThemeListener(QObject *parent) : QObject(parent)
{
    refreshColorScheme();
    QCoreApplication::instance()->installNativeEventFilter(this);
}

WindowsThemeListener::~WindowsThemeListener()
{
    QCoreApplication::instance()->removeNativeEventFilter(this);
}

Qt::ColorScheme WindowsThemeListener::colorScheme() const
{
    return m_colorScheme;
}

Qt::ColorScheme WindowsThemeListener::applicationColorScheme() const
{
    return m_applicationColorScheme;
}

bool WindowsThemeListener::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        const auto *nativeMessage = static_cast<const MSG *>(message);
        if (nativeMessage->message == WM_SETTINGCHANGE || nativeMessage->message == WM_THEMECHANGED) {
            QTimer::singleShot(0, this, &WindowsThemeListener::refreshColorScheme);
        }
    }
    return false;
}

void WindowsThemeListener::refreshColorScheme()
{
    const QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                             QSettings::NativeFormat);
    const auto readColorScheme = [&settings](const QString &key) {
        bool valid = false;
        const int lightTheme = settings.value(key).toInt(&valid);
        return valid && (lightTheme == 0 || lightTheme == 1)
                ? (lightTheme == 0 ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light)
                : Qt::ColorScheme::Unknown;
    };
    const auto applicationColorScheme = readColorScheme("AppsUseLightTheme");
    const auto colorScheme = readColorScheme("SystemUsesLightTheme");
    if (m_applicationColorScheme != applicationColorScheme) {
        m_applicationColorScheme = applicationColorScheme;
        emit applicationColorSchemeChanged(applicationColorScheme);
    }
    if (m_colorScheme != colorScheme) {
        m_colorScheme = colorScheme;
        emit colorSchemeChanged(colorScheme);
    }
}
