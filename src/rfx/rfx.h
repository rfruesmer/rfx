#pragma once


// #####################################################################################################################
// MACROS
// #####################################################################################################################

#define VK_SUCCEEDED(result) result == VK_SUCCESS
#define VK_FAILED(result) result != VK_SUCCESS

#define RFX_THROW(message) RFX_THROW_TYPED(message, std::runtime_error)
#define RFX_THROW_TYPED(message, type)      throw type(message + std::string("\n") + std::string(__FILE__) + std::string("(") + std::to_string(__LINE__) + std::string(")"))
#define RFX_CHECK_ARGUMENT(expr)            if (!(expr)) RFX_THROW_TYPED(std::string("Illegal argument: ") + #expr, std::invalid_argument)
#define RFX_CHECK_STATE(expr, message)      if (!(expr)) RFX_THROW(std::string("Illegal state: ") + message)
#define ThrowIfFailed(result)               if (result != VK_SUCCESS) RFX_THROW("Failure with VkResult of " + std::to_string(result))

#define RFX_THROW_NOT_IMPLEMENTED() \
    RFX_THROW("not implemented yet")

