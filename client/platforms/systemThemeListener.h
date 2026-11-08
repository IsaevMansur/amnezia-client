#ifndef SYSTEMTHEMELISTENER_H
#define SYSTEMTHEMELISTENER_H

#include <QObject>

class SystemThemeListener : public QObject
{
    Q_OBJECT

public:
    explicit SystemThemeListener(QObject *parent = nullptr);

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
