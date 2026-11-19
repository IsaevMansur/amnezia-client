#ifndef THEMECONTROLLER_H
#define THEMECONTROLLER_H

#include <QObject>

class SecureAppSettingsRepository;

class ThemeController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt::ColorScheme applicationColorScheme READ applicationColorScheme NOTIFY applicationColorSchemeChanged)
    Q_PROPERTY(Qt::ColorScheme systemColorScheme READ systemColorScheme NOTIFY systemColorSchemeChanged)
    Q_PROPERTY(ThemeMode themeMode READ themeMode WRITE setThemeMode NOTIFY themeModeChanged)
    Q_PROPERTY(Qt::ColorScheme effectiveColorScheme READ effectiveColorScheme NOTIFY effectiveColorSchemeChanged)
    Q_PROPERTY(Qt::ColorScheme systemApplicationColorScheme READ applicationColorScheme NOTIFY applicationColorSchemeChanged)

public:
    enum ThemeMode {
        Dark,
        Light,
        System
    };
    Q_ENUM(ThemeMode)

    explicit ThemeController(QObject *parent = nullptr, SecureAppSettingsRepository *settings = nullptr);

    Qt::ColorScheme applicationColorScheme() const;
    Qt::ColorScheme systemColorScheme() const;
    ThemeMode themeMode() const;
    Qt::ColorScheme effectiveColorScheme() const;
    Q_INVOKABLE void setThemeMode(ThemeMode mode);

signals:
    void themeModeChanged();
    void effectiveColorSchemeChanged(Qt::ColorScheme colorScheme);
    void applicationColorSchemeChanged(Qt::ColorScheme colorScheme);
    void systemColorSchemeChanged(Qt::ColorScheme colorScheme);

private:
    void updateEffectiveColorScheme();
    void restoreThemeMode();
    void updateApplicationColorScheme(Qt::ColorScheme colorScheme);
    void updateSystemColorScheme(Qt::ColorScheme colorScheme);

    Qt::ColorScheme m_applicationColorScheme = Qt::ColorScheme::Unknown;
    Qt::ColorScheme m_systemColorScheme = Qt::ColorScheme::Unknown;
    ThemeMode m_themeMode = Dark;
    Qt::ColorScheme m_effectiveColorScheme = Qt::ColorScheme::Dark;
    SecureAppSettingsRepository *m_settings;
};

#endif
