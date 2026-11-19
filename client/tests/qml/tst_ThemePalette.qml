import QtQuick
import QtTest
import Style 1.0

TestCase {
    name: "ThemePalette"

    Rectangle {
        id: surface
        color: AmneziaStyle.color.bgSurface
    }

    function luminance(color) {
        const channels = [color.r, color.g, color.b].map(function(channel) {
            return channel <= 0.04045 ? channel / 12.92 : Math.pow((channel + 0.055) / 1.055, 2.4)
        })
        return channels[0] * 0.2126 + channels[1] * 0.7152 + channels[2] * 0.0722
    }

    function contrast(first, second) {
        const a = luminance(first)
        const b = luminance(second)
        return (Math.max(a, b) + 0.05) / (Math.min(a, b) + 0.05)
    }

    function cleanup() {
        AmneziaStyle.lightTheme = false
    }

    function test_lightContrast_data() {
        const rows = []
        const text = ["textPrimary", "textSecondary", "textTertiary", "textQuaternary", "textAccent",
                      "statusDestructive", "statusWarning", "statusSuccess", "promoRecommendation", "promoUpdate"]
        const backgrounds = ["bgBase", "bgSurface", "bgSurfaceAlt", "bgSheet", "bgField"]
        text.forEach(function(foreground) {
            backgrounds.forEach(function(background) {
                rows.push({ tag: foreground + "/" + background, foreground: foreground, background: background, minimum: 4.5 })
            })
        })
        const invertedBackgrounds = ["bgAction", "bgActionHover", "bgActionPress", "bgSurfaceInverse", "bgSurfaceInverseAlt"]
        invertedBackgrounds.forEach(function(background) {
            rows.push({ tag: "textInverted/" + background, foreground: "textInverted", background: background, minimum: 4.5 })
        })
        const indicators = ["iconDefault", "iconSecondary", "iconTertiary", "borderField", "borderFocus", "accentLight"]
        indicators.forEach(function(foreground) {
            rows.push({ tag: foreground + "/bgField", foreground: foreground, background: "bgField", minimum: 3 })
        })
        rows.push({ tag: "selection", foreground: "textSelection", background: "accentDeep", minimum: 4.5 })
        rows.push({ tag: "snackbarLink", foreground: "textInfoInverse", background: "bgSurfaceInverse", minimum: 4.5 })
        rows.push({ tag: "pressedDestructive", foreground: "statusDestructive", background: "bgPanel", minimum: 4.5 })
        return rows
    }

    function test_lightContrast(data) {
        AmneziaStyle.lightTheme = true
        const ratio = contrast(AmneziaStyle.color[data.foreground], AmneziaStyle.color[data.background])
        verify(ratio >= data.minimum, data.tag + ": " + ratio.toFixed(2))
    }

    function test_livePaletteBindings() {
        AmneziaStyle.lightTheme = false
        compare(surface.color, "#18181b")
        compare(AmneziaStyle.color.textPrimary, "#fafafa")
        AmneziaStyle.lightTheme = true
        compare(surface.color, "#ffffff")
        verify(contrast(AmneziaStyle.color.textPrimary, surface.color) >= 4.5)
        AmneziaStyle.lightTheme = false
        compare(surface.color, "#18181b")
        compare(AmneziaStyle.color.textPrimary, "#fafafa")
    }

    function test_staticResources() {
        AmneziaStyle.lightTheme = false
        const white = AmneziaStyle.color.bgStaticWhite
        const textWhite = AmneziaStyle.color.textStaticWhite
        const transparent = AmneziaStyle.color.transparent
        AmneziaStyle.lightTheme = true
        compare(AmneziaStyle.color.bgStaticWhite, white)
        compare(AmneziaStyle.color.textStaticWhite, textWhite)
        compare(AmneziaStyle.color.transparent, transparent)
    }

    function test_selectionContrast_data() {
        return [{ tag: "dark", light: false }, { tag: "light", light: true }]
    }

    function test_selectionContrast(data) {
        AmneziaStyle.lightTheme = data.light
        verify(contrast(AmneziaStyle.color.textSelection, AmneziaStyle.color.accentDeep) >= 4.5)
        verify(contrast(AmneziaStyle.color.textInfoInverse, AmneziaStyle.color.bgSurfaceInverse) >= 4.5)
    }
}
