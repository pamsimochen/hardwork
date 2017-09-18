/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-z63
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class ti_sysbios_family_c64p_tesla
{
    static final String VERS = "@(#) xdc-z63\n";

    static final Proto.Elm $$T_Bool = Proto.Elm.newBool();
    static final Proto.Elm $$T_Num = Proto.Elm.newNum();
    static final Proto.Elm $$T_Str = Proto.Elm.newStr();
    static final Proto.Elm $$T_Obj = Proto.Elm.newObj();

    static final Proto.Fxn $$T_Met = new Proto.Fxn(null, null, 0, -1, false);
    static final Proto.Map $$T_Map = new Proto.Map($$T_Obj);
    static final Proto.Arr $$T_Vec = new Proto.Arr($$T_Obj);

    static final XScriptO $$DEFAULT = Value.DEFAULT;
    static final Object $$UNDEF = Undefined.instance;

    static final Proto.Obj $$Package = (Proto.Obj)Global.get("$$Package");
    static final Proto.Obj $$Module = (Proto.Obj)Global.get("$$Module");
    static final Proto.Obj $$Instance = (Proto.Obj)Global.get("$$Instance");
    static final Proto.Obj $$Params = (Proto.Obj)Global.get("$$Params");

    static final Object $$objFldGet = Global.get("$$objFldGet");
    static final Object $$objFldSet = Global.get("$$objFldSet");
    static final Object $$proxyGet = Global.get("$$proxyGet");
    static final Object $$proxySet = Global.get("$$proxySet");
    static final Object $$delegGet = Global.get("$$delegGet");
    static final Object $$delegSet = Global.get("$$delegSet");

    Scriptable xdcO;
    Session ses;
    Value.Obj om;

    boolean isROV;
    boolean isCFG;

    Proto.Obj pkgP;
    Value.Obj pkgV;

    ArrayList<Object> imports = new ArrayList<Object>();
    ArrayList<Object> loggables = new ArrayList<Object>();
    ArrayList<Object> mcfgs = new ArrayList<Object>();
    ArrayList<Object> icfgs = new ArrayList<Object>();
    ArrayList<String> inherits = new ArrayList<String>();
    ArrayList<Object> proxies = new ArrayList<Object>();
    ArrayList<Object> sizes = new ArrayList<Object>();
    ArrayList<Object> tdefs = new ArrayList<Object>();

    void $$IMPORTS()
    {
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.interfaces");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.family.c64p");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.family.c64p.tesla", new Value.Obj("ti.sysbios.family.c64p.tesla", pkgP));
    }

    void TimerSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.c64p.tesla.TimerSupport", new Value.Obj("ti.sysbios.family.c64p.tesla.TimerSupport", po));
        pkgV.bind("TimerSupport", vo);
        // decls 
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc", new Proto.Enm("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.TimerSupport$$Emulation", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.Emulation", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.TimerSupport$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.Module_State", new Proto.Str(spo, false));
    }

    void Wugen$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Wugen.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.c64p.tesla.Wugen", new Value.Obj("ti.sysbios.family.c64p.tesla.Wugen", po));
        pkgV.bind("Wugen", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Wugen$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.Module_State", new Proto.Str(spo, false));
    }

    void Power$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power", new Value.Obj("ti.sysbios.family.c64p.tesla.Power", po));
        pkgV.bind("Power", vo);
        // decls 
        om.bind("ti.sysbios.family.c64p.tesla.Power.Suspend", new Proto.Enm("ti.sysbios.family.c64p.tesla.Power.Suspend"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power$$CpuSysRegs", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.Power.CpuSysRegs", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power$$IntcConfig", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.Power.IntcConfig", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power$$SyscConfig", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.Power.SyscConfig", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power$$UnicacheConfig", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.Power.UnicacheConfig", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power$$AmmuConfig", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.Power.AmmuConfig", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power$$EdmaConfig", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.Power.EdmaConfig", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.tesla.Power$$SubsystemContext", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.tesla.Power.SubsystemContext", new Proto.Str(spo, false));
    }

    void TimerSupport$$CONSTS()
    {
        // module TimerSupport
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_NONE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.tesla"), "ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_NONE", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_ARM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.tesla"), "ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_ARM", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_DSP", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.tesla"), "ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_DSP", xdc.services.intern.xsr.Enum.intValue(2L)+0));
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_BOTH", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.tesla"), "ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_BOTH", xdc.services.intern.xsr.Enum.intValue(3L)+0));
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport.enable", new Extern("ti_sysbios_family_c64p_tesla_TimerSupport_enable__E", "xdc_Void(*)(xdc_UInt,xdc_runtime_Error_Block*)", true, false));
    }

    void Wugen$$CONSTS()
    {
        // module Wugen
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.applyMasks", new Extern("ti_sysbios_family_c64p_tesla_Wugen_applyMasks__E", "xdc_Void(*)(xdc_UInt32[])", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.disableDma", new Extern("ti_sysbios_family_c64p_tesla_Wugen_disableDma__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.disableEvent", new Extern("ti_sysbios_family_c64p_tesla_Wugen_disableEvent__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.enableDma", new Extern("ti_sysbios_family_c64p_tesla_Wugen_enableDma__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.enableEvent", new Extern("ti_sysbios_family_c64p_tesla_Wugen_enableEvent__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.getMasks", new Extern("ti_sysbios_family_c64p_tesla_Wugen_getMasks__E", "xdc_Void(*)(xdc_UInt32[])", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.ierToMasks", new Extern("ti_sysbios_family_c64p_tesla_Wugen_ierToMasks__E", "xdc_Void(*)(xdc_UInt32[])", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.hwiRegisterHook", new Extern("ti_sysbios_family_c64p_tesla_Wugen_hwiRegisterHook__I", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.hwiCreateHook", new Extern("ti_sysbios_family_c64p_tesla_Wugen_hwiCreateHook__I", "xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.hwiBeginHook", new Extern("ti_sysbios_family_c64p_tesla_Wugen_hwiBeginHook__I", "xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.intToIrq", new Extern("ti_sysbios_family_c64p_tesla_Wugen_intToIrq__I", "xdc_UInt(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.evtToIrq", new Extern("ti_sysbios_family_c64p_tesla_Wugen_evtToIrq__I", "xdc_UInt(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Wugen.intToEvt", new Extern("ti_sysbios_family_c64p_tesla_Wugen_intToEvt__I", "xdc_UInt(*)(xdc_UInt)", true, false));
    }

    void Power$$CONSTS()
    {
        // module Power
        om.bind("ti.sysbios.family.c64p.tesla.Power.Suspend_RETENTION", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.c64p.tesla.Power.Suspend", "ti.sysbios.family.c64p.tesla"), "ti.sysbios.family.c64p.tesla.Power.Suspend_RETENTION", 0));
        om.bind("ti.sysbios.family.c64p.tesla.Power.Suspend_HIBERNATE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.c64p.tesla.Power.Suspend", "ti.sysbios.family.c64p.tesla"), "ti.sysbios.family.c64p.tesla.Power.Suspend_HIBERNATE", 1));
        om.bind("ti.sysbios.family.c64p.tesla.Power.idleCPU", new Extern("ti_sysbios_family_c64p_tesla_Power_idleCPU__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Power.suspend", new Extern("ti_sysbios_family_c64p_tesla_Power_suspend__E", "xdc_UInt(*)(ti_sysbios_family_c64p_tesla_Power_Suspend)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Power.resetFxn", new Extern("ti_sysbios_family_c64p_tesla_Power_resetFxn__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.c64p.tesla.Power.standby", new Extern("ti_sysbios_family_c64p_tesla_Power_standby__I", "xdc_UInt(*)(xdc_UInt32*)", true, false));
    }

    void TimerSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Wugen$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Power$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TimerSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Wugen$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Power$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TimerSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Emulation", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("suspSrc", "Nti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc;;0;1;2;3"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.TimerSupport.Emulation']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.TimerSupport.Emulation']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.TimerSupport.Emulation'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Module_State", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("suspSrc", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.TimerSupport.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.TimerSupport.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.TimerSupport.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Wugen$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.Module_State", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("hwiHookId", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.Wugen.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.Wugen.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.Wugen.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Power$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.Power.CpuSysRegs", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("AMR", "UInt32"));
        sizes.add(Global.newArray("CSR", "UInt32"));
        sizes.add(Global.newArray("IER", "UInt32"));
        sizes.add(Global.newArray("ISTP", "UInt32"));
        sizes.add(Global.newArray("IRP", "UInt32"));
        sizes.add(Global.newArray("SSR", "UInt32"));
        sizes.add(Global.newArray("GPLYB", "UInt32"));
        sizes.add(Global.newArray("GFPGFR", "UInt32"));
        sizes.add(Global.newArray("TSR", "UInt32"));
        sizes.add(Global.newArray("ITSR", "UInt32"));
        sizes.add(Global.newArray("IERR", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.Power.CpuSysRegs']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.Power.CpuSysRegs']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.Power.CpuSysRegs'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.Power.IntcConfig", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("EVTMASK0", "UInt32"));
        sizes.add(Global.newArray("EVTMASK1", "UInt32"));
        sizes.add(Global.newArray("EVTMASK2", "UInt32"));
        sizes.add(Global.newArray("EVTMASK3", "UInt32"));
        sizes.add(Global.newArray("EXPMASK0", "UInt32"));
        sizes.add(Global.newArray("EXPMASK1", "UInt32"));
        sizes.add(Global.newArray("EXPMASK2", "UInt32"));
        sizes.add(Global.newArray("EXPMASK3", "UInt32"));
        sizes.add(Global.newArray("INTMUX1", "UInt32"));
        sizes.add(Global.newArray("INTMUX2", "UInt32"));
        sizes.add(Global.newArray("INTMUX3", "UInt32"));
        sizes.add(Global.newArray("AEGMUX0", "UInt32"));
        sizes.add(Global.newArray("AEGMUX1", "UInt32"));
        sizes.add(Global.newArray("INTDMASK", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.Power.IntcConfig']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.Power.IntcConfig']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.Power.IntcConfig'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.Power.SyscConfig", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("SYSCONFIG", "UInt32"));
        sizes.add(Global.newArray("VBUSM2OCP", "UInt32"));
        sizes.add(Global.newArray("EDMA", "UInt32"));
        sizes.add(Global.newArray("CORE", "UInt32"));
        sizes.add(Global.newArray("IVA_ICTRL", "UInt32"));
        sizes.add(Global.newArray("IDLEDLY", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.Power.SyscConfig']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.Power.SyscConfig']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.Power.SyscConfig'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.Power.UnicacheConfig", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("CONFIG", "UInt32"));
        sizes.add(Global.newArray("OCP", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.Power.UnicacheConfig']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.Power.UnicacheConfig']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.Power.UnicacheConfig'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.Power.AmmuConfig", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("largeAddr", "A8;UInt32"));
        sizes.add(Global.newArray("largePolicy", "A8;UInt32"));
        sizes.add(Global.newArray("medAddr", "A7;UInt32"));
        sizes.add(Global.newArray("medPolicy", "A7;UInt32"));
        sizes.add(Global.newArray("smallAddr", "A3;UInt32"));
        sizes.add(Global.newArray("smallPolicy", "A3;UInt32"));
        sizes.add(Global.newArray("mmuConfig", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.Power.AmmuConfig']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.Power.AmmuConfig']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.Power.AmmuConfig'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.Power.EdmaConfig", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("CLKGDIS", "UInt32"));
        sizes.add(Global.newArray("DCHMAP", "A64;UInt32"));
        sizes.add(Global.newArray("QCHMAP", "A8;UInt32"));
        sizes.add(Global.newArray("DMAQNUM", "A8;UInt32"));
        sizes.add(Global.newArray("QDMAQNUM", "UInt32"));
        sizes.add(Global.newArray("QUETCMAP", "UInt32"));
        sizes.add(Global.newArray("QUEPRI", "UInt32"));
        sizes.add(Global.newArray("regionAccessBits", "A24;UInt32"));
        sizes.add(Global.newArray("QWMTHRA", "UInt32"));
        sizes.add(Global.newArray("AETCTL", "UInt32"));
        sizes.add(Global.newArray("IER", "UInt32"));
        sizes.add(Global.newArray("IERH", "UInt32"));
        sizes.add(Global.newArray("QEER", "UInt32"));
        sizes.add(Global.newArray("PaRAMs", "A1024;UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.Power.EdmaConfig']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.Power.EdmaConfig']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.Power.EdmaConfig'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.tesla.Power.SubsystemContext", "ti.sysbios.family.c64p.tesla");
        sizes.clear();
        sizes.add(Global.newArray("cpuSysRegs", "Sti.sysbios.family.c64p.tesla.Power;CpuSysRegs"));
        sizes.add(Global.newArray("configL1", "Sti.sysbios.family.c64p.tesla.Power;UnicacheConfig"));
        sizes.add(Global.newArray("configL2", "Sti.sysbios.family.c64p.tesla.Power;UnicacheConfig"));
        sizes.add(Global.newArray("configINTC", "Sti.sysbios.family.c64p.tesla.Power;IntcConfig"));
        sizes.add(Global.newArray("configSYSC", "Sti.sysbios.family.c64p.tesla.Power;SyscConfig"));
        sizes.add(Global.newArray("configAMMU", "Sti.sysbios.family.c64p.tesla.Power;AmmuConfig"));
        sizes.add(Global.newArray("configEDMA", "Sti.sysbios.family.c64p.tesla.Power;EdmaConfig"));
        sizes.add(Global.newArray("tscRunning", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.tesla.Power.SubsystemContext']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.tesla.Power.SubsystemContext']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.tesla.Power.SubsystemContext'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void TimerSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/c64p/tesla/TimerSupport.xs");
        om.bind("ti.sysbios.family.c64p.tesla.TimerSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Module", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.TimerSupport.Module", om.findStrict("ti.sysbios.interfaces.ITimerSupport.Module", "ti.sysbios.family.c64p.tesla"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("timer", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Emulation", "ti.sysbios.family.c64p.tesla"), false), Global.newArray(new Object[]{}), "w");
            po.addFld("controlMsuspendMux1Addr", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.TimerSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.TimerSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.TimerSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.TimerSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct TimerSupport.Emulation
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport$$Emulation", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.TimerSupport.Emulation", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("suspSrc", (Proto)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.tesla"), $$UNDEF, "w");
        // struct TimerSupport.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport$$Module_State", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.TimerSupport.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("suspSrc", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.tesla"), false), $$DEFAULT, "w");
    }

    void Wugen$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/c64p/tesla/Wugen.xs");
        om.bind("ti.sysbios.family.c64p.tesla.Wugen$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.Module", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Wugen.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.family.c64p.tesla"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.Wugen$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.Wugen$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.Wugen$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.Wugen$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct Wugen.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Wugen$$Module_State", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Wugen.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("hwiHookId", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void Power$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/c64p/tesla/Power.xs");
        om.bind("ti.sysbios.family.c64p.tesla.Power$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power.Module", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Power.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.family.c64p.tesla"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("tpccRegs", new Proto.Adr("xdc_Ptr", "Pv"), 0x01C00000L, "w");
            po.addFld("syscRegs", new Proto.Adr("xdc_Ptr", "Pv"), 0x01C20000L, "w");
            po.addFld("idle", $$T_Bool, false, "w");
            po.addFld("loadSegment", $$T_Str, "EXT_RAM", "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.Power$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.Power$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.Power$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.tesla.Power$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct Power.CpuSysRegs
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$CpuSysRegs", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Power.CpuSysRegs", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("AMR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("CSR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("IER", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("ISTP", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("IRP", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("SSR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("GPLYB", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("GFPGFR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("TSR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("ITSR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("IERR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct Power.IntcConfig
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$IntcConfig", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Power.IntcConfig", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("EVTMASK0", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("EVTMASK1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("EVTMASK2", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("EVTMASK3", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("EXPMASK0", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("EXPMASK1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("EXPMASK2", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("EXPMASK3", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("INTMUX1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("INTMUX2", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("INTMUX3", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("AEGMUX0", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("AEGMUX1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("INTDMASK", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct Power.SyscConfig
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$SyscConfig", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Power.SyscConfig", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("SYSCONFIG", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("VBUSM2OCP", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("EDMA", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("CORE", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("IVA_ICTRL", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("IDLEDLY", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct Power.UnicacheConfig
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$UnicacheConfig", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Power.UnicacheConfig", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("CONFIG", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("OCP", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct Power.AmmuConfig
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$AmmuConfig", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Power.AmmuConfig", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("largeAddr", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(8L)), $$DEFAULT, "w");
                po.addFld("largePolicy", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(8L)), $$DEFAULT, "w");
                po.addFld("medAddr", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(7L)), $$DEFAULT, "w");
                po.addFld("medPolicy", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(7L)), $$DEFAULT, "w");
                po.addFld("smallAddr", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(3L)), $$DEFAULT, "w");
                po.addFld("smallPolicy", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(3L)), $$DEFAULT, "w");
                po.addFld("mmuConfig", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct Power.EdmaConfig
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$EdmaConfig", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Power.EdmaConfig", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("CLKGDIS", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("DCHMAP", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(64L)), $$DEFAULT, "w");
                po.addFld("QCHMAP", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(8L)), $$DEFAULT, "w");
                po.addFld("DMAQNUM", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(8L)), $$DEFAULT, "w");
                po.addFld("QDMAQNUM", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("QUETCMAP", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("QUEPRI", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("regionAccessBits", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(24L)), $$DEFAULT, "w");
                po.addFld("QWMTHRA", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("AETCTL", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("IER", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("IERH", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("QEER", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("PaRAMs", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(1024L)), $$DEFAULT, "w");
        // struct Power.SubsystemContext
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$SubsystemContext", "ti.sysbios.family.c64p.tesla");
        po.init("ti.sysbios.family.c64p.tesla.Power.SubsystemContext", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("cpuSysRegs", (Proto)om.findStrict("ti.sysbios.family.c64p.tesla.Power.CpuSysRegs", "ti.sysbios.family.c64p.tesla"), $$DEFAULT, "w");
                po.addFld("configL1", (Proto)om.findStrict("ti.sysbios.family.c64p.tesla.Power.UnicacheConfig", "ti.sysbios.family.c64p.tesla"), $$DEFAULT, "w");
                po.addFld("configL2", (Proto)om.findStrict("ti.sysbios.family.c64p.tesla.Power.UnicacheConfig", "ti.sysbios.family.c64p.tesla"), $$DEFAULT, "w");
                po.addFld("configINTC", (Proto)om.findStrict("ti.sysbios.family.c64p.tesla.Power.IntcConfig", "ti.sysbios.family.c64p.tesla"), $$DEFAULT, "w");
                po.addFld("configSYSC", (Proto)om.findStrict("ti.sysbios.family.c64p.tesla.Power.SyscConfig", "ti.sysbios.family.c64p.tesla"), $$DEFAULT, "w");
                po.addFld("configAMMU", (Proto)om.findStrict("ti.sysbios.family.c64p.tesla.Power.AmmuConfig", "ti.sysbios.family.c64p.tesla"), $$DEFAULT, "w");
                po.addFld("configEDMA", (Proto)om.findStrict("ti.sysbios.family.c64p.tesla.Power.EdmaConfig", "ti.sysbios.family.c64p.tesla"), $$DEFAULT, "w");
                po.addFld("tscRunning", $$T_Bool, $$UNDEF, "w");
    }

    void TimerSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport", "ti.sysbios.family.c64p.tesla");
        vo.bind("Emulation$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.TimerSupport.Emulation", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport$$Emulation", "ti.sysbios.family.c64p.tesla");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.TimerSupport.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport$$Module_State", "ti.sysbios.family.c64p.tesla");
        po.bind("suspSrc$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_SuspSrc", "isScalar", true));
    }

    void Wugen$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Wugen", "ti.sysbios.family.c64p.tesla");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.Wugen.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Wugen$$Module_State", "ti.sysbios.family.c64p.tesla");
    }

    void Power$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power", "ti.sysbios.family.c64p.tesla");
        vo.bind("CpuSysRegs$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.Power.CpuSysRegs", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$CpuSysRegs", "ti.sysbios.family.c64p.tesla");
        vo.bind("IntcConfig$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.Power.IntcConfig", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$IntcConfig", "ti.sysbios.family.c64p.tesla");
        vo.bind("SyscConfig$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.Power.SyscConfig", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$SyscConfig", "ti.sysbios.family.c64p.tesla");
        vo.bind("UnicacheConfig$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.Power.UnicacheConfig", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$UnicacheConfig", "ti.sysbios.family.c64p.tesla");
        vo.bind("AmmuConfig$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.Power.AmmuConfig", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$AmmuConfig", "ti.sysbios.family.c64p.tesla");
        vo.bind("EdmaConfig$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.Power.EdmaConfig", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$EdmaConfig", "ti.sysbios.family.c64p.tesla");
        vo.bind("SubsystemContext$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.tesla.Power.SubsystemContext", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power$$SubsystemContext", "ti.sysbios.family.c64p.tesla");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.family.c64p.tesla.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.family.c64p.tesla"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/c64p/tesla/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.family.c64p.tesla"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.family.c64p.tesla"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.family.c64p.tesla"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.family.c64p.tesla"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.family.c64p.tesla"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.family.c64p.tesla"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.family.c64p.tesla", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.family.c64p.tesla");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.family.c64p.tesla.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.family.c64p.tesla'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.c64p.tesla.ae64T',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.c64p.tesla.ae64T', {target: 'ti.targets.elf.C64T', suffix: 'e64T'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void TimerSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport", "ti.sysbios.family.c64p.tesla");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Module", "ti.sysbios.family.c64p.tesla");
        vo.init2(po, "ti.sysbios.family.c64p.tesla.TimerSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport$$capsule", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.c64p.tesla", "ti.sysbios.family.c64p.tesla"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        icfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("SuspSrc", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.tesla"));
        vo.bind("Emulation", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Emulation", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Emulation", "ti.sysbios.family.c64p.tesla"));
        mcfgs.add("timer");
        mcfgs.add("controlMsuspendMux1Addr");
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Module_State", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.Module_State", "ti.sysbios.family.c64p.tesla"));
        vo.bind("SuspSrc_NONE", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_NONE", "ti.sysbios.family.c64p.tesla"));
        vo.bind("SuspSrc_ARM", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_ARM", "ti.sysbios.family.c64p.tesla"));
        vo.bind("SuspSrc_DSP", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_DSP", "ti.sysbios.family.c64p.tesla"));
        vo.bind("SuspSrc_BOTH", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.SuspSrc_BOTH", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.c64p.tesla")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("enable", om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport.enable", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_c64p_tesla_TimerSupport_Module__startupDone__E", "ti_sysbios_family_c64p_tesla_TimerSupport_enable__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("TimerSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TimerSupport");
    }

    void Wugen$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Wugen", "ti.sysbios.family.c64p.tesla");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.Module", "ti.sysbios.family.c64p.tesla");
        vo.init2(po, "ti.sysbios.family.c64p.tesla.Wugen", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen$$capsule", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.c64p.tesla", "ti.sysbios.family.c64p.tesla"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        icfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.Module_State", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.Module_State", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.c64p.tesla")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("applyMasks", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.applyMasks", "ti.sysbios.family.c64p.tesla"));
        vo.bind("disableDma", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.disableDma", "ti.sysbios.family.c64p.tesla"));
        vo.bind("disableEvent", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.disableEvent", "ti.sysbios.family.c64p.tesla"));
        vo.bind("enableDma", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.enableDma", "ti.sysbios.family.c64p.tesla"));
        vo.bind("enableEvent", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.enableEvent", "ti.sysbios.family.c64p.tesla"));
        vo.bind("getMasks", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.getMasks", "ti.sysbios.family.c64p.tesla"));
        vo.bind("ierToMasks", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.ierToMasks", "ti.sysbios.family.c64p.tesla"));
        vo.bind("hwiRegisterHook", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.hwiRegisterHook", "ti.sysbios.family.c64p.tesla"));
        vo.bind("hwiCreateHook", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.hwiCreateHook", "ti.sysbios.family.c64p.tesla"));
        vo.bind("hwiBeginHook", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.hwiBeginHook", "ti.sysbios.family.c64p.tesla"));
        vo.bind("intToIrq", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.intToIrq", "ti.sysbios.family.c64p.tesla"));
        vo.bind("evtToIrq", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.evtToIrq", "ti.sysbios.family.c64p.tesla"));
        vo.bind("intToEvt", om.findStrict("ti.sysbios.family.c64p.tesla.Wugen.intToEvt", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_c64p_tesla_Wugen_Module__startupDone__E", "ti_sysbios_family_c64p_tesla_Wugen_applyMasks__E", "ti_sysbios_family_c64p_tesla_Wugen_disableDma__E", "ti_sysbios_family_c64p_tesla_Wugen_disableEvent__E", "ti_sysbios_family_c64p_tesla_Wugen_enableDma__E", "ti_sysbios_family_c64p_tesla_Wugen_enableEvent__E", "ti_sysbios_family_c64p_tesla_Wugen_getMasks__E", "ti_sysbios_family_c64p_tesla_Wugen_ierToMasks__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "applyMasks", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "disableDma", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "disableEvent", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "enableDma", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "enableEvent", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "getMasks", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "ierToMasks", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Wugen", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Wugen");
    }

    void Power$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power", "ti.sysbios.family.c64p.tesla");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.tesla.Power.Module", "ti.sysbios.family.c64p.tesla");
        vo.init2(po, "ti.sysbios.family.c64p.tesla.Power", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.c64p.tesla.Power$$capsule", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.c64p.tesla", "ti.sysbios.family.c64p.tesla"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        icfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("Suspend", om.findStrict("ti.sysbios.family.c64p.tesla.Power.Suspend", "ti.sysbios.family.c64p.tesla"));
        mcfgs.add("tpccRegs");
        mcfgs.add("syscRegs");
        mcfgs.add("idle");
        vo.bind("CpuSysRegs", om.findStrict("ti.sysbios.family.c64p.tesla.Power.CpuSysRegs", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.Power.CpuSysRegs", "ti.sysbios.family.c64p.tesla"));
        vo.bind("IntcConfig", om.findStrict("ti.sysbios.family.c64p.tesla.Power.IntcConfig", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.Power.IntcConfig", "ti.sysbios.family.c64p.tesla"));
        vo.bind("SyscConfig", om.findStrict("ti.sysbios.family.c64p.tesla.Power.SyscConfig", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.Power.SyscConfig", "ti.sysbios.family.c64p.tesla"));
        vo.bind("UnicacheConfig", om.findStrict("ti.sysbios.family.c64p.tesla.Power.UnicacheConfig", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.Power.UnicacheConfig", "ti.sysbios.family.c64p.tesla"));
        vo.bind("AmmuConfig", om.findStrict("ti.sysbios.family.c64p.tesla.Power.AmmuConfig", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.Power.AmmuConfig", "ti.sysbios.family.c64p.tesla"));
        vo.bind("EdmaConfig", om.findStrict("ti.sysbios.family.c64p.tesla.Power.EdmaConfig", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.Power.EdmaConfig", "ti.sysbios.family.c64p.tesla"));
        vo.bind("SubsystemContext", om.findStrict("ti.sysbios.family.c64p.tesla.Power.SubsystemContext", "ti.sysbios.family.c64p.tesla"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.tesla.Power.SubsystemContext", "ti.sysbios.family.c64p.tesla"));
        vo.bind("Suspend_RETENTION", om.findStrict("ti.sysbios.family.c64p.tesla.Power.Suspend_RETENTION", "ti.sysbios.family.c64p.tesla"));
        vo.bind("Suspend_HIBERNATE", om.findStrict("ti.sysbios.family.c64p.tesla.Power.Suspend_HIBERNATE", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.c64p.tesla")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("idleCPU", om.findStrict("ti.sysbios.family.c64p.tesla.Power.idleCPU", "ti.sysbios.family.c64p.tesla"));
        vo.bind("suspend", om.findStrict("ti.sysbios.family.c64p.tesla.Power.suspend", "ti.sysbios.family.c64p.tesla"));
        vo.bind("resetFxn", om.findStrict("ti.sysbios.family.c64p.tesla.Power.resetFxn", "ti.sysbios.family.c64p.tesla"));
        vo.bind("standby", om.findStrict("ti.sysbios.family.c64p.tesla.Power.standby", "ti.sysbios.family.c64p.tesla"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_c64p_tesla_Power_Module__startupDone__E", "ti_sysbios_family_c64p_tesla_Power_idleCPU__E", "ti_sysbios_family_c64p_tesla_Power_suspend__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Power", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Power");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.c64p.tesla.TimerSupport", "ti.sysbios.family.c64p.tesla"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.c64p.tesla.Wugen", "ti.sysbios.family.c64p.tesla"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.c64p.tesla.Power", "ti.sysbios.family.c64p.tesla"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.family.c64p.tesla.TimerSupport")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.c64p.tesla.Wugen")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.c64p.tesla.Power")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.family.c64p.tesla")).add(pkgV);
    }

    public void exec( Scriptable xdcO, Session ses )
    {
        this.xdcO = xdcO;
        this.ses = ses;
        om = (Value.Obj)xdcO.get("om", null);

        Object o = om.geto("$name");
        String s = o instanceof String ? (String)o : null;
        isCFG = s != null && s.equals("cfg");
        isROV = s != null && s.equals("rov");

        $$IMPORTS();
        $$OBJECTS();
        TimerSupport$$OBJECTS();
        Wugen$$OBJECTS();
        Power$$OBJECTS();
        TimerSupport$$CONSTS();
        Wugen$$CONSTS();
        Power$$CONSTS();
        TimerSupport$$CREATES();
        Wugen$$CREATES();
        Power$$CREATES();
        TimerSupport$$FUNCTIONS();
        Wugen$$FUNCTIONS();
        Power$$FUNCTIONS();
        TimerSupport$$SIZES();
        Wugen$$SIZES();
        Power$$SIZES();
        TimerSupport$$TYPES();
        Wugen$$TYPES();
        Power$$TYPES();
        if (isROV) {
            TimerSupport$$ROV();
            Wugen$$ROV();
            Power$$ROV();
        }//isROV
        $$SINGLETONS();
        TimerSupport$$SINGLETONS();
        Wugen$$SINGLETONS();
        Power$$SINGLETONS();
        $$INITIALIZATION();
    }
}
