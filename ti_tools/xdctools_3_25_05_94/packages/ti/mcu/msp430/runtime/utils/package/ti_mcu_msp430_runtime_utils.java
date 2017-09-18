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

public class ti_mcu_msp430_runtime_utils
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
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.utils.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.mcu.msp430.runtime.utils", new Value.Obj("ti.mcu.msp430.runtime.utils", pkgP));
    }

    void Atomic$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.utils.Atomic.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.utils.Atomic", new Value.Obj("ti.mcu.msp430.runtime.utils.Atomic", po));
        pkgV.bind("Atomic", vo);
        // decls 
    }

    void Clock$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.utils.Clock.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.utils.Clock", new Value.Obj("ti.mcu.msp430.runtime.utils.Clock", po));
        pkgV.bind("Clock", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.utils.Clock$$TimerRegs", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.utils.Clock.TimerRegs", new Proto.Str(spo, false));
    }

    void Stack$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.utils.Stack.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.utils.Stack", new Value.Obj("ti.mcu.msp430.runtime.utils.Stack", po));
        pkgV.bind("Stack", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.utils.Stack$$Status", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.utils.Stack.Status", new Proto.Str(spo, false));
    }

    void Led$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.utils.Led.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.utils.Led", new Value.Obj("ti.mcu.msp430.runtime.utils.Led", po));
        pkgV.bind("Led", vo);
        // decls 
    }

    void Button$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.utils.Button.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.utils.Button", new Value.Obj("ti.mcu.msp430.runtime.utils.Button", po));
        pkgV.bind("Button", vo);
        // decls 
    }

    void Atomic$$CONSTS()
    {
        // module Atomic
        om.bind("ti.mcu.msp430.runtime.utils.Atomic.inc8", new Extern("ti_mcu_msp430_runtime_utils_Atomic_inc8__E", "xdc_Void(*)(xdc_UInt8*)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Atomic.dec8", new Extern("ti_mcu_msp430_runtime_utils_Atomic_dec8__E", "xdc_Void(*)(xdc_UInt8*)", true, false));
    }

    void Clock$$CONSTS()
    {
        // module Clock
        om.bind("ti.mcu.msp430.runtime.utils.Clock.TA2", 0x160L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.TA3", 0x160L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.TA5", 0x160L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.TB3", 0x180L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.TB7", 0x180L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.T1A2", 0x180L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.T0A3", 0x340L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.T0A5", 0x340L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.T1A3", 0x380L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.T1A5", 0x380L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.T2A3", 0x400L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.T0B7", 0x3d0L);
        om.bind("ti.mcu.msp430.runtime.utils.Clock.delay", new Extern("ti_mcu_msp430_runtime_utils_Clock_delay__E", "xdc_Void(*)(xdc_UInt16)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Clock.sleep", new Extern("ti_mcu_msp430_runtime_utils_Clock_sleep__E", "xdc_Void(*)(xdc_UInt16,xdc_UInt)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Clock.getTime", new Extern("ti_mcu_msp430_runtime_utils_Clock_getTime__E", "xdc_UInt16(*)(xdc_Void)", true, false));
    }

    void Stack$$CONSTS()
    {
        // module Stack
        om.bind("ti.mcu.msp430.runtime.utils.Stack.check", new Extern("ti_mcu_msp430_runtime_utils_Stack_check__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Stack.fill", new Extern("ti_mcu_msp430_runtime_utils_Stack_fill__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Stack.getUnused", new Extern("ti_mcu_msp430_runtime_utils_Stack_getUnused__E", "xdc_Int(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Stack.getStatus", new Extern("ti_mcu_msp430_runtime_utils_Stack_getStatus__E", "xdc_Void(*)(ti_mcu_msp430_runtime_utils_Stack_Status*)", true, false));
    }

    void Led$$CONSTS()
    {
        // module Led
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT0", 0x11L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT1", 0x21L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT2", 0x29L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT3", 0x19L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT4", 0x1DL);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT5", 0x31L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT6", 0x35L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT7", 0x3AL);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT8", 0x3BL);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTA_L", 0x202L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTA_H", 0x203L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTB_L", 0x222L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTB_H", 0x223L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTC_L", 0x242L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTC_H", 0x243L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTD_L", 0x262L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTD_H", 0x263L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTE_L", 0x282L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTE_H", 0x283L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTF_L", 0x2A2L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT0DIR", 0x12L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT1DIR", 0x22L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT2DIR", 0x2AL);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT3DIR", 0x1AL);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT4DIR", 0x1EL);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT5DIR", 0x32L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT6DIR", 0x36L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT7DIR", 0x3CL);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORT8DIR", 0x3DL);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTADIR_L", 0x204L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTADIR_H", 0x205L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTBDIR_L", 0x224L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTBDIR_H", 0x225L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTCDIR_L", 0x244L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTCDIR_H", 0x245L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTDDIR_L", 0x264L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTDDIR_H", 0x265L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTEDIR_L", 0x284L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTEDIR_H", 0x285L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.PORTFDIR_L", 0x2A4L);
        om.bind("ti.mcu.msp430.runtime.utils.Led.off", new Extern("ti_mcu_msp430_runtime_utils_Led_off__E", "xdc_Void(*)(xdc_Bits8)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Led.on", new Extern("ti_mcu_msp430_runtime_utils_Led_on__E", "xdc_Void(*)(xdc_Bits8)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Led.toggle", new Extern("ti_mcu_msp430_runtime_utils_Led_toggle__E", "xdc_Void(*)(xdc_Bits8)", true, false));
    }

    void Button$$CONSTS()
    {
        // module Button
        om.bind("ti.mcu.msp430.runtime.utils.Button.enable", new Extern("ti_mcu_msp430_runtime_utils_Button_enable__E", "xdc_Void(*)(xdc_Bits8)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Button.disable", new Extern("ti_mcu_msp430_runtime_utils_Button_disable__E", "xdc_Void(*)(xdc_Bits8)", true, false));
        om.bind("ti.mcu.msp430.runtime.utils.Button.read", new Extern("ti_mcu_msp430_runtime_utils_Button_read__E", "xdc_Bits8(*)(xdc_Bits8)", true, false));
    }

    void Atomic$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Clock$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Stack$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Led$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Button$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Atomic$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Clock$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Stack$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Led$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Button$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Atomic$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Clock$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.mcu.msp430.runtime.utils.Clock.TimerRegs", "ti.mcu.msp430.runtime.utils");
        sizes.clear();
        sizes.add(Global.newArray("CTL", "UInt16"));
        sizes.add(Global.newArray("CCTL", "A7;UInt16"));
        sizes.add(Global.newArray("R", "UInt16"));
        sizes.add(Global.newArray("CCR", "A7;UInt16"));
        sizes.add(Global.newArray("EX0", "UInt16"));
        sizes.add(Global.newArray("IV", "UInt16"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.mcu.msp430.runtime.utils.Clock.TimerRegs']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.mcu.msp430.runtime.utils.Clock.TimerRegs']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.mcu.msp430.runtime.utils.Clock.TimerRegs'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Stack$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.mcu.msp430.runtime.utils.Stack.Status", "ti.mcu.msp430.runtime.utils");
        sizes.clear();
        sizes.add(Global.newArray("unused", "TInt"));
        sizes.add(Global.newArray("used", "TInt"));
        sizes.add(Global.newArray("curDepth", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.mcu.msp430.runtime.utils.Stack.Status']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.mcu.msp430.runtime.utils.Stack.Status']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.mcu.msp430.runtime.utils.Stack.Status'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Led$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Button$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Atomic$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Atomic.Module", "ti.mcu.msp430.runtime.utils");
        po.init("ti.mcu.msp430.runtime.utils.Atomic.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.mcu.msp430.runtime.utils"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void Clock$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Clock.Module", "ti.mcu.msp430.runtime.utils");
        po.init("ti.mcu.msp430.runtime.utils.Clock.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.mcu.msp430.runtime.utils"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("TA2", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x160L, "rh");
                po.addFld("TA3", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x160L, "rh");
                po.addFld("TA5", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x160L, "rh");
                po.addFld("TB3", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x180L, "rh");
                po.addFld("TB7", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x180L, "rh");
                po.addFld("T1A2", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x180L, "rh");
                po.addFld("T0A3", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x340L, "rh");
                po.addFld("T0A5", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x340L, "rh");
                po.addFld("T1A3", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x380L, "rh");
                po.addFld("T1A5", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x380L, "rh");
                po.addFld("T2A3", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x400L, "rh");
                po.addFld("T0B7", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x3d0L, "rh");
        if (isCFG) {
            po.addFld("TIMER", new Proto.Adr("ti_mcu_msp430_runtime_utils_Clock_TimerRegs*", "PS"), 0x180L, "w");
            po.addFld("fastClockKHz", Proto.Elm.newCNum("(xdc_Int)"), 8000L, "w");
            po.addFld("slowClockHz", Proto.Elm.newCNum("(xdc_Int)"), 12000L, "w");
        }//isCFG
        // typedef Clock.TimeValue
        om.bind("ti.mcu.msp430.runtime.utils.Clock.TimeValue", Proto.Elm.newCNum("(xdc_UInt16)"));
        // struct Clock.TimerRegs
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Clock$$TimerRegs", "ti.mcu.msp430.runtime.utils");
        po.init("ti.mcu.msp430.runtime.utils.Clock.TimerRegs", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("CTL", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("CCTL", new Proto.Arr(Proto.Elm.newCNum("(xdc_Bits16)"), false, xdc.services.intern.xsr.Enum.intValue(7L)), $$DEFAULT, "w");
                po.addFld("R", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("CCR", new Proto.Arr(Proto.Elm.newCNum("(xdc_Bits16)"), false, xdc.services.intern.xsr.Enum.intValue(7L)), $$DEFAULT, "w");
                po.addFld("EX0", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("IV", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
    }

    void Stack$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Stack.Module", "ti.mcu.msp430.runtime.utils");
        po.init("ti.mcu.msp430.runtime.utils.Stack.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.mcu.msp430.runtime.utils"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("UNUSED", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.mcu.msp430.runtime.utils"), Global.newObject("mask", 0x0100L, "msg", "unused stack space = %d words"), "w");
        }//isCFG
        // struct Stack.Status
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Stack$$Status", "ti.mcu.msp430.runtime.utils");
        po.init("ti.mcu.msp430.runtime.utils.Stack.Status", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("unused", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("used", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("curDepth", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void Led$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Led.Module", "ti.mcu.msp430.runtime.utils");
        po.init("ti.mcu.msp430.runtime.utils.Led.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.mcu.msp430.runtime.utils"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("PORT0", new Proto.Adr("xdc_Ptr", "Pv"), 0x11L, "rh");
                po.addFld("PORT1", new Proto.Adr("xdc_Ptr", "Pv"), 0x21L, "rh");
                po.addFld("PORT2", new Proto.Adr("xdc_Ptr", "Pv"), 0x29L, "rh");
                po.addFld("PORT3", new Proto.Adr("xdc_Ptr", "Pv"), 0x19L, "rh");
                po.addFld("PORT4", new Proto.Adr("xdc_Ptr", "Pv"), 0x1DL, "rh");
                po.addFld("PORT5", new Proto.Adr("xdc_Ptr", "Pv"), 0x31L, "rh");
                po.addFld("PORT6", new Proto.Adr("xdc_Ptr", "Pv"), 0x35L, "rh");
                po.addFld("PORT7", new Proto.Adr("xdc_Ptr", "Pv"), 0x3AL, "rh");
                po.addFld("PORT8", new Proto.Adr("xdc_Ptr", "Pv"), 0x3BL, "rh");
                po.addFld("PORTA_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x202L, "rh");
                po.addFld("PORTA_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x203L, "rh");
                po.addFld("PORTB_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x222L, "rh");
                po.addFld("PORTB_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x223L, "rh");
                po.addFld("PORTC_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x242L, "rh");
                po.addFld("PORTC_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x243L, "rh");
                po.addFld("PORTD_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x262L, "rh");
                po.addFld("PORTD_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x263L, "rh");
                po.addFld("PORTE_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x282L, "rh");
                po.addFld("PORTE_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x283L, "rh");
                po.addFld("PORTF_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x2A2L, "rh");
                po.addFld("PORT0DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x12L, "rh");
                po.addFld("PORT1DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x22L, "rh");
                po.addFld("PORT2DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x2AL, "rh");
                po.addFld("PORT3DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x1AL, "rh");
                po.addFld("PORT4DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x1EL, "rh");
                po.addFld("PORT5DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x32L, "rh");
                po.addFld("PORT6DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x36L, "rh");
                po.addFld("PORT7DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x3CL, "rh");
                po.addFld("PORT8DIR", new Proto.Adr("xdc_Ptr", "Pv"), 0x3DL, "rh");
                po.addFld("PORTADIR_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x204L, "rh");
                po.addFld("PORTADIR_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x205L, "rh");
                po.addFld("PORTBDIR_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x224L, "rh");
                po.addFld("PORTBDIR_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x225L, "rh");
                po.addFld("PORTCDIR_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x244L, "rh");
                po.addFld("PORTCDIR_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x245L, "rh");
                po.addFld("PORTDDIR_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x264L, "rh");
                po.addFld("PORTDDIR_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x265L, "rh");
                po.addFld("PORTEDIR_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x284L, "rh");
                po.addFld("PORTEDIR_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x285L, "rh");
                po.addFld("PORTFDIR_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x2A4L, "rh");
        if (isCFG) {
            po.addFld("PORT", new Proto.Adr("xdc_Ptr", "Pv"), 0x21L, "w");
            po.addFld("DIRCFG", new Proto.Adr("xdc_Ptr", "Pv"), 0x22L, "w");
            po.addFld("RED", Proto.Elm.newCNum("(xdc_Bits8)"), 0x1L, "w");
            po.addFld("GREEN", Proto.Elm.newCNum("(xdc_Bits8)"), 0x2L, "w");
        }//isCFG
    }

    void Button$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Button.Module", "ti.mcu.msp430.runtime.utils");
        po.init("ti.mcu.msp430.runtime.utils.Button.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.mcu.msp430.runtime.utils"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("PORT0", new Proto.Adr("xdc_Ptr", "Pv"), 0x11L, "rh");
            po.addFld("PORT1", new Proto.Adr("xdc_Ptr", "Pv"), 0x21L, "rh");
            po.addFld("PORT2", new Proto.Adr("xdc_Ptr", "Pv"), 0x29L, "rh");
            po.addFld("PORT0_IE", new Proto.Adr("xdc_Ptr", "Pv"), 0x15L, "rh");
            po.addFld("PORT0_IFG", new Proto.Adr("xdc_Ptr", "Pv"), 0x13L, "rh");
            po.addFld("PORT0_OUT", new Proto.Adr("xdc_Ptr", "Pv"), 0x11L, "rh");
            po.addFld("PORT0_REN", new Proto.Adr("xdc_Ptr", "Pv"), 0x11L, "rh");
            po.addFld("PORT1_IE", new Proto.Adr("xdc_Ptr", "Pv"), 0x25L, "rh");
            po.addFld("PORT1_IFG", new Proto.Adr("xdc_Ptr", "Pv"), 0x23L, "rh");
            po.addFld("PORT1_OUT", new Proto.Adr("xdc_Ptr", "Pv"), 0x21L, "rh");
            po.addFld("PORT1_REN", new Proto.Adr("xdc_Ptr", "Pv"), 0x27L, "rh");
            po.addFld("PORT2_IE", new Proto.Adr("xdc_Ptr", "Pv"), 0x2DL, "rh");
            po.addFld("PORT2_IFG", new Proto.Adr("xdc_Ptr", "Pv"), 0x2BL, "rh");
            po.addFld("PORT2_OUT", new Proto.Adr("xdc_Ptr", "Pv"), 0x29L, "rh");
            po.addFld("PORT2_REN", new Proto.Adr("xdc_Ptr", "Pv"), 0x2FL, "rh");
            po.addFld("PORTA_IE_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x21AL, "rh");
            po.addFld("PORTA_IFG_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x21CL, "rh");
            po.addFld("PORTA_OUT_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x202L, "rh");
            po.addFld("PORTA_REN_L", new Proto.Adr("xdc_Ptr", "Pv"), 0x206L, "rh");
            po.addFld("PORTA_IE_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x21BL, "rh");
            po.addFld("PORTA_IFG_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x21DL, "rh");
            po.addFld("PORTA_OUT_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x203L, "rh");
            po.addFld("PORTA_REN_H", new Proto.Adr("xdc_Ptr", "Pv"), 0x207L, "rh");
            po.addFld("PORT", new Proto.Adr("xdc_Ptr", "Pv"), 0x21L, "w");
            po.addFld("PORT_IE", new Proto.Adr("xdc_Ptr", "Pv"), 0x25L, "w");
            po.addFld("PORT_OUT", new Proto.Adr("xdc_Ptr", "Pv"), 0x21L, "w");
            po.addFld("PORT_REN", new Proto.Adr("xdc_Ptr", "Pv"), 0x27L, "w");
            po.addFld("PORT_IFG", new Proto.Adr("xdc_Ptr", "Pv"), 0x23L, "w");
            po.addFld("SWITCH1", Proto.Elm.newCNum("(xdc_Bits8)"), 0x1L, "w");
            po.addFld("SWITCH2", Proto.Elm.newCNum("(xdc_Bits8)"), 0x2L, "w");
        }//isCFG
    }

    void Atomic$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Atomic", "ti.mcu.msp430.runtime.utils");
    }

    void Clock$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Clock", "ti.mcu.msp430.runtime.utils");
        vo.bind("TimerRegs$fetchDesc", Global.newObject("type", "ti.mcu.msp430.runtime.utils.Clock.TimerRegs", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Clock$$TimerRegs", "ti.mcu.msp430.runtime.utils");
    }

    void Stack$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Stack", "ti.mcu.msp430.runtime.utils");
        vo.bind("Status$fetchDesc", Global.newObject("type", "ti.mcu.msp430.runtime.utils.Stack.Status", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Stack$$Status", "ti.mcu.msp430.runtime.utils");
    }

    void Led$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Led", "ti.mcu.msp430.runtime.utils");
    }

    void Button$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Button", "ti.mcu.msp430.runtime.utils");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.mcu.msp430.runtime.utils.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.mcu.msp430.runtime.utils"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/mcu/msp430/runtime/utils/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.mcu.msp430.runtime.utils"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.mcu.msp430.runtime.utils"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.mcu.msp430.runtime.utils"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.mcu.msp430.runtime.utils"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.mcu.msp430.runtime.utils"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.mcu.msp430.runtime.utils"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.mcu.msp430.runtime.utils", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.mcu.msp430.runtime.utils");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.mcu.msp430.runtime.utils.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.mcu.msp430.runtime.utils'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.a430',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.a430X',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.a430XS',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.ae430',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.ae430X',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.ae430XS',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.ar430',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.ar430XL',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.utils.ar430XS',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.a430', {target: 'ti.targets.msp430.MSP430', suffix: '430'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.a430X', {target: 'ti.targets.msp430.MSP430X', suffix: '430X'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.a430XS', {target: 'ti.targets.msp430.MSP430X_small', suffix: '430XS'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.ae430', {target: 'ti.targets.msp430.elf.MSP430', suffix: 'e430'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.ae430X', {target: 'ti.targets.msp430.elf.MSP430X', suffix: 'e430X'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.ae430XS', {target: 'ti.targets.msp430.elf.MSP430X_small', suffix: 'e430XS'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.ar430', {target: 'iar.targets.msp430.MSP430', suffix: 'r430'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.ar430XL', {target: 'iar.targets.msp430.MSP430X_large', suffix: 'r430XL'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.utils.ar430XS', {target: 'iar.targets.msp430.MSP430X_small', suffix: 'r430XS'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Atomic$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Atomic", "ti.mcu.msp430.runtime.utils");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Atomic.Module", "ti.mcu.msp430.runtime.utils");
        vo.init2(po, "ti.mcu.msp430.runtime.utils.Atomic", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime.utils", "ti.mcu.msp430.runtime.utils"));
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
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime.utils")).add(vo);
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
        vo.bind("inc8", om.findStrict("ti.mcu.msp430.runtime.utils.Atomic.inc8", "ti.mcu.msp430.runtime.utils"));
        vo.bind("dec8", om.findStrict("ti.mcu.msp430.runtime.utils.Atomic.dec8", "ti.mcu.msp430.runtime.utils"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_utils_Atomic_Module__startupDone__E", "ti_mcu_msp430_runtime_utils_Atomic_inc8__E", "ti_mcu_msp430_runtime_utils_Atomic_dec8__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Atomic", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Atomic");
    }

    void Clock$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Clock", "ti.mcu.msp430.runtime.utils");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Clock.Module", "ti.mcu.msp430.runtime.utils");
        vo.init2(po, "ti.mcu.msp430.runtime.utils.Clock", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime.utils", "ti.mcu.msp430.runtime.utils"));
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
        vo.bind("TimeValue", om.findStrict("ti.mcu.msp430.runtime.utils.Clock.TimeValue", "ti.mcu.msp430.runtime.utils"));
        vo.bind("TimerRegs", om.findStrict("ti.mcu.msp430.runtime.utils.Clock.TimerRegs", "ti.mcu.msp430.runtime.utils"));
        tdefs.add(om.findStrict("ti.mcu.msp430.runtime.utils.Clock.TimerRegs", "ti.mcu.msp430.runtime.utils"));
        mcfgs.add("TIMER");
        mcfgs.add("fastClockKHz");
        mcfgs.add("slowClockHz");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime.utils")).add(vo);
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
        vo.bind("delay", om.findStrict("ti.mcu.msp430.runtime.utils.Clock.delay", "ti.mcu.msp430.runtime.utils"));
        vo.bind("sleep", om.findStrict("ti.mcu.msp430.runtime.utils.Clock.sleep", "ti.mcu.msp430.runtime.utils"));
        vo.bind("getTime", om.findStrict("ti.mcu.msp430.runtime.utils.Clock.getTime", "ti.mcu.msp430.runtime.utils"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_utils_Clock_Module__startupDone__E", "ti_mcu_msp430_runtime_utils_Clock_delay__E", "ti_mcu_msp430_runtime_utils_Clock_sleep__E", "ti_mcu_msp430_runtime_utils_Clock_getTime__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "delay", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "sleep", "entry", "0x%x, 0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "getTime", "entry", "", "exit", "%d"));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Clock", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Clock");
    }

    void Stack$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Stack", "ti.mcu.msp430.runtime.utils");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Stack.Module", "ti.mcu.msp430.runtime.utils");
        vo.init2(po, "ti.mcu.msp430.runtime.utils.Stack", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime.utils", "ti.mcu.msp430.runtime.utils"));
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
        vo.bind("Status", om.findStrict("ti.mcu.msp430.runtime.utils.Stack.Status", "ti.mcu.msp430.runtime.utils"));
        tdefs.add(om.findStrict("ti.mcu.msp430.runtime.utils.Stack.Status", "ti.mcu.msp430.runtime.utils"));
        mcfgs.add("UNUSED");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime.utils")).add(vo);
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
        vo.bind("check", om.findStrict("ti.mcu.msp430.runtime.utils.Stack.check", "ti.mcu.msp430.runtime.utils"));
        vo.bind("fill", om.findStrict("ti.mcu.msp430.runtime.utils.Stack.fill", "ti.mcu.msp430.runtime.utils"));
        vo.bind("getUnused", om.findStrict("ti.mcu.msp430.runtime.utils.Stack.getUnused", "ti.mcu.msp430.runtime.utils"));
        vo.bind("getStatus", om.findStrict("ti.mcu.msp430.runtime.utils.Stack.getStatus", "ti.mcu.msp430.runtime.utils"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_utils_Stack_Module__startupDone__E", "ti_mcu_msp430_runtime_utils_Stack_check__E", "ti_mcu_msp430_runtime_utils_Stack_fill__E", "ti_mcu_msp430_runtime_utils_Stack_getUnused__E", "ti_mcu_msp430_runtime_utils_Stack_getStatus__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("UNUSED"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "fill", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "getUnused", "entry", "", "exit", "%d"));
        loggables.add(Global.newObject("name", "getStatus", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Stack", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Stack");
    }

    void Led$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Led", "ti.mcu.msp430.runtime.utils");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Led.Module", "ti.mcu.msp430.runtime.utils");
        vo.init2(po, "ti.mcu.msp430.runtime.utils.Led", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime.utils", "ti.mcu.msp430.runtime.utils"));
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
        mcfgs.add("PORT");
        mcfgs.add("DIRCFG");
        mcfgs.add("RED");
        mcfgs.add("GREEN");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime.utils")).add(vo);
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
        vo.bind("off", om.findStrict("ti.mcu.msp430.runtime.utils.Led.off", "ti.mcu.msp430.runtime.utils"));
        vo.bind("on", om.findStrict("ti.mcu.msp430.runtime.utils.Led.on", "ti.mcu.msp430.runtime.utils"));
        vo.bind("toggle", om.findStrict("ti.mcu.msp430.runtime.utils.Led.toggle", "ti.mcu.msp430.runtime.utils"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_utils_Led_Module__startupDone__E", "ti_mcu_msp430_runtime_utils_Led_off__E", "ti_mcu_msp430_runtime_utils_Led_on__E", "ti_mcu_msp430_runtime_utils_Led_toggle__E"));
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
        pkgV.bind("Led", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Led");
    }

    void Button$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Button", "ti.mcu.msp430.runtime.utils");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.utils.Button.Module", "ti.mcu.msp430.runtime.utils");
        vo.init2(po, "ti.mcu.msp430.runtime.utils.Button", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime.utils", "ti.mcu.msp430.runtime.utils"));
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
        mcfgs.add("PORT");
        mcfgs.add("PORT_IE");
        mcfgs.add("PORT_OUT");
        mcfgs.add("PORT_REN");
        mcfgs.add("PORT_IFG");
        mcfgs.add("SWITCH1");
        mcfgs.add("SWITCH2");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime.utils")).add(vo);
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
        vo.bind("enable", om.findStrict("ti.mcu.msp430.runtime.utils.Button.enable", "ti.mcu.msp430.runtime.utils"));
        vo.bind("disable", om.findStrict("ti.mcu.msp430.runtime.utils.Button.disable", "ti.mcu.msp430.runtime.utils"));
        vo.bind("read", om.findStrict("ti.mcu.msp430.runtime.utils.Button.read", "ti.mcu.msp430.runtime.utils"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_utils_Button_Module__startupDone__E", "ti_mcu_msp430_runtime_utils_Button_enable__E", "ti_mcu_msp430_runtime_utils_Button_disable__E", "ti_mcu_msp430_runtime_utils_Button_read__E"));
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
        pkgV.bind("Button", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Button");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.utils.Atomic", "ti.mcu.msp430.runtime.utils"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.utils.Clock", "ti.mcu.msp430.runtime.utils"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.utils.Stack", "ti.mcu.msp430.runtime.utils"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.utils.Led", "ti.mcu.msp430.runtime.utils"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.utils.Button", "ti.mcu.msp430.runtime.utils"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.utils.Atomic")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.utils.Clock")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.utils.Stack")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.utils.Led")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.utils.Button")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.mcu.msp430.runtime.utils")).add(pkgV);
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
        Atomic$$OBJECTS();
        Clock$$OBJECTS();
        Stack$$OBJECTS();
        Led$$OBJECTS();
        Button$$OBJECTS();
        Atomic$$CONSTS();
        Clock$$CONSTS();
        Stack$$CONSTS();
        Led$$CONSTS();
        Button$$CONSTS();
        Atomic$$CREATES();
        Clock$$CREATES();
        Stack$$CREATES();
        Led$$CREATES();
        Button$$CREATES();
        Atomic$$FUNCTIONS();
        Clock$$FUNCTIONS();
        Stack$$FUNCTIONS();
        Led$$FUNCTIONS();
        Button$$FUNCTIONS();
        Atomic$$SIZES();
        Clock$$SIZES();
        Stack$$SIZES();
        Led$$SIZES();
        Button$$SIZES();
        Atomic$$TYPES();
        Clock$$TYPES();
        Stack$$TYPES();
        Led$$TYPES();
        Button$$TYPES();
        if (isROV) {
            Atomic$$ROV();
            Clock$$ROV();
            Stack$$ROV();
            Led$$ROV();
            Button$$ROV();
        }//isROV
        $$SINGLETONS();
        Atomic$$SINGLETONS();
        Clock$$SINGLETONS();
        Stack$$SINGLETONS();
        Led$$SINGLETONS();
        Button$$SINGLETONS();
        $$INITIALIZATION();
    }
}
