//
// C++ Implementation: k9script
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9script.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include "bswap.h"
#include "dvdread.h"
#include <stdlib.h>

k9Script::k9Script(ifo_handle_t *_ifo, k9DVD *_dvd ) {
    m_ifo=_ifo;
    m_dvd=_dvd;

}


k9Script::~k9Script() {}


vm_cmd_t *k9Script::GOTO(char line,char register1,char value) {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0;
    cmd[1]=0xA1;
    cmd[2]=0;
    cmd[3]=register1;
    cmd[4]=0;
    cmd[5]=value;
    cmd[6]=0;
    cmd[7]=line;
    return &m_cmd;
}

vm_cmd_t *k9Script::GOTO(char line) {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0;
    cmd[1]=0x01;
    cmd[2]=0;
    cmd[3]=0;
    cmd[4]=0;
    cmd[5]=0;
    cmd[6]=0;
    cmd[7]=line;
    return &m_cmd;
}

vm_cmd_t *k9Script::JUMPTT(char title,char register1,char register2) {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0x30;
    cmd[1]=0x22;
    cmd[2]=0;
    cmd[3]=0;
    cmd[4]=0;
    cmd[5]=title;
    cmd[6]=register1;
    cmd[7]=register2;
    return &m_cmd;
}

vm_cmd_t *k9Script::JUMPVMGM_MENU() {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0x30;
    cmd[1]=0x06;
    cmd[2]=0;
    cmd[3]=0;
    cmd[4]=0;
    cmd[5]=0x42;
    cmd[6]=0;
    cmd[7]=0;
    return &m_cmd;
}

vm_cmd_t *k9Script::CALLVMGM_MENU() {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0x30;
    cmd[1]=0x08;
    cmd[2]=0;
    cmd[3]=0;
    cmd[4]=0x01;
    cmd[5]=0x42;
    cmd[6]=0;
    cmd[7]=0;
    return &m_cmd;
}


vm_cmd_t *k9Script::EXIT(char register1,char register2) {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0x30;
    cmd[1]=0x21;
    cmd[2]=0;
    cmd[3]=0;
    cmd[4]=0;
    cmd[5]=0;
    cmd[6]=register1;
    cmd[7]=register2;
    return &m_cmd;
}


vm_cmd_t *k9Script::setGPRM(char numReg,uint16_t value) {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0x71;
    cmd[1]=0;
    cmd[2]=0;
    cmd[3]=numReg;
    B2N_16(value);
    memcpy(cmd+4,&value,sizeof(uint16_t));
    cmd[6]=0;
    cmd[7]=0;
    return &m_cmd;

}
vm_cmd_t *k9Script::setGPRMREG(char numReg,uint16_t value) {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0x61;
    cmd[1]=0;
    cmd[2]=0;
    cmd[3]=numReg;
    B2N_16(value);
    memcpy(cmd+4,&value,sizeof(uint16_t));
    cmd[6]=0;
    cmd[7]=0;
    return &m_cmd;

}


vm_cmd_t *k9Script::setSTN(char numAudio,char numSubpicture) {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0x51;
    cmd[1]=0;
    cmd[2]=0;
    cmd[3]=numAudio==0 ? 0 : 0x80+numAudio-1;
    cmd[4]=numSubpicture==0 ? 0 : 0xC0+numSubpicture-1;
    cmd[5]=0;
    cmd[6]=0;
    cmd[7]=0;
    return &m_cmd;

}

vm_cmd_t *k9Script::setSTN(char numAngle) {
    uchar* cmd=(uchar*) &m_cmd;
    cmd[0]=0x51;
    cmd[1]=0;
    cmd[2]=0;
    cmd[3]=0;
    cmd[4]=0;
    cmd[5]=numAngle==0 ? 0 : 0x80+numAngle-1;
    cmd[6]=0;
    cmd[7]=0;
    return &m_cmd;
}

void k9Script::updatePGCIUT() {
    //first, free the old PGCIUT
    ifoFree_PGCI_UT(m_ifo);

    m_ifo->pgci_ut = (pgci_ut_t*) malloc(sizeof(pgci_ut_t));
    pgci_ut_t* pgci_ut = m_ifo->pgci_ut;
    pgci_ut->zero_1=0;
    pgci_ut->nr_of_lus=1;

    pgci_ut->lu = (pgci_lu_t*) malloc(sizeof(pgci_lu_t));
    pgci_lu_t *lu =pgci_ut->lu;
    uchar * lng=(uchar*) &(lu->lang_code);
    lng[0]='n';
    lng[1]='e';
    lu->lang_extension=0;
    lu->exists=0x80;

    pgcit_t *pgcit=(pgcit_t*) malloc(sizeof(pgcit_t));
    lu->pgcit=pgcit;
    memset(pgcit,0,sizeof(pgcit_t));
    pgcit->nr_of_pgci_srp=1;
    pgci_srp_t * srp=(pgci_srp_t*)malloc(sizeof(pgci_srp_t));
    memset(srp,0,sizeof(pgci_srp_t));
    pgcit->pgci_srp=srp;
    srp->entry_id=0x82;

    pgc_t *pgc=(pgc_t*)malloc(sizeof(pgc_t));
    srp->pgc=pgc;
    memset(pgc,0,sizeof(pgc_t));

    pgc_command_tbl_t * command_tbl=(pgc_command_tbl_t*) malloc(sizeof(pgc_command_tbl_t));
    pgc->command_tbl=command_tbl;
    pgc->command_tbl_offset=1;
    memset(command_tbl,0,sizeof(pgc_command_tbl_t));

    if (m_ifo->vmgi_mat != NULL)
        updatePGCIUT_VMG(command_tbl);
    else
        updatePGCIUT_VTS(command_tbl);
}

void k9Script::updateFPPGC() {
    ifoFree_FP_PGC(m_ifo);
    pgc_t*pgc=(pgc_t*)malloc(sizeof(pgc_t));
    m_ifo->first_play_pgc=pgc;
    memset(pgc,0,sizeof(pgc_t));

    pgc_command_tbl_t * command_tbl=(pgc_command_tbl_t*) malloc(sizeof(pgc_command_tbl_t));
    pgc->command_tbl=command_tbl;
    pgc->command_tbl_offset=1;
    memset(command_tbl,0,sizeof(pgc_command_tbl_t));
    // set GRPM(0)= First Title
    addPreCmd(command_tbl,JUMPTT(m_dvd->getstart()->getnumTitle(),0,0));
    // jump VMGM Menu
    //addPreCmd(command_tbl,JUMPVMGM_MENU());

}

void k9Script::updatePGC(pgc_t *_pgc ,int numVTS,int numPGC) {
    k9DVDTitle *title=NULL;
    for (int i=0; i <m_dvd->gettitleCount();i++) {
        title=m_dvd->gettitle(i);
        if (title->getVTS()== numVTS && title->getTTN()==numPGC &&  title->getIndexed())
            break;
    }

    pgc_command_tbl_t *command_tbl=_pgc->command_tbl;
    if (command_tbl ==NULL) {
        command_tbl=(pgc_command_tbl_t*) malloc(sizeof(pgc_command_tbl_t));
        _pgc->command_tbl=command_tbl;
        _pgc->command_tbl_offset=1;
        memset(command_tbl,0,sizeof(pgc_command_tbl_t));
    }

    if (numPGC==0)
        return;

    for (int i=0; i < command_tbl->nr_of_pre;i++) {
        //replace all JUMPs by a goto to the last line of cell commands ( link to next title)
        char *cmd=(char*)&(command_tbl->pre_cmds[i]);
        if (cmd[0]==0x30) {
            char NOP[8]={0,0,0,0,0,0,0,0};
            memcpy(cmd,NOP,8);
        }

    }
    if (title->isSelected()) {
        char numSubP=0,numAudio=0;
        if (title->getDefSubtitle() !=NULL)
            numSubP=title->getDefSubtitle()->getnum();
        if (title->getDefAudio() !=NULL)
            numAudio=title->getDefAudio()->getID();
        if (numSubP+numAudio >0) {
            char *c=(char*)command_tbl->pre_cmds;
            bool binsert=false;
            if (c==NULL)
                binsert=true;
            else if ( *c==0x51)
                binsert=true;
            if (binsert)
                memcpy(command_tbl->pre_cmds,setSTN( numAudio,numSubP),8);
            else
                insertPreCmd(command_tbl,setSTN( numAudio,numSubP));
        }
    }

    for (int i=0; i < command_tbl->nr_of_cell;i++) {
        //replace all JUMPs by a goto to the last line of cell commands ( link to next title)
        JumpVmg(&(command_tbl->cell_cmds[i]));

    }
    for (int i=0; i < command_tbl->nr_of_post;i++) {
        //replace all JUMPs by a goto to the last line of cell commands ( link to next title)
        JumpVmg(&(command_tbl->post_cmds[i]));
    }


    bool insertCall=true;
    if (command_tbl->nr_of_post !=0) {
        vm_cmd_t *cmd=&command_tbl->post_cmds[command_tbl->nr_of_post -1];
        if (memcmp(cmd,CALLVMGM_MENU(),8)==0)
            insertCall=false;
    }

    if (insertCall)
        for (int i=0; i <m_dvd->gettitleCount();i++) {
            k9DVDTitle *title=m_dvd->gettitle(i);
            if (title->getVTS()== numVTS && title->getTTN()==numPGC && title->isSelected() && title->getIndexed()) {
                addPostCmd(command_tbl,CALLVMGM_MENU());
                break;
            }
        }
}

// replace the actual jump by a jump to he vmgm menu
void k9Script::JumpVmg(vm_cmd_t * command) {
    char *cmd=(char*) command;
    if (cmd[0]==0x30) {
        cmd[1]=(cmd[1] & 0xF0) + 0x08;  //0x20 =conditionnal jump
        cmd[2]=cmd[3]=0;
        cmd[4]=0x01;
        cmd[5]=0x42;
    }

}


void k9Script::updatePGCIUT_VTS(pgc_command_tbl_t *command_tbl) {
    addPreCmd( command_tbl, JUMPVMGM_MENU());
}


void k9Script::updatePGCIUT_VMG(pgc_command_tbl_t *command_tbl) {
    command_tbl->nr_of_pre=0;
    command_tbl->pre_cmds=NULL;

    //initialisation of the registers
    for (int i=0;i<=15;i++)
        if (i!=1)
            addPreCmd( command_tbl,setGPRM( i,0));

    addTitles(command_tbl);
}

void k9Script::addTitles(pgc_command_tbl_t *command_tbl) {
    char NOP[8]={0,0,0,0,0,0,0,0};

    //Set GPRM(1) = current title (SPRM(4))
    addPreCmd(command_tbl,setGPRMREG(1,0x84));

    int nbSelected=0;
    for (int i=0;i<m_dvd->gettitleCount();i++) {
        k9DVDTitle *title=m_dvd->gettitle(i);
        if (title->isSelected() && title->getIndexed())
            nbSelected++;
    }
    int a=0;
    for (int i=0;i<m_dvd->gettitleCount();i++) {
        k9DVDTitle *title=m_dvd->gettitle(i);
        if (title->isSelected() && title->getIndexed()) {
            //SI GPRM1=numTitle, GOTO i*3
            addPreCmd(command_tbl,GOTO(16+nbSelected+a*3+2,1,title->getnumTitle()));
            a++;
        }
    }
    addPreCmd( command_tbl,EXIT(0,0));

    for (int i=0;i<m_dvd->gettitleCount();i++) {
        k9DVDTitle *title=m_dvd->gettitle(i);
        if (title->isSelected() && title->getIndexed()) {
            //initialization of register 1
            addPreCmd(command_tbl,setGPRM(1,0));
            //set default subtitle and audio stream
            addPreCmd(command_tbl,(vm_cmd_t*)NOP);
            //jump to title
            if (title->getnextTitle()!=NULL)
                addPreCmd(command_tbl,JUMPTT(title->getnextTitle()->getnumTitle(),0,0));
            else
                addPreCmd(command_tbl,EXIT(0,0));
        }
    }


}

void k9Script::addPreCmd(pgc_command_tbl_t *command_tbl,vm_cmd_t *cmd) {
    command_tbl->nr_of_pre++;
    if (command_tbl->pre_cmds == NULL)
        command_tbl->pre_cmds=(vm_cmd_t*)malloc(sizeof(vm_cmd_t));
    else
        command_tbl->pre_cmds=(vm_cmd_t*) realloc(command_tbl->pre_cmds,sizeof(vm_cmd_t)*command_tbl->nr_of_pre);

    memcpy(&(command_tbl->pre_cmds[command_tbl->nr_of_pre-1]),cmd,sizeof(vm_cmd_t));
}

void k9Script::insertPreCmd(pgc_command_tbl_t *command_tbl,vm_cmd_t *cmd) {
    command_tbl->nr_of_pre++;
    if (command_tbl->pre_cmds == NULL)
        command_tbl->pre_cmds=(vm_cmd_t*)malloc(sizeof(vm_cmd_t));
    else
        command_tbl->pre_cmds=(vm_cmd_t*) realloc(command_tbl->pre_cmds,sizeof(vm_cmd_t)*command_tbl->nr_of_pre);

    for (int i=command_tbl->nr_of_pre-2;i>=0;i--) {
        memcpy(&(command_tbl->pre_cmds[i+1]),&(command_tbl->pre_cmds[i]),sizeof(vm_cmd_t));
        char *cmd=(char*)&(command_tbl->pre_cmds[i+1]);
        if (cmd[0]==0 && (cmd[1]&0x01==0x01))
            cmd[7]++;

    }

    memcpy(&(command_tbl->pre_cmds[0]),cmd,sizeof(vm_cmd_t));
}



void k9Script::addPostCmd(pgc_command_tbl_t *command_tbl,vm_cmd_t *cmd) {
    command_tbl->nr_of_post++;
    if (command_tbl->post_cmds == NULL)
        command_tbl->post_cmds=(vm_cmd_t*)malloc(sizeof(vm_cmd_t));
    else
        command_tbl->post_cmds=(vm_cmd_t*) realloc(command_tbl->post_cmds,sizeof(vm_cmd_t)*command_tbl->nr_of_post);

    memcpy(&(command_tbl->post_cmds[command_tbl->nr_of_post-1]),cmd,sizeof(vm_cmd_t));
}

void k9Script::addCellCmd(pgc_command_tbl_t *command_tbl,vm_cmd_t *cmd) {
    command_tbl->nr_of_cell++;
    if (command_tbl->cell_cmds == NULL)
        command_tbl->cell_cmds=(vm_cmd_t*)malloc(sizeof(vm_cmd_t));
    else
        command_tbl->cell_cmds=(vm_cmd_t*) realloc(command_tbl->cell_cmds,sizeof(vm_cmd_t)*command_tbl->nr_of_cell);

    memcpy(&(command_tbl->cell_cmds[command_tbl->nr_of_cell-1]),cmd,sizeof(vm_cmd_t));
}
