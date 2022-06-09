// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_H_
#define ELECTRON_SHELL_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "components/keyed_service/core/keyed_service.h"
#include "shell/browser/electron_browser_context.h"
#include "third_party/blink/public/common/bluetooth/web_bluetooth_device_id.h"
#include "third_party/blink/public/mojom/bluetooth/web_bluetooth.mojom-forward.h"

namespace base {
class Value;
}  // namespace base

namespace content {
class BrowserContext;
}  // namespace content

namespace device {
class BluetoothDevice;
class BluetoothUUID;
}  // namespace device

namespace url {
class Origin;
}  // namespace url

namespace electron {

// Manages the permissions for Web Bluetooth device objects. A Web Bluetooth
// permission object consists of its WebBluetoothDeviceId and set of Bluetooth
// service UUIDs. The WebBluetoothDeviceId is generated randomly by this class
// and is unique for a given Bluetooth device address and origin pair, so this
// class stores this mapping and provides utility methods to convert between
// the WebBluetoothDeviceId and Bluetooth device address.
class BluetoothChooserContext : public KeyedService {
 public:
  explicit BluetoothChooserContext(ElectronBrowserContext* context);
  ~BluetoothChooserContext() override;

  // Set class as move-only.
  BluetoothChooserContext(const BluetoothChooserContext&) = delete;
  BluetoothChooserContext& operator=(const BluetoothChooserContext&) = delete;

  // Helper methods for converting between a WebBluetoothDeviceId and a
  // Bluetooth device address string for a given origin pair.
  blink::WebBluetoothDeviceId GetWebBluetoothDeviceId(
      const url::Origin& origin,
      const std::string& device_address);
  std::string GetDeviceAddress(const url::Origin& origin,
                               const blink::WebBluetoothDeviceId& device_id);

  // Bluetooth scanning specific interface for generating WebBluetoothDeviceIds
  // for scanned devices.
  blink::WebBluetoothDeviceId AddScannedDevice(
      const url::Origin& origin,
      const std::string& device_address);

  // Bluetooth-specific interface for granting and checking permissions.
  blink::WebBluetoothDeviceId GrantServiceAccessPermission(
      const url::Origin& origin,
      const device::BluetoothDevice* device,
      const blink::mojom::WebBluetoothRequestDeviceOptions* options);
  bool HasDevicePermission(const url::Origin& origin,
                           const blink::WebBluetoothDeviceId& device_id);
  void RevokeDevicePermissionWebInitiated(
      const url::Origin& origin,
      const blink::WebBluetoothDeviceId& device_id);
  bool IsAllowedToAccessAtLeastOneService(
      const url::Origin& origin,
      const blink::WebBluetoothDeviceId& device_id);
  bool IsAllowedToAccessService(const url::Origin& origin,
                                const blink::WebBluetoothDeviceId& device_id,
                                const device::BluetoothUUID& service);
  bool IsAllowedToAccessManufacturerData(
      const url::Origin& origin,
      const blink::WebBluetoothDeviceId& device_id,
      uint16_t manufacturer_code);

  static blink::WebBluetoothDeviceId GetObjectDeviceId(
      const base::Value& object);

  std::u16string GetObjectDisplayName(const base::Value& object);

  std::vector<std::unique_ptr<base::Value>> GetGrantedObjects(
      const url::Origin& origin);

 private:
  base::Value FindDeviceObject(const url::Origin& origin,
                               const blink::WebBluetoothDeviceId& device_id);

  // This map records the generated Web Bluetooth IDs for devices discovered via
  // the Scanning API. Each requesting/embedding origin pair has its own version
  // of this map so that IDs cannot be correlated between cross-origin sites.
  using DeviceAddressToIdMap =
      std::map<std::string, blink::WebBluetoothDeviceId>;
  std::map<url::Origin, DeviceAddressToIdMap> scanned_devices_;

  ElectronBrowserContext* browser_context_;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_H_
