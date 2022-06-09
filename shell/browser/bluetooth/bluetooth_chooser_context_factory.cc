// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/browser/bluetooth/bluetooth_chooser_context_factory.h"

#include "base/no_destructor.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"

namespace electron {

// static
BluetoothChooserContextFactory* BluetoothChooserContextFactory::GetInstance() {
  static base::NoDestructor<BluetoothChooserContextFactory> factory;
  return factory.get();
}

// static
BluetoothChooserContext* BluetoothChooserContextFactory::GetForBrowserContext(
    content::BrowserContext* context) {
  return static_cast<BluetoothChooserContext*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

// static
BluetoothChooserContext*
BluetoothChooserContextFactory::GetForBrowserContextIfExists(
    content::BrowserContext* context) {
  return static_cast<BluetoothChooserContext*>(
      GetInstance()->GetServiceForBrowserContext(context, /*create=*/false));
}

BluetoothChooserContextFactory::BluetoothChooserContextFactory()
    : BrowserContextKeyedServiceFactory(
          "BluetoothChooserContext",
          BrowserContextDependencyManager::GetInstance()) {}

BluetoothChooserContextFactory::~BluetoothChooserContextFactory() = default;

KeyedService* BluetoothChooserContextFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  auto* browser_context =
      static_cast<electron::ElectronBrowserContext*>(context);
  return new BluetoothChooserContext(browser_context);
}

content::BrowserContext* BluetoothChooserContextFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return context;
}

void BluetoothChooserContextFactory::BrowserContextShutdown(
    content::BrowserContext* context) {}

}  // namespace electron
