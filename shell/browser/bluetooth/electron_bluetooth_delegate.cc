// Copyright (c) 2020 Microsoft, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/bluetooth/electron_bluetooth_delegate.h"

#include <memory>
#include <utility>

#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "device/bluetooth/bluetooth_device.h"
#include "device/bluetooth/public/cpp/bluetooth_uuid.h"
#include "shell/browser/api/electron_api_web_contents.h"
#include "shell/browser/bluetooth/bluetooth_chooser_context.h"
#include "shell/browser/bluetooth/bluetooth_chooser_context_factory.h"
#include "shell/browser/lib/bluetooth_chooser.h"
#include "third_party/blink/public/common/bluetooth/web_bluetooth_device_id.h"
#include "third_party/blink/public/mojom/bluetooth/web_bluetooth.mojom.h"

using blink::WebBluetoothDeviceId;
using content::RenderFrameHost;
using content::WebContents;
using device::BluetoothUUID;

namespace electron {

BluetoothChooserContext* GetBluetoothChooserContext(
    content::RenderFrameHost* frame) {
  auto* web_contents = content::WebContents::FromRenderFrameHost(frame);
  auto* browser_context = web_contents->GetBrowserContext();
  return BluetoothChooserContextFactory::GetForBrowserContext(browser_context);
}

ElectronBluetoothDelegate::ElectronBluetoothDelegate() = default;

ElectronBluetoothDelegate::~ElectronBluetoothDelegate() = default;

std::unique_ptr<content::BluetoothChooser>
ElectronBluetoothDelegate::RunBluetoothChooser(
    content::RenderFrameHost* frame,
    const content::BluetoothChooser::EventHandler& event_handler) {
  auto* api_web_contents =
      api::WebContents::From(content::WebContents::FromRenderFrameHost(frame));
  return std::make_unique<BluetoothChooser>(api_web_contents, event_handler);
}

// The following methods are not currently called in Electron.
std::unique_ptr<content::BluetoothScanningPrompt>
ElectronBluetoothDelegate::ShowBluetoothScanningPrompt(
    content::RenderFrameHost* frame,
    const content::BluetoothScanningPrompt::EventHandler& event_handler) {
  // Fired from  watchAdvertisements()???
  // navigator.bluetooth.requestLEScan?
  LOG(INFO) << "IN ElectronBluetoothDelegate::ShowBluetoothScanningPrompt ";
  return nullptr;
}

WebBluetoothDeviceId ElectronBluetoothDelegate::GetWebBluetoothDeviceId(
    RenderFrameHost* frame,
    const std::string& device_address) {
  return GetBluetoothChooserContext(frame)->GetWebBluetoothDeviceId(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device_address);
}

std::string ElectronBluetoothDelegate::GetDeviceAddress(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id) {
  return GetBluetoothChooserContext(frame)->GetDeviceAddress(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device_id);
}

WebBluetoothDeviceId ElectronBluetoothDelegate::AddScannedDevice(
    RenderFrameHost* frame,
    const std::string& device_address) {
  return GetBluetoothChooserContext(frame)->AddScannedDevice(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device_address);
}

WebBluetoothDeviceId ElectronBluetoothDelegate::GrantServiceAccessPermission(
    RenderFrameHost* frame,
    const device::BluetoothDevice* device,
    const blink::mojom::WebBluetoothRequestDeviceOptions* options) {
  return GetBluetoothChooserContext(frame)->GrantServiceAccessPermission(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device, options);
}

bool ElectronBluetoothDelegate::HasDevicePermission(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id) {
  return GetBluetoothChooserContext(frame)->HasDevicePermission(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device_id);
}

void ElectronBluetoothDelegate::RevokeDevicePermissionWebInitiated(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id) {
  GetBluetoothChooserContext(frame)->RevokeDevicePermissionWebInitiated(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device_id);
}

bool ElectronBluetoothDelegate::IsAllowedToAccessService(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id,
    const BluetoothUUID& service) {
  return GetBluetoothChooserContext(frame)->IsAllowedToAccessService(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device_id, service);
}

bool ElectronBluetoothDelegate::IsAllowedToAccessAtLeastOneService(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id) {
  return GetBluetoothChooserContext(frame)->IsAllowedToAccessAtLeastOneService(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device_id);
}

bool ElectronBluetoothDelegate::IsAllowedToAccessManufacturerData(
    RenderFrameHost* frame,
    const WebBluetoothDeviceId& device_id,
    uint16_t manufacturer_code) {
  return GetBluetoothChooserContext(frame)->IsAllowedToAccessManufacturerData(
      frame->GetMainFrame()->GetLastCommittedOrigin(), device_id,
      manufacturer_code);
}

std::vector<blink::mojom::WebBluetoothDevicePtr>
ElectronBluetoothDelegate::GetPermittedDevices(
    content::RenderFrameHost* frame) {
  auto* context = GetBluetoothChooserContext(frame);

  std::vector<std::unique_ptr<base::Value>> devices =
      context->GetGrantedObjects(
          frame->GetMainFrame()->GetLastCommittedOrigin());
  std::vector<blink::mojom::WebBluetoothDevicePtr> permitted_devices;

  for (const auto& device : devices) {
    auto permitted_device = blink::mojom::WebBluetoothDevice::New();
    permitted_device->id = BluetoothChooserContext::GetObjectDeviceId(*device);
    permitted_device->name =
        base::UTF16ToUTF8(context->GetObjectDisplayName(*device));
    permitted_devices.push_back(std::move(permitted_device));
  }

  return permitted_devices;
}

void ElectronBluetoothDelegate::ShowDevicePairPrompt(
    content::RenderFrameHost* frame,
    const std::u16string& device_identifier,
    PairPromptCallback callback,
    PairingKind pairing_kind) {
  // 3684873: [Bluetooth][Win/Linux] Add bluetooth pair confirmation prompt |
  // https://chromium-review.googlesource.com/c/chromium/src/+/3684873 3688827:
  // [Bluetooth][Win/Linux] Add confirm pairing mode support runtime flag |
  // https://chromium-review.googlesource.com/c/chromium/src/+/3688827
  LOG(INFO) << "IN ShowDevicePairConfirmPrompt with device "
            << device_identifier;
  std::move(callback).Run(BluetoothDelegate::PairPromptResult(
      BluetoothDelegate::PairPromptStatus::kCancelled));
}

}  // namespace electron
