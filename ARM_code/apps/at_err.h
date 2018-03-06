/*
*********************************************************************************************************
*         DW Project       
*
*Hardware platform : 
*
* File             : at_err.h
* version          : v1.0
* By               : 
* Modify         : 
* Date             : 
*********************************************************************************************************
*/
#ifndef AT_ERR_H
#define AT_ERR_H


#define AT_ERR_MAX_NUM   1024


typedef INT32U AT_ERR_TYPE;


extern INT32U At_Err_Log[AT_ERR_MAX_NUM];


extern void AtErrLogInit ();


extern void AtErrLogWrite (AT_ERR_TYPE at_err);



#endif

