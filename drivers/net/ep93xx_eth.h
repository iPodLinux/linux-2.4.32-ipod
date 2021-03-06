/*------------------------------------------------------------------------
 * ep93xx_eth.h
 * : header file of Ethernet Device Driver for Cirrus Logic EP93xx.
 *
 * Copyright (C) 2003 by Cirrus Logic www.cirrus.com 
 * This software may be used and distributed according to the terms
 * of the GNU Public License.
 *
 * This file contains device related information like register info
 * and register access method macros for the Ethernet device 
 * embedded within Cirrus Logic's EP93xx SOC chip.  
 *
 * Information contained in this file was obtained from 
 * the EP9312 Manual Revision 0.12 and 0.14 from Cirrus Logic.
 *
 * History
 * 05/18/01  Sungwook Kim  Initial release
 * 03/25/2003  Melody Modified for EP92xx
 *--------------------------------------------------------------------------*/


#ifndef _EP9213_ETH_H_
#define _EP9213_ETH_H_


/*---------------------------------------------------------------
 *  Definition of H/W Defects and Their Workarounds
 *-------------------------------------------------------------*/



/*---------------------------------------------------------------
 *  Data types used in this driver
 *-------------------------------------------------------------*/
typedef  unsigned char   U8;
typedef  unsigned short  U16;
typedef  unsigned long   U32;
typedef  unsigned int    UINT;



/*---------------------------------------------------------------
 * Definition of the registers.
 * For details, refer to the datasheet  .
 * 
 * Basically, most registers are 32 bits width register.
 * But some are 16 bits and some are 6 or 8 bytes long.
 *-------------------------------------------------------------*/

#define  REG_RxCTL  0x0000  /*offset to Receiver Control Reg*/
#define  RxCTL_PauseA  (1<<20)
#define  RxCTL_RxFCE1  (1<<19)
#define  RxCTL_RxFCE0  (1<<18)
#define  RxCTL_BCRC    (1<<17)
#define  RxCTL_SRxON   (1<<16)
#define  RxCTL_RCRCA   (1<<13)
#define  RxCTL_RA      (1<<12)
#define  RxCTL_PA      (1<<11)
#define  RxCTL_BA      (1<<10)
#define  RxCTL_MA      (1<<9)
#define  RxCTL_IAHA    (1<<8)
#define  RxCTL_IA3     (1<<3)
#define  RxCTL_IA2     (1<<2)
#define  RxCTL_IA1     (1<<1)
#define  RxCTL_IA0     (1<<0)

#define  REG_TxCTL  0x0004  /*offset to Transmit Control Reg*/
#define  TxCTL_DefDis  (1<<7)
#define  TxCTL_MBE     (1<<6)
#define  TxCTL_ICRC    (1<<5)
#define  TxCTL_TxPD    (1<<5)
#define  TxCTL_OColl   (1<<3)
#define  TxCTL_SP      (1<<2)
#define  TxCTL_PB      (1<<1)
#define  TxCTL_STxON   (1<<0)

#define  REG_TestCTL   0x0008  /*Test Control Reg, R/W*/
#define  TestCTL_MACF  (1<<7)
#define  TestCTL_MFDX  (1<<6)
#define  TestCTL_DB    (1<<5)
#define  TestCTL_MIIF  (1<<4)

#define  REG_MIICmd  0x0010  /*offset to MII Command Reg, R/W*/
#define  MIICmd_OP     (0x03<<14)
#define  MIICmd_OP_RD  (2<<14)
#define  MIICmd_OP_WR  (1<<14)
#define  MIICmd_PHYAD  (0x1f<<5)
#define  MIICmd_REGAD  (0x1f<<0)

#define  REG_MIIData  0x0014  /*offset to MII Data Reg, R/W*/
#define  MIIData_MIIData  (0xffff<<0)

#define  REG_MIISts  0x0018  /*offset to MII Status Reg, R*/
#define  MIISts_Busy  (1<<0)

#define  REG_SelfCTL  0x0020  /*offset to Self Control Reg*/
#define  SelfCTL_RWP    (1<<7)    /*Remote Wake Pin*/
#define  SelfCTL_GPO0   (1<<5)
#define  SelfCTL_PUWE   (1<<4)
#define  SelfCTL_PDWE   (1<<3)
#define  SelfCTL_MIIL   (1<<2)
#define  SelfCTL_RESET  (1<<0)

#define  REG_IntEn   0x0024  /*Interrupt Enable Reg, R/W*/
#define  IntEn_RWIE    (1<<30)
#define  IntEn_RxMIE   (1<<29)
#define  IntEn_RxBIE   (1<<28)
#define  IntEn_RxSQIE  (1<<27)
#define  IntEn_TxLEIE  (1<<26)
#define  IntEn_ECIE    (1<<25)
#define  IntEn_TxUHIE  (1<<24)
#define  IntEn_MOIE    (1<<18)
#define  IntEn_TxCOIE  (1<<17)
#define  IntEn_RxROIE  (1<<16)
#define  IntEn_MIIIE   (1<<12)
#define  IntEn_PHYSIE  (1<<11)
#define  IntEn_TIE     (1<<10)
#define  IntEn_SWIE    (1<<8)
#define  IntEn_TxSQIE   (1<<3)
#define  IntEn_RxEOFIE  (1<<2)
#define  IntEn_RxEOBIE  (1<<1)
#define  IntEn_RxHDRIE  (1<<0)

#define  REG_IntStsP  0x0028  /*offset to Interrupt Status Preserve Reg, R/W*/
#define  REG_IntStsC  0x002c  /*offset to Interrupt Status Clear Reg, R*/
#define  IntSts_RWI    (1<<30)
#define  IntSts_RxMI   (1<<29)
#define  IntSts_RxBI   (1<<28)
#define  IntSts_RxSQI  (1<<27)
#define  IntSts_TxLEI  (1<<26)
#define  IntSts_ECI    (1<<25)
#define  IntSts_TxUHI  (1<<24)
#define  IntSts_MOI    (1<<18)
#define  IntSts_TxCOI  (1<<17)
#define  IntSts_RxROI  (1<<16)
#define  IntSts_MIII   (1<<12)
#define  IntSts_PHYSI  (1<<11)
#define  IntSts_TI     (1<<10)
#define  IntSts_AHBE   (1<<9)
#define  IntSts_SWI    (1<<8)
#define  IntSts_OTHER  (1<<4)
#define  IntSts_TxSQ   (1<<3)
#define  IntSts_RxSQ   (1<<2)

#define  REG_GT  0x0040  /*offset to General Timer Reg*/
#define  GT_GTC  (0xffff<<16)
#define  GT_GTP  (0xffff<<0)

#define  REG_FCT  0x0044  /*offset to Flow Control Timer Reg*/
#define  FCT_FCT  (0x00ffffff<<0)

#define  REG_FCF  0x0048  /*offset to Flow Control Format Reg*/
#define  FCF_MACCT  (0xffff<<16)
#define  FCF_TPT    (0xffff<<0)

#define  REG_AFP  0x004c  /*offset to Address Filter Pointer Reg*/
#define  AFP_AFP  (0x07<<0)  /*Address Filter Pointer (bank control for REG_IndAD)*/
#define  AFP_AFP_IA0   0       /*Primary Individual Address (MAC Addr)*/
#define  AFP_AFP_IA1   1       /*Individual Address 1*/
#define  AFP_AFP_IA2   2       /*Individual Address 2*/
#define  AFP_AFP_IA3   3       /*Individual Address 3*/
#define  AFP_AFP_DTxP  6       /*Destination Address of Tx Pause Frame*/
#define  AFP_AFP_HASH  7       /*Hash Table*/

#define  REG_IndAD      0x0050  /*offset to Individual Address Reg, n bytes, R/W*/

#define  REG_GIntSts    0x0060  /*offset to Global Interrupt Status Reg (writing 1 will clear)*/
#define  REG_GIntROS    0x0068  /*offset to Global Interrupt Status Read Only Reg*/
#define  GIntSts_INT  (1<<15)     /*Global Interrupt Request Status*/

#define  REG_GIntMsk    0x0064  /*offset to Global Interrupt Mask Reg*/
#define  GIntMsk_IntEn  (1<<15)   /*Global Interrupt Enable*/

#define  REG_GIntFrc    0x006c  /*offset to Global Interrupt Force Reg*/
#define  GIntFrc_INT  (1<<15)     /*Force to set GIntSts*/

#define  REG_TxCollCnt  0x0070  /*Transmit Collision Count Reg, R*/
#define  REG_RxMissCnt  0x0074  /*Receive Miss Count Reg, R*/
#define  REG_RxRntCnt   0x0078  /*Receive Runt Count Reg, R*/

#define  REG_BMCtl  0x0080  /*offset to Bus Master Control Reg, R/W*/
#define  BMCtl_MT     (1<<13)
#define  BMCtl_TT     (1<<12)
#define  BMCtl_UnH    (1<<11)
#define  BMCtl_TxChR  (1<<10)
#define  BMCtl_TxDis  (1<<9)
#define  BMCtl_TxEn   (1<<8)
#define  BMCtl_EH2    (1<<6)
#define  BMCtl_EH1    (1<<5)
#define  BMCtl_EEOB   (1<<4)
#define  BMCtl_RxChR  (1<<2)
#define  BMCtl_RxDis  (1<<1)
#define  BMCtl_RxEn   (1<<0)

#define  REG_BMSts  0x0084  /*offset to Bus Master Status Reg, R*/
#define  BMSts_TxAct  (1<<7)
#define  BMSts_TP     (1<<4)
#define  BMSts_RxAct  (1<<3)
#define  BMSts_QID    (0x07<<0)
#define  BMSts_QID_RxDt   (0<<0)
#define  BMSts_QID_TxDt   (1<<0)
#define  BMSts_QID_RxSts  (2<<0)
#define  BMSts_QID_TxSts  (3<<0)
#define  BMSts_QID_RxDesc (4<<0)
#define  BMSts_QID_TxDesc (5<<0)

#define  REG_RBCA   0x0088  /*offset to Receive Buffer Current Address Reg, R*/
#define  REG_TBCA   0x008c  /*offset to Transmit Buffer Current Address Reg, R*/

#define  REG_RxDBA  0x0090  /*offset to Receive Descriptor Queue Base Address Reg, R/W*/
#define  REG_RxDBL  0x0094  /*offset to Receive Descriptor Queue Base Length Reg, R/W, 16bits*/
#define  REG_RxDCL  0x0096  /*offset to Receive Descriptor Queue Current Length Reg, R/W, 16bits*/
#define  REG_RxDCA  0x0098  /*offset to Receive Descriptor Queue Current Address Reg, R/W*/

#define  REG_RxDEQ  0x009c  /*offset to Receive Descriptor Enqueue Reg, R/W*/
#define  RxDEQ_RDV  (0xffff<<16)  /*R 16bit; Receive Descriptor Value*/
#define  RxDEQ_RDI  (0xff<<0)     /*W 8bit; Receive Descriptor Increment*/

#define  REG_RxSBA  0x00a0  /*offset to Receive Status Queue Base Address Reg, R/W*/
#define  REG_RxSBL  0x00a4  /*offset to Receive Status Queue Base Length Reg, R/W, 16bits*/
#define  REG_RxSCL  0x00a6  /*offset to Receive Status Queue Current Length Reg, R/W, 16bits*/
#define  REG_RxSCA  0x00a8  /*offset to Receive Status Queue Current Address Reg, R/W*/

#define  REG_RxSEQ  0x00ac  /*offset to Receive Status Queue Current Address Reg, R/W*/
#define  RxSEQ_RSV  (0xffff<<16)
#define  RxSEQ_RSI  (0xff<<0)

#define  REG_TxDBA  0x00b0  /*offset to Transmit Descriptor Queue Base Address Reg, R/W*/
#define  REG_TxDBL  0x00b4  /*offset to Transmit Descriptor Queue Base Length Reg, R/W, 16bits*/
#define  REG_TxDCL  0x00b6  /*offset to Transmit Descriptor Queue Current Length Reg, R/W, 16bits*/
#define  REG_TxDCA  0x00b8  /*offset to Transmit Descriptor Queue Current Address Reg, R/W*/

#define  REG_TxDEQ  0x00bc  /*offset to Transmit Descriptor Queue Current Address Reg, R/W*/
#define  TxDEQ_TDV  (0xffff<<16)
#define  TxDEQ_TDI  (0xff<<0)

#define  REG_TxSBA  0x00c0  /*offset to Transmit Status Queue Base Address Reg, R/W*/
#define  REG_TxSBL  0x00c4  /*offset to Transmit Status Queue Base Length Reg, R/W, 16bits*/
#define  REG_TxSCL  0x00c6  /*offset to Transmit Status Queue Current Length Reg, R/W, 16bits*/
#define  REG_TxSCA  0x00c8  /*offset to Transmit Status Queue Current Address Reg, R/W*/

#define  REG_RxBTH  0x00d0  /*offset to Receive Buffer Threshold Reg, R/W*/
#define  RxBTH_RDHT  (0x03ff<<16)
#define  RxBTH_RDST  (0x03ff<<0)

#define  REG_TxBTH  0x00d4  /*offset to Transmit Buffer Threshold Reg, R/W*/
#define  TxBTH_TDHT  (0x03ff<<16)
#define  TxBTH_TDST  (0x03ff<<0)

#define  REG_RxSTH  0x00d8  /*offset to Receive Status Threshold Reg, R/W*/
#define  RxSTH_RSHT  (0x003f<<16)
#define  RxSTH_RSST  (0x003f<<0)

#define  REG_TxSTH  0x00dc  /*offset to Transmit Status Threshold Reg, R/W*/
#define  TxSTH_TSHT  (0x003f<<16)
#define  TxSTH_TSST  (0x003f<<0)

#define  REG_RxDTH  0x00e0  /*offset to Receive Descriptor Threshold Reg, R/W*/
#define  RxDTH_RDHT  (0x003f<<16)
#define  RxDTH_RDST  (0x003f<<0)

#define  REG_TxDTH  0x00e4  /*offset to Transmit Descriptor Threshold Reg, R/W*/
#define  TxDTH_TDHT  (0x003f<<16)
#define  TxDTH_TDST  (0x003f<<0)

#define  REG_MaxFL  0x00e8  /*offset to Max Frame Length Reg, R/W*/
#define  MaxFL_MFL  (0x07ff<<16)
#define  MaxFL_TST  (0x07ff<<0)

#define  REG_RxHL  0x00ec  /*offset to Receive Header Length Reg, R/W*/
#define  RxHL_RHL2  (0x07ff<<16)
#define  RxHL_RHL1  (0x03ff<<0)

#define  REG_MACCFG0  0x0100  /*offset to Test Reg #0, R/W*/
#define  MACCFG0_DbgSel  (1<<7)
#define  MACCFG0_LCKEN   (1<<6)
#define  MACCFG0_LRATE   (1<<5)
#define  MACCFG0_RXERR   (1<<4)
#define  MACCFG0_BIT33   (1<<2)
#define  MACCFG0_PMEEN   (1<<1)
#define  MACCFG0_PMEST   (1<<0)

#define  REG_MACCFG1  0x0104  /*offset to Test Reg #1, R/W*/
#define  REG_MACCFG2  0x0108  /*offset to Test Reg #2, R*/
#define  REG_MACCFG3  0x010c  /*offset to Test Reg #3, R*/



/*---------------------------------------------------------------
 * Definition of Descriptor/Status Queue Entry
 *-------------------------------------------------------------*/

typedef  union receiveDescriptor  {  /*data structure of Receive Descriptor Queue Entry*/
    struct {        /*whole value*/
        U32  e0,      /*1st dword entry*/
             e1;      /*2nd dword entry*/
    }  w;
    struct {        /*bit field definitions*/
        U32  ba:32,   /*Buffer Address (keep in mind this is physical address)*/
             bl:16,   /*b15-0; Buffer Length*/
             bi:15,   /*b30-16; Buffer Index*/
             nsof:1;  /*b31; Not Start Of Frame*/
    }  f;
}  receiveDescriptor;


typedef  union receiveStatus  {  /*data structure of Receive Status Queue Entry*/
    struct {         /*whole word*/
        U32  e0,       /*1st dword entry*/
             e1;       /*2nd dword entry*/
    }  w;
    struct {         /*bit field*/
        U32  rsrv1:8,  /*b7-0: reserved*/
             hti:6,    /*b13-8: Hash Table Index*/
             rsrv2:1,  /*b14: reserved*/
             crci:1,   /*b15: CRC Included*/
             crce:1,   /*b16: CRC Error*/
             edata:1,  /*b17: Extra Data*/
             runt:1,   /*b18: Runt Frame*/
             fe:1,     /*b19: Framing Error*/
             oe:1,     /*b20: Overrun Error*/
             rxerr:1,  /*b21: Rx Error*/
             am:2,     /*b23-22: Address Match*/
             rsrv3:4,  /*b27-24: reserved*/
             eob:1,    /*b28: End Of Buffer*/
             eof:1,    /*b29: End Of Frame*/
             rwe:1,    /*b30: Received Without Error*/
             rfp:1,    /*b31: Receive Frame Processed*/
             fl:16,    /*b15-0: frame length*/
             bi:15,    /*b30-16: Buffer Index*/
             rfp2:1;   /*b31: Receive Frame Processed at 2nd word*/
    }  f;
}  receiveStatus;


typedef  union transmitDescriptor  {  /*data structure of Transmit Descriptor Queue Entry*/
    struct {         /*whole value*/
        U32  e0,       /*1st dword entry*/
             e1;       /*2nd dword entry*/
    }  w;
    struct {         /*bit field*/
        U32  ba:32,    /*b31-0: Buffer Address (keep in mind this is physical address)*/
             bl:12,    /*b11-0: Buffer Length*/
             rsrv1:3,  /*b14-12: reserved*/
             af:1,     /*b15: Abort Frame*/
             bi:15,    /*b30-16: Buffer Index*/
             eof:1;    /*b31: End Of Frame*/

    }  f;
}  transmitDescriptor;


typedef  union transmitStatus  {  /*data structure of Transmit Status Queue Entry*/
    struct {         /*whole word*/
        U32  e0;       /*1st dword entry*/
    }  w;
    struct {         /*bit field*/
        U32  bi:15,    /*b14-0: Buffer Index*/
             rsrv3:1,  /*b15: reserved*/
             ncoll:5,  /*b20-16: Number of Collisions*/
             rsrv2:3,  /*b23-21: reserved*/
             ecoll:1,  /*b24: Excess Collisions*/
             txu:1,    /*b25: Tx Underrun*/
             ow:1,     /*b26: Out of Window*/
             rsrv1:1,  /*b27: reserved*/
             lcrs:1,   /*b28: Loss of CRS*/
             fa:1,     /*b29: Frame Abort*/
             txwe:1,   /*b30: Transmitted Without Error*/
             txfp:1;   /*b31: Transmit Frame Processed*/
    }  f;
}  transmitStatus;



/*---------------------------------------------------------------
 *  Size of device registers occupied in memory/IO address map
 *-------------------------------------------------------------*/
#define  DEV_REG_SPACE  0x00010000

/*
#define U8 unsigned char
#define U16 unsigned short
#define U32 unsigned long
*/

/*---------------------------------------------------------------
 * A definition of register access macros
 *-------------------------------------------------------------*/
#define  _RegRd(type,ofs)     (*(volatile type*)(ofs))
#define  _RegWr(type,ofs,dt)  *(volatile type*)(ofs)=((type)(dt))

#define  RegRd8(ofs)   _RegRd(U8,(char*)pD->base_addr+(ofs))
#define  RegRd16(ofs)  _RegRd(U16,(char*)pD->base_addr+(ofs))
#define  RegRd32(ofs)  _RegRd(U32,(char*)pD->base_addr+(ofs))
#define  RegWr8(ofs,dt)   _RegWr(U8,(char*)pD->base_addr+(ofs),(dt))
#define  RegWr16(ofs,dt)  _RegWr(U16,(char*)pD->base_addr+(ofs),(dt))
#define  RegWr32(ofs,dt)  _RegWr(U32,(char*)pD->base_addr+(ofs),(dt))



#endif  /* _EP9213_ETH_H_ */

