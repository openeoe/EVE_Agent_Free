/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.old-api.conf,v 1.3 2002/10/17 09:40:46 dts12 Exp $
 */
#ifndef USERACCESSCONTROLTABLE_H
#define USERACCESSCONTROLTABLE_H

/* function declarations */
void init_userAccessControlTable(void);
FindVarMethod var_userAccessControlTable;
FindVarMethod var_userAccessControlTable;
    WriteMethod write_userAccessControlID;
    WriteMethod write_userAccessControlPassword;
    WriteMethod write_userAccessControlName;
    WriteMethod write_userAccessControlDescription;
    WriteMethod write_userAccessControlGroupId;
    WriteMethod write_userAccessControlCreatedDate;
    WriteMethod write_userAccessControlUpdatedDate;
    WriteMethod write_userAccessControlStatus;

#endif /* USERACCESSCONTROLTABLE_H */