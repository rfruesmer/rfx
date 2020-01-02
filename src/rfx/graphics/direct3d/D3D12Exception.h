#pragma once

#ifndef ThrowIfFailed
#define ThrowIfFailed(hr__)                                              \
{                                                                     \
    if(FAILED(hr__)) { throw rfx::D3D12Exception(hr__, __FUNCTION__, __FILE__, __LINE__); } \
}
#endif

namespace rfx
{
class D3D12Exception : public std::exception
{
public:
    D3D12Exception(HRESULT hr, const char* function, const char* file, int line);

    char const* what() const override;

private:
    std::string message;
};
}
