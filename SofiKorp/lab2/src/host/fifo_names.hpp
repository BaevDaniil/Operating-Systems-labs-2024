#pragma once

#include "host.hpp"
using TempHost = Host<FIFOConnection>;
using TempClientInfo = ConnectClient<FIFOConnection>;