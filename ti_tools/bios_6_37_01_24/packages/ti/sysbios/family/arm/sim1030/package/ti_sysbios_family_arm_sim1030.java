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

public class ti_sysbios_family_arm_sim1030
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
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.family.arm.sim1030", new Value.Obj("ti.sysbios.family.arm.sim1030", pkgP));
    }

    void Hwi$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi", new Value.Obj("ti.sysbios.family.arm.sim1030.Hwi", po));
        pkgV.bind("Hwi", vo);
        // decls 
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.HookSet", om.findStrict("ti.sysbios.interfaces.IHwi.HookSet", "ti.sysbios.family.arm.sim1030"));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.MaskingOption", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.family.arm.sim1030"));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.StackInfo", om.findStrict("ti.sysbios.interfaces.IHwi.StackInfo", "ti.sysbios.family.arm.sim1030"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$L1Intc", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.L1Intc", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$L2Intc", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.L2Intc", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$InterruptObj", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.InterruptObj", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.Instance_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.Module_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.family.arm.sim1030.Hwi.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$Object", new Proto.Obj());
        Object objP = om.bind("ti.sysbios.family.arm.sim1030.Hwi.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$Params", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.family.arm.sim1030.Hwi.Object", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Instance_State", "ti.sysbios.family.arm.sim1030"));
        }//isROV
    }

    void Timer$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Timer.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.sim1030.Timer", new Value.Obj("ti.sysbios.family.arm.sim1030.Timer", po));
        pkgV.bind("Timer", vo);
        // decls 
        om.bind("ti.sysbios.family.arm.sim1030.Timer.StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.sim1030"));
        om.bind("ti.sysbios.family.arm.sim1030.Timer.RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.sim1030"));
        om.bind("ti.sysbios.family.arm.sim1030.Timer.Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.family.arm.sim1030"));
        om.bind("ti.sysbios.family.arm.sim1030.Timer.PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.sim1030"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Timer$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.sim1030.Timer.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.family.arm.sim1030.Timer.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Timer$$Object", new Proto.Obj());
        Object objP = om.bind("ti.sysbios.family.arm.sim1030.Timer.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Timer$$Params", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.sim1030.Timer.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.family.arm.sim1030.Timer.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.family.arm.sim1030.Timer.Object", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Instance_State", "ti.sysbios.family.arm.sim1030"));
        }//isROV
    }

    void TimestampProvider$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider", new Value.Obj("ti.sysbios.family.arm.sim1030.TimestampProvider", po));
        pkgV.bind("TimestampProvider", vo);
        // decls 
    }

    void Clobber$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.sim1030.Clobber.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.sim1030.Clobber", new Value.Obj("ti.sysbios.family.arm.sim1030.Clobber", po));
        pkgV.bind("Clobber", vo);
        // decls 
    }

    void Hwi$$CONSTS()
    {
        // module Hwi
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.NUM_INTERRUPTS", 16L);
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.l1Intc", new Extern("ti_sysbios_family_arm_sim1030_Hwi_l1Intc", "ti_sysbios_family_arm_sim1030_Hwi_L1Intc*", false, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.l2Intc", new Extern("ti_sysbios_family_arm_sim1030_Hwi_l2Intc", "ti_sysbios_family_arm_sim1030_Hwi_L2Intc*", false, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.getStackInfo", new Extern("ti_sysbios_family_arm_sim1030_Hwi_getStackInfo__E", "xdc_Bool(*)(ti_sysbios_interfaces_IHwi_StackInfo*,xdc_Bool)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.startup", new Extern("ti_sysbios_family_arm_sim1030_Hwi_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.switchFromBootStack", new Extern("ti_sysbios_family_arm_sim1030_Hwi_switchFromBootStack__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.post", new Extern("ti_sysbios_family_arm_sim1030_Hwi_post__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.getTaskSP", new Extern("ti_sysbios_family_arm_sim1030_Hwi_getTaskSP__E", "xdc_Char*(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.disableInterrupt", new Extern("ti_sysbios_family_arm_sim1030_Hwi_disableInterrupt__E", "xdc_UInt(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.enableInterrupt", new Extern("ti_sysbios_family_arm_sim1030_Hwi_enableInterrupt__E", "xdc_UInt(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.restoreInterrupt", new Extern("ti_sysbios_family_arm_sim1030_Hwi_restoreInterrupt__E", "xdc_Void(*)(xdc_UInt,xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.clearInterrupt", new Extern("ti_sysbios_family_arm_sim1030_Hwi_clearInterrupt__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.getHandle", new Extern("ti_sysbios_family_arm_sim1030_Hwi_getHandle__E", "ti_sysbios_family_arm_sim1030_Hwi_Handle(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.enableFIQ", new Extern("ti_sysbios_family_arm_sim1030_Hwi_enableFIQ__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.disableFIQ", new Extern("ti_sysbios_family_arm_sim1030_Hwi_disableFIQ__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.restoreFIQ", new Extern("ti_sysbios_family_arm_sim1030_Hwi_restoreFIQ__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.enableIRQ", new Extern("ti_sysbios_family_arm_sim1030_Hwi_enableIRQ__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.disableIRQ", new Extern("ti_sysbios_family_arm_sim1030_Hwi_disableIRQ__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.restoreIRQ", new Extern("ti_sysbios_family_arm_sim1030_Hwi_restoreIRQ__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.disableIER", new Extern("ti_sysbios_family_arm_sim1030_Hwi_disableIER__E", "xdc_Bits16(*)(xdc_Bits16)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.enableIER", new Extern("ti_sysbios_family_arm_sim1030_Hwi_enableIER__E", "xdc_Bits16(*)(xdc_Bits16)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.restoreIER", new Extern("ti_sysbios_family_arm_sim1030_Hwi_restoreIER__E", "xdc_Bits16(*)(xdc_Bits16)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.dispatchIRQC", new Extern("ti_sysbios_family_arm_sim1030_Hwi_dispatchIRQC__I", "xdc_Void(*)(xdc_UArg)", true, false));
    }

    void Timer$$CONSTS()
    {
        // module Timer
        om.bind("ti.sysbios.family.arm.sim1030.Timer.getNumTimers", new Extern("ti_sysbios_family_arm_sim1030_Timer_getNumTimers__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Timer.getStatus", new Extern("ti_sysbios_family_arm_sim1030_Timer_getStatus__E", "ti_sysbios_interfaces_ITimer_Status(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Timer.startup", new Extern("ti_sysbios_family_arm_sim1030_Timer_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void TimestampProvider$$CONSTS()
    {
        // module TimestampProvider
        om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider.get32", new Extern("ti_sysbios_family_arm_sim1030_TimestampProvider_get32__E", "xdc_Bits32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider.get64", new Extern("ti_sysbios_family_arm_sim1030_TimestampProvider_get64__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider.getFreq", new Extern("ti_sysbios_family_arm_sim1030_TimestampProvider_getFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
    }

    void Clobber$$CONSTS()
    {
        // module Clobber
        om.bind("ti.sysbios.family.arm.sim1030.Clobber.trashRegs", new Extern("ti_sysbios_family_arm_sim1030_Clobber_trashRegs__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Clobber.postIntr", new Extern("ti_sysbios_family_arm_sim1030_Clobber_postIntr__E", "xdc_Void(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.sim1030.Clobber.checkRegs", new Extern("ti_sysbios_family_arm_sim1030_Clobber_checkRegs__E", "xdc_UInt(*)(xdc_UInt)", true, false));
    }

    void Hwi$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$sim1030$Hwi$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.family.arm.sim1030.Hwi.create() called before xdc.useModule('ti.sysbios.family.arm.sim1030.Hwi')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.intNum, inst.$args.hwiFxn, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$create", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Module", "ti.sysbios.family.arm.sim1030"), om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Instance", "ti.sysbios.family.arm.sim1030"), 3, 2, false));
                        fxn.addArg(0, "intNum", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "hwiFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Params", "ti.sysbios.family.arm.sim1030"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$sim1030$Hwi$$create = function( intNum, hwiFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.sim1030.Hwi'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.family.arm.sim1030.Hwi.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.family.arm.sim1030']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {intNum:intNum, hwiFxn:hwiFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.family.arm.sim1030.Hwi'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [intNum, hwiFxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.sim1030.Hwi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Module", "ti.sysbios.family.arm.sim1030"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$Object", "ti.sysbios.family.arm.sim1030"), null);
                        fxn.addArg(1, "intNum", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "hwiFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Params", "ti.sysbios.family.arm.sim1030"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$sim1030$Hwi$$construct = function( __obj, intNum, hwiFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.sim1030.Hwi'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {intNum:intNum, hwiFxn:hwiFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.family.arm.sim1030.Hwi'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.sim1030.Hwi'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Timer$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$sim1030$Timer$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.family.arm.sim1030.Timer.create() called before xdc.useModule('ti.sysbios.family.arm.sim1030.Timer')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.id, inst.$args.tickFxn, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.sim1030.Timer$$create", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Module", "ti.sysbios.family.arm.sim1030"), om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Instance", "ti.sysbios.family.arm.sim1030"), 3, 2, false));
                        fxn.addArg(0, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Params", "ti.sysbios.family.arm.sim1030"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$sim1030$Timer$$create = function( id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.sim1030.Timer'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.family.arm.sim1030.Timer.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.family.arm.sim1030']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.family.arm.sim1030.Timer'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [id, tickFxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.sim1030.Timer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.sim1030.Timer$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Module", "ti.sysbios.family.arm.sim1030"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$Object", "ti.sysbios.family.arm.sim1030"), null);
                        fxn.addArg(1, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Params", "ti.sysbios.family.arm.sim1030"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$sim1030$Timer$$construct = function( __obj, id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.sim1030.Timer'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.family.arm.sim1030.Timer'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.sim1030.Timer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void TimestampProvider$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Clobber$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Hwi$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Hwi.inUseMeta
        fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.sim1030.Hwi$$inUseMeta", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Module", "ti.sysbios.family.arm.sim1030"), $$T_Bool, 1, 1, false));
                fxn.addArg(0, "intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF);
    }

    void Timer$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TimestampProvider$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Clobber$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Hwi$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.L1Intc", "ti.sysbios.family.arm.sim1030");
        sizes.clear();
        sizes.add(Global.newArray("ITR", "UInt32"));
        sizes.add(Global.newArray("MIR", "UInt32"));
        sizes.add(Global.newArray("res", "A2;UInt32"));
        sizes.add(Global.newArray("SIR_IRQ", "UInt32"));
        sizes.add(Global.newArray("SIR_FIQ", "UInt32"));
        sizes.add(Global.newArray("CONTROL", "UInt32"));
        sizes.add(Global.newArray("ILR", "A32;UInt32"));
        sizes.add(Global.newArray("SISR", "UInt32"));
        sizes.add(Global.newArray("GMR", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.L1Intc']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.L1Intc']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.L1Intc'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.L2Intc", "ti.sysbios.family.arm.sim1030");
        sizes.clear();
        sizes.add(Global.newArray("ITR1", "UInt32"));
        sizes.add(Global.newArray("MIR1", "UInt32"));
        sizes.add(Global.newArray("res1", "A2;UInt32"));
        sizes.add(Global.newArray("SIR_IRQ", "UInt32"));
        sizes.add(Global.newArray("SIR_FIQ", "UInt32"));
        sizes.add(Global.newArray("CONTROL", "UInt32"));
        sizes.add(Global.newArray("ILR1", "A32;UInt32"));
        sizes.add(Global.newArray("SISR1", "UInt32"));
        sizes.add(Global.newArray("STATUS", "UInt32"));
        sizes.add(Global.newArray("OCP_CFG", "UInt32"));
        sizes.add(Global.newArray("INTH_REV", "UInt32"));
        sizes.add(Global.newArray("res2", "A21;UInt32"));
        sizes.add(Global.newArray("ITR2", "UInt32"));
        sizes.add(Global.newArray("MIR2", "UInt32"));
        sizes.add(Global.newArray("res3", "A5;UInt32"));
        sizes.add(Global.newArray("ILR2", "A32;UInt32"));
        sizes.add(Global.newArray("SISR2", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.L2Intc']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.L2Intc']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.L2Intc'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.InterruptObj", "ti.sysbios.family.arm.sim1030");
        sizes.clear();
        sizes.add(Global.newArray("used", "UShort"));
        sizes.add(Global.newArray("useDispatcher", "UShort"));
        sizes.add(Global.newArray("fxn", "UFxn"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.InterruptObj']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.InterruptObj']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.InterruptObj'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Instance_State", "ti.sysbios.family.arm.sim1030");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("disableMask", "UInt16"));
        sizes.add(Global.newArray("restoreMask", "UInt16"));
        sizes.add(Global.newArray("arg", "UIArg"));
        sizes.add(Global.newArray("fxn", "UFxn"));
        sizes.add(Global.newArray("irp", "UIArg"));
        sizes.add(Global.newArray("hookEnv", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Module_State", "ti.sysbios.family.arm.sim1030");
        sizes.clear();
        sizes.add(Global.newArray("ierMask", "UInt16"));
        sizes.add(Global.newArray("IER", "UInt16"));
        sizes.add(Global.newArray("irq0Mask", "UInt32"));
        sizes.add(Global.newArray("irp", "UInt"));
        sizes.add(Global.newArray("taskSP", "UPtr"));
        sizes.add(Global.newArray("isrStack", "UPtr"));
        sizes.add(Global.newArray("isrStackBase", "UPtr"));
        sizes.add(Global.newArray("isrStackSize", "UPtr"));
        sizes.add(Global.newArray("fiqStack", "UPtr"));
        sizes.add(Global.newArray("fiqStackSize", "USize"));
        sizes.add(Global.newArray("dispatchTable", "A16;UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.sim1030.Hwi.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Timer$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Instance_State", "ti.sysbios.family.arm.sim1030");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.sim1030.Timer.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.sim1030.Timer.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.sim1030.Timer.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void TimestampProvider$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Clobber$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Hwi$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/sim1030/Hwi.xs");
        om.bind("ti.sysbios.family.arm.sim1030.Hwi$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Module", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.Module", om.findStrict("ti.sysbios.interfaces.IHwi.Module", "ti.sysbios.family.arm.sim1030"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("NUM_INTERRUPTS", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
        if (isCFG) {
            po.addFld("resetFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "wh");
            po.addFld("undefinedInstFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "wh");
            po.addFld("swiFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "wh");
            po.addFld("prefetchAbortFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "wh");
            po.addFld("dataAbortFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "wh");
            po.addFld("reservedFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "wh");
            po.addFld("irqFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "wh");
            po.addFld("fiqFunc", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), $$UNDEF, "wh");
            po.addFld("fiqStack", new Proto.Adr("xdc_Ptr", "Pv"), null, "w");
            po.addFld("fiqStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "wh");
            po.addFld("fiqStackSection", $$T_Str, null, "wh");
            po.addFld("ierMask", Proto.Elm.newCNum("(xdc_Bits16)"), 0x0000L, "w");
            po.addFld("E_alreadyDefined", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.sim1030"), Global.newObject("msg", "E_alreadyDefined: Hwi already defined: intr# %d"), "w");
            po.addFld("LM_begin", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.family.arm.sim1030"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_begin: hwi: 0x%x, func: 0x%x, preThread: %d, intNum: %d, irp: 0x%x"), "w");
            po.addFld("LD_end", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.family.arm.sim1030"), Global.newObject("mask", 0x0200L, "msg", "LD_end: hwi: 0x%x"), "w");
            po.addFld("swiDisable", new Proto.Adr("xdc_UInt(*)(xdc_Void)", "PFn"), $$UNDEF, "w");
            po.addFld("swiRestoreHwi", new Proto.Adr("xdc_Void(*)(xdc_UInt)", "PFv"), $$UNDEF, "w");
            po.addFld("taskDisable", new Proto.Adr("xdc_UInt(*)(xdc_Void)", "PFn"), $$UNDEF, "w");
            po.addFld("taskRestoreHwi", new Proto.Adr("xdc_Void(*)(xdc_UInt)", "PFv"), $$UNDEF, "w");
            po.addFld("hooks", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.HookSet", "ti.sysbios.family.arm.sim1030"), true), Global.newArray(new Object[]{}), "w");
            po.addFld("interrupt", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.InterruptObj", "ti.sysbios.family.arm.sim1030"), false, xdc.services.intern.xsr.Enum.intValue(16L)), $$DEFAULT, "wh");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$create", "ti.sysbios.family.arm.sim1030"), Global.get("ti$sysbios$family$arm$sim1030$Hwi$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$construct", "ti.sysbios.family.arm.sim1030"), Global.get("ti$sysbios$family$arm$sim1030$Hwi$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Hwi$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Hwi$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Hwi$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Hwi$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Hwi$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Hwi$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "addHookSet");
                if (fxn != null) po.addFxn("addHookSet", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.IHwi$$addHookSet", "ti.sysbios.family.arm.sim1030"), fxn);
                fxn = Global.get(cap, "viewGetStackInfo");
                if (fxn != null) po.addFxn("viewGetStackInfo", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.IHwi$$viewGetStackInfo", "ti.sysbios.family.arm.sim1030"), fxn);
                po.addFxn("inUseMeta", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$inUseMeta", "ti.sysbios.family.arm.sim1030"), Global.get(cap, "inUseMeta"));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Instance", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.Instance", om.findStrict("ti.sysbios.interfaces.IHwi.Instance", "ti.sysbios.family.arm.sim1030"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("NUM_INTERRUPTS", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
        if (isCFG) {
            po.addFld("disableMask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("restoreMask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.sim1030"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$Params", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.Params", om.findStrict("ti.sysbios.interfaces.IHwi$$Params", "ti.sysbios.family.arm.sim1030"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("NUM_INTERRUPTS", Proto.Elm.newCNum("(xdc_Int)"), 16L, "rh");
        if (isCFG) {
            po.addFld("disableMask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("restoreMask", Proto.Elm.newCNum("(xdc_Bits16)"), 0L, "w");
            po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), 0L, "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.sim1030"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$Object", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.Object", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Instance", "ti.sysbios.family.arm.sim1030"));
        // typedef Hwi.VectorFuncPtr
        om.bind("ti.sysbios.family.arm.sim1030.Hwi.VectorFuncPtr", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
        // struct Hwi.L1Intc
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$L1Intc", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.L1Intc", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ITR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("MIR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("res", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(2L)), $$DEFAULT, "w");
                po.addFld("SIR_IRQ", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("SIR_FIQ", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("CONTROL", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("ILR", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(32L)), $$DEFAULT, "w");
                po.addFld("SISR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("GMR", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct Hwi.L2Intc
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$L2Intc", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.L2Intc", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ITR1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("MIR1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("res1", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(2L)), $$DEFAULT, "w");
                po.addFld("SIR_IRQ", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("SIR_FIQ", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("CONTROL", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("ILR1", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(32L)), $$DEFAULT, "w");
                po.addFld("SISR1", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("STATUS", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("OCP_CFG", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("INTH_REV", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("res2", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(21L)), $$DEFAULT, "w");
                po.addFld("ITR2", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("MIR2", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
                po.addFld("res3", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(5L)), $$DEFAULT, "w");
                po.addFld("ILR2", new Proto.Arr(Proto.Elm.newCNum("(xdc_UInt32)"), false, xdc.services.intern.xsr.Enum.intValue(32L)), $$DEFAULT, "w");
                po.addFld("SISR2", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
        // struct Hwi.InterruptObj
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$InterruptObj", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.InterruptObj", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("used", $$T_Bool, $$UNDEF, "w");
                po.addFld("useDispatcher", $$T_Bool, $$UNDEF, "w");
                po.addFld("fxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF, "w");
        // struct Hwi.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$Instance_State", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("disableMask", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("restoreMask", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("fxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF, "w");
                po.addFld("irp", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("hookEnv", new Proto.Arr(new Proto.Adr("xdc_Ptr", "Pv"), false), $$DEFAULT, "w");
        // struct Hwi.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$Module_State", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Hwi.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ierMask", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("IER", Proto.Elm.newCNum("(xdc_Bits16)"), $$UNDEF, "w");
                po.addFld("irq0Mask", Proto.Elm.newCNum("(xdc_Bits32)"), $$UNDEF, "w");
                po.addFld("irp", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("taskSP", new Proto.Adr("xdc_Char*", "Pn"), $$UNDEF, "w");
                po.addFld("isrStack", new Proto.Adr("xdc_Char*", "Pn"), $$UNDEF, "w");
                po.addFld("isrStackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("isrStackSize", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("fiqStack", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("fiqStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("dispatchTable", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Handle", "ti.sysbios.family.arm.sim1030"), false, xdc.services.intern.xsr.Enum.intValue(16L)), $$DEFAULT, "w");
    }

    void Timer$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/sim1030/Timer.xs");
        om.bind("ti.sysbios.family.arm.sim1030.Timer$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Module", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Timer.Module", om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.family.arm.sim1030"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$create", "ti.sysbios.family.arm.sim1030"), Global.get("ti$sysbios$family$arm$sim1030$Timer$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$construct", "ti.sysbios.family.arm.sim1030"), Global.get("ti$sysbios$family$arm$sim1030$Timer$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Timer$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Timer$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Timer$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Timer$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Timer$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.Timer$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getFreqMeta");
                if (fxn != null) po.addFxn("getFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimer$$getFreqMeta", "ti.sysbios.family.arm.sim1030"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Instance", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Timer.Instance", om.findStrict("ti.sysbios.interfaces.ITimer.Instance", "ti.sysbios.family.arm.sim1030"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.sim1030"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$Params", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Timer.Params", om.findStrict("ti.sysbios.interfaces.ITimer$$Params", "ti.sysbios.family.arm.sim1030"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.sim1030"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$Object", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Timer.Object", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Instance", "ti.sysbios.family.arm.sim1030"));
        // struct Timer.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$Instance_State", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Timer.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void TimestampProvider$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/sim1030/TimestampProvider.xs");
        om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider.Module", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.TimestampProvider.Module", om.findStrict("xdc.runtime.ITimestampProvider.Module", "ti.sysbios.family.arm.sim1030"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.sim1030.TimestampProvider$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void Clobber$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Clobber.Module", "ti.sysbios.family.arm.sim1030");
        po.init("ti.sysbios.family.arm.sim1030.Clobber.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.family.arm.sim1030"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("postIntrCheck", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
        }//isCFG
    }

    void Hwi$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi", "ti.sysbios.family.arm.sim1030");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$Instance_State", "ti.sysbios.family.arm.sim1030");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("L1Intc$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.sim1030.Hwi.L1Intc", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$L1Intc", "ti.sysbios.family.arm.sim1030");
        vo.bind("L2Intc$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.sim1030.Hwi.L2Intc", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$L2Intc", "ti.sysbios.family.arm.sim1030");
        vo.bind("InterruptObj$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.sim1030.Hwi.InterruptObj", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$InterruptObj", "ti.sysbios.family.arm.sim1030");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.sim1030.Hwi.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$Instance_State", "ti.sysbios.family.arm.sim1030");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.sim1030.Hwi.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$Module_State", "ti.sysbios.family.arm.sim1030");
        po.bind("fiqStack$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
    }

    void Timer$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer", "ti.sysbios.family.arm.sim1030");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$Instance_State", "ti.sysbios.family.arm.sim1030");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.sim1030.Timer.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$Instance_State", "ti.sysbios.family.arm.sim1030");
    }

    void TimestampProvider$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider", "ti.sysbios.family.arm.sim1030");
    }

    void Clobber$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Clobber", "ti.sysbios.family.arm.sim1030");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.family.arm.sim1030.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.family.arm.sim1030"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/sim1030/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.family.arm.sim1030"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.family.arm.sim1030"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.family.arm.sim1030"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.family.arm.sim1030"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.family.arm.sim1030"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.family.arm.sim1030"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.family.arm.sim1030", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.family.arm.sim1030");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.family.arm.sim1030.");
        pkgV.bind("$vers", Global.newArray("2, 0, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.family.arm.sim1030'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.arm.sim1030.ae9',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.arm.sim1030.ae9', {target: 'ti.targets.arm.elf.Arm9', suffix: 'e9'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Hwi$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi", "ti.sysbios.family.arm.sim1030");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Module", "ti.sysbios.family.arm.sim1030");
        vo.init2(po, "ti.sysbios.family.arm.sim1030.Hwi", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi$$capsule", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Instance", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Instance", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Params", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Params", "ti.sysbios.family.arm.sim1030"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Params", "ti.sysbios.family.arm.sim1030")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Handle", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.sim1030", "ti.sysbios.family.arm.sim1030"));
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
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.IHwi.FuncPtr", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Irp", om.findStrict("ti.sysbios.interfaces.IHwi.Irp", "ti.sysbios.family.arm.sim1030"));
        vo.bind("HookSet", om.findStrict("ti.sysbios.interfaces.IHwi.HookSet", "ti.sysbios.family.arm.sim1030"));
        tdefs.add(om.findStrict("ti.sysbios.interfaces.IHwi.HookSet", "ti.sysbios.family.arm.sim1030"));
        vo.bind("MaskingOption", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption", "ti.sysbios.family.arm.sim1030"));
        vo.bind("StackInfo", om.findStrict("ti.sysbios.interfaces.IHwi.StackInfo", "ti.sysbios.family.arm.sim1030"));
        tdefs.add(om.findStrict("ti.sysbios.interfaces.IHwi.StackInfo", "ti.sysbios.family.arm.sim1030"));
        mcfgs.add("dispatcherAutoNestingSupport");
        mcfgs.add("dispatcherSwiSupport");
        mcfgs.add("dispatcherTaskSupport");
        mcfgs.add("dispatcherIrpTrackingSupport");
        vo.bind("VectorFuncPtr", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.VectorFuncPtr", "ti.sysbios.family.arm.sim1030"));
        vo.bind("L1Intc", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.L1Intc", "ti.sysbios.family.arm.sim1030"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.L1Intc", "ti.sysbios.family.arm.sim1030"));
        vo.bind("L2Intc", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.L2Intc", "ti.sysbios.family.arm.sim1030"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.L2Intc", "ti.sysbios.family.arm.sim1030"));
        vo.bind("l1Intc", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.l1Intc", "ti.sysbios.family.arm.sim1030"));
        vo.bind("l2Intc", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.l2Intc", "ti.sysbios.family.arm.sim1030"));
        mcfgs.add("fiqStack");
        mcfgs.add("ierMask");
        mcfgs.add("E_alreadyDefined");
        mcfgs.add("LM_begin");
        mcfgs.add("LD_end");
        mcfgs.add("swiDisable");
        icfgs.add("swiDisable");
        mcfgs.add("swiRestoreHwi");
        icfgs.add("swiRestoreHwi");
        mcfgs.add("taskDisable");
        icfgs.add("taskDisable");
        mcfgs.add("taskRestoreHwi");
        icfgs.add("taskRestoreHwi");
        mcfgs.add("hooks");
        icfgs.add("hooks");
        vo.bind("InterruptObj", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.InterruptObj", "ti.sysbios.family.arm.sim1030"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.InterruptObj", "ti.sysbios.family.arm.sim1030"));
        icfgs.add("interrupt");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Instance_State", "ti.sysbios.family.arm.sim1030"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Instance_State", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Module_State", "ti.sysbios.family.arm.sim1030"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Module_State", "ti.sysbios.family.arm.sim1030"));
        vo.bind("MaskingOption_NONE", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_NONE", "ti.sysbios.family.arm.sim1030"));
        vo.bind("MaskingOption_ALL", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_ALL", "ti.sysbios.family.arm.sim1030"));
        vo.bind("MaskingOption_SELF", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_SELF", "ti.sysbios.family.arm.sim1030"));
        vo.bind("MaskingOption_BITMASK", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_BITMASK", "ti.sysbios.family.arm.sim1030"));
        vo.bind("MaskingOption_LOWER", om.findStrict("ti.sysbios.interfaces.IHwi.MaskingOption_LOWER", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.sim1030")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.family.arm.sim1030.Hwi$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$family$arm$sim1030$Hwi$$__initObject"));
        }//isCFG
        vo.bind("getStackInfo", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.getStackInfo", "ti.sysbios.family.arm.sim1030"));
        vo.bind("startup", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.startup", "ti.sysbios.family.arm.sim1030"));
        vo.bind("switchFromBootStack", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.switchFromBootStack", "ti.sysbios.family.arm.sim1030"));
        vo.bind("post", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.post", "ti.sysbios.family.arm.sim1030"));
        vo.bind("getTaskSP", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.getTaskSP", "ti.sysbios.family.arm.sim1030"));
        vo.bind("disableInterrupt", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.disableInterrupt", "ti.sysbios.family.arm.sim1030"));
        vo.bind("enableInterrupt", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.enableInterrupt", "ti.sysbios.family.arm.sim1030"));
        vo.bind("restoreInterrupt", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.restoreInterrupt", "ti.sysbios.family.arm.sim1030"));
        vo.bind("clearInterrupt", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.clearInterrupt", "ti.sysbios.family.arm.sim1030"));
        vo.bind("getHandle", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.getHandle", "ti.sysbios.family.arm.sim1030"));
        vo.bind("enableFIQ", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.enableFIQ", "ti.sysbios.family.arm.sim1030"));
        vo.bind("disableFIQ", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.disableFIQ", "ti.sysbios.family.arm.sim1030"));
        vo.bind("restoreFIQ", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.restoreFIQ", "ti.sysbios.family.arm.sim1030"));
        vo.bind("enableIRQ", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.enableIRQ", "ti.sysbios.family.arm.sim1030"));
        vo.bind("disableIRQ", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.disableIRQ", "ti.sysbios.family.arm.sim1030"));
        vo.bind("restoreIRQ", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.restoreIRQ", "ti.sysbios.family.arm.sim1030"));
        vo.bind("disableIER", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.disableIER", "ti.sysbios.family.arm.sim1030"));
        vo.bind("enableIER", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.enableIER", "ti.sysbios.family.arm.sim1030"));
        vo.bind("restoreIER", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.restoreIER", "ti.sysbios.family.arm.sim1030"));
        vo.bind("dispatchIRQC", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.dispatchIRQC", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_sim1030_Hwi_Handle__label__E", "ti_sysbios_family_arm_sim1030_Hwi_Module__startupDone__E", "ti_sysbios_family_arm_sim1030_Hwi_Object__create__E", "ti_sysbios_family_arm_sim1030_Hwi_Object__delete__E", "ti_sysbios_family_arm_sim1030_Hwi_Object__destruct__E", "ti_sysbios_family_arm_sim1030_Hwi_Object__get__E", "ti_sysbios_family_arm_sim1030_Hwi_Object__first__E", "ti_sysbios_family_arm_sim1030_Hwi_Object__next__E", "ti_sysbios_family_arm_sim1030_Hwi_Params__init__E", "ti_sysbios_family_arm_sim1030_Hwi_getStackInfo__E", "ti_sysbios_family_arm_sim1030_Hwi_startup__E", "ti_sysbios_family_arm_sim1030_Hwi_switchFromBootStack__E", "ti_sysbios_family_arm_sim1030_Hwi_post__E", "ti_sysbios_family_arm_sim1030_Hwi_getTaskSP__E", "ti_sysbios_family_arm_sim1030_Hwi_disableInterrupt__E", "ti_sysbios_family_arm_sim1030_Hwi_enableInterrupt__E", "ti_sysbios_family_arm_sim1030_Hwi_restoreInterrupt__E", "ti_sysbios_family_arm_sim1030_Hwi_clearInterrupt__E", "ti_sysbios_family_arm_sim1030_Hwi_getFunc__E", "ti_sysbios_family_arm_sim1030_Hwi_setFunc__E", "ti_sysbios_family_arm_sim1030_Hwi_getHookContext__E", "ti_sysbios_family_arm_sim1030_Hwi_setHookContext__E", "ti_sysbios_family_arm_sim1030_Hwi_getIrp__E", "ti_sysbios_family_arm_sim1030_Hwi_disable__E", "ti_sysbios_family_arm_sim1030_Hwi_enable__E", "ti_sysbios_family_arm_sim1030_Hwi_restore__E", "ti_sysbios_family_arm_sim1030_Hwi_getHandle__E", "ti_sysbios_family_arm_sim1030_Hwi_enableFIQ__E", "ti_sysbios_family_arm_sim1030_Hwi_disableFIQ__E", "ti_sysbios_family_arm_sim1030_Hwi_restoreFIQ__E", "ti_sysbios_family_arm_sim1030_Hwi_enableIRQ__E", "ti_sysbios_family_arm_sim1030_Hwi_disableIRQ__E", "ti_sysbios_family_arm_sim1030_Hwi_restoreIRQ__E", "ti_sysbios_family_arm_sim1030_Hwi_disableIER__E", "ti_sysbios_family_arm_sim1030_Hwi_enableIER__E", "ti_sysbios_family_arm_sim1030_Hwi_restoreIER__E", "ti_sysbios_family_arm_sim1030_Hwi_reconfig__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LM_begin", "LD_end"));
        vo.bind("$$errorDescCfgs", Global.newArray("E_alreadyDefined"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./Hwi.xdt");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.family.arm.sim1030.Hwi.Object", "ti.sysbios.family.arm.sim1030"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./Hwi.xdt");
        pkgV.bind("Hwi", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Hwi");
    }

    void Timer$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer", "ti.sysbios.family.arm.sim1030");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Module", "ti.sysbios.family.arm.sim1030");
        vo.init2(po, "ti.sysbios.family.arm.sim1030.Timer", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.sim1030.Timer$$capsule", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Instance", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Instance", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Params", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Params", "ti.sysbios.family.arm.sim1030"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Params", "ti.sysbios.family.arm.sim1030")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Handle", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.sim1030", "ti.sysbios.family.arm.sim1030"));
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
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.ITimer.FuncPtr", "ti.sysbios.family.arm.sim1030"));
        vo.bind("StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.sim1030"));
        vo.bind("RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.family.arm.sim1030"));
        vo.bind("PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Instance_State", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Instance_State", "ti.sysbios.family.arm.sim1030"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Instance_State", "ti.sysbios.family.arm.sim1030"));
        vo.bind("StartMode_AUTO", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_AUTO", "ti.sysbios.family.arm.sim1030"));
        vo.bind("StartMode_USER", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_USER", "ti.sysbios.family.arm.sim1030"));
        vo.bind("RunMode_CONTINUOUS", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS", "ti.sysbios.family.arm.sim1030"));
        vo.bind("RunMode_ONESHOT", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_ONESHOT", "ti.sysbios.family.arm.sim1030"));
        vo.bind("RunMode_DYNAMIC", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_DYNAMIC", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Status_INUSE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_INUSE", "ti.sysbios.family.arm.sim1030"));
        vo.bind("Status_FREE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_FREE", "ti.sysbios.family.arm.sim1030"));
        vo.bind("PeriodType_MICROSECS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS", "ti.sysbios.family.arm.sim1030"));
        vo.bind("PeriodType_COUNTS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_COUNTS", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.sim1030")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.sysbios.family.arm.sim1030.Timer$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$family$arm$sim1030$Timer$$__initObject"));
        }//isCFG
        vo.bind("getNumTimers", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.getNumTimers", "ti.sysbios.family.arm.sim1030"));
        vo.bind("getStatus", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.getStatus", "ti.sysbios.family.arm.sim1030"));
        vo.bind("startup", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.startup", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_sim1030_Timer_Handle__label__E", "ti_sysbios_family_arm_sim1030_Timer_Module__startupDone__E", "ti_sysbios_family_arm_sim1030_Timer_Object__create__E", "ti_sysbios_family_arm_sim1030_Timer_Object__delete__E", "ti_sysbios_family_arm_sim1030_Timer_Object__destruct__E", "ti_sysbios_family_arm_sim1030_Timer_Object__get__E", "ti_sysbios_family_arm_sim1030_Timer_Object__first__E", "ti_sysbios_family_arm_sim1030_Timer_Object__next__E", "ti_sysbios_family_arm_sim1030_Timer_Params__init__E", "ti_sysbios_family_arm_sim1030_Timer_getNumTimers__E", "ti_sysbios_family_arm_sim1030_Timer_getStatus__E", "ti_sysbios_family_arm_sim1030_Timer_startup__E", "ti_sysbios_family_arm_sim1030_Timer_getMaxTicks__E", "ti_sysbios_family_arm_sim1030_Timer_setNextTick__E", "ti_sysbios_family_arm_sim1030_Timer_start__E", "ti_sysbios_family_arm_sim1030_Timer_stop__E", "ti_sysbios_family_arm_sim1030_Timer_setPeriod__E", "ti_sysbios_family_arm_sim1030_Timer_setPeriodMicroSecs__E", "ti_sysbios_family_arm_sim1030_Timer_getPeriod__E", "ti_sysbios_family_arm_sim1030_Timer_getCount__E", "ti_sysbios_family_arm_sim1030_Timer_getFreq__E", "ti_sysbios_family_arm_sim1030_Timer_getFunc__E", "ti_sysbios_family_arm_sim1030_Timer_setFunc__E", "ti_sysbios_family_arm_sim1030_Timer_trigger__E", "ti_sysbios_family_arm_sim1030_Timer_getExpiredCounts__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.family.arm.sim1030.Timer.Object", "ti.sysbios.family.arm.sim1030"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Timer", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Timer");
    }

    void TimestampProvider$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider", "ti.sysbios.family.arm.sim1030");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider.Module", "ti.sysbios.family.arm.sim1030");
        vo.init2(po, "ti.sysbios.family.arm.sim1030.TimestampProvider", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider$$capsule", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.sim1030", "ti.sysbios.family.arm.sim1030"));
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
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.sim1030")).add(vo);
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
        vo.bind("get32", om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider.get32", "ti.sysbios.family.arm.sim1030"));
        vo.bind("get64", om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider.get64", "ti.sysbios.family.arm.sim1030"));
        vo.bind("getFreq", om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider.getFreq", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_sim1030_TimestampProvider_Module__startupDone__E", "ti_sysbios_family_arm_sim1030_TimestampProvider_get32__E", "ti_sysbios_family_arm_sim1030_TimestampProvider_get64__E", "ti_sysbios_family_arm_sim1030_TimestampProvider_getFreq__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "get32", "entry", "", "exit", "0x%x"));
        loggables.add(Global.newObject("name", "get64", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "getFreq", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("TimestampProvider", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TimestampProvider");
    }

    void Clobber$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Clobber", "ti.sysbios.family.arm.sim1030");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.sim1030.Clobber.Module", "ti.sysbios.family.arm.sim1030");
        vo.init2(po, "ti.sysbios.family.arm.sim1030.Clobber", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.sim1030", "ti.sysbios.family.arm.sim1030"));
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
        mcfgs.add("postIntrCheck");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.sim1030")).add(vo);
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
        vo.bind("trashRegs", om.findStrict("ti.sysbios.family.arm.sim1030.Clobber.trashRegs", "ti.sysbios.family.arm.sim1030"));
        vo.bind("postIntr", om.findStrict("ti.sysbios.family.arm.sim1030.Clobber.postIntr", "ti.sysbios.family.arm.sim1030"));
        vo.bind("checkRegs", om.findStrict("ti.sysbios.family.arm.sim1030.Clobber.checkRegs", "ti.sysbios.family.arm.sim1030"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_sim1030_Clobber_Module__startupDone__E", "ti_sysbios_family_arm_sim1030_Clobber_trashRegs__E", "ti_sysbios_family_arm_sim1030_Clobber_postIntr__E", "ti_sysbios_family_arm_sim1030_Clobber_checkRegs__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "trashRegs", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "postIntr", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "checkRegs", "entry", "0x%x", "exit", "0x%x"));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Clobber", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Clobber");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.family.arm.sim1030")).findStrict("PARAMS", "ti.sysbios.family.arm.sim1030");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi", "ti.sysbios.family.arm.sim1030")).findStrict("PARAMS", "ti.sysbios.family.arm.sim1030");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.family.arm.sim1030.Timer", "ti.sysbios.family.arm.sim1030")).findStrict("PARAMS", "ti.sysbios.family.arm.sim1030");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.sim1030.Hwi", "ti.sysbios.family.arm.sim1030"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.sim1030.Timer", "ti.sysbios.family.arm.sim1030"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.sim1030.TimestampProvider", "ti.sysbios.family.arm.sim1030"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.sim1030.Clobber", "ti.sysbios.family.arm.sim1030"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.family.arm.sim1030.Hwi")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.sim1030.Timer")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.sim1030.TimestampProvider")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.sim1030.Clobber")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.family.arm.sim1030")).add(pkgV);
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
        Hwi$$OBJECTS();
        Timer$$OBJECTS();
        TimestampProvider$$OBJECTS();
        Clobber$$OBJECTS();
        Hwi$$CONSTS();
        Timer$$CONSTS();
        TimestampProvider$$CONSTS();
        Clobber$$CONSTS();
        Hwi$$CREATES();
        Timer$$CREATES();
        TimestampProvider$$CREATES();
        Clobber$$CREATES();
        Hwi$$FUNCTIONS();
        Timer$$FUNCTIONS();
        TimestampProvider$$FUNCTIONS();
        Clobber$$FUNCTIONS();
        Hwi$$SIZES();
        Timer$$SIZES();
        TimestampProvider$$SIZES();
        Clobber$$SIZES();
        Hwi$$TYPES();
        Timer$$TYPES();
        TimestampProvider$$TYPES();
        Clobber$$TYPES();
        if (isROV) {
            Hwi$$ROV();
            Timer$$ROV();
            TimestampProvider$$ROV();
            Clobber$$ROV();
        }//isROV
        $$SINGLETONS();
        Hwi$$SINGLETONS();
        Timer$$SINGLETONS();
        TimestampProvider$$SINGLETONS();
        Clobber$$SINGLETONS();
        $$INITIALIZATION();
    }
}
