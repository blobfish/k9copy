//
// C++ Interface: k9script
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9SCRIPT_H
#define K9SCRIPT_H
#include "k9common.h"
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9DVD;

class k9Script{
private:
   ifo_handle_t *m_ifo;
   k9DVD *m_dvd;
   vm_cmd_t m_cmd;
protected:
   vm_cmd_t *JUMPTT(char title,char register1,char register2);
   vm_cmd_t *JUMPVMGM_MENU();
   vm_cmd_t *GOTO(char line,char register1,char value);
   vm_cmd_t *GOTO(char line);
   vm_cmd_t *setGPRM(char numReg,uint16_t value);
   vm_cmd_t *setGPRMREG(char numReg,uint16_t value);

   vm_cmd_t *CALLVMGM_MENU();
   vm_cmd_t *setSTN(char numAudio,char numSubpicture);
   vm_cmd_t *setSTN(char numAngle);

   vm_cmd_t *EXIT(char register1,char register2);
   void addPreCmd(pgc_command_tbl_t *command_tbl,vm_cmd_t *cmd);
   void insertPreCmd(pgc_command_tbl_t *command_tbl,vm_cmd_t *cmd);
   void addPostCmd(pgc_command_tbl_t *command_tbl,vm_cmd_t *cmd);
   void addCellCmd(pgc_command_tbl_t *command_tbl,vm_cmd_t *cmd);
   void updatePGCIUT_VMG(pgc_command_tbl_t *command_tbl);
   void updatePGCIUT_VTS(pgc_command_tbl_t *command_tbl);
   void JumpVmg(vm_cmd_t * cmd) ;
   void addTitles(pgc_command_tbl_t *command_tbl);
public:    
    k9Script(ifo_handle_t *_ifo,k9DVD *_dvd);
    void updatePGCIUT();
    void updateFPPGC();
    void updatePGC(pgc_t *_pgc,int numVTS,int numPGC);
    ~k9Script();
};

#endif
