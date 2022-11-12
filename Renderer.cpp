﻿#include "pch.hpp"
#include "Renderer.hpp"
#include "Blue-Flag's External Menu.hpp"

Renderer::Renderer(const HWND targetHWND, Settings& s)
{
	TargetHWND = targetHWND;
	settings   = shared_ptr <Settings>(&s);
}

void RendererGDI::init(HWND hWnd)
{
}

void RendererGDI::drawText(const wstring& str, int x, int y, D3DCOLOR color) const
{
}

void RendererGDI::drawText(const wstring& str, int x, int y, int w, int h, D3DCOLOR color, DWORD flags) const
{
}

void RendererGDI::render() const
{
}

RendererGDI::RendererGDI(const HWND overlayHWND, const HWND targetHWND, Settings& s): Renderer(targetHWND, s)
{
	init(overlayHWND);
}

void RendererD3D12::init(HWND hWnd)
{
}

void RendererD3D12::drawText(const wstring& str, int x, int y, D3DCOLOR color) const
{
}

void RendererD3D12::drawText(const wstring& str, int x, int y, int w, int h, D3DCOLOR color, DWORD flags) const
{
}

void RendererD3D12::render() const
{
}

RendererD3D12::RendererD3D12(const HWND overlayHWND, const HWND targetHWND, Settings& s): Renderer(targetHWND, s)
{
	init(overlayHWND);
}

void RendererD3D9::init(const HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &object)))
	{
		exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
	}

	ZeroMemory(&params, sizeof params);

	const auto& settings = getSettings();

	params.BackBufferWidth  = settings->OverlayWidth;
	params.BackBufferHeight = settings->OverlayHeight;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;
	if (SUCCEEDED(object->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, true,D3DMULTISAMPLE_16_SAMPLES, NULL)))
	{
		params.MultiSampleType = D3DMULTISAMPLE_16_SAMPLES;
	}
	else
	{
		params.MultiSampleType = D3DMULTISAMPLE_NONE;
		OutputDebugString(L"MultiSample Not Supported\n");
		MessageBox(nullptr, L"MultiSample Not Supported", OverlayTitle.c_str(), MB_OK);
	}
	params.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow          = hWnd;
	params.Windowed               = true;
	params.EnableAutoDepthStencil = true;
	params.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, nullptr, &device)))
	{
		MessageBox(nullptr, L"Create D3D Device Failed", OverlayTitle.c_str(), MB_RETRYCANCEL);
		KillMenu();
	}
	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
	D3DXCreateFont(device, 50, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, settings->FontName.c_str(), &font);
}

void RendererD3D9::drawText(const wstring& str, const int x, const int y, const D3DCOLOR color) const
{
	RECT rect = { x, y };
	font->DrawTextW(nullptr, str.c_str(), static_cast <int>(str.size()), &rect, DT_NOCLIP, color);
}

void RendererD3D9::drawText(const wstring& str, const int x, const int y, const int w, const int h, const D3DCOLOR color, DWORD flags) const
{
	RECT rect = { x, y, x + w, y + h };
	font->DrawTextW(nullptr, str.c_str(), static_cast <int>(str.size()), &rect, DT_NOCLIP, color);
}

void RendererD3D9::drawBox(const int x, const int y, const int w, const int h, const D3DCOLOR color) const
{
	const D3DRECT rect = { x, y, x + w, y + h };
	device->Clear(1, &rect, D3DCLEAR_TARGET, color, 1.f, 0);
}

void RendererD3D9::drawBoxBorder(const int x, const int y, const int w, const int h, const int borderSize, const D3DCOLOR color, const D3DCOLOR borderColor) const
{
	drawBox(x, y, w, h, borderColor);
	drawBox(x + borderSize, y + borderSize, w - borderSize * 2, h - borderSize * 2, color);
}

void RendererD3D9::render() const
{
	OutputDebugString(L"Renderer Called\n");

	if (device == nullptr)
	{
		return;
	}

	device->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);
	device->BeginScene();

	if (const auto settings = getSettings(); settings->ActiveMenu && (settings->AlwaysShow || targetHWND() == GetForegroundWindow()))
	{
		drawBoxBorder(settings->OverlayWidth / 10, settings->OverlayHeight / 10, 600, 260, 2, D3DCOLOR_ARGB(255, 56, 120, 226),D3DCOLOR_ARGB(255, 1, 1, 1));
		drawText(OverlayTitle, settings->OverlayWidth / 10, settings->OverlayHeight / 10, 300, 25, D3DCOLOR_ARGB(255, 1, 1, 1),DT_CENTER | DT_VCENTER);
	}

	device->EndScene();
	device->PresentEx(nullptr, nullptr, nullptr, nullptr, 0);
}

RendererD3D9::RendererD3D9(const HWND overlayHWND, const HWND targetHWND, Settings& s): Renderer(targetHWND, s)
{
	init(overlayHWND);
}
