#pragma once

/// <summary> nullptr を考慮し、delete処理する。 </summary>
template<typename T>
inline void SafeDelete(T*& _Ptr)
{
    if (_Ptr != nullptr)
    {
        delete _Ptr;
        _Ptr = nullptr;
    }
}

/// <summary> nullptr を考慮し、delete[]処理する。 </summary>
template<typename T>
inline void SafeDeleteArray(T*& _Ptr)
{
    if (_Ptr != nullptr)
    {
        delete[] _Ptr;
        _Ptr = nullptr;
    }
}

/// <summary> nullptr を考慮し、Release()メソッドを実行 </summary>
template<typename T>
inline void SafeRelease(T*& _Ptr)
{
    if (_Ptr != nullptr)
    {
        _Ptr->Release();
        _Ptr = nullptr;
    }
}

/// <summary> nullptrを考慮し、Term()メソッドを実行し、delete処理する。 </summary>
template<typename T>
inline void SafeTerm(T*& _Ptr)
{
    if (_Ptr != nullptr)
    {
        _Ptr->Term();
        delete _Ptr;
        _Ptr = nullptr;
    }
}