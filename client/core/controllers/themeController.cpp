#include "themeController.h"

#include "core/repositories/secureAppSettingsRepository.h"
#include "platforms/systemThemeListener.h"

ThemeController::ThemeController(QObject *parent, SecureAppSettingsRepository *settings)
    : QObject(parent), m_settings(settings)
{
    auto *listener = new SystemThemeListener(this);
    m_applicationColorScheme = listener->applicationColorScheme();
    m_systemColorScheme = listener->systemColorScheme();
    connect(listener, &SystemThemeListener::applicationColorSchemeChanged, this,
            &ThemeController::updateApplicationColorScheme);
    connect(listener, &SystemThemeListener::systemColorSchemeChanged, this, &ThemeController::updateSystemColorScheme);
    if (m_settings) {
        connect(m_settings, &SecureAppSettingsRepository::settingsCleared, this, &ThemeController::restoreThemeMode);
        connect(m_settings, &SecureAppSettingsRepository::themeModeChanged, this, &ThemeController::restoreThemeMode);
    }
    restoreThemeMode();
}

ThemeController::ThemeMode ThemeController::themeMode() const
{
    return m_themeMode;
}

Qt::ColorScheme ThemeController::effectiveColorScheme() const
{
    return m_effectiveColorScheme;
}

void ThemeController::setThemeMode(ThemeMode mode)
{
    if (mode < Dark || mode > System || m_themeMode == mode) {
        return;
    }
    m_themeMode = mode;
    if (m_settings) {
        m_settings->setThemeMode(mode);
    }
    updateEffectiveColorScheme();
    emit themeModeChanged();
}

void ThemeController::restoreThemeMode()
{
    const int storedMode = m_settings ? m_settings->themeMode() : Dark;
    const auto mode = storedMode >= Dark && storedMode <= System ? static_cast<ThemeMode>(storedMode) : Dark;
    const bool changed = m_themeMode != mode;
    m_themeMode = mode;
    updateEffectiveColorScheme();
    if (changed) {
        emit themeModeChanged();
    }
}

void ThemeController::updateEffectiveColorScheme()
{
    const auto colorScheme =
            m_themeMode == Light || (m_themeMode == System && m_applicationColorScheme == Qt::ColorScheme::Light)
            ? Qt::ColorScheme::Light
            : Qt::ColorScheme::Dark;
    if (m_effectiveColorScheme == colorScheme) {
        return;
    }
    m_effectiveColorScheme = colorScheme;
    emit effectiveColorSchemeChanged(colorScheme);
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
    updateEffectiveColorScheme();
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
