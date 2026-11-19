pragma Singleton

import QtQuick

QtObject {
    id: root

    property bool lightTheme: typeof ThemeController !== "undefined" && ThemeController !== null
                             && ThemeController.effectiveColorScheme === Qt.Light

    property QtObject color: QtObject {
        readonly property color transparent: 'transparent'

        readonly property color textPrimary: root.lightTheme ? '#18181B' : '#FAFAFA'
        readonly property color textSecondary: root.lightTheme ? '#27272A' : '#E4E4E7'
        readonly property color textTertiary: root.lightTheme ? '#52525B' : '#A1A1AA'
        readonly property color textQuaternary: root.lightTheme ? '#62626C' : '#86868F'
        readonly property color textInverted: root.lightTheme ? '#FAFAFA' : '#09090B'
        readonly property color textAccent: root.lightTheme ? '#9A3412' : '#FDBA74'
        readonly property color textInfoInverse: root.lightTheme ? '#38BDF8' : '#0369A1'
        readonly property color textSelection: '#FAFAFA'
        readonly property color textStaticWhite: '#FFFFFF'

        readonly property color bgBase: root.lightTheme ? '#FAFAFA' : '#101012'
        readonly property color bgSurface: root.lightTheme ? '#FFFFFF' : '#18181B'
        readonly property color bgSurfaceAlt: root.lightTheme ? '#F4F4F5' : '#1F1F23'
        readonly property color bgSheet: root.lightTheme ? '#FFFFFF' : '#313136'
        readonly property color bgPanel: root.lightTheme ? '#E4E4E7' : '#3F3F46'
        readonly property color bgPanelAlt: root.lightTheme ? '#D4D4D8' : '#52525B'
        readonly property color bgOverlay: root.lightTheme ? '#6609090B' : '#8009090B'
        readonly property color bgStateLayer: root.lightTheme ? '#1409090B' : '#14FAFAFA'
        readonly property color bgStateLayerPressed: root.lightTheme ? '#1A09090B' : '#1AFAFAFA'
        readonly property color bgStateLayerAccent: root.lightTheme ? '#149A3412' : '#14FDBA74'
        readonly property color bgSurfaceInverse: root.lightTheme ? '#27272A' : '#E4E4E7'
        readonly property color bgSurfaceInverseAlt: root.lightTheme ? '#3F3F46' : '#D4D4D8'
        readonly property color bgControlTrack: root.lightTheme ? '#E4E4E7' : '#1F1F23'
        readonly property color bgField: root.lightTheme ? '#F4F4F5' : '#1F1F23'
        readonly property color bgSelected: root.lightTheme ? '#E4E4E7' : '#1F1F23'
        readonly property color bgAccentSubtle: root.lightTheme ? '#1AD97706' : '#4DD97706'
        readonly property color bgStaticWhite: '#FFFFFF'

        readonly property color bgAction: root.lightTheme ? '#27272A' : '#E4E4E7'
        readonly property color bgActionHover: root.lightTheme ? '#3F3F46' : '#D4D4D8'
        readonly property color bgActionPress: root.lightTheme ? '#52525B' : '#A1A1AA'

        readonly property color borderFaint: root.lightTheme ? '#E4E4E7' : '#313136'
        readonly property color borderSoft: root.lightTheme ? '#D4D4D8' : '#3F3F46'
        readonly property color borderMedium: root.lightTheme ? '#A1A1AA' : '#52525B'
        readonly property color borderBold: root.lightTheme ? '#71717A' : '#A1A1AA'
        readonly property color borderEmphasis: root.lightTheme ? '#27272A' : '#E4E4E7'
        readonly property color borderAccent: root.lightTheme ? '#9A3412' : '#FDBA74'
        readonly property color borderField: root.lightTheme ? '#71717A' : '#52525B'
        readonly property color borderFocus: root.lightTheme ? '#27272A' : '#E4E4E7'

        readonly property color accentLight: root.lightTheme ? '#9A3412' : '#FDBA74'
        readonly property color accentDefault: root.lightTheme ? '#C2410C' : '#F59E0B'
        readonly property color accentDark: root.lightTheme ? '#9A3412' : '#D97706'
        readonly property color accentDeep: root.lightTheme ? '#7C2D12' : '#78350F'

        readonly property color statusDestructive: root.lightTheme ? '#B91C1C' : '#F87171'
        readonly property color statusWarning: root.lightTheme ? '#854D0E' : '#EAB308'
        readonly property color statusSuccess: root.lightTheme ? '#166534' : '#4ADE80'

        readonly property color promoRecommendation: root.lightTheme ? '#6D28D9' : '#A78BFA'
        readonly property color promoUpdate: root.lightTheme ? '#0369A1' : '#38BDF8'
        readonly property color promoOverlay: '#148B5CF6'
        readonly property color promoPink: root.lightTheme ? '#BE185D' : '#DB2777'
        readonly property color promoPinkDeep: root.lightTheme ? '#831843' : '#9D174D'

        readonly property color iconDefault: root.lightTheme ? '#27272A' : '#E4E4E7'
        readonly property color iconSecondary: root.lightTheme ? '#52525B' : '#A1A1AA'
        readonly property color iconTertiary: root.lightTheme ? '#71717A' : '#3F3F46'
        readonly property color iconInverted: root.lightTheme ? '#FAFAFA' : '#09090B'

        readonly property color shadowLayer1: root.lightTheme ? '#14000000' : '#26000000'
        readonly property color shadowLayer2: root.lightTheme ? '#26000000' : '#4D000000'
        readonly property color shadowGlow: root.lightTheme ? '#F59E0B' : '#FBB751'
    }
}
