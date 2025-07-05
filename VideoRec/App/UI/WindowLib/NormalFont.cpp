#include "NormalFont.h"

NormalFont::NormalFont()
{
    // Initialize NONCLIENTMETRICS structure
    NONCLIENTMETRICSW non_client_metrics = {};
    non_client_metrics.cbSize = sizeof(non_client_metrics);

    // Obtain non-client metrics
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(non_client_metrics), &non_client_metrics, 0);

    // Create the new font
    _normal_font = CreateFontIndirectW(&non_client_metrics.lfMessageFont);
}

NormalFont::~NormalFont()
{
    DeleteObject((HGDIOBJ)_normal_font);
}

void NormalFont::SetFont(HWND hwnd)
{
    // Set the new font
    SendMessageW(hwnd, WM_SETFONT, (WPARAM)_normal_font, NULL);
}

void NormalFont::SetFont(WndBase *wnd)
{
    // Set the new font
    wnd->SendMsg(WM_SETFONT, (WPARAM)_normal_font, NULL);
}
