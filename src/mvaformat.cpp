#include "mvaformat.h"
#include "mvaformat_impl.hpp"
template MVAFORMAT_API bool MVSA::MVAIO::SerializeSave<MVSA::Scene>(
    const MVSA::Scene &, const std::string &, int, uint32_t);

template MVAFORMAT_API bool MVSA::MVAIO::SerializeLoad<MVSA::Scene>(
    MVSA::Scene &,
    const std::string &,
    int* , uint32_t *);
