#include "themeController.h"

#include <QGuiApplication>
#include <QStyleHints>

#ifdef Q_OS_WINDOWS
    #include "platforms/windows/windowsThemeListener.h"
#endif

ThemeController::ThemeController(QObject *parent) : QObject(parent)
{
    auto *styleHints = QGuiApplication::styleHints();
    m_applicationColorScheme = styleHints->colorScheme();
    m_systemColorScheme = m_applicationColorScheme;
    connect(styleHints, &QStyleHints::colorSchemeChanged, this, &ThemeController::updateApplicationColorScheme);

#ifdef Q_OS_WINDOWS
    auto *listener = new WindowsThemeListener(this);
    m_windowsColorScheme = listener->colorScheme();
    if (m_windowsColorScheme != Qt::ColorScheme::Unknown) {
        m_systemColorScheme = m_windowsColorScheme;
    }
    connect(listener, &WindowsThemeListener::colorSchemeChanged, this, [this](Qt::ColorScheme colorScheme) {
        m_windowsColorScheme = colorScheme;
        updateSystemColorScheme(colorScheme == Qt::ColorScheme::Unknown ? m_applicationColorScheme : colorScheme);
    });
#endif
}

Qt::ColorScheme ThemeController::applicationColorScheme() const
{
    return m_applicationColorScheme;
}

Qt::ColorScheme ThemeController::systemColorScheme() const
{
    return m_systemColorScheme;
}

void ThemeController::updateApplicationColorScheme(Qt::ColorScheme colorScheme)
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

void ThemeController::updateSystemColorScheme(Qt::ColorScheme colorScheme)
{
    if (m_systemColorScheme == colorScheme) {
        return;
    }
    m_systemColorScheme = colorScheme;
    emit systemColorSchemeChanged(colorScheme);
}
