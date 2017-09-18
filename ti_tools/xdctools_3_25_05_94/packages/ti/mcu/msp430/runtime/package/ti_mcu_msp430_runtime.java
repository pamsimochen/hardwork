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

public class ti_mcu_msp430_runtime
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
        pkgP = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.mcu.msp430.runtime", new Value.Obj("ti.mcu.msp430.runtime", pkgP));
    }

    void SysBuf$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.SysBuf.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.SysBuf", new Value.Obj("ti.mcu.msp430.runtime.SysBuf", po));
        pkgV.bind("SysBuf", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.SysBuf$$Module_State", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.SysBuf.Module_State", new Proto.Str(spo, false));
    }

    void SysUart$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.SysUart.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.SysUart", new Value.Obj("ti.mcu.msp430.runtime.SysUart", po));
        pkgV.bind("SysUart", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.SysUart$$Module_State", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.SysUart.Module_State", new Proto.Str(spo, false));
    }

    void SysCallback$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.SysCallback.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.SysCallback", new Value.Obj("ti.mcu.msp430.runtime.SysCallback", po));
        pkgV.bind("SysCallback", vo);
        // decls 
    }

    void LoggerUart$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerUart.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.LoggerUart", new Value.Obj("ti.mcu.msp430.runtime.LoggerUart", po));
        pkgV.bind("LoggerUart", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerUart$$Instance_State", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.LoggerUart.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.mcu.msp430.runtime.LoggerUart.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerUart$$Object", new Proto.Obj());
        Object objP = om.bind("ti.mcu.msp430.runtime.LoggerUart.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerUart$$Params", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.LoggerUart.Params", new Proto.Str(po, false));
        om.bind("ti.mcu.msp430.runtime.LoggerUart.Handle", insP);
        if (isROV) {
            om.bind("ti.mcu.msp430.runtime.LoggerUart.Object", om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Instance_State", "ti.mcu.msp430.runtime"));
        }//isROV
    }

    void LoggerCallback$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerCallback.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.LoggerCallback", new Value.Obj("ti.mcu.msp430.runtime.LoggerCallback", po));
        pkgV.bind("LoggerCallback", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerCallback$$Instance_State", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.LoggerCallback.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.mcu.msp430.runtime.LoggerCallback.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerCallback$$Object", new Proto.Obj());
        Object objP = om.bind("ti.mcu.msp430.runtime.LoggerCallback.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerCallback$$Params", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.LoggerCallback.Params", new Proto.Str(po, false));
        om.bind("ti.mcu.msp430.runtime.LoggerCallback.Handle", insP);
        if (isROV) {
            om.bind("ti.mcu.msp430.runtime.LoggerCallback.Object", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Instance_State", "ti.mcu.msp430.runtime"));
        }//isROV
    }

    void GateGIE$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.GateGIE.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.GateGIE", new Value.Obj("ti.mcu.msp430.runtime.GateGIE", po));
        pkgV.bind("GateGIE", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.GateGIE$$Instance_State", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.GateGIE.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.mcu.msp430.runtime.GateGIE.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.GateGIE$$Object", new Proto.Obj());
        Object objP = om.bind("ti.mcu.msp430.runtime.GateGIE.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.GateGIE$$Params", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.GateGIE.Params", new Proto.Str(po, false));
        om.bind("ti.mcu.msp430.runtime.GateGIE.Handle", insP);
        if (isROV) {
            om.bind("ti.mcu.msp430.runtime.GateGIE.Object", om.findStrict("ti.mcu.msp430.runtime.GateGIE.Instance_State", "ti.mcu.msp430.runtime"));
        }//isROV
    }

    void HeapNull$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.HeapNull.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.HeapNull", new Value.Obj("ti.mcu.msp430.runtime.HeapNull", po));
        pkgV.bind("HeapNull", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.HeapNull$$Instance_State", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.HeapNull.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.mcu.msp430.runtime.HeapNull.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.HeapNull$$Object", new Proto.Obj());
        Object objP = om.bind("ti.mcu.msp430.runtime.HeapNull.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.HeapNull$$Params", new Proto.Obj());
        om.bind("ti.mcu.msp430.runtime.HeapNull.Params", new Proto.Str(po, false));
        om.bind("ti.mcu.msp430.runtime.HeapNull.Handle", insP);
        if (isROV) {
            om.bind("ti.mcu.msp430.runtime.HeapNull.Object", om.findStrict("ti.mcu.msp430.runtime.HeapNull.Instance_State", "ti.mcu.msp430.runtime"));
        }//isROV
    }

    void LoggerUart_TimestampProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy", new Value.Obj("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy", po));
        pkgV.bind("LoggerUart_TimestampProxy", vo);
        // decls 
    }

    void LoggerCallback_TimestampProxy$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy", new Value.Obj("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy", po));
        pkgV.bind("LoggerCallback_TimestampProxy", vo);
        // decls 
    }

    void SysBuf$$CONSTS()
    {
        // module SysBuf
        om.bind("ti.mcu.msp430.runtime.SysBuf.abort", new Extern("ti_mcu_msp430_runtime_SysBuf_abort__E", "xdc_Void(*)(xdc_CString)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysBuf.exit", new Extern("ti_mcu_msp430_runtime_SysBuf_exit__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysBuf.flush", new Extern("ti_mcu_msp430_runtime_SysBuf_flush__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysBuf.putch", new Extern("ti_mcu_msp430_runtime_SysBuf_putch__E", "xdc_Void(*)(xdc_Char)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysBuf.ready", new Extern("ti_mcu_msp430_runtime_SysBuf_ready__E", "xdc_Bool(*)(xdc_Void)", true, false));
    }

    void SysUart$$CONSTS()
    {
        // module SysUart
        om.bind("ti.mcu.msp430.runtime.SysUart.abort", new Extern("ti_mcu_msp430_runtime_SysUart_abort__E", "xdc_Void(*)(xdc_CString)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysUart.exit", new Extern("ti_mcu_msp430_runtime_SysUart_exit__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysUart.flush", new Extern("ti_mcu_msp430_runtime_SysUart_flush__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysUart.putch", new Extern("ti_mcu_msp430_runtime_SysUart_putch__E", "xdc_Void(*)(xdc_Char)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysUart.ready", new Extern("ti_mcu_msp430_runtime_SysUart_ready__E", "xdc_Bool(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysUart.rxtxInit", new Extern("ti_mcu_msp430_runtime_SysUart_rxtxInit__I", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysUart.tx", new Extern("ti_mcu_msp430_runtime_SysUart_tx__I", "xdc_Void(*)(xdc_Char)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysUart.rx", new Extern("ti_mcu_msp430_runtime_SysUart_rx__I", "xdc_Void(*)(xdc_Char)", true, false));
    }

    void SysCallback$$CONSTS()
    {
        // module SysCallback
        om.bind("ti.mcu.msp430.runtime.SysCallback.abort", new Extern("ti_mcu_msp430_runtime_SysCallback_abort__E", "xdc_Void(*)(xdc_CString)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysCallback.exit", new Extern("ti_mcu_msp430_runtime_SysCallback_exit__E", "xdc_Void(*)(xdc_Int)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysCallback.flush", new Extern("ti_mcu_msp430_runtime_SysCallback_flush__E", "xdc_Void(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysCallback.putch", new Extern("ti_mcu_msp430_runtime_SysCallback_putch__E", "xdc_Void(*)(xdc_Char)", true, false));
        om.bind("ti.mcu.msp430.runtime.SysCallback.ready", new Extern("ti_mcu_msp430_runtime_SysCallback_ready__E", "xdc_Bool(*)(xdc_Void)", true, false));
    }

    void LoggerUart$$CONSTS()
    {
        // module LoggerUart
    }

    void LoggerCallback$$CONSTS()
    {
        // module LoggerCallback
    }

    void GateGIE$$CONSTS()
    {
        // module GateGIE
        om.bind("ti.mcu.msp430.runtime.GateGIE.query", new Extern("ti_mcu_msp430_runtime_GateGIE_query__E", "xdc_Bool(*)(xdc_Int)", true, false));
    }

    void HeapNull$$CONSTS()
    {
        // module HeapNull
    }

    void LoggerUart_TimestampProxy$$CONSTS()
    {
        // module LoggerUart_TimestampProxy
        om.bind("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.get32", new Extern("ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_get32__E", "xdc_Bits32(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.get64", new Extern("ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_get64__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*)", true, false));
        om.bind("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.getFreq", new Extern("ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_getFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
    }

    void LoggerCallback_TimestampProxy$$CONSTS()
    {
        // module LoggerCallback_TimestampProxy
        om.bind("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.get32", new Extern("ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_get32__E", "xdc_Bits32(*)(xdc_Void)", true, false));
        om.bind("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.get64", new Extern("ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_get64__E", "xdc_Void(*)(xdc_runtime_Types_Timestamp64*)", true, false));
        om.bind("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.getFreq", new Extern("ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_getFreq__E", "xdc_Void(*)(xdc_runtime_Types_FreqHz*)", true, false));
    }

    void SysBuf$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SysUart$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SysCallback$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LoggerUart$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$LoggerUart$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.mcu.msp430.runtime.LoggerUart.create() called before xdc.useModule('ti.mcu.msp430.runtime.LoggerUart')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.mcu.msp430.runtime.LoggerUart$$create", new Proto.Fxn(om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Module", "ti.mcu.msp430.runtime"), om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Instance", "ti.mcu.msp430.runtime"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Params", "ti.mcu.msp430.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$LoggerUart$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.mcu.msp430.runtime.LoggerUart'];\n");
                sb.append("var __inst = xdc.om['ti.mcu.msp430.runtime.LoggerUart.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.mcu.msp430.runtime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.mcu.msp430.runtime.LoggerUart'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.mcu.msp430.runtime.LoggerUart'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.mcu.msp430.runtime.LoggerUart$$construct", new Proto.Fxn(om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Module", "ti.mcu.msp430.runtime"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$Object", "ti.mcu.msp430.runtime"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Params", "ti.mcu.msp430.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$LoggerUart$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.mcu.msp430.runtime.LoggerUart'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.mcu.msp430.runtime.LoggerUart'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.mcu.msp430.runtime.LoggerUart'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerCallback$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$LoggerCallback$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.mcu.msp430.runtime.LoggerCallback.create() called before xdc.useModule('ti.mcu.msp430.runtime.LoggerCallback')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.mcu.msp430.runtime.LoggerCallback$$create", new Proto.Fxn(om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Module", "ti.mcu.msp430.runtime"), om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Instance", "ti.mcu.msp430.runtime"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Params", "ti.mcu.msp430.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$LoggerCallback$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.mcu.msp430.runtime.LoggerCallback'];\n");
                sb.append("var __inst = xdc.om['ti.mcu.msp430.runtime.LoggerCallback.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.mcu.msp430.runtime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.mcu.msp430.runtime.LoggerCallback'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.mcu.msp430.runtime.LoggerCallback'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.mcu.msp430.runtime.LoggerCallback$$construct", new Proto.Fxn(om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Module", "ti.mcu.msp430.runtime"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$Object", "ti.mcu.msp430.runtime"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Params", "ti.mcu.msp430.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$LoggerCallback$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.mcu.msp430.runtime.LoggerCallback'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.mcu.msp430.runtime.LoggerCallback'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.mcu.msp430.runtime.LoggerCallback'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void GateGIE$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$GateGIE$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.mcu.msp430.runtime.GateGIE.create() called before xdc.useModule('ti.mcu.msp430.runtime.GateGIE')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.mcu.msp430.runtime.GateGIE$$create", new Proto.Fxn(om.findStrict("ti.mcu.msp430.runtime.GateGIE.Module", "ti.mcu.msp430.runtime"), om.findStrict("ti.mcu.msp430.runtime.GateGIE.Instance", "ti.mcu.msp430.runtime"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.mcu.msp430.runtime.GateGIE.Params", "ti.mcu.msp430.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$GateGIE$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.mcu.msp430.runtime.GateGIE'];\n");
                sb.append("var __inst = xdc.om['ti.mcu.msp430.runtime.GateGIE.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.mcu.msp430.runtime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.mcu.msp430.runtime.GateGIE'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.mcu.msp430.runtime.GateGIE'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.mcu.msp430.runtime.GateGIE$$construct", new Proto.Fxn(om.findStrict("ti.mcu.msp430.runtime.GateGIE.Module", "ti.mcu.msp430.runtime"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.mcu.msp430.runtime.GateGIE$$Object", "ti.mcu.msp430.runtime"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.mcu.msp430.runtime.GateGIE.Params", "ti.mcu.msp430.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$GateGIE$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.mcu.msp430.runtime.GateGIE'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.mcu.msp430.runtime.GateGIE'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.mcu.msp430.runtime.GateGIE'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void HeapNull$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$HeapNull$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.mcu.msp430.runtime.HeapNull.create() called before xdc.useModule('ti.mcu.msp430.runtime.HeapNull')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.mcu.msp430.runtime.HeapNull$$create", new Proto.Fxn(om.findStrict("ti.mcu.msp430.runtime.HeapNull.Module", "ti.mcu.msp430.runtime"), om.findStrict("ti.mcu.msp430.runtime.HeapNull.Instance", "ti.mcu.msp430.runtime"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.mcu.msp430.runtime.HeapNull.Params", "ti.mcu.msp430.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$HeapNull$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.mcu.msp430.runtime.HeapNull'];\n");
                sb.append("var __inst = xdc.om['ti.mcu.msp430.runtime.HeapNull.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.mcu.msp430.runtime']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.mcu.msp430.runtime.HeapNull'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.mcu.msp430.runtime.HeapNull'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.mcu.msp430.runtime.HeapNull$$construct", new Proto.Fxn(om.findStrict("ti.mcu.msp430.runtime.HeapNull.Module", "ti.mcu.msp430.runtime"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.mcu.msp430.runtime.HeapNull$$Object", "ti.mcu.msp430.runtime"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.mcu.msp430.runtime.HeapNull.Params", "ti.mcu.msp430.runtime"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$mcu$msp430$runtime$HeapNull$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.mcu.msp430.runtime.HeapNull'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.mcu.msp430.runtime.HeapNull'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.mcu.msp430.runtime.HeapNull'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void LoggerUart_TimestampProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LoggerCallback_TimestampProxy$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SysBuf$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SysUart$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SysCallback$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LoggerUart$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LoggerCallback$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void GateGIE$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void HeapNull$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LoggerUart_TimestampProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void LoggerCallback_TimestampProxy$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SysBuf$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.mcu.msp430.runtime.SysBuf.Module_State", "ti.mcu.msp430.runtime");
        sizes.clear();
        sizes.add(Global.newArray("outbuf", "UPtr"));
        sizes.add(Global.newArray("outidx", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.mcu.msp430.runtime.SysBuf.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.mcu.msp430.runtime.SysBuf.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.mcu.msp430.runtime.SysBuf.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SysUart$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.mcu.msp430.runtime.SysUart.Module_State", "ti.mcu.msp430.runtime");
        sizes.clear();
        sizes.add(Global.newArray("lineIdx", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.mcu.msp430.runtime.SysUart.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.mcu.msp430.runtime.SysUart.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.mcu.msp430.runtime.SysUart.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SysCallback$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void LoggerUart$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Instance_State", "ti.mcu.msp430.runtime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("enabled", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.mcu.msp430.runtime.LoggerUart.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.mcu.msp430.runtime.LoggerUart.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.mcu.msp430.runtime.LoggerUart.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerCallback$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Instance_State", "ti.mcu.msp430.runtime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        sizes.add(Global.newArray("enabled", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.mcu.msp430.runtime.LoggerCallback.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.mcu.msp430.runtime.LoggerCallback.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.mcu.msp430.runtime.LoggerCallback.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void GateGIE$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.mcu.msp430.runtime.GateGIE.Instance_State", "ti.mcu.msp430.runtime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.mcu.msp430.runtime.GateGIE.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.mcu.msp430.runtime.GateGIE.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.mcu.msp430.runtime.GateGIE.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void HeapNull$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.mcu.msp430.runtime.HeapNull.Instance_State", "ti.mcu.msp430.runtime");
        sizes.clear();
        sizes.add(Global.newArray("__fxns", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.mcu.msp430.runtime.HeapNull.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.mcu.msp430.runtime.HeapNull.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.mcu.msp430.runtime.HeapNull.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LoggerUart_TimestampProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void LoggerCallback_TimestampProxy$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void SysBuf$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/mcu/msp430/runtime/SysBuf.xs");
        om.bind("ti.mcu.msp430.runtime.SysBuf$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysBuf.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.SysBuf.Module", om.findStrict("xdc.runtime.ISystemSupport.Module", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("bufSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "w");
            po.addFld("bufName", $$T_Str, "SYSBUF", "wh");
            po.addFld("sectionName", $$T_Str, null, "wh");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.SysBuf$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.SysBuf$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.SysBuf$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.SysBuf$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct SysBuf.Module_State
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysBuf$$Module_State", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.SysBuf.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("outbuf", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("outidx", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void SysUart$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/mcu/msp430/runtime/SysUart.xs");
        om.bind("ti.mcu.msp430.runtime.SysUart$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysUart.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.SysUart.Module", om.findStrict("xdc.runtime.ISystemSupport.Module", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("getLineFxn", new Proto.Adr("xdc_Void(*)(xdc_Char[],xdc_Int)", "PFv"), null, "w");
            po.addFld("lineSize", Proto.Elm.newCNum("(xdc_Int)"), 0L, "w");
            po.addFld("lineBuf", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.SysUart$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.SysUart$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.SysUart$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.SysUart$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // typedef SysUart.GetLineFxn
        om.bind("ti.mcu.msp430.runtime.SysUart.GetLineFxn", new Proto.Adr("xdc_Void(*)(xdc_Char[],xdc_Int)", "PFv"));
        // struct SysUart.Module_State
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysUart$$Module_State", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.SysUart.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("lineIdx", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
    }

    void SysCallback$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysCallback.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.SysCallback.Module", om.findStrict("xdc.runtime.ISystemSupport.Module", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("putCharFxn", new Proto.Adr("xdc_Void(*)(xdc_Char)", "PFv"), null, "w");
            po.addFld("atExitFxn", new Proto.Adr("xdc_Int(*)(xdc_Void(*)(xdc_Void))", "PFn"), null, "w");
            po.addFld("abortFxn", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), null, "w");
            po.addFld("initFxn", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"), null, "w");
            po.addFld("exitFxn", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"), null, "w");
        }//isCFG
        // typedef SysCallback.PutCharFxn
        om.bind("ti.mcu.msp430.runtime.SysCallback.PutCharFxn", new Proto.Adr("xdc_Void(*)(xdc_Char)", "PFv"));
        // typedef SysCallback.AtExitFxn
        om.bind("ti.mcu.msp430.runtime.SysCallback.AtExitFxn", new Proto.Adr("xdc_Int(*)(xdc_Void(*)(xdc_Void))", "PFn"));
        // typedef SysCallback.AbortFxn
        om.bind("ti.mcu.msp430.runtime.SysCallback.AbortFxn", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
        // typedef SysCallback.InitFxn
        om.bind("ti.mcu.msp430.runtime.SysCallback.InitFxn", new Proto.Adr("xdc_Void(*)(xdc_Void)", "PFv"));
        // typedef SysCallback.ExitFxn
        om.bind("ti.mcu.msp430.runtime.SysCallback.ExitFxn", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"));
    }

    void LoggerUart$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/mcu/msp430/runtime/LoggerUart.xs");
        om.bind("ti.mcu.msp430.runtime.LoggerUart$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerUart.Module", om.findStrict("xdc.runtime.ILogger.Module", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFldV("TimestampProxy", (Proto)om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.mcu.msp430.runtime"), null, "wh", $$delegGet, $$delegSet);
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$create", "ti.mcu.msp430.runtime"), Global.get("ti$mcu$msp430$runtime$LoggerUart$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$construct", "ti.mcu.msp430.runtime"), Global.get("ti$mcu$msp430$runtime$LoggerUart$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerUart$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerUart$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerUart$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerUart$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerUart$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerUart$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.mcu.msp430.runtime"), fxn);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Instance", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerUart.Instance", om.findStrict("xdc.runtime.ILogger.Instance", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.mcu.msp430.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$Params", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerUart.Params", om.findStrict("xdc.runtime.ILogger$$Params", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.mcu.msp430.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$Object", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerUart.Object", om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Instance", "ti.mcu.msp430.runtime"));
        // struct LoggerUart.Instance_State
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$Instance_State", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerUart.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
    }

    void LoggerCallback$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/mcu/msp430/runtime/LoggerCallback.xs");
        om.bind("ti.mcu.msp430.runtime.LoggerCallback$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerCallback.Module", om.findStrict("xdc.runtime.ILogger.Module", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFldV("TimestampProxy", (Proto)om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.mcu.msp430.runtime"), null, "wh", $$delegGet, $$delegSet);
            po.addFld("callbackFxn", new Proto.Adr("xdc_Void(*)(xdc_runtime_Log_EventRec*)", "PFv"), null, "w");
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$create", "ti.mcu.msp430.runtime"), Global.get("ti$mcu$msp430$runtime$LoggerCallback$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$construct", "ti.mcu.msp430.runtime"), Global.get("ti$mcu$msp430$runtime$LoggerCallback$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerCallback$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerCallback$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerCallback$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerCallback$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerCallback$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.LoggerCallback$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "getMetaArgs");
                if (fxn != null) po.addFxn("getMetaArgs", (Proto.Fxn)om.findStrict("xdc.runtime.ILogger$$getMetaArgs", "ti.mcu.msp430.runtime"), fxn);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Instance", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerCallback.Instance", om.findStrict("xdc.runtime.ILogger.Instance", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.mcu.msp430.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$Params", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerCallback.Params", om.findStrict("xdc.runtime.ILogger$$Params", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.mcu.msp430.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$Object", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerCallback.Object", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Instance", "ti.mcu.msp430.runtime"));
        // typedef LoggerCallback.CallbackFxn
        om.bind("ti.mcu.msp430.runtime.LoggerCallback.CallbackFxn", new Proto.Adr("xdc_Void(*)(xdc_runtime_Log_EventRec*)", "PFv"));
        // struct LoggerCallback.Instance_State
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$Instance_State", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerCallback.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("enabled", $$T_Bool, $$UNDEF, "w");
    }

    void GateGIE$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/mcu/msp430/runtime/GateGIE.xs");
        om.bind("ti.mcu.msp430.runtime.GateGIE$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.GateGIE.Module", om.findStrict("xdc.runtime.IGateProvider.Module", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.mcu.msp430.runtime.GateGIE$$create", "ti.mcu.msp430.runtime"), Global.get("ti$mcu$msp430$runtime$GateGIE$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.mcu.msp430.runtime.GateGIE$$construct", "ti.mcu.msp430.runtime"), Global.get("ti$mcu$msp430$runtime$GateGIE$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.GateGIE$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.GateGIE$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.GateGIE$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.GateGIE$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.GateGIE$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.GateGIE$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "queryMeta");
                if (fxn != null) po.addFxn("queryMeta", (Proto.Fxn)om.findStrict("xdc.runtime.IGateProvider$$queryMeta", "ti.mcu.msp430.runtime"), fxn);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE.Instance", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.GateGIE.Instance", om.findStrict("xdc.runtime.IGateProvider.Instance", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.mcu.msp430.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE$$Params", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.GateGIE.Params", om.findStrict("xdc.runtime.IGateProvider$$Params", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("Q_BLOCKING", Proto.Elm.newCNum("(xdc_Int)"), 1L, "rh");
                po.addFld("Q_PREEMPTING", Proto.Elm.newCNum("(xdc_Int)"), 2L, "rh");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.mcu.msp430.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE$$Object", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.GateGIE.Object", om.findStrict("ti.mcu.msp430.runtime.GateGIE.Instance", "ti.mcu.msp430.runtime"));
        // struct GateGIE.Instance_State
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE$$Instance_State", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.GateGIE.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void HeapNull$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/mcu/msp430/runtime/HeapNull.xs");
        om.bind("ti.mcu.msp430.runtime.HeapNull$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.HeapNull.Module", om.findStrict("xdc.runtime.IHeap.Module", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.mcu.msp430.runtime.HeapNull$$create", "ti.mcu.msp430.runtime"), Global.get("ti$mcu$msp430$runtime$HeapNull$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.mcu.msp430.runtime.HeapNull$$construct", "ti.mcu.msp430.runtime"), Global.get("ti$mcu$msp430$runtime$HeapNull$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.HeapNull$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.HeapNull$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.HeapNull$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.HeapNull$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.HeapNull$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.mcu.msp430.runtime.HeapNull$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull.Instance", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.HeapNull.Instance", om.findStrict("xdc.runtime.IHeap.Instance", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.mcu.msp430.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull$$Params", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.HeapNull.Params", om.findStrict("xdc.runtime.IHeap$$Params", "ti.mcu.msp430.runtime"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.mcu.msp430.runtime"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull$$Object", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.HeapNull.Object", om.findStrict("ti.mcu.msp430.runtime.HeapNull.Instance", "ti.mcu.msp430.runtime"));
        // struct HeapNull.Instance_State
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull$$Instance_State", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.HeapNull.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
    }

    void LoggerUart_TimestampProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.Module", om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.mcu.msp430.runtime"));
                po.addFld("delegate$", (Proto)om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.mcu.msp430.runtime"), null, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void LoggerCallback_TimestampProxy$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.Module", "ti.mcu.msp430.runtime");
        po.init("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.Module", om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.mcu.msp430.runtime"));
                po.addFld("delegate$", (Proto)om.findStrict("xdc.runtime.ITimestampClient.Module", "ti.mcu.msp430.runtime"), null, "wh");
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
    }

    void SysBuf$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.SysBuf", "ti.mcu.msp430.runtime");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.mcu.msp430.runtime.SysBuf.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysBuf$$Module_State", "ti.mcu.msp430.runtime");
        po.bind("outbuf$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
    }

    void SysUart$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.SysUart", "ti.mcu.msp430.runtime");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.mcu.msp430.runtime.SysUart.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysUart$$Module_State", "ti.mcu.msp430.runtime");
    }

    void SysCallback$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.SysCallback", "ti.mcu.msp430.runtime");
    }

    void LoggerUart$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$Instance_State", "ti.mcu.msp430.runtime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.mcu.msp430.runtime.LoggerUart.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$Instance_State", "ti.mcu.msp430.runtime");
    }

    void LoggerCallback$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$Instance_State", "ti.mcu.msp430.runtime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.mcu.msp430.runtime.LoggerCallback.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$Instance_State", "ti.mcu.msp430.runtime");
    }

    void GateGIE$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE$$Instance_State", "ti.mcu.msp430.runtime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.mcu.msp430.runtime.GateGIE.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE$$Instance_State", "ti.mcu.msp430.runtime");
    }

    void HeapNull$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull$$Instance_State", "ti.mcu.msp430.runtime");
        po.addFld("__fxns", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.mcu.msp430.runtime.HeapNull.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull$$Instance_State", "ti.mcu.msp430.runtime");
    }

    void LoggerUart_TimestampProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy", "ti.mcu.msp430.runtime");
    }

    void LoggerCallback_TimestampProxy$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy", "ti.mcu.msp430.runtime");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.mcu.msp430.runtime.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.mcu.msp430.runtime"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/mcu/msp430/runtime/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.mcu.msp430.runtime"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.mcu.msp430.runtime"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.mcu.msp430.runtime"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.mcu.msp430.runtime"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.mcu.msp430.runtime"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.mcu.msp430.runtime"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.mcu.msp430.runtime", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.mcu.msp430.runtime");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.mcu.msp430.runtime.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.mcu.msp430.runtime'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/ti.mcu.msp430.runtime.a430',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.a430X',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.a430XS',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.ae430',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.ae430X',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.ae430XS',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.ar430',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.ar430XL',\n");
            sb.append("'lib/ti.mcu.msp430.runtime.ar430XS',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/ti.mcu.msp430.runtime.a430', {target: 'ti.targets.msp430.MSP430', suffix: '430'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.a430X', {target: 'ti.targets.msp430.MSP430X', suffix: '430X'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.a430XS', {target: 'ti.targets.msp430.MSP430X_small', suffix: '430XS'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.ae430', {target: 'ti.targets.msp430.elf.MSP430', suffix: 'e430'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.ae430X', {target: 'ti.targets.msp430.elf.MSP430X', suffix: 'e430X'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.ae430XS', {target: 'ti.targets.msp430.elf.MSP430X_small', suffix: 'e430XS'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.ar430', {target: 'iar.targets.msp430.MSP430', suffix: 'r430'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.ar430XL', {target: 'iar.targets.msp430.MSP430X_large', suffix: 'r430XL'}],\n");
            sb.append("['lib/ti.mcu.msp430.runtime.ar430XS', {target: 'iar.targets.msp430.MSP430X_small', suffix: 'r430XS'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void SysBuf$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.SysBuf", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysBuf.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.SysBuf", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.mcu.msp430.runtime.SysBuf$$capsule", "ti.mcu.msp430.runtime"));
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
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
        mcfgs.add("bufSize");
        vo.bind("Module_State", om.findStrict("ti.mcu.msp430.runtime.SysBuf.Module_State", "ti.mcu.msp430.runtime"));
        tdefs.add(om.findStrict("ti.mcu.msp430.runtime.SysBuf.Module_State", "ti.mcu.msp430.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
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
        vo.bind("abort", om.findStrict("ti.mcu.msp430.runtime.SysBuf.abort", "ti.mcu.msp430.runtime"));
        vo.bind("exit", om.findStrict("ti.mcu.msp430.runtime.SysBuf.exit", "ti.mcu.msp430.runtime"));
        vo.bind("flush", om.findStrict("ti.mcu.msp430.runtime.SysBuf.flush", "ti.mcu.msp430.runtime"));
        vo.bind("putch", om.findStrict("ti.mcu.msp430.runtime.SysBuf.putch", "ti.mcu.msp430.runtime"));
        vo.bind("ready", om.findStrict("ti.mcu.msp430.runtime.SysBuf.ready", "ti.mcu.msp430.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_SysBuf_Module__startupDone__E", "ti_mcu_msp430_runtime_SysBuf_abort__E", "ti_mcu_msp430_runtime_SysBuf_exit__E", "ti_mcu_msp430_runtime_SysBuf_flush__E", "ti_mcu_msp430_runtime_SysBuf_putch__E", "ti_mcu_msp430_runtime_SysBuf_ready__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "abort", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "exit", "entry", "%d", "exit", ""));
        loggables.add(Global.newObject("name", "flush", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "putch", "entry", "%d", "exit", ""));
        loggables.add(Global.newObject("name", "ready", "entry", "", "exit", "%d"));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SysBuf", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SysBuf");
    }

    void SysUart$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.SysUart", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysUart.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.SysUart", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.mcu.msp430.runtime.SysUart$$capsule", "ti.mcu.msp430.runtime"));
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
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
        vo.bind("GetLineFxn", om.findStrict("ti.mcu.msp430.runtime.SysUart.GetLineFxn", "ti.mcu.msp430.runtime"));
        mcfgs.add("getLineFxn");
        mcfgs.add("lineSize");
        mcfgs.add("lineBuf");
        icfgs.add("lineBuf");
        vo.bind("Module_State", om.findStrict("ti.mcu.msp430.runtime.SysUart.Module_State", "ti.mcu.msp430.runtime"));
        tdefs.add(om.findStrict("ti.mcu.msp430.runtime.SysUart.Module_State", "ti.mcu.msp430.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
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
        vo.bind("abort", om.findStrict("ti.mcu.msp430.runtime.SysUart.abort", "ti.mcu.msp430.runtime"));
        vo.bind("exit", om.findStrict("ti.mcu.msp430.runtime.SysUart.exit", "ti.mcu.msp430.runtime"));
        vo.bind("flush", om.findStrict("ti.mcu.msp430.runtime.SysUart.flush", "ti.mcu.msp430.runtime"));
        vo.bind("putch", om.findStrict("ti.mcu.msp430.runtime.SysUart.putch", "ti.mcu.msp430.runtime"));
        vo.bind("ready", om.findStrict("ti.mcu.msp430.runtime.SysUart.ready", "ti.mcu.msp430.runtime"));
        vo.bind("rxtxInit", om.findStrict("ti.mcu.msp430.runtime.SysUart.rxtxInit", "ti.mcu.msp430.runtime"));
        vo.bind("tx", om.findStrict("ti.mcu.msp430.runtime.SysUart.tx", "ti.mcu.msp430.runtime"));
        vo.bind("rx", om.findStrict("ti.mcu.msp430.runtime.SysUart.rx", "ti.mcu.msp430.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_SysUart_Module__startupDone__E", "ti_mcu_msp430_runtime_SysUart_abort__E", "ti_mcu_msp430_runtime_SysUart_exit__E", "ti_mcu_msp430_runtime_SysUart_flush__E", "ti_mcu_msp430_runtime_SysUart_putch__E", "ti_mcu_msp430_runtime_SysUart_ready__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./SysUart.xdt");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "abort", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "exit", "entry", "%d", "exit", ""));
        loggables.add(Global.newObject("name", "flush", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "putch", "entry", "%d", "exit", ""));
        loggables.add(Global.newObject("name", "ready", "entry", "", "exit", "%d"));
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./SysUart.xdt");
        pkgV.bind("SysUart", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SysUart");
    }

    void SysCallback$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.SysCallback", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.SysCallback.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.SysCallback", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
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
        vo.bind("PutCharFxn", om.findStrict("ti.mcu.msp430.runtime.SysCallback.PutCharFxn", "ti.mcu.msp430.runtime"));
        vo.bind("AtExitFxn", om.findStrict("ti.mcu.msp430.runtime.SysCallback.AtExitFxn", "ti.mcu.msp430.runtime"));
        vo.bind("AbortFxn", om.findStrict("ti.mcu.msp430.runtime.SysCallback.AbortFxn", "ti.mcu.msp430.runtime"));
        vo.bind("InitFxn", om.findStrict("ti.mcu.msp430.runtime.SysCallback.InitFxn", "ti.mcu.msp430.runtime"));
        vo.bind("ExitFxn", om.findStrict("ti.mcu.msp430.runtime.SysCallback.ExitFxn", "ti.mcu.msp430.runtime"));
        mcfgs.add("putCharFxn");
        mcfgs.add("atExitFxn");
        mcfgs.add("abortFxn");
        mcfgs.add("initFxn");
        mcfgs.add("exitFxn");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
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
        vo.bind("abort", om.findStrict("ti.mcu.msp430.runtime.SysCallback.abort", "ti.mcu.msp430.runtime"));
        vo.bind("exit", om.findStrict("ti.mcu.msp430.runtime.SysCallback.exit", "ti.mcu.msp430.runtime"));
        vo.bind("flush", om.findStrict("ti.mcu.msp430.runtime.SysCallback.flush", "ti.mcu.msp430.runtime"));
        vo.bind("putch", om.findStrict("ti.mcu.msp430.runtime.SysCallback.putch", "ti.mcu.msp430.runtime"));
        vo.bind("ready", om.findStrict("ti.mcu.msp430.runtime.SysCallback.ready", "ti.mcu.msp430.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_SysCallback_Module__startupDone__E", "ti_mcu_msp430_runtime_SysCallback_abort__E", "ti_mcu_msp430_runtime_SysCallback_exit__E", "ti_mcu_msp430_runtime_SysCallback_flush__E", "ti_mcu_msp430_runtime_SysCallback_putch__E", "ti_mcu_msp430_runtime_SysCallback_ready__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./SysCallback.xdt");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "abort", "entry", "0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "exit", "entry", "%d", "exit", ""));
        loggables.add(Global.newObject("name", "flush", "entry", "", "exit", ""));
        loggables.add(Global.newObject("name", "putch", "entry", "%d", "exit", ""));
        loggables.add(Global.newObject("name", "ready", "entry", "", "exit", "%d"));
        vo.bind("$$loggables", loggables.toArray());
        vo.bind("TEMPLATE$", "./SysCallback.xdt");
        pkgV.bind("SysCallback", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SysCallback");
    }

    void LoggerUart$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.LoggerUart", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.mcu.msp430.runtime.LoggerUart$$capsule", "ti.mcu.msp430.runtime"));
        vo.bind("Instance", om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Instance", "ti.mcu.msp430.runtime"));
        vo.bind("Params", om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Params", "ti.mcu.msp430.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Params", "ti.mcu.msp430.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Handle", "ti.mcu.msp430.runtime"));
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
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
        vo.bind("TimestampProxy$proxy", om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy", "ti.mcu.msp430.runtime"));
        proxies.add("TimestampProxy");
        vo.bind("Instance_State", om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Instance_State", "ti.mcu.msp430.runtime"));
        tdefs.add(om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Instance_State", "ti.mcu.msp430.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.mcu.msp430.runtime.LoggerUart$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$mcu$msp430$runtime$LoggerUart$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_LoggerUart_Handle__label__E", "ti_mcu_msp430_runtime_LoggerUart_Module__startupDone__E", "ti_mcu_msp430_runtime_LoggerUart_Object__create__E", "ti_mcu_msp430_runtime_LoggerUart_Object__delete__E", "ti_mcu_msp430_runtime_LoggerUart_Object__destruct__E", "ti_mcu_msp430_runtime_LoggerUart_Object__get__E", "ti_mcu_msp430_runtime_LoggerUart_Object__first__E", "ti_mcu_msp430_runtime_LoggerUart_Object__next__E", "ti_mcu_msp430_runtime_LoggerUart_Params__init__E", "ti_mcu_msp430_runtime_LoggerUart_enable__E", "ti_mcu_msp430_runtime_LoggerUart_disable__E", "ti_mcu_msp430_runtime_LoggerUart_write0__E", "ti_mcu_msp430_runtime_LoggerUart_write1__E", "ti_mcu_msp430_runtime_LoggerUart_write2__E", "ti_mcu_msp430_runtime_LoggerUart_write4__E", "ti_mcu_msp430_runtime_LoggerUart_write8__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.mcu.msp430.runtime.LoggerUart.Object", "ti.mcu.msp430.runtime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "enable", "entry", "%p", "exit", "%d"));
        loggables.add(Global.newObject("name", "disable", "entry", "%p", "exit", "%d"));
        loggables.add(Global.newObject("name", "write0", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "write1", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "write2", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "write4", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "write8", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LoggerUart", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerUart");
    }

    void LoggerCallback$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.LoggerCallback", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback$$capsule", "ti.mcu.msp430.runtime"));
        vo.bind("Instance", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Instance", "ti.mcu.msp430.runtime"));
        vo.bind("Params", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Params", "ti.mcu.msp430.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Params", "ti.mcu.msp430.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Handle", "ti.mcu.msp430.runtime"));
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
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
        vo.bind("TimestampProxy$proxy", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy", "ti.mcu.msp430.runtime"));
        proxies.add("TimestampProxy");
        vo.bind("CallbackFxn", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.CallbackFxn", "ti.mcu.msp430.runtime"));
        mcfgs.add("callbackFxn");
        vo.bind("Instance_State", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Instance_State", "ti.mcu.msp430.runtime"));
        tdefs.add(om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Instance_State", "ti.mcu.msp430.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.mcu.msp430.runtime.LoggerCallback$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$mcu$msp430$runtime$LoggerCallback$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_LoggerCallback_Handle__label__E", "ti_mcu_msp430_runtime_LoggerCallback_Module__startupDone__E", "ti_mcu_msp430_runtime_LoggerCallback_Object__create__E", "ti_mcu_msp430_runtime_LoggerCallback_Object__delete__E", "ti_mcu_msp430_runtime_LoggerCallback_Object__destruct__E", "ti_mcu_msp430_runtime_LoggerCallback_Object__get__E", "ti_mcu_msp430_runtime_LoggerCallback_Object__first__E", "ti_mcu_msp430_runtime_LoggerCallback_Object__next__E", "ti_mcu_msp430_runtime_LoggerCallback_Params__init__E", "ti_mcu_msp430_runtime_LoggerCallback_enable__E", "ti_mcu_msp430_runtime_LoggerCallback_disable__E", "ti_mcu_msp430_runtime_LoggerCallback_write0__E", "ti_mcu_msp430_runtime_LoggerCallback_write1__E", "ti_mcu_msp430_runtime_LoggerCallback_write2__E", "ti_mcu_msp430_runtime_LoggerCallback_write4__E", "ti_mcu_msp430_runtime_LoggerCallback_write8__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback.Object", "ti.mcu.msp430.runtime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "enable", "entry", "%p", "exit", "%d"));
        loggables.add(Global.newObject("name", "disable", "entry", "%p", "exit", "%d"));
        loggables.add(Global.newObject("name", "write0", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "write1", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "write2", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "write4", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "write8", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LoggerCallback", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerCallback");
    }

    void GateGIE$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.GateGIE.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.GateGIE", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.mcu.msp430.runtime.GateGIE$$capsule", "ti.mcu.msp430.runtime"));
        vo.bind("Instance", om.findStrict("ti.mcu.msp430.runtime.GateGIE.Instance", "ti.mcu.msp430.runtime"));
        vo.bind("Params", om.findStrict("ti.mcu.msp430.runtime.GateGIE.Params", "ti.mcu.msp430.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.mcu.msp430.runtime.GateGIE.Params", "ti.mcu.msp430.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.mcu.msp430.runtime.GateGIE.Handle", "ti.mcu.msp430.runtime"));
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
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
        vo.bind("Instance_State", om.findStrict("ti.mcu.msp430.runtime.GateGIE.Instance_State", "ti.mcu.msp430.runtime"));
        tdefs.add(om.findStrict("ti.mcu.msp430.runtime.GateGIE.Instance_State", "ti.mcu.msp430.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.mcu.msp430.runtime.GateGIE$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$mcu$msp430$runtime$GateGIE$$__initObject"));
        }//isCFG
        vo.bind("query", om.findStrict("ti.mcu.msp430.runtime.GateGIE.query", "ti.mcu.msp430.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_GateGIE_Handle__label__E", "ti_mcu_msp430_runtime_GateGIE_Module__startupDone__E", "ti_mcu_msp430_runtime_GateGIE_Object__create__E", "ti_mcu_msp430_runtime_GateGIE_Object__delete__E", "ti_mcu_msp430_runtime_GateGIE_Object__destruct__E", "ti_mcu_msp430_runtime_GateGIE_Object__get__E", "ti_mcu_msp430_runtime_GateGIE_Object__first__E", "ti_mcu_msp430_runtime_GateGIE_Object__next__E", "ti_mcu_msp430_runtime_GateGIE_Params__init__E", "ti_mcu_msp430_runtime_GateGIE_query__E", "ti_mcu_msp430_runtime_GateGIE_enter__E", "ti_mcu_msp430_runtime_GateGIE_leave__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.mcu.msp430.runtime.GateGIE.Object", "ti.mcu.msp430.runtime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "query", "entry", "%d", "exit", "%d"));
        loggables.add(Global.newObject("name", "enter", "entry", "%p", "exit", "%p"));
        loggables.add(Global.newObject("name", "leave", "entry", "%p, %p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("GateGIE", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GateGIE");
    }

    void HeapNull$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.HeapNull.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.HeapNull", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.mcu.msp430.runtime.HeapNull$$capsule", "ti.mcu.msp430.runtime"));
        vo.bind("Instance", om.findStrict("ti.mcu.msp430.runtime.HeapNull.Instance", "ti.mcu.msp430.runtime"));
        vo.bind("Params", om.findStrict("ti.mcu.msp430.runtime.HeapNull.Params", "ti.mcu.msp430.runtime"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.mcu.msp430.runtime.HeapNull.Params", "ti.mcu.msp430.runtime")).newInstance());
        vo.bind("Handle", om.findStrict("ti.mcu.msp430.runtime.HeapNull.Handle", "ti.mcu.msp430.runtime"));
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
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
        vo.bind("Instance_State", om.findStrict("ti.mcu.msp430.runtime.HeapNull.Instance_State", "ti.mcu.msp430.runtime"));
        tdefs.add(om.findStrict("ti.mcu.msp430.runtime.HeapNull.Instance_State", "ti.mcu.msp430.runtime"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
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
            vo.bind("$$meta_iobj", om.has("ti.mcu.msp430.runtime.HeapNull$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$mcu$msp430$runtime$HeapNull$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_HeapNull_Handle__label__E", "ti_mcu_msp430_runtime_HeapNull_Module__startupDone__E", "ti_mcu_msp430_runtime_HeapNull_Object__create__E", "ti_mcu_msp430_runtime_HeapNull_Object__delete__E", "ti_mcu_msp430_runtime_HeapNull_Object__destruct__E", "ti_mcu_msp430_runtime_HeapNull_Object__get__E", "ti_mcu_msp430_runtime_HeapNull_Object__first__E", "ti_mcu_msp430_runtime_HeapNull_Object__next__E", "ti_mcu_msp430_runtime_HeapNull_Params__init__E", "ti_mcu_msp430_runtime_HeapNull_getStats__E", "ti_mcu_msp430_runtime_HeapNull_alloc__E", "ti_mcu_msp430_runtime_HeapNull_free__E", "ti_mcu_msp430_runtime_HeapNull_isBlocking__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.mcu.msp430.runtime.HeapNull.Object", "ti.mcu.msp430.runtime"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "getStats", "entry", "%p, %p", "exit", ""));
        loggables.add(Global.newObject("name", "alloc", "entry", "%p, 0x%x, 0x%x, %p", "exit", "%p"));
        loggables.add(Global.newObject("name", "free", "entry", "%p, %p, 0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "isBlocking", "entry", "%p", "exit", "%d"));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("HeapNull", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HeapNull");
    }

    void LoggerUart_TimestampProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.LoggerUart_TimestampProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 0);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("get32", om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.get32", "ti.mcu.msp430.runtime"));
        vo.bind("get64", om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.get64", "ti.mcu.msp430.runtime"));
        vo.bind("getFreq", om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy.getFreq", "ti.mcu.msp430.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Handle__label", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Module__startupDone", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Object__create", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Object__delete", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Object__destruct", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Object__get", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Object__first", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Object__next", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Params__init", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Proxy__abstract", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__Proxy__delegate", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__get32", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__get64", "ti_mcu_msp430_runtime_LoggerUart_TimestampProxy_DELEGATE__getFreq"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 1);
        loggables.clear();
        loggables.add(Global.newObject("name", "get32", "entry", "", "exit", "0x%x"));
        loggables.add(Global.newObject("name", "get64", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "getFreq", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LoggerUart_TimestampProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerUart_TimestampProxy");
    }

    void LoggerCallback_TimestampProxy$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy", "ti.mcu.msp430.runtime");
        po = (Proto.Obj)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.Module", "ti.mcu.msp430.runtime");
        vo.init2(po, "ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.mcu.msp430.runtime", "ti.mcu.msp430.runtime"));
        tdefs.clear();
        proxies.clear();
        proxies.add("delegate$");
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.runtime");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.mcu.msp430.runtime")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 0);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("get32", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.get32", "ti.mcu.msp430.runtime"));
        vo.bind("get64", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.get64", "ti.mcu.msp430.runtime"));
        vo.bind("getFreq", om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy.getFreq", "ti.mcu.msp430.runtime"));
        vo.bind("$$fxntab", Global.newArray("ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Handle__label", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Module__startupDone", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Object__create", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Object__delete", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Object__destruct", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Object__get", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Object__first", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Object__next", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Params__init", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Proxy__abstract", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__Proxy__delegate", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__get32", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__get64", "ti_mcu_msp430_runtime_LoggerCallback_TimestampProxy_DELEGATE__getFreq"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 1);
        loggables.clear();
        loggables.add(Global.newObject("name", "get32", "entry", "", "exit", "0x%x"));
        loggables.add(Global.newObject("name", "get64", "entry", "%p", "exit", ""));
        loggables.add(Global.newObject("name", "getFreq", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LoggerCallback_TimestampProxy", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LoggerCallback_TimestampProxy");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.mcu.msp430.runtime")).findStrict("PARAMS", "ti.mcu.msp430.runtime");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.mcu.msp430.runtime.LoggerUart", "ti.mcu.msp430.runtime")).findStrict("PARAMS", "ti.mcu.msp430.runtime");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback", "ti.mcu.msp430.runtime")).findStrict("PARAMS", "ti.mcu.msp430.runtime");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.mcu.msp430.runtime.GateGIE", "ti.mcu.msp430.runtime")).findStrict("PARAMS", "ti.mcu.msp430.runtime");
            Global.put(dstP, "instance", srcP);
            dstP = (Scriptable)((XScriptO)om.findStrict("ti.mcu.msp430.runtime.HeapNull", "ti.mcu.msp430.runtime")).findStrict("PARAMS", "ti.mcu.msp430.runtime");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.SysBuf", "ti.mcu.msp430.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.SysUart", "ti.mcu.msp430.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.SysCallback", "ti.mcu.msp430.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.LoggerUart", "ti.mcu.msp430.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback", "ti.mcu.msp430.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.GateGIE", "ti.mcu.msp430.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.HeapNull", "ti.mcu.msp430.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy", "ti.mcu.msp430.runtime"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy", "ti.mcu.msp430.runtime"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.SysBuf")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.SysUart")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.SysCallback")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.LoggerUart")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.LoggerCallback")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.GateGIE")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.HeapNull")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.LoggerUart_TimestampProxy")).bless();
        ((Value.Obj)om.getv("ti.mcu.msp430.runtime.LoggerCallback_TimestampProxy")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.mcu.msp430.runtime")).add(pkgV);
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
        SysBuf$$OBJECTS();
        SysUart$$OBJECTS();
        SysCallback$$OBJECTS();
        LoggerUart$$OBJECTS();
        LoggerCallback$$OBJECTS();
        GateGIE$$OBJECTS();
        HeapNull$$OBJECTS();
        LoggerUart_TimestampProxy$$OBJECTS();
        LoggerCallback_TimestampProxy$$OBJECTS();
        SysBuf$$CONSTS();
        SysUart$$CONSTS();
        SysCallback$$CONSTS();
        LoggerUart$$CONSTS();
        LoggerCallback$$CONSTS();
        GateGIE$$CONSTS();
        HeapNull$$CONSTS();
        LoggerUart_TimestampProxy$$CONSTS();
        LoggerCallback_TimestampProxy$$CONSTS();
        SysBuf$$CREATES();
        SysUart$$CREATES();
        SysCallback$$CREATES();
        LoggerUart$$CREATES();
        LoggerCallback$$CREATES();
        GateGIE$$CREATES();
        HeapNull$$CREATES();
        LoggerUart_TimestampProxy$$CREATES();
        LoggerCallback_TimestampProxy$$CREATES();
        SysBuf$$FUNCTIONS();
        SysUart$$FUNCTIONS();
        SysCallback$$FUNCTIONS();
        LoggerUart$$FUNCTIONS();
        LoggerCallback$$FUNCTIONS();
        GateGIE$$FUNCTIONS();
        HeapNull$$FUNCTIONS();
        LoggerUart_TimestampProxy$$FUNCTIONS();
        LoggerCallback_TimestampProxy$$FUNCTIONS();
        SysBuf$$SIZES();
        SysUart$$SIZES();
        SysCallback$$SIZES();
        LoggerUart$$SIZES();
        LoggerCallback$$SIZES();
        GateGIE$$SIZES();
        HeapNull$$SIZES();
        LoggerUart_TimestampProxy$$SIZES();
        LoggerCallback_TimestampProxy$$SIZES();
        SysBuf$$TYPES();
        SysUart$$TYPES();
        SysCallback$$TYPES();
        LoggerUart$$TYPES();
        LoggerCallback$$TYPES();
        GateGIE$$TYPES();
        HeapNull$$TYPES();
        LoggerUart_TimestampProxy$$TYPES();
        LoggerCallback_TimestampProxy$$TYPES();
        if (isROV) {
            SysBuf$$ROV();
            SysUart$$ROV();
            SysCallback$$ROV();
            LoggerUart$$ROV();
            LoggerCallback$$ROV();
            GateGIE$$ROV();
            HeapNull$$ROV();
            LoggerUart_TimestampProxy$$ROV();
            LoggerCallback_TimestampProxy$$ROV();
        }//isROV
        $$SINGLETONS();
        SysBuf$$SINGLETONS();
        SysUart$$SINGLETONS();
        SysCallback$$SINGLETONS();
        LoggerUart$$SINGLETONS();
        LoggerCallback$$SINGLETONS();
        GateGIE$$SINGLETONS();
        HeapNull$$SINGLETONS();
        LoggerUart_TimestampProxy$$SINGLETONS();
        LoggerCallback_TimestampProxy$$SINGLETONS();
        $$INITIALIZATION();
    }
}
