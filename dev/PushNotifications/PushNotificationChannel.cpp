﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "PushNotificationChannel.h"
#include "Microsoft.Windows.PushNotifications.PushNotificationChannel.g.cpp"
#include <winrt\Windows.Networking.PushNotifications.h>
#include <winrt\Windows.Foundation.h>

namespace winrt::Windows
{
    using namespace winrt::Windows::Networking::PushNotifications;
    using namespace winrt::Windows::Foundation;
}
namespace winrt::Microsoft
{
    using namespace winrt::Microsoft::Windows::PushNotifications;
}

namespace winrt::Microsoft::Windows::PushNotifications::implementation
{
    PushNotificationChannel::PushNotificationChannel(winrt::Windows::PushNotificationChannel const& channel): m_channel(channel) {}

    winrt::Windows::Uri PushNotificationChannel::Uri()
    {
        return winrt::Windows::Uri{ m_channel.Uri() };
    }
    winrt::Windows::DateTime PushNotificationChannel::ExpirationTime()
    {
        return m_channel.ExpirationTime();
    }
    void PushNotificationChannel::Close()
    {
        try
        {
            m_channel.Close();
        }
        catch (...)
        {
            auto channelCloseException = hresult_error(to_hresult());
            if (channelCloseException.code() != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
            {
                throw channelCloseException;
            }
        }
    }

    winrt::event_token PushNotificationChannel::PushReceived(winrt::Windows::TypedEventHandler<winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel, winrt::Microsoft::Windows::PushNotifications::PushNotificationReceivedEventArgs> const& handler)
    {
        return m_channel.PushNotificationReceived([weak_self = get_weak(), handler](auto&&, auto&& args)
        {
            auto strong = weak_self.get();
            if (strong)
            {
                handler(*strong, PushNotificationReceivedEventArgs::CreateFromPushNotificationReceivedEventArgs(args));
            };
        });
    }

    void PushNotificationChannel::PushReceived(winrt::event_token const& token) noexcept
    {
        m_channel.PushNotificationReceived(token);
    }
}
