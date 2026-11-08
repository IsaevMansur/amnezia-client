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
    bool valid = false;
    const int lightTheme = settings.value("SystemUsesLightTheme").toInt(&valid);
    Qt::ColorScheme colorScheme = Qt::ColorScheme::Unknown;
    if (valid && (lightTheme == 0 || lightTheme == 1)) {
        colorScheme = lightTheme == 0 ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light;
    }
    if (m_colorScheme == colorScheme) {
        return;
    }
    m_colorScheme = colorScheme;
    emit colorSchemeChanged(colorScheme);
}
