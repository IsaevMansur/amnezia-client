#include "themeController.h"

#include "platforms/systemThemeListener.h"

ThemeController::ThemeController(QObject *parent) : QObject(parent)
{
    auto *listener = new SystemThemeListener(this);
    m_applicationColorScheme = listener->applicationColorScheme();
    m_systemColorScheme = listener->systemColorScheme();
    connect(listener, &SystemThemeListener::applicationColorSchemeChanged, this,
            &ThemeController::updateApplicationColorScheme);
    connect(listener, &SystemThemeListener::systemColorSchemeChanged, this, &ThemeController::updateSystemColorScheme);
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
}

void ThemeController::updateSystemColorScheme(Qt::ColorScheme colorScheme)
{
    if (m_systemColorScheme == colorScheme) {
        return;
    }
    m_systemColorScheme = colorScheme;
    emit systemColorSchemeChanged(colorScheme);
}
