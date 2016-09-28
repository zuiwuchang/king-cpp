#include "reader.h"

KING_C_API
KTcp_HREADER KING_C_CALL KTcp_CreateReader(const std::size_t hsize,KTcp_FuncGetMessageSize func)
{
    KTcp_HREADER hReader = NULL;
    try
    {
        hReader = new KTcp_READER(hsize,func);
    }
    catch(const std::bad_alloc&)
    {

    }
    return hReader;
}

KING_C_API
void KING_C_CALL KTcp_DestoryReader(KTcp_HREADER hReader)
{
    delete hReader;
}
