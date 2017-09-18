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

public class ti_bios_support
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
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.bios.support.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.bios.support", new Value.Obj("ti.bios.support", pkgP));
    }

    void Lck$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.support.Lck.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.support.Lck", new Value.Obj("ti.bios.support.Lck", po));
        pkgV.bind("Lck", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.support.Lck$$Instance_State", new Proto.Obj());
        om.bind("ti.bios.support.Lck.Instance_State", new Proto.Str(spo, false));
        // insts 
        Object insP = om.bind("ti.bios.support.Lck.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.support.Lck$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.support.Lck.Object", new Proto.Str(po, false));
        po = (Proto.Obj)om.bind("ti.bios.support.Lck$$Params", new Proto.Obj());
        om.bind("ti.bios.support.Lck.Params", new Proto.Str(po, false));
        om.bind("ti.bios.support.Lck.Handle", insP);
        if (isROV) {
            om.bind("ti.bios.support.Lck.Object", om.findStrict("ti.bios.support.Lck.Instance_State", "ti.bios.support"));
        }//isROV
    }

    void Sys$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.support.Sys.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.support.Sys", new Value.Obj("ti.bios.support.Sys", po));
        pkgV.bind("Sys", vo);
        // decls 
    }

    void Lck$$CONSTS()
    {
        // module Lck
    }

    void Sys$$CONSTS()
    {
        // module Sys
        om.bind("ti.bios.support.Sys.error", new Extern("ti_bios_support_Sys_error__E", "xdc_Void(*)(xdc_String,xdc_Int,xdc_VaList)", true, false));
        om.bind("ti.bios.support.Sys.abort", new Extern("ti_bios_support_Sys_abort__E", "xdc_Void(*)(xdc_String,xdc_VaList)", true, false));
        om.bind("ti.bios.support.Sys.exit", new Extern("ti_bios_support_Sys_exit__E", "xdc_Void(*)(xdc_Int)", true, false));
    }

    void Lck$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        if (isCFG) {
            sb = new StringBuilder();
            sb.append("ti$bios$support$Lck$$__initObject = function( inst ) {\n");
                sb.append("if (!this.$used) {\n");
                    sb.append("throw new Error(\"Function ti.bios.support.Lck.create() called before xdc.useModule('ti.bios.support.Lck')\");\n");
                sb.append("}\n");
                sb.append("var name = xdc.module('xdc.runtime.Text').defineRopeCord(inst.instance.name);\n");
                sb.append("inst.$object.$$bind('__name', name);\n");
                sb.append("this.instance$static$init.$fxn.apply(inst, [inst.$object, inst, inst.$module]);\n");
                sb.append("inst.$seal();\n");
            sb.append("};\n");
            Global.eval(sb.toString());
            fxn = (Proto.Fxn)om.bind("ti.bios.support.Lck$$create", new Proto.Fxn(om.findStrict("ti.bios.support.Lck.Module", "ti.bios.support"), om.findStrict("ti.bios.support.Lck.Instance", "ti.bios.support"), 1, 0, false));
                        fxn.addArg(0, "__params", (Proto)om.findStrict("ti.bios.support.Lck.Params", "ti.bios.support"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$bios$support$Lck$$create = function( __params ) {\n");
                sb.append("var __mod = xdc.om['ti.bios.support.Lck'];\n");
                sb.append("var __inst = xdc.om['ti.bios.support.Lck.Instance'].$$make();\n");
                sb.append("__inst.$$bind('$package', xdc.om['ti.bios.support']);\n");
                sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
                sb.append("__inst.$$bind('$category', 'Instance');\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$instances.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', new xdc.om['ti.bios.support.Lck'].Instance_State);\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("var save = xdc.om.$curpkg;\n");
                sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
                sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
                sb.append("xdc.om.$$bind('$curpkg', save);\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.bios.support.Lck'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return __inst;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
        if (isCFG) {
            fxn = (Proto.Fxn)om.bind("ti.bios.support.Lck$$construct", new Proto.Fxn(om.findStrict("ti.bios.support.Lck.Module", "ti.bios.support"), null, 2, 0, false));
                        fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.support.Lck$$Object", "ti.bios.support"), null);
                        fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.support.Lck.Params", "ti.bios.support"), Global.newObject());
            sb = new StringBuilder();
            sb.append("ti$bios$support$Lck$$construct = function( __obj, __params ) {\n");
                sb.append("var __mod = xdc.om['ti.bios.support.Lck'];\n");
                sb.append("var __inst = __obj;\n");
                sb.append("__inst.$$bind('$args', {});\n");
                sb.append("__inst.$$bind('$module', __mod);\n");
                sb.append("__mod.$objects.$add(__inst);\n");
                sb.append("__inst.$$bind('$object', xdc.om['ti.bios.support.Lck'].Instance_State.$$make(__inst.$$parent, __inst.$name));\n");
                sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
                sb.append("__inst.$$bless();\n");
                sb.append("if (xdc.om.$$phase >= 5) xdc.om['ti.bios.support.Lck'].__initObject(__inst);\n");
                sb.append("__inst.$$bind('$$phase', xdc.om.$$phase);\n");
                sb.append("return null;\n");
            sb.append("}\n");
            Global.eval(sb.toString());
        }//isCFG
    }

    void Sys$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Lck$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Sys$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Lck$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.support.Lck.Instance_State", "ti.bios.support");
        sizes.clear();
        sizes.add(Global.newArray("value", "TInt"));
        sizes.add(Global.newArray("owner", "UPtr"));
        sizes.add(Global.newArray("sem", "Sti.sysbios.knl.Semaphore;Instance_State"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.support.Lck.Instance_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.support.Lck.Instance_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.support.Lck.Instance_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void Sys$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Lck$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/support/Lck.xs");
        om.bind("ti.bios.support.Lck$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.support.Lck.Module", "ti.bios.support");
        po.init("ti.bios.support.Lck.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.support"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        if (isCFG) {
                        po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.support.Lck$$create", "ti.bios.support"), Global.get("ti$bios$support$Lck$$create"));
                        po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.support.Lck$$construct", "ti.bios.support"), Global.get("ti$bios$support$Lck$$construct"));
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.support.Lck$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.support.Lck$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.support.Lck$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.bios.support.Lck$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.support.Lck$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$static$init");
        if (fxn != null) om.bind("ti.bios.support.Lck$$instance$static$init", true);
        if (fxn != null) po.addFxn("instance$static$init", $$T_Met, fxn);
        po = (Proto.Obj)om.findStrict("ti.bios.support.Lck.Instance", "ti.bios.support");
        po.init("ti.bios.support.Lck.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.bios.support"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.bios.support.Lck$$Params", "ti.bios.support");
        po.init("ti.bios.support.Lck.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("instance", (Proto)om.findStrict("xdc.runtime.IInstance.Params", "ti.bios.support"), $$UNDEF, "w");
        }//isCFG
        po = (Proto.Obj)om.findStrict("ti.bios.support.Lck$$Object", "ti.bios.support");
        po.init("ti.bios.support.Lck.Object", om.findStrict("ti.bios.support.Lck.Instance", "ti.bios.support"));
        // struct Lck.Instance_State
        po = (Proto.Obj)om.findStrict("ti.bios.support.Lck$$Instance_State", "ti.bios.support");
        po.init("ti.bios.support.Lck.Instance_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("value", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("owner", (Proto)om.findStrict("ti.sysbios.knl.Task.Handle", "ti.bios.support"), $$UNDEF, "w");
                po.addFldV("sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.bios.support"), $$DEFAULT, "wh", $$objFldGet, $$objFldSet);
                po.addFld("Object_field_sem", (Proto)om.findStrict("ti.sysbios.knl.Semaphore.Object", "ti.bios.support"), $$DEFAULT, "w");
    }

    void Sys$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("ti.bios.support.Sys.Module", "ti.bios.support");
        po.init("ti.bios.support.Sys.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.support"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("E_error", (Proto)om.findStrict("xdc.runtime.Error$$Id", "ti.bios.support"), Global.newObject("msg", "SYS_error called: error id = 0x%x"), "w");
            po.addFld("errorFxn", new Proto.Adr("xdc_Void(*)(xdc_String,xdc_Int,xdc_VaList)", "PFv"), null, "w");
            po.addFld("abortFxn", new Proto.Adr("xdc_Void(*)(xdc_String,xdc_VaList)", "PFv"), null, "w");
            po.addFld("exitFxn", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"), null, "w");
        }//isCFG
        // typedef Sys.ErrorFxn
        om.bind("ti.bios.support.Sys.ErrorFxn", new Proto.Adr("xdc_Void(*)(xdc_String,xdc_Int,xdc_VaList)", "PFv"));
        // typedef Sys.AbortFxn
        om.bind("ti.bios.support.Sys.AbortFxn", new Proto.Adr("xdc_Void(*)(xdc_String,xdc_VaList)", "PFv"));
        // typedef Sys.ExitFxn
        om.bind("ti.bios.support.Sys.ExitFxn", new Proto.Adr("xdc_Void(*)(xdc_Int)", "PFv"));
    }

    void Lck$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.support.Lck", "ti.bios.support");
        vo.bind("Instance_State$fetchDesc", Global.newObject("type", "ti.bios.support.Lck.Instance_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.support.Lck$$Instance_State", "ti.bios.support");
    }

    void Sys$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.support.Sys", "ti.bios.support");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.bios.support.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.bios.support"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/support/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.bios.support"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.bios.support"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.bios.support"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.bios.support"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.bios.support"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.bios.support"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.bios.support", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.bios.support");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.bios.support.");
        pkgV.bind("$vers", Global.newArray("2, 0, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.bios.support'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/release/ti.bios.support.a28L',\n");
            sb.append("'lib/release/ti.bios.support.a28FP',\n");
            sb.append("'lib/release/ti.bios.support.a64P',\n");
            sb.append("'lib/release/ti.bios.support.a64Pe',\n");
            sb.append("'lib/release/ti.bios.support.a674',\n");
            sb.append("'lib/release/ti.bios.support.ae64P',\n");
            sb.append("'lib/release/ti.bios.support.ae64Pe',\n");
            sb.append("'lib/release/ti.bios.support.ae674',\n");
            sb.append("'lib/release/ti.bios.support.ae64T',\n");
            sb.append("'lib/release/ti.bios.support.ae66',\n");
            sb.append("'lib/release/ti.bios.support.ae66e',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/release/ti.bios.support.a28L', {target: 'ti.targets.C28_large', suffix: '28L'}],\n");
            sb.append("['lib/release/ti.bios.support.a28FP', {target: 'ti.targets.C28_float', suffix: '28FP'}],\n");
            sb.append("['lib/release/ti.bios.support.a64P', {target: 'ti.targets.C64P', suffix: '64P'}],\n");
            sb.append("['lib/release/ti.bios.support.a64Pe', {target: 'ti.targets.C64P_big_endian', suffix: '64Pe'}],\n");
            sb.append("['lib/release/ti.bios.support.a674', {target: 'ti.targets.C674', suffix: '674'}],\n");
            sb.append("['lib/release/ti.bios.support.ae64P', {target: 'ti.targets.elf.C64P', suffix: 'e64P'}],\n");
            sb.append("['lib/release/ti.bios.support.ae64Pe', {target: 'ti.targets.elf.C64P_big_endian', suffix: 'e64Pe'}],\n");
            sb.append("['lib/release/ti.bios.support.ae674', {target: 'ti.targets.elf.C674', suffix: 'e674'}],\n");
            sb.append("['lib/release/ti.bios.support.ae64T', {target: 'ti.targets.elf.C64T', suffix: 'e64T'}],\n");
            sb.append("['lib/release/ti.bios.support.ae66', {target: 'ti.targets.elf.C66', suffix: 'e66'}],\n");
            sb.append("['lib/release/ti.bios.support.ae66e', {target: 'ti.targets.elf.C66_big_endian', suffix: 'e66e'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Lck$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.support.Lck", "ti.bios.support");
        po = (Proto.Obj)om.findStrict("ti.bios.support.Lck.Module", "ti.bios.support");
        vo.init2(po, "ti.bios.support.Lck", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.support.Lck$$capsule", "ti.bios.support"));
        vo.bind("Instance", om.findStrict("ti.bios.support.Lck.Instance", "ti.bios.support"));
        vo.bind("Params", om.findStrict("ti.bios.support.Lck.Params", "ti.bios.support"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.support.Lck.Params", "ti.bios.support")).newInstance());
        vo.bind("Handle", om.findStrict("ti.bios.support.Lck.Handle", "ti.bios.support"));
        vo.bind("$package", om.findStrict("ti.bios.support", "ti.bios.support"));
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
        vo.bind("Instance_State", om.findStrict("ti.bios.support.Lck.Instance_State", "ti.bios.support"));
        tdefs.add(om.findStrict("ti.bios.support.Lck.Instance_State", "ti.bios.support"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.support")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", om.has("ti.bios.support.Lck$$instance$static$init", null) ? 1 : 0);
            vo.bind("__initObject", Global.get("ti$bios$support$Lck$$__initObject"));
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_support_Lck_Handle__label__E", "ti_bios_support_Lck_Module__startupDone__E", "ti_bios_support_Lck_Object__create__E", "ti_bios_support_Lck_Object__delete__E", "ti_bios_support_Lck_Object__destruct__E", "ti_bios_support_Lck_Object__get__E", "ti_bios_support_Lck_Object__first__E", "ti_bios_support_Lck_Object__next__E", "ti_bios_support_Lck_Params__init__E", "ti_bios_support_Lck_pend__E", "ti_bios_support_Lck_post__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.support.Lck.Object", "ti.bios.support"));
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "pend", "entry", "%p, 0x%x", "exit", "%d"));
        loggables.add(Global.newObject("name", "post", "entry", "%p", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Lck", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Lck");
    }

    void Sys$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.support.Sys", "ti.bios.support");
        po = (Proto.Obj)om.findStrict("ti.bios.support.Sys.Module", "ti.bios.support");
        vo.init2(po, "ti.bios.support.Sys", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("ti.bios.support", "ti.bios.support"));
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
        mcfgs.add("E_error");
        vo.bind("ErrorFxn", om.findStrict("ti.bios.support.Sys.ErrorFxn", "ti.bios.support"));
        vo.bind("AbortFxn", om.findStrict("ti.bios.support.Sys.AbortFxn", "ti.bios.support"));
        vo.bind("ExitFxn", om.findStrict("ti.bios.support.Sys.ExitFxn", "ti.bios.support"));
        mcfgs.add("errorFxn");
        mcfgs.add("abortFxn");
        mcfgs.add("exitFxn");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.support")).add(vo);
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
        vo.bind("error", om.findStrict("ti.bios.support.Sys.error", "ti.bios.support"));
        vo.bind("abort", om.findStrict("ti.bios.support.Sys.abort", "ti.bios.support"));
        vo.bind("exit", om.findStrict("ti.bios.support.Sys.exit", "ti.bios.support"));
        vo.bind("$$fxntab", Global.newArray("ti_bios_support_Sys_Module__startupDone__E", "ti_bios_support_Sys_error__E", "ti_bios_support_Sys_abort__E", "ti_bios_support_Sys_exit__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray("E_error"));
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        loggables.add(Global.newObject("name", "error", "entry", "\"%s\", %d, 0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "abort", "entry", "\"%s\", 0x%x", "exit", ""));
        loggables.add(Global.newObject("name", "exit", "entry", "%d", "exit", ""));
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Sys", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Sys");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
            Object srcP = ((XScriptO)om.findStrict("xdc.runtime.IInstance", "ti.bios.support")).findStrict("PARAMS", "ti.bios.support");
            Scriptable dstP;

            dstP = (Scriptable)((XScriptO)om.findStrict("ti.bios.support.Lck", "ti.bios.support")).findStrict("PARAMS", "ti.bios.support");
            Global.put(dstP, "instance", srcP);
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.support.Lck", "ti.bios.support"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.support.Sys", "ti.bios.support"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.bios.support.Lck")).bless();
        ((Value.Obj)om.getv("ti.bios.support.Sys")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.bios.support")).add(pkgV);
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
        Lck$$OBJECTS();
        Sys$$OBJECTS();
        Lck$$CONSTS();
        Sys$$CONSTS();
        Lck$$CREATES();
        Sys$$CREATES();
        Lck$$FUNCTIONS();
        Sys$$FUNCTIONS();
        Lck$$SIZES();
        Sys$$SIZES();
        Lck$$TYPES();
        Sys$$TYPES();
        if (isROV) {
            Lck$$ROV();
            Sys$$ROV();
        }//isROV
        $$SINGLETONS();
        Lck$$SINGLETONS();
        Sys$$SINGLETONS();
        $$INITIALIZATION();
    }
}
