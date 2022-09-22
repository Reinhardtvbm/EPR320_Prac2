/* 
 * File: external.h
 * Author: Reinhardt von Brandis-Martini
 * Comments: clap/snap and capacitive touch functions
 * Revision history: 0.1
 * Date: 3 September 2022
 */
  
#ifndef EXTERNAL_H
#define	EXTERNAL_H

#include <xc.h>
#include <stdbool.h>

bool run_touch(void);
bool run_clap(void);

#endif	/* EXTERNAL_H */

