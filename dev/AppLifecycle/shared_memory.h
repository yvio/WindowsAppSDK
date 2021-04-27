﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.
#pragma once

template <typename T>
struct dynamic_shared_memory
{
    size_t size{ 0 };
    T data{ 0 };
};

template <typename T>
class shared_memory
{
public:
    shared_memory() {}

    bool open(std::wstring name, size_t size)
    {
        m_name = name;

        auto createdFile = open_internal(size);
        if (createdFile)
        {
            clear();
            m_view.get()->size = size;
        }
        else
        {
            // file already exists, reopen with stored size.
            auto newSize = m_view.get()->size;
            m_view.reset();
            m_file.reset();
            open_internal(newSize);
        }

        return createdFile;
    }

    bool open(std::wstring name)
    {
        return open(name, sizeof(T));
    }

    void clear()
    {
        // Clear only the data portion, not the size.
        memset(get(), 0, size());
    }

    void resize(size_t size)
    {
        // TODO: See if we can fail if resizing isn't going to work (someone still has the old file open still)
        auto name = m_name;
        reset();
        open(name, size);
    }

    size_t size()
    {
        return m_view.get()->size;
    }

    T* get()
    {
        return &m_view.get()->data;
    }

    T* operator->()
    {
        return get();
    }

    void reset()
    {
        m_name.erase();
        m_view.reset();
        m_file.reset();
    }

protected:
    bool open_internal(size_t size)
    {
        m_file = wil::unique_handle(CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(size), m_name.c_str()));
        THROW_LAST_ERROR_IF_NULL(m_file);

        bool createdFile = (GetLastError() != ERROR_ALREADY_EXISTS);
        m_view.reset(reinterpret_cast<dynamic_shared_memory<T>*>(MapViewOfFile(m_file.get(), FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, size)));
        THROW_LAST_ERROR_IF_NULL(m_view);

        return createdFile;
    }

	std::wstring m_name;
	wil::unique_handle m_file;
	wil::unique_mapview_ptr<dynamic_shared_memory<T>> m_view;
};