﻿#include "pch.h"
#include <ToastActivationCallback.h>
#include "ToastActivatedEventArgs.h"
#include "externs.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/base.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <iostream>
#include <ToastNotificationUtility.h>

namespace winrt
{
    using namespace Microsoft::Windows::ToastNotifications;
    using namespace Windows::ApplicationModel::Core;
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
}

HRESULT __stdcall ToastActivationCallback::Activate(
    LPCWSTR /* appUserModelId */,
    LPCWSTR invokedArgs,
    [[maybe_unused]] NOTIFICATION_USER_INPUT_DATA const* data,
    [[maybe_unused]] ULONG dataCount) noexcept try
{
    winrt::IMap<winrt::hstring, winrt::hstring> userInput{ winrt::single_threaded_map<winrt::hstring, winrt::hstring>() };
    for (unsigned long i = 0; i < dataCount; i++)
    {
        userInput.Insert(data[i].Key, data[i].Value);
    }

    winrt::ToastActivatedEventArgs activatedEventArgs = winrt::make<winrt::Microsoft::Windows::ToastNotifications::implementation::ToastActivatedEventArgs>(invokedArgs, userInput);

    
    if (GetToastHandlers())
    {
        /* As the process is already launched, we invoke the foreground toast event handlers with the activatedeventargs */
        GetToastHandlers()(*this, activatedEventArgs);
    }
    else
    {
        /* Activation results in a process launch, we cache the activatedEventArgs in the COM static store
           and fire an event to let the main thread know that it is okay to infer into AppLifeCycle::GetToastActivatedEventArgs().
        */
        auto appProperties = winrt::CoreApplication::Properties();
        appProperties.Insert(ACTIVATED_EVENT_ARGS_KEY, activatedEventArgs);
        SetEvent(GetWaitHandleForArgs().get());
    }

    return S_OK;
}
CATCH_RETURN()
