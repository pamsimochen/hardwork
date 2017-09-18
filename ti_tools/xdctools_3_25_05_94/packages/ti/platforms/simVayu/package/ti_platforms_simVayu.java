/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-z57
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class ti_platforms_simVayu
{
    static final String VERS = "@(#) xdc-z57\n";

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
        Global.callFxn("loadPackage", xdcO, "ti.catalog.arp32");
        Global.callFxn("loadPackage", xdcO, "ti.catalog.c6000");
        Global.callFxn("loadPackage", xdcO, "ti.catalog.arm.cortexm4");
        Global.callFxn("loadPackage", xdcO, "ti.catalog.arm.cortexa15");
        Global.callFxn("loadPackage", xdcO, "xdc.platform");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.platforms.simVayu.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.platforms.simVayu", new Value.Obj("ti.platforms.simVayu", pkgP));
    }

    void Platform$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.platforms.simVayu.Platform.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.platforms.simVayu.Platform", new Value.Obj("ti.platforms.simVayu.Platform", po));
        pkgV.bind("Platform", vo);
        // decls 
        om.bind("ti.platforms.simVayu.Platform.Board", om.findStrict("xdc.platform.IPlatform.Board", "ti.platforms.simVayu"));
        om.bind("ti.platforms.simVayu.Platform.Memory", om.findStrict("xdc.platform.IPlatform.Memory", "ti.platforms.simVayu"));
        // insts 
        Object insP = om.bind("ti.platforms.simVayu.Platform.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.platforms.simVayu.Platform$$Object", new Proto.Obj());
        Object objP = om.bind("ti.platforms.simVayu.Platform.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.platforms.simVayu.Platform$$Params", new Proto.Obj());
        om.bind("ti.platforms.simVayu.Platform.Params", new Proto.Str(po, true));
    }

    void Platform$$CONSTS()
    {
        // module Platform
    }

    void Platform$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.platforms.simVayu.Platform$$create", new Proto.Fxn(om.findStrict("ti.platforms.simVayu.Platform.Module", "ti.platforms.simVayu"), om.findStrict("ti.platforms.simVayu.Platform.Instance", "ti.platforms.simVayu"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.platforms.simVayu.Platform.Params", "ti.platforms.simVayu"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$platforms$simVayu$Platform$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.platforms.simVayu.Platform'];\n");
            sb.append("var __inst = xdc.om['ti.platforms.simVayu.Platform.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.platforms.simVayu']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.externalMemoryMap = __mod.PARAMS.externalMemoryMap;\n");
            sb.append("__inst.customMemoryMap = __mod.PARAMS.customMemoryMap;\n");
            sb.append("__inst.renameMap = __mod.PARAMS.renameMap;\n");
            sb.append("__inst.dataMemory = __mod.PARAMS.dataMemory;\n");
            sb.append("__inst.codeMemory = __mod.PARAMS.codeMemory;\n");
            sb.append("__inst.stackMemory = __mod.PARAMS.stackMemory;\n");
            sb.append("__inst.sectMap = __mod.PARAMS.sectMap;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.platforms.simVayu.Platform$$construct", new Proto.Fxn(om.findStrict("ti.platforms.simVayu.Platform.Module", "ti.platforms.simVayu"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.platforms.simVayu.Platform$$Object", "ti.platforms.simVayu"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.platforms.simVayu.Platform.Params", "ti.platforms.simVayu"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$platforms$simVayu$Platform$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.platforms.simVayu.Platform'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.externalMemoryMap = __mod.PARAMS.externalMemoryMap;\n");
            sb.append("__inst.customMemoryMap = __mod.PARAMS.customMemoryMap;\n");
            sb.append("__inst.renameMap = __mod.PARAMS.renameMap;\n");
            sb.append("__inst.dataMemory = __mod.PARAMS.dataMemory;\n");
            sb.append("__inst.codeMemory = __mod.PARAMS.codeMemory;\n");
            sb.append("__inst.stackMemory = __mod.PARAMS.stackMemory;\n");
            sb.append("__inst.sectMap = __mod.PARAMS.sectMap;\n");
            sb.append("__inst.peripherals = __mod.PARAMS.peripherals;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Platform$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Platform$$SIZES()
    {
    }

    void Platform$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/platforms/simVayu/Platform.xs");
        om.bind("ti.platforms.simVayu.Platform$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.platforms.simVayu.Platform.Module", "ti.platforms.simVayu");
        po.init("ti.platforms.simVayu.Platform.Module", om.findStrict("xdc.platform.IPlatform.Module", "ti.platforms.simVayu"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("BOARD", (Proto)om.findStrict("xdc.platform.IPlatform.Board", "ti.platforms.simVayu"), Global.newObject("id", "0", "boardName", "simVayu", "boardFamily", "simVayu", "boardRevision", null), "rh");
        po.addFld("DSP", (Proto)om.findStrict("xdc.platform.IExeContext.Cpu", "ti.platforms.simVayu"), Global.newObject("id", "0", "clockRate", 1000L, "catalogName", "ti.catalog.c6000", "deviceName", "Vayu", "revision", "1.0"), "rh");
        po.addFld("M4", (Proto)om.findStrict("xdc.platform.IExeContext.Cpu", "ti.platforms.simVayu"), Global.newObject("id", "1", "clockRate", 266.0, "catalogName", "ti.catalog.arm.cortexm4", "deviceName", "Vayu", "revision", "1.0"), "rh");
        po.addFld("EVE", (Proto)om.findStrict("xdc.platform.IExeContext.Cpu", "ti.platforms.simVayu"), Global.newObject("id", "2", "clockRate", 200.0, "catalogName", "ti.catalog.arp32", "deviceName", "Vayu", "revision", "1.0"), "rh");
        po.addFld("GPP", (Proto)om.findStrict("xdc.platform.IExeContext.Cpu", "ti.platforms.simVayu"), Global.newObject("id", "3", "clockRate", 800.0, "catalogName", "ti.catalog.arm.cortexa15", "deviceName", "Vayu", "revision", "1.0"), "rh");
        po.addFld("SR_0", $$T_Obj, Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)"), "rh");
        po.addFld("DSP1", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"DSP1_PROG", Global.newObject("name", "DSP1_PROG", "space", "code/data", "access", "RWX", "base", 0x8C000000L, "len", 0x1000000L, "comment", "DSP1 Program Memory (16 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "DSP1_PROG", "dataMemory", "DSP1_PROG", "stackMemory", "DSP1_PROG", "l1DMode", "32k", "l1PMode", "32k", "l2Mode", "128k"), "rh");
        po.addFld("DSP2", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"DSP2_PROG", Global.newObject("name", "DSP2_PROG", "space", "code/data", "access", "RWX", "base", 0x8D000000L, "len", 0x1000000L, "comment", "DSP2 Program Memory (16 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "DSP2_PROG", "dataMemory", "DSP2_PROG", "stackMemory", "DSP2_PROG", "l1DMode", "32k", "l1PMode", "32k", "l2Mode", "128k"), "rh");
        po.addFld("EVE1", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"EVEVECS", Global.newObject("name", "EVEVECS", "space", "code/data", "access", "RWX", "base", 0x80000000L, "len", 0x100L, "page", 0L, "comment", "EVE1 Vector Table (256 B)")}), Global.newArray(new Object[]{"EVE1_PROG", Global.newObject("name", "EVE1_PROG", "space", "code/data", "access", "RWX", "base", 0x81000000L, "len", 0x400000L, "page", 1L, "comment", "EVE1 Program Memory (4 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "EVE1_PROG", "dataMemory", "EVE1_PROG", "stackMemory", "EVE1_PROG"), "rh");
        po.addFld("EVE2", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"EVEVECS", Global.newObject("name", "EVEVECS", "space", "code/data", "access", "RWX", "base", 0x80010000L, "len", 0x100L, "page", 0L, "comment", "EVE2 Vector Table (256 B)")}), Global.newArray(new Object[]{"EVE2_PROG", Global.newObject("name", "EVE2_PROG", "space", "code/data", "access", "RWX", "base", 0x82000000L, "len", 0x400000L, "page", 1L, "comment", "EVE2 Program Memory (4 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "EVE2_PROG", "dataMemory", "EVE2_PROG", "stackMemory", "EVE2_PROG"), "rh");
        po.addFld("EVE3", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"EVEVECS", Global.newObject("name", "EVEVECS", "space", "code/data", "access", "RWX", "base", 0x80020000L, "len", 0x100L, "page", 0L, "comment", "EVE3 Vector Table (256 B)")}), Global.newArray(new Object[]{"EVE3_PROG", Global.newObject("name", "EVE3_PROG", "space", "code/data", "access", "RWX", "base", 0x83000000L, "len", 0x400000L, "page", 1L, "comment", "EVE3 Program Memory (4 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "EVE3_PROG", "dataMemory", "EVE3_PROG", "stackMemory", "EVE3_PROG"), "rh");
        po.addFld("EVE4", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"EVEVECS", Global.newObject("name", "EVEVECS", "space", "code/data", "access", "RWX", "base", 0x80030000L, "len", 0x100L, "page", 0L, "comment", "EVE4 Vector Table (256 B)")}), Global.newArray(new Object[]{"EVE4_PROG", Global.newObject("name", "EVE4_PROG", "space", "code/data", "access", "RWX", "base", 0x84000000L, "len", 0x400000L, "page", 1L, "comment", "EVE4 Program Memory (4 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "EVE4_PROG", "dataMemory", "EVE4_PROG", "stackMemory", "EVE4_PROG"), "rh");
        po.addFld("IPU1", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"IPU1_PROG", Global.newObject("name", "IPU1_PROG", "space", "code/data", "access", "RWX", "base", 0x8A000000L, "len", 0x800000L, "comment", "IPU1 Program Memory (8 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "IPU1_PROG", "dataMemory", "IPU1_PROG", "stackMemory", "IPU1_PROG"), "rh");
        po.addFld("IPU2", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"IPU2_PROG", Global.newObject("name", "IPU2_PROG", "space", "code/data", "access", "RWX", "base", 0x8A800000L, "len", 0x800000L, "comment", "IPU2 Program Memory (8 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "IPU2_PROG", "dataMemory", "IPU2_PROG", "stackMemory", "IPU2_PROG"), "rh");
        po.addFld("HOST", $$T_Obj, Global.newObject("externalMemoryMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"HOST_PROG", Global.newObject("name", "HOST_PROG", "space", "code/data", "access", "RWX", "base", 0x8B000000L, "len", 0x1000000L, "comment", "HOST Program Memory (16 MB)")}), Global.newArray(new Object[]{"SR_0", Global.newObject("name", "SR_0", "space", "data", "access", "RWX", "base", 0x8E000000L, "len", 0x1000000L, "comment", "SR#0 Memory (16 MB)")})}), "codeMemory", "HOST_PROG", "dataMemory", "HOST_PROG", "stackMemory", "HOST_PROG"), "rh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.platforms.simVayu.Platform$$create", "ti.platforms.simVayu"), Global.get("ti$platforms$simVayu$Platform$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.platforms.simVayu.Platform$$construct", "ti.platforms.simVayu"), Global.get("ti$platforms$simVayu$Platform$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.platforms.simVayu.Platform$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.platforms.simVayu.Platform$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.platforms.simVayu.Platform$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.platforms.simVayu.Platform$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.platforms.simVayu.Platform.Instance", "ti.platforms.simVayu");
        po.init("ti.platforms.simVayu.Platform.Instance", om.findStrict("xdc.platform.IPlatform.Instance", "ti.platforms.simVayu"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("externalMemoryMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.platforms.simVayu")), Global.newArray(new Object[]{Global.newArray(new Object[]{"EXT_RAM", Global.newObject("comment", "1 GB External RAM Memory", "name", "EXT_RAM", "base", 0x80000000L, "len", 0x40000000L)})}), "rh");
        po.addFld("l1PMode", $$T_Str, "32k", "wh");
        po.addFld("l1DMode", $$T_Str, "32k", "wh");
        po.addFld("l2Mode", $$T_Str, "0k", "wh");
                fxn = Global.get(cap, "getCpuDataSheet");
                if (fxn != null) po.addFxn("getCpuDataSheet", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getCpuDataSheet", "ti.platforms.simVayu"), fxn);
                fxn = Global.get(cap, "getCreateArgs");
                if (fxn != null) po.addFxn("getCreateArgs", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getCreateArgs", "ti.platforms.simVayu"), fxn);
                fxn = Global.get(cap, "getExeContext");
                if (fxn != null) po.addFxn("getExeContext", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getExeContext", "ti.platforms.simVayu"), fxn);
                fxn = Global.get(cap, "getExecCmd");
                if (fxn != null) po.addFxn("getExecCmd", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getExecCmd", "ti.platforms.simVayu"), fxn);
                fxn = Global.get(cap, "getLinkTemplate");
                if (fxn != null) po.addFxn("getLinkTemplate", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getLinkTemplate", "ti.platforms.simVayu"), fxn);
        po = (Proto.Obj)om.findStrict("ti.platforms.simVayu.Platform$$Params", "ti.platforms.simVayu");
        po.init("ti.platforms.simVayu.Platform.Params", om.findStrict("xdc.platform.IPlatform$$Params", "ti.platforms.simVayu"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("externalMemoryMap", new Proto.Map((Proto)om.findStrict("xdc.platform.IPlatform.Memory", "ti.platforms.simVayu")), Global.newArray(new Object[]{Global.newArray(new Object[]{"EXT_RAM", Global.newObject("comment", "1 GB External RAM Memory", "name", "EXT_RAM", "base", 0x80000000L, "len", 0x40000000L)})}), "rh");
        po.addFld("l1PMode", $$T_Str, "32k", "wh");
        po.addFld("l1DMode", $$T_Str, "32k", "wh");
        po.addFld("l2Mode", $$T_Str, "0k", "wh");
        po = (Proto.Obj)om.findStrict("ti.platforms.simVayu.Platform$$Object", "ti.platforms.simVayu");
        po.init("ti.platforms.simVayu.Platform.Object", om.findStrict("ti.platforms.simVayu.Platform.Instance", "ti.platforms.simVayu"));
                fxn = Global.get(cap, "getCpuDataSheet");
                if (fxn != null) po.addFxn("getCpuDataSheet", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getCpuDataSheet", "ti.platforms.simVayu"), fxn);
                fxn = Global.get(cap, "getCreateArgs");
                if (fxn != null) po.addFxn("getCreateArgs", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getCreateArgs", "ti.platforms.simVayu"), fxn);
                fxn = Global.get(cap, "getExeContext");
                if (fxn != null) po.addFxn("getExeContext", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getExeContext", "ti.platforms.simVayu"), fxn);
                fxn = Global.get(cap, "getExecCmd");
                if (fxn != null) po.addFxn("getExecCmd", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getExecCmd", "ti.platforms.simVayu"), fxn);
                fxn = Global.get(cap, "getLinkTemplate");
                if (fxn != null) po.addFxn("getLinkTemplate", (Proto.Fxn)om.findStrict("xdc.platform.IPlatform$$getLinkTemplate", "ti.platforms.simVayu"), fxn);
    }

    void Platform$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.platforms.simVayu.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.platforms.simVayu"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.platforms.simVayu", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.platforms.simVayu");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.platforms.simVayu.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.catalog.arp32", Global.newArray()));
        imports.add(Global.newArray("ti.catalog.c6000", Global.newArray()));
        imports.add(Global.newArray("ti.catalog.arm.cortexm4", Global.newArray()));
        imports.add(Global.newArray("ti.catalog.arm.cortexa15", Global.newArray()));
        imports.add(Global.newArray("xdc.platform", Global.newArray(1, 0, 1)));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.platforms.simVayu'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Platform$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.platforms.simVayu.Platform", "ti.platforms.simVayu");
        po = (Proto.Obj)om.findStrict("ti.platforms.simVayu.Platform.Module", "ti.platforms.simVayu");
        vo.init2(po, "ti.platforms.simVayu.Platform", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.platforms.simVayu.Platform$$capsule", "ti.platforms.simVayu"));
        vo.bind("Instance", om.findStrict("ti.platforms.simVayu.Platform.Instance", "ti.platforms.simVayu"));
        vo.bind("Params", om.findStrict("ti.platforms.simVayu.Platform.Params", "ti.platforms.simVayu"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.platforms.simVayu.Platform.Params", "ti.platforms.simVayu")).newInstance());
        vo.bind("$package", om.findStrict("ti.platforms.simVayu", "ti.platforms.simVayu"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Board", om.findStrict("xdc.platform.IPlatform.Board", "ti.platforms.simVayu"));
        tdefs.add(om.findStrict("xdc.platform.IPlatform.Board", "ti.platforms.simVayu"));
        vo.bind("Memory", om.findStrict("xdc.platform.IPlatform.Memory", "ti.platforms.simVayu"));
        tdefs.add(om.findStrict("xdc.platform.IPlatform.Memory", "ti.platforms.simVayu"));
        vo.bind("MemoryMap", om.findStrict("xdc.platform.IPlatform.MemoryMap", "ti.platforms.simVayu"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.platform");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.platforms.simVayu")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.platforms.simVayu.Platform$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.platforms.simVayu.Platform.Object", "ti.platforms.simVayu"));
        pkgV.bind("Platform", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Platform");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.platforms.simVayu.Platform", "ti.platforms.simVayu"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.platforms.simVayu.Platform")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.platforms.simVayu")).add(pkgV);
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
        Platform$$OBJECTS();
        Platform$$CONSTS();
        Platform$$CREATES();
        Platform$$FUNCTIONS();
        Platform$$SIZES();
        Platform$$TYPES();
        if (isROV) {
            Platform$$ROV();
        }//isROV
        $$SINGLETONS();
        Platform$$SINGLETONS();
        $$INITIALIZATION();
    }
}