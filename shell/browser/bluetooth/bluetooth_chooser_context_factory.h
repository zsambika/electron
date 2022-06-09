// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_FACTORY_H_
#define ELECTRON_SHELL_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_FACTORY_H_

#include "base/no_destructor.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
#include "shell/browser/bluetooth/bluetooth_chooser_context.h"

class Profile;

namespace electron {

class BluetoothChooserContextFactory
    : public BrowserContextKeyedServiceFactory {
 public:
  static BluetoothChooserContext* GetForBrowserContext(
      content::BrowserContext* context);
  static BluetoothChooserContext* GetForBrowserContextIfExists(
      content::BrowserContext* context);
  static BluetoothChooserContextFactory* GetInstance();

  // Move-only class.
  BluetoothChooserContextFactory(const BluetoothChooserContextFactory&) =
      delete;
  BluetoothChooserContextFactory& operator=(
      const BluetoothChooserContextFactory&) = delete;

 private:
  friend base::NoDestructor<BluetoothChooserContextFactory>;

  BluetoothChooserContextFactory();
  ~BluetoothChooserContextFactory() override;

  // BrowserContextKeyedServiceFactory implementation:
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
  void BrowserContextShutdown(content::BrowserContext* context) override;
};

#endif  // ELECTRON_SHELL_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_FACTORY_H_

}  // namespace electron
