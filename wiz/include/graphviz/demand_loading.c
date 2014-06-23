/* $Id: demand_loading.c,v 1.6 2009/01/21 16:47:00 arif Exp $ $Revision: 1.6 $ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

/*visual studio*/
#ifdef WIN32_DLL
#ifndef GVC_EXPORTS
__declspec(dllimport) int Demand_Loading = 1;
#else
__declspec(dllexport) int Demand_Loading = 1;
#endif
#endif
/*end visual studio*/

#ifndef WIN32_DLL
#ifdef GVDLL
__declspec(dllexport) int Demand_Loading = 1;
#else
const int Demand_Loading = 1;
#endif
#endif