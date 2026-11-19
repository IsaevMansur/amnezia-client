#include "systemThemeListener.h"

#include <QGuiApplication>
#include <QStyleHints>

#ifdef Q_OS_WINDOWS
    #include "platforms/windows/windowsThemeListener.h"
#endif

SystemThemeListener::SystemThemeListener(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_WINDOWS
    auto *listener = new WindowsThemeListener(this);
    m_applicationColorScheme = listener->applicationColorScheme();
    m_windowsColorScheme = listener->colorScheme();
    m_systemColorScheme =
            m_windowsColorScheme == Qt::ColorScheme::Unknown ? m_applicationColorScheme : m_windowsColorScheme;
    connect(listener, &WindowsThemeListener::applicationColorSchemeChanged, this,
            &SystemThemeListener::updateApplicationColorScheme);
    connect(listener, &WindowsThemeListener::colorSchemeChanged, this, [this](Qt::ColorScheme colorScheme) {
        m_windowsColorScheme = colorScheme;
        updateSystemColorScheme(colorScheme == Qt::ColorScheme::Unknown ? m_applicationColorScheme : colorScheme);
    });
#else
    auto *styleHints = QGuiApplication::styleHints();
    m_applicationColorScheme = styleHints->colorScheme();
    m_systemColorScheme = m_applicationColorScheme;
    connect(styleHints, &QStyleHints::colorSchemeChanged, this, &SystemThemeListener::updateApplicationColorScheme);
#endif
}

Qt::ColorScheme SystemThemeListener::applicationColorScheme() const
{
    return m_applicationColorScheme;
}

Qt::ColorScheme SystemThemeListener::systemColorScheme() const
{
    return m_systemColorScheme;
}

void SystemThemeListener::updateApplicationColorScheme(Qt::ColorScheme colorScheme)
{
    if (m_applicationColorScheme == colorScheme) {
        return;
    }
    m_applicationColorScheme = colorScheme;
    emit applicationColorSchemeChanged(colorScheme);
#ifdef Q_OS_WINDOWS
    if (m_windowsColorScheme != Qt::ColorScheme::Unknown) {
        return;
    }
#endif
    updateSystemColorScheme(colorScheme);
}

void SystemThemeListener::updateSystemColorScheme(Qt::ColorScheme colorScheme)
{
    if (m_systemColorScheme == colorScheme) {
        return;
    }
    m_systemColorScheme = colorScheme;
    emit systemColorSchemeChanged(colorScheme);
}
