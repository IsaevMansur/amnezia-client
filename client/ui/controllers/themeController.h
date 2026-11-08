#ifndef THEMECONTROLLER_H
#define THEMECONTROLLER_H

#include <QObject>

class ThemeController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt::ColorScheme applicationColorScheme READ applicationColorScheme NOTIFY applicationColorSchemeChanged)
    Q_PROPERTY(Qt::ColorScheme systemColorScheme READ systemColorScheme NOTIFY systemColorSchemeChanged)

public:
    explicit ThemeController(QObject *parent = nullptr);

    Qt::ColorScheme applicationColorScheme() const;
    Qt::ColorScheme systemColorScheme() const;

signals:
    void applicationColorSchemeChanged(Qt::ColorScheme colorScheme);
    void systemColorSchemeChanged(Qt::ColorScheme colorScheme);

private:
    void updateApplicationColorScheme(Qt::ColorScheme colorScheme);
    void updateSystemColorScheme(Qt::ColorScheme colorScheme);

    Qt::ColorScheme m_applicationColorScheme = Qt::ColorScheme::Unknown;
    Qt::ColorScheme m_systemColorScheme = Qt::ColorScheme::Unknown;
#ifdef Q_OS_WINDOWS
    Qt::ColorScheme m_windowsColorScheme = Qt::ColorScheme::Unknown;
#endif
};

#endif
