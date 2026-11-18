// vstbridge: opt-in scaling of editor sizes by Wine's system DPI.
//
// Some plugins (iZotope RX) draw their GUI scaled by Wine's DPI but report and
// accept sizes in unscaled pixels through `IPlugView::getSize()`,
// `IPlugView::onSize()` and `IPlugFrame::resizeView()`, so the host window ends
// up smaller than the content. When the `VSTBRIDGE_EDITOR_DPI_SIZE=1`
// environment variable is set for the plugin's Wine host (the `environment`
// option in `vstbridge.toml`), sizes the host sees are multiplied by
// `GetDpiForSystem() / 96`. The plugin's `onSize()` gets that scaled size as is, since
// these plugins size their own child window from it in physical pixels.

#pragma once

#include <windows.h>

#include <cmath>
#include <cstdlib>
#include <cstring>

#include <pluginterfaces/gui/iplugview.h>

inline double editor_dpi_size_factor() noexcept {
    static const double factor = [] {
        const char* enabled = std::getenv("VSTBRIDGE_EDITOR_DPI_SIZE");
        if (!enabled || std::strcmp(enabled, "1") != 0) {
            return 1.0;
        }

        const UINT dpi = GetDpiForSystem();
        return dpi > 0 ? static_cast<double>(dpi) / 96.0 : 1.0;
    }();

    return factor;
}

/**
 * Convert a size reported by the plugin to what we show to the host.
 */
inline Steinberg::ViewRect editor_size_to_host(
    const Steinberg::ViewRect& rect) noexcept {
    const double f = editor_dpi_size_factor();
    return Steinberg::ViewRect(std::lround(rect.left * f),
                               std::lround(rect.top * f),
                               std::lround(rect.right * f),
                               std::lround(rect.bottom * f));
}
