/*
*********************************************************************************************************
*         DW Project       
*
*Hardware platform : 
*
* File             : at_err.c
* version          : v1.0
* By               : 
* Modify         : 
* Date             : 
*********************************************************************************************************
*/
#include "global.h"
#include "net.h"
#include "at_cfg.h"
#include "at_err.h"




AT_ERR_TYPE At_Err_Log[AT_ERR_MAX_NUM];

static INT32U At_Err_Log_Index;

static INT32U At_Err_Log_Num;

void AtErrLogInit ()
{
    Mem_Clr((void *)At_Err_Log, (CPU_SIZE_T)(sizeof(AT_ERR_TYPE) * AT_ERR_MAX_NUM));
    At_Err_Log_Index = 0;
    At_Err_Log_Num = 0;
}



void AtErrLogWrite (AT_ERR_TYPE at_err)
{
    At_Err_Log[At_Err_Log_Index] = at_err;

    At_Err_Log_Index++;
    At_Err_Log_Num++;

    if (AT_ERR_MAX_NUM == At_Err_Log_Index) {
        At_Err_Log_Index = 0;
    }

}



void AtErrLogRead (void)
{




}


