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

public class ti_sysbios_family_arm_tm4l
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
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.hal");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.family.arm.tm4l", new Value.Obj("ti.sysbios.family.arm.tm4l", pkgP));
    }

    void Timer$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer", new Value.Obj("ti.sysbios.family.arm.tm4l.Timer", po));
        pkgV.bind("Timer", vo);
        // decls 
        om.bind("ti.sysbios.family.arm.tm4l.Timer.StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.tm4l"));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.tm4l"));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.family.arm.tm4l"));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.tm4l"));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Source", new Proto.Enm("ti.sysbios.family.arm.tm4l.Timer.Source"));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.ID", new Proto.Enm("ti.sysbios.family.arm.tm4l.Timer.ID"));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX", new Proto.Enm("ti.sysbios.family.arm.tm4l.Timer.IDEX"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$Control", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Control", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$BasicView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.Timer.BasicView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$DeviceView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.Timer.DeviceView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$ModuleView", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.Timer.ModuleView", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$TimerDevice", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.Timer.TimerDevice", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$Instance_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Instance_State", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Module_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.sysbios.family.arm.tm4l.Timer.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$Object", new Proto.Obj());
        Object objP = om.bind("ti.sysbios.family.arm.tm4l.Timer.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Timer$$Params", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Params", new Proto.Str(po, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Handle", insP);
        if (isROV) {
            om.bind("ti.sysbios.family.arm.tm4l.Timer.Object", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Instance_State", "ti.sysbios.family.arm.tm4l"));
        }//isROV
    }

    void TimestampProvider$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider", new Value.Obj("ti.sysbios.family.arm.tm4l.TimestampProvider", po));
        pkgV.bind("TimestampProvider", vo);
        // decls 
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.Source", new Proto.Enm("ti.sysbios.family.arm.tm4l.TimestampProvider.Source"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State", new Proto.Str(spo, false));
    }

    void Power$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.arm.tm4l.Power.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.arm.tm4l.Power", new Value.Obj("ti.sysbios.family.arm.tm4l.Power", po));
        pkgV.bind("Power", vo);
        // decls 
    }

    void Timer$$CONSTS()
    {
        // module Timer
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Source_ACLK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.Source_ACLK", xdc.services.intern.xsr.Enum.intValue(0x100L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Source_SMCLK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.Source_SMCLK", xdc.services.intern.xsr.Enum.intValue(0x200L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Source_EXTERNAL", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.Source_EXTERNAL", xdc.services.intern.xsr.Enum.intValue(0x000L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.Source_EXTERNAL_INVERTED", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.Source_EXTERNAL_INVERTED", xdc.services.intern.xsr.Enum.intValue(0x300L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.ID_1", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.ID_1", xdc.services.intern.xsr.Enum.intValue(0x0L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.ID_2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.ID_2", xdc.services.intern.xsr.Enum.intValue(0x40L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.ID_4", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.ID_4", xdc.services.intern.xsr.Enum.intValue(0x80L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.ID_8", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.ID_8", xdc.services.intern.xsr.Enum.intValue(0xC0L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX_1", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.IDEX_1", xdc.services.intern.xsr.Enum.intValue(0x0L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX_2", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.IDEX_2", xdc.services.intern.xsr.Enum.intValue(0x1L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX_3", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.IDEX_3", xdc.services.intern.xsr.Enum.intValue(0x2L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX_4", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.IDEX_4", xdc.services.intern.xsr.Enum.intValue(0x3L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX_5", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.IDEX_5", xdc.services.intern.xsr.Enum.intValue(0x4L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX_6", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.IDEX_6", xdc.services.intern.xsr.Enum.intValue(0x5L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX_7", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.IDEX_7", xdc.services.intern.xsr.Enum.intValue(0x6L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.IDEX_8", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.Timer.IDEX_8", xdc.services.intern.xsr.Enum.intValue(0x7L)+0));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.MAX_PERIOD", 0x0000ffffL);
        om.bind("ti.sysbios.family.arm.tm4l.Timer.MIN_SWEEP_PERIOD", 1L);
        om.bind("ti.sysbios.family.arm.tm4l.Timer.getNumTimers", new Extern("ti_sysbios_family_arm_tm4l_Timer_getNumTimers__E", "xdc_UInt(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.getStatus", new Extern("ti_sysbios_family_arm_tm4l_Timer_getStatus__E", "ti_sysbios_interfaces_ITimer_Status(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.startup", new Extern("ti_sysbios_family_arm_tm4l_Timer_startup__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.oneShotStub", new Extern("ti_sysbios_family_arm_tm4l_Timer_oneShotStub__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.oneShotNestStub", new Extern("ti_sysbios_family_arm_tm4l_Timer_oneShotNestStub__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.periodicStub", new Extern("ti_sysbios_family_arm_tm4l_Timer_periodicStub__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.periodicNestStub", new Extern("ti_sysbios_family_arm_tm4l_Timer_periodicNestStub__E", "xdc_Void(*)(xdc_UArg)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.getHandle", new Extern("ti_sysbios_family_arm_tm4l_Timer_getHandle__E", "ti_sysbios_family_arm_tm4l_Timer_Handle(*)(xdc_UInt)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.initDevice", new Extern("ti_sysbios_family_arm_tm4l_Timer_initDevice__I", "xdc_Void(*)(ti_sysbios_family_arm_tm4l_Timer_Object*)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.postInit", new Extern("ti_sysbios_family_arm_tm4l_Timer_postInit__I", "xdc_Int(*)(ti_sysbios_family_arm_tm4l_Timer_Object*,xdc_runtime_Error_Block*)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.Timer.checkOverflow", new Extern("ti_sysbios_family_arm_tm4l_Timer_checkOverflow__I", "xdc_Bool(*)(xdc_UInt32,xdc_UInt32)", true, false));
    }

    void TimestampProvider$$CONSTS()
    {
        // module TimestampProvider
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.Source_ACLK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Source", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.TimestampProvider.Source_ACLK", 0));
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.Source_SMCLK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Source", "ti.sysbios.family.arm.tm4l"), "ti.sysbios.family.arm.tm4l.TimestampProvider.Source_SMCLK", 1));
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.get32", new Extern("ti_sysbios_family_arm_tm4l_TimestampProvider_get32__E", "xdc_Bits32(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.get64", new Extern("ti_sysbios_family_arm_tm4l_TimestampProvider_get64__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.getFreq", new Extern("ti_sysbios_family_arm_tm4l_TimestampProvider_getFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.startTimer", new Extern("ti_sysbios_family_arm_tm4l_TimestampProvider_startTimer__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider.rolloverFunc", new Extern("ti_sysbios_family_arm_tm4l_TimestampProvider_rolloverFunc__E", "xdc_Void(*)(xdc_UArg)", true, false));
    }

    void Power$$CONSTS()
    {
        // module Power
        om.bind("ti.sysbios.family.arm.tm4l.Power.idleCPU", new Extern("ti_sysbios_family_arm_tm4l_Power_idleCPU__E", "xdc_Void(*)(xdc_Void)", true, false));
    }

    void Timer$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$tm4l$Timer$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.sysbios.family.arm.tm4l.Timer.create() called before xdc.useModule('ti.sysbios.family.arm.tm4l.Timer')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst.$args.id, inst.$args.tickFxn, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.tm4l.Timer$$create", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Module", "ti.sysbios.family.arm.tm4l"), om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Instance", "ti.sysbios.family.arm.tm4l"), 3, 2, false));
                        fxn.addArg(0, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(1, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(2, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Params", "ti.sysbios.family.arm.tm4l"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$tm4l$Timer$$create = function( id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.tm4l.Timer'];\n");
                sb.append("var __inst = xdc.om['ti.sysbios.family.arm.tm4l.Timer.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.sysbios.family.arm.tm4l']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.sysbios.family.arm.tm4l.Timer'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [id, tickFxn]);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.tm4l.Timer'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.sysbios.family.arm.tm4l.Timer$$construct", new Proto.Fxn(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Module", "ti.sysbios.family.arm.tm4l"), null, 4, 2, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Object", "ti.sysbios.family.arm.tm4l"), null);
                        fxn.addArg(1, "id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
                        fxn.addArg(2, "tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF);
                        fxn.addArg(3, "__params", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Params", "ti.sysbios.family.arm.tm4l"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$sysbios$family$arm$tm4l$Timer$$construct = function( __obj, id, tickFxn, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.sysbios.family.arm.tm4l.Timer'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {id:id, tickFxn:tickFxn});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.sysbios.family.arm.tm4l.Timer'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.sysbios.family.arm.tm4l.Timer'].__initObject(__inst);\n");
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

    void Power$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Timer$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TimestampProvider$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Power$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Timer$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Control", "ti.sysbios.family.arm.tm4l");
        sizes.clear();
        sizes.add(Global.newArray("source", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Control']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Control']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Control'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.TimerDevice", "ti.sysbios.family.arm.tm4l");
        sizes.clear();
        sizes.add(Global.newArray("intNum", "UInt"));
        sizes.add(Global.newArray("baseAddr", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.TimerDevice']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.TimerDevice']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.TimerDevice'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Instance_State", "ti.sysbios.family.arm.tm4l");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("staticInst", "UShort"));
        sizes.add(Global.newArray("id", "TInt"));
        sizes.add(Global.newArray("controlRegInit", "UInt"));
        sizes.add(Global.newArray("runMode", "Nti.sysbios.interfaces.ITimer.RunMode;;;;"));
        sizes.add(Global.newArray("startMode", "Nti.sysbios.interfaces.ITimer.StartMode;;;"));
        sizes.add(Global.newArray("period", "UInt"));
        sizes.add(Global.newArray("periodType", "Nti.sysbios.interfaces.ITimer.PeriodType;;;"));
        sizes.add(Global.newArray("intNum", "UInt"));
        sizes.add(Global.newArray("arg", "UIArg"));
        sizes.add(Global.newArray("tickFxn", "UFxn"));
        sizes.add(Global.newArray("frequency", "Sxdc.runtime.Types;FreqHz"));
        sizes.add(Global.newArray("hwi", "UPtr"));
        sizes.add(Global.newArray("prevThreshold", "UInt"));
        sizes.add(Global.newArray("synchronous", "UShort"));
        sizes.add(Global.newArray("inputDivider", "UInt"));
        sizes.add(Global.newArray("inputDividerExp", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Module_State", "ti.sysbios.family.arm.tm4l");
        sizes.clear();
        sizes.add(Global.newArray("availMask", "UInt"));
        sizes.add(Global.newArray("device", "UPtr"));
        sizes.add(Global.newArray("handles", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.tm4l.Timer.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void TimestampProvider$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State", "ti.sysbios.family.arm.tm4l");
        sizes.clear();
        sizes.add(Global.newArray("timer", "UPtr"));
        sizes.add(Global.newArray("rollovers", "UInt32"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Power$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Timer$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/tm4l/Timer.xs");
        om.bind("ti.sysbios.family.arm.tm4l.Timer$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Module", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.Module", om.findStrict("ti.sysbios.interfaces.ITimer.Module", "ti.sysbios.family.arm.tm4l"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0x0000ffffL, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("supportsDynamic", $$T_Bool, true, "wh");
            po.addFld("defaultDynamic", $$T_Bool, false, "wh");
            po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.sysbios.family.arm.tm4l"), $$UNDEF, "wh");
            po.addFld("E_invalidTimer", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.tm4l"), Global.newObject("msg", "E_invalidTimer: Invalid Timer Id %d"), "w");
            po.addFld("E_notAvailable", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.tm4l"), Global.newObject("msg", "E_notAvailable: Timer not available %d"), "w");
            po.addFld("E_cannotSupport", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.tm4l"), Global.newObject("msg", "E_cannotSupport: Timer cannot support requested period %d"), "w");
            po.addFld("E_runtimeCreate", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.sysbios.family.arm.tm4l"), Global.newObject("msg", "E_runtimeCreate: Runtime Timer create is not supported %d"), "w");
            po.addFld("anyMask", Proto.Elm.newCNum("(xdc_UInt)"), 0xFL, "w");
            po.addFld("keepAwake", $$T_Bool, false, "w");
            po.addFld("startupNeeded", Proto.Elm.newCNum("(xdc_UInt)"), false, "w");
            po.addFld("numTimerDevices", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$create", "ti.sysbios.family.arm.tm4l"), Global.get("ti$sysbios$family$arm$tm4l$Timer$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$construct", "ti.sysbios.family.arm.tm4l"), Global.get("ti$sysbios$family$arm$tm4l$Timer$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Timer$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Timer$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Timer$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Timer$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Timer$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Timer$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getFreqMeta");
                if (fxn != null) po.addFxn("getFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimer$$getFreqMeta", "ti.sysbios.family.arm.tm4l"), fxn);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Instance", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.Instance", om.findStrict("ti.sysbios.interfaces.ITimer.Instance", "ti.sysbios.family.arm.tm4l"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0x0000ffffL, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("clockSource", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source", "ti.sysbios.family.arm.tm4l"), om.find("ti.sysbios.family.arm.tm4l.Timer.Source_ACLK"), "w");
            po.addFld("synchronous", $$T_Bool, false, "w");
            po.addFld("inputDivider", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID", "ti.sysbios.family.arm.tm4l"), om.find("ti.sysbios.family.arm.tm4l.Timer.ID_1"), "w");
            po.addFld("inputDividerExp", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), om.find("ti.sysbios.family.arm.tm4l.Timer.IDEX_1"), "w");
            po.addFld("nesting", $$T_Bool, false, "w");
            po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
            po.addFld("controlRegInit", Proto.Elm.newCNum("(xdc_UInt)"), om.find("ti.sysbios.family.arm.tm4l.Timer.Source_ACLK"), "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.tm4l"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Params", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.Params", om.findStrict("ti.sysbios.interfaces.ITimer$$Params", "ti.sysbios.family.arm.tm4l"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ANY", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "rh");
                po.addFld("MAX_PERIOD", Proto.Elm.newCNum("(xdc_UInt)"), 0x0000ffffL, "rh");
                po.addFld("MIN_SWEEP_PERIOD", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
        if (isCFG) {
            po.addFld("clockSource", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source", "ti.sysbios.family.arm.tm4l"), om.find("ti.sysbios.family.arm.tm4l.Timer.Source_ACLK"), "w");
            po.addFld("synchronous", $$T_Bool, false, "w");
            po.addFld("inputDivider", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID", "ti.sysbios.family.arm.tm4l"), om.find("ti.sysbios.family.arm.tm4l.Timer.ID_1"), "w");
            po.addFld("inputDividerExp", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"), om.find("ti.sysbios.family.arm.tm4l.Timer.IDEX_1"), "w");
            po.addFld("nesting", $$T_Bool, false, "w");
            po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "w");
            po.addFld("controlRegInit", Proto.Elm.newCNum("(xdc_UInt)"), om.find("ti.sysbios.family.arm.tm4l.Timer.Source_ACLK"), "w");
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.sysbios.family.arm.tm4l"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Object", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.Object", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Instance", "ti.sysbios.family.arm.tm4l"));
        // struct Timer.Control
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Control", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.Control", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("source", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct Timer.BasicView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$BasicView", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.BasicView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("halTimerHandle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("label", $$T_Str, $$UNDEF, "w");
                po.addFld("id", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("configuration", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Control", "ti.sysbios.family.arm.tm4l"), $$DEFAULT, "w");
                po.addFld("startMode", $$T_Str, $$UNDEF, "w");
                po.addFld("runMode", $$T_Str, $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("periodType", $$T_Str, $$UNDEF, "w");
                po.addFld("synchronous", $$T_Bool, $$UNDEF, "w");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("tickFxn", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("frequency", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("hwiHandle", $$T_Str, $$UNDEF, "w");
                po.addFxn("$xml", $$T_Met, Global.get("$$xml"));
        // struct Timer.DeviceView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$DeviceView", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.DeviceView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("id", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("device", $$T_Str, $$UNDEF, "w");
                po.addFld("devAddr", $$T_Str, $$UNDEF, "w");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("runMode", $$T_Str, $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("currCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("remainingCount", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("state", $$T_Str, $$UNDEF, "w");
        // struct Timer.ModuleView
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$ModuleView", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.ModuleView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("availMask", $$T_Str, $$UNDEF, "w");
        // struct Timer.TimerDevice
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$TimerDevice", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.TimerDevice", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("baseAddr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        // struct Timer.Instance_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Instance_State", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("staticInst", $$T_Bool, $$UNDEF, "w");
                po.addFld("id", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("controlRegInit", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("runMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.tm4l"), $$UNDEF, "w");
                po.addFld("startMode", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.tm4l"), $$UNDEF, "w");
                po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("periodType", (Proto)om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.tm4l"), $$UNDEF, "w");
                po.addFld("intNum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "w");
                po.addFld("tickFxn", new Proto.Adr("xdc_Void(*)(xdc_UArg)", "PFv"), $$UNDEF, "w");
                po.addFld("frequency", (Proto)om.findStrict("xdc.runtime.Types.FreqHz", "ti.sysbios.family.arm.tm4l"), $$DEFAULT, "w");
                po.addFld("hwi", (Proto)om.findStrict("ti.sysbios.hal.Hwi.Handle", "ti.sysbios.family.arm.tm4l"), $$UNDEF, "w");
                po.addFld("prevThreshold", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("synchronous", $$T_Bool, $$UNDEF, "w");
                po.addFld("inputDivider", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("inputDividerExp", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct Timer.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Module_State", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Timer.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("availMask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("device", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.TimerDevice", "ti.sysbios.family.arm.tm4l"), false), $$DEFAULT, "w");
                po.addFld("handles", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Handle", "ti.sysbios.family.arm.tm4l"), false), $$DEFAULT, "w");
    }

    void TimestampProvider$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/tm4l/TimestampProvider.xs");
        om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Module", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.TimestampProvider.Module", om.findStrict("ti.sysbios.interfaces.ITimestamp.Module", "ti.sysbios.family.arm.tm4l"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("timerId", Proto.Elm.newCNum("(xdc_UInt)"), Global.eval("~0"), "w");
            po.addFld("useClockTimer", $$T_Bool, $$UNDEF, "w");
            po.addFld("clockSource", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Source", "ti.sysbios.family.arm.tm4l"), om.find("ti.sysbios.family.arm.tm4l.TimestampProvider.Source_ACLK"), "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.TimestampProvider$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "getFreqMeta");
                if (fxn != null) po.addFxn("getFreqMeta", (Proto.Fxn)om.findStrict("ti.sysbios.interfaces.ITimestamp$$getFreqMeta", "ti.sysbios.family.arm.tm4l"), fxn);
        // struct TimestampProvider.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider$$Module_State", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("timer", (Proto)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Handle", "ti.sysbios.family.arm.tm4l"), $$UNDEF, "w");
                po.addFld("rollovers", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "w");
    }

    void Power$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/tm4l/Power.xs");
        om.bind("ti.sysbios.family.arm.tm4l.Power$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Power.Module", "ti.sysbios.family.arm.tm4l");
        po.init("ti.sysbios.family.arm.tm4l.Power.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.family.arm.tm4l"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("idle", $$T_Bool, false, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Power$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Power$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Power$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.arm.tm4l.Power$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void Timer$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer", "ti.sysbios.family.arm.tm4l");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Instance_State", "ti.sysbios.family.arm.tm4l");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Control$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.tm4l.Timer.Control", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Control", "ti.sysbios.family.arm.tm4l");
        vo.bind("TimerDevice$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.tm4l.Timer.TimerDevice", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$TimerDevice", "ti.sysbios.family.arm.tm4l");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.tm4l.Timer.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Instance_State", "ti.sysbios.family.arm.tm4l");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.tm4l.Timer.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$Module_State", "ti.sysbios.family.arm.tm4l");
        po.bind("device$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.tm4l.Timer.TimerDevice", "isScalar", false));
        po.bind("handles$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Ptr", "isScalar", true));
    }

    void TimestampProvider$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider", "ti.sysbios.family.arm.tm4l");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider$$Module_State", "ti.sysbios.family.arm.tm4l");
    }

    void Power$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Power", "ti.sysbios.family.arm.tm4l");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.family.arm.tm4l.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.family.arm.tm4l"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/arm/tm4l/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.family.arm.tm4l"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.family.arm.tm4l"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.family.arm.tm4l"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.family.arm.tm4l"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.family.arm.tm4l"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.family.arm.tm4l"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.family.arm.tm4l", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.family.arm.tm4l");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.family.arm.tm4l.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.family.arm.tm4l'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.arm.tm4l.aem4',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.arm.tm4l.aem4f',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.arm.tm4l.am4g',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.arm.tm4l.am4fg',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.arm.tm4l.arm4',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.arm.tm4l.arm4f',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.arm.tm4l.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.arm.tm4l.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.arm.tm4l.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.arm.tm4l.am4fg', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.arm.tm4l.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.arm.tm4l.arm4f', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Timer$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer", "ti.sysbios.family.arm.tm4l");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Module", "ti.sysbios.family.arm.tm4l");
        vo.init2(po, "ti.sysbios.family.arm.tm4l.Timer", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.tm4l.Timer$$capsule", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Instance", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Instance", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Params", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Params", "ti.sysbios.family.arm.tm4l"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Params", "ti.sysbios.family.arm.tm4l")).newInstance());
        vo.bind("Handle", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Handle", "ti.sysbios.family.arm.tm4l"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.tm4l", "ti.sysbios.family.arm.tm4l"));
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
        vo.bind("FuncPtr", om.findStrict("ti.sysbios.interfaces.ITimer.FuncPtr", "ti.sysbios.family.arm.tm4l"));
        vo.bind("StartMode", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode", "ti.sysbios.family.arm.tm4l"));
        vo.bind("RunMode", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Status", om.findStrict("ti.sysbios.interfaces.ITimer.Status", "ti.sysbios.family.arm.tm4l"));
        vo.bind("PeriodType", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Source", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source", "ti.sysbios.family.arm.tm4l"));
        vo.bind("ID", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Control", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Control", "ti.sysbios.family.arm.tm4l"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Control", "ti.sysbios.family.arm.tm4l"));
        vo.bind("BasicView", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.BasicView", "ti.sysbios.family.arm.tm4l"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.BasicView", "ti.sysbios.family.arm.tm4l"));
        vo.bind("DeviceView", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.DeviceView", "ti.sysbios.family.arm.tm4l"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.DeviceView", "ti.sysbios.family.arm.tm4l"));
        vo.bind("ModuleView", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ModuleView", "ti.sysbios.family.arm.tm4l"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ModuleView", "ti.sysbios.family.arm.tm4l"));
        mcfgs.add("E_invalidTimer");
        mcfgs.add("E_notAvailable");
        mcfgs.add("E_cannotSupport");
        mcfgs.add("E_runtimeCreate");
        mcfgs.add("anyMask");
        mcfgs.add("keepAwake");
        mcfgs.add("startupNeeded");
        icfgs.add("startupNeeded");
        vo.bind("TimerDevice", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.TimerDevice", "ti.sysbios.family.arm.tm4l"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.TimerDevice", "ti.sysbios.family.arm.tm4l"));
        mcfgs.add("numTimerDevices");
        icfgs.add("numTimerDevices");
        vo.bind("Instance_State", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Instance_State", "ti.sysbios.family.arm.tm4l"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Instance_State", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Module_State", "ti.sysbios.family.arm.tm4l"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Module_State", "ti.sysbios.family.arm.tm4l"));
        vo.bind("StartMode_AUTO", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_AUTO", "ti.sysbios.family.arm.tm4l"));
        vo.bind("StartMode_USER", om.findStrict("ti.sysbios.interfaces.ITimer.StartMode_USER", "ti.sysbios.family.arm.tm4l"));
        vo.bind("RunMode_CONTINUOUS", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_CONTINUOUS", "ti.sysbios.family.arm.tm4l"));
        vo.bind("RunMode_ONESHOT", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_ONESHOT", "ti.sysbios.family.arm.tm4l"));
        vo.bind("RunMode_DYNAMIC", om.findStrict("ti.sysbios.interfaces.ITimer.RunMode_DYNAMIC", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Status_INUSE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_INUSE", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Status_FREE", om.findStrict("ti.sysbios.interfaces.ITimer.Status_FREE", "ti.sysbios.family.arm.tm4l"));
        vo.bind("PeriodType_MICROSECS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_MICROSECS", "ti.sysbios.family.arm.tm4l"));
        vo.bind("PeriodType_COUNTS", om.findStrict("ti.sysbios.interfaces.ITimer.PeriodType_COUNTS", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Source_ACLK", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source_ACLK", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Source_SMCLK", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source_SMCLK", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Source_EXTERNAL", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source_EXTERNAL", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Source_EXTERNAL_INVERTED", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Source_EXTERNAL_INVERTED", "ti.sysbios.family.arm.tm4l"));
        vo.bind("ID_1", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID_1", "ti.sysbios.family.arm.tm4l"));
        vo.bind("ID_2", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID_2", "ti.sysbios.family.arm.tm4l"));
        vo.bind("ID_4", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID_4", "ti.sysbios.family.arm.tm4l"));
        vo.bind("ID_8", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.ID_8", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX_1", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX_1", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX_2", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX_2", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX_3", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX_3", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX_4", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX_4", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX_5", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX_5", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX_6", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX_6", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX_7", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX_7", "ti.sysbios.family.arm.tm4l"));
        vo.bind("IDEX_8", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.IDEX_8", "ti.sysbios.family.arm.tm4l"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.tm4l")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.sysbios.family.arm.tm4l.Timer$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$sysbios$family$arm$tm4l$Timer$$__initObject"));
        }//isCFG
        vo.bind("getNumTimers", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.getNumTimers", "ti.sysbios.family.arm.tm4l"));
        vo.bind("getStatus", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.getStatus", "ti.sysbios.family.arm.tm4l"));
        vo.bind("startup", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.startup", "ti.sysbios.family.arm.tm4l"));
        vo.bind("oneShotStub", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.oneShotStub", "ti.sysbios.family.arm.tm4l"));
        vo.bind("oneShotNestStub", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.oneShotNestStub", "ti.sysbios.family.arm.tm4l"));
        vo.bind("periodicStub", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.periodicStub", "ti.sysbios.family.arm.tm4l"));
        vo.bind("periodicNestStub", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.periodicNestStub", "ti.sysbios.family.arm.tm4l"));
        vo.bind("getHandle", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.getHandle", "ti.sysbios.family.arm.tm4l"));
        vo.bind("initDevice", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.initDevice", "ti.sysbios.family.arm.tm4l"));
        vo.bind("postInit", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.postInit", "ti.sysbios.family.arm.tm4l"));
        vo.bind("checkOverflow", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.checkOverflow", "ti.sysbios.family.arm.tm4l"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_tm4l_Timer_Handle__label__E", "ti_sysbios_family_arm_tm4l_Timer_Module__startupDone__E", "ti_sysbios_family_arm_tm4l_Timer_Object__create__E", "ti_sysbios_family_arm_tm4l_Timer_Object__delete__E", "ti_sysbios_family_arm_tm4l_Timer_Object__destruct__E", "ti_sysbios_family_arm_tm4l_Timer_Object__get__E", "ti_sysbios_family_arm_tm4l_Timer_Object__first__E", "ti_sysbios_family_arm_tm4l_Timer_Object__next__E", "ti_sysbios_family_arm_tm4l_Timer_Params__init__E", "ti_sysbios_family_arm_tm4l_Timer_getNumTimers__E", "ti_sysbios_family_arm_tm4l_Timer_getStatus__E", "ti_sysbios_family_arm_tm4l_Timer_startup__E", "ti_sysbios_family_arm_tm4l_Timer_getMaxTicks__E", "ti_sysbios_family_arm_tm4l_Timer_setNextTick__E", "ti_sysbios_family_arm_tm4l_Timer_start__E", "ti_sysbios_family_arm_tm4l_Timer_stop__E", "ti_sysbios_family_arm_tm4l_Timer_setPeriod__E", "ti_sysbios_family_arm_tm4l_Timer_setPeriodMicroSecs__E", "ti_sysbios_family_arm_tm4l_Timer_getPeriod__E", "ti_sysbios_family_arm_tm4l_Timer_getCount__E", "ti_sysbios_family_arm_tm4l_Timer_getFreq__E", "ti_sysbios_family_arm_tm4l_Timer_getFunc__E", "ti_sysbios_family_arm_tm4l_Timer_setFunc__E", "ti_sysbios_family_arm_tm4l_Timer_trigger__E", "ti_sysbios_family_arm_tm4l_Timer_getExpiredCounts__E", "ti_sysbios_family_arm_tm4l_Timer_oneShotStub__E", "ti_sysbios_family_arm_tm4l_Timer_oneShotNestStub__E", "ti_sysbios_family_arm_tm4l_Timer_periodicStub__E", "ti_sysbios_family_arm_tm4l_Timer_periodicNestStub__E", "ti_sysbios_family_arm_tm4l_Timer_getHandle__E", "ti_sysbios_family_arm_tm4l_Timer_reconfig__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray("E_invalidTimer", "E_notAvailable", "E_cannotSupport", "E_runtimeCreate"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.sysbios.family.arm.tm4l.Timer.Object", "ti.sysbios.family.arm.tm4l"));
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "oneShotStub", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "oneShotNestStub", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "periodicStub", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "periodicNestStub", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "getHandle", "entry", "0x%x", "exit", "%p"));
        loggables.add(Global.newObject("name", "reconfig", "entry", "%p, %p, %p, %p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Timer", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Timer");
    }

    void TimestampProvider$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider", "ti.sysbios.family.arm.tm4l");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Module", "ti.sysbios.family.arm.tm4l");
        vo.init2(po, "ti.sysbios.family.arm.tm4l.TimestampProvider", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider$$capsule", "ti.sysbios.family.arm.tm4l"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.tm4l", "ti.sysbios.family.arm.tm4l"));
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
        vo.bind("Source", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Source", "ti.sysbios.family.arm.tm4l"));
        mcfgs.add("timerId");
        mcfgs.add("useClockTimer");
        mcfgs.add("clockSource");
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State", "ti.sysbios.family.arm.tm4l"));
        tdefs.add(om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Module_State", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Source_ACLK", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Source_ACLK", "ti.sysbios.family.arm.tm4l"));
        vo.bind("Source_SMCLK", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.Source_SMCLK", "ti.sysbios.family.arm.tm4l"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.tm4l")).add(vo);
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
        vo.bind("get32", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.get32", "ti.sysbios.family.arm.tm4l"));
        vo.bind("get64", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.get64", "ti.sysbios.family.arm.tm4l"));
        vo.bind("getFreq", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.getFreq", "ti.sysbios.family.arm.tm4l"));
        vo.bind("startTimer", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.startTimer", "ti.sysbios.family.arm.tm4l"));
        vo.bind("rolloverFunc", om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider.rolloverFunc", "ti.sysbios.family.arm.tm4l"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_tm4l_TimestampProvider_Module__startupDone__E", "ti_sysbios_family_arm_tm4l_TimestampProvider_get32__E", "ti_sysbios_family_arm_tm4l_TimestampProvider_get64__E", "ti_sysbios_family_arm_tm4l_TimestampProvider_getFreq__E", "ti_sysbios_family_arm_tm4l_TimestampProvider_startTimer__E", "ti_sysbios_family_arm_tm4l_TimestampProvider_rolloverFunc__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "startTimer", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "rolloverFunc", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("TimestampProvider", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TimestampProvider");
    }

    void Power$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Power", "ti.sysbios.family.arm.tm4l");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Power.Module", "ti.sysbios.family.arm.tm4l");
        vo.init2(po, "ti.sysbios.family.arm.tm4l.Power", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.arm.tm4l.Power$$capsule", "ti.sysbios.family.arm.tm4l"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.arm.tm4l", "ti.sysbios.family.arm.tm4l"));
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
        mcfgs.add("idle");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.arm.tm4l")).add(vo);
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
        vo.bind("idleCPU", om.findStrict("ti.sysbios.family.arm.tm4l.Power.idleCPU", "ti.sysbios.family.arm.tm4l"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_arm_tm4l_Power_Module__startupDone__E", "ti_sysbios_family_arm_tm4l_Power_idleCPU__E"));
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
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.sysbios.family.arm.tm4l")).findStrict("PARAMS", "ti.sysbios.family.arm.tm4l");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.sysbios.family.arm.tm4l.Timer", "ti.sysbios.family.arm.tm4l")).findStrict("PARAMS", "ti.sysbios.family.arm.tm4l");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.tm4l.Timer", "ti.sysbios.family.arm.tm4l"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.tm4l.TimestampProvider", "ti.sysbios.family.arm.tm4l"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.arm.tm4l.Power", "ti.sysbios.family.arm.tm4l"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.sysbios.family.arm.tm4l.Timer", "ti.sysbios.family.arm.tm4l");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Basic", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitBasic", "structName", "BasicView")}), Global.newArray(new Object[]{"Device", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE"), "viewInitFxn", "viewInitDevice", "structName", "DeviceView")}), Global.newArray(new Object[]{"Module", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE"), "viewInitFxn", "viewInitModule", "structName", "ModuleView")})}))));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.family.arm.tm4l.Timer")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.tm4l.TimestampProvider")).bless();
        ((Value.Obj)om.getv("ti.sysbios.family.arm.tm4l.Power")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.family.arm.tm4l")).add(pkgV);
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
        Timer$$OBJECTS();
        TimestampProvider$$OBJECTS();
        Power$$OBJECTS();
        Timer$$CONSTS();
        TimestampProvider$$CONSTS();
        Power$$CONSTS();
        Timer$$CREATES();
        TimestampProvider$$CREATES();
        Power$$CREATES();
        Timer$$FUNCTIONS();
        TimestampProvider$$FUNCTIONS();
        Power$$FUNCTIONS();
        Timer$$SIZES();
        TimestampProvider$$SIZES();
        Power$$SIZES();
        Timer$$TYPES();
        TimestampProvider$$TYPES();
        Power$$TYPES();
        if (isROV) {
            Timer$$ROV();
            TimestampProvider$$ROV();
            Power$$ROV();
        }//isROV
        $$SINGLETONS();
        Timer$$SINGLETONS();
        TimestampProvider$$SINGLETONS();
        Power$$SINGLETONS();
        $$INITIALIZATION();
    }
}
