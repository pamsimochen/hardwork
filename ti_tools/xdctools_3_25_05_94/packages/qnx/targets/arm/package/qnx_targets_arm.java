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

public class qnx_targets_arm
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
        Global.callFxn("loadPackage", xdcO, "xdc.bld");
        Global.callFxn("loadPackage", xdcO, "gnu.targets");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("qnx.targets.arm.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("qnx.targets.arm", new Value.Obj("qnx.targets.arm", pkgP));
    }

    void Qnx$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("qnx.targets.arm.Qnx.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("qnx.targets.arm.Qnx", new Value.Obj("qnx.targets.arm.Qnx", po));
        pkgV.bind("Qnx", vo);
        // decls 
        om.bind("qnx.targets.arm.Qnx.Model", om.findStrict("xdc.bld.ITarget.Model", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.Extension", om.findStrict("xdc.bld.ITarget.Extension", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.Command", om.findStrict("xdc.bld.ITarget2.Command", "qnx.targets.arm"));
        om.bind("qnx.targets.arm.Qnx.Options", om.findStrict("xdc.bld.ITarget2.Options", "qnx.targets.arm"));
    }

    void Qnx$$CONSTS()
    {
        // module Qnx
    }

    void Qnx$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Qnx$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Qnx$$SIZES()
    {
    }

    void Qnx$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "qnx/targets/arm/Qnx.xs");
        om.bind("qnx.targets.arm.Qnx$$capsule", cap);
        po = (Proto.Obj)om.findStrict("qnx.targets.arm.Qnx.Module", "qnx.targets.arm");
        po.init("qnx.targets.arm.Qnx.Module", om.findStrict("xdc.bld.ITarget2.Module", "qnx.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "Qnx", "rh");
        po.addFld("os", $$T_Str, "Qnx", "rh");
        po.addFld("suffix", $$T_Str, "qv7", "rh");
        po.addFld("isa", $$T_Str, "v7A", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "qnx.targets.arm"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "qnx.targets.arm.rts", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.arm", "wh");
        po.addFld("QNX_HOST", $$T_Str, "", "wh");
        po.addFld("QNX_TARGET", $$T_Str, "", "wh");
        po.addFld("QNX_CONFIGURATION", $$T_Str, "", "wh");
        po.addFld("GCCVERS", $$T_Str, "", "wh");
        po.addFld("stdInclude", $$T_Str, "qnx/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "qnx.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/qcc -Vgcc_ntoarmv7 -EL -c", "opts", ""), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "qnx.targets.arm"), Global.newObject("prefix", "", "suffix", "-Dfar= "), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "qnx.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/qcc -c -x assembler", "opts", ""), "rh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "qnx.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/ntoarm-ar ", "opts", "cr"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "qnx.targets.arm"), Global.newObject("cmd", "$(rootDir)/bin/qcc -Vgcc_ntoarmv7 -EL ", "opts", ""), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "qnx.targets.arm"), Global.newObject("prefix", "", "suffix", ""), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "qnx.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 4L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 4L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 4L), "t_LLong", Global.newObject("size", 8L, "align", 4L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("qnx.targets.arm.Qnx$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("qnx.targets.arm.Qnx$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("qnx.targets.arm.Qnx$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "qnx.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "qnx.targets.arm"), fxn);
    }

    void Qnx$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("qnx.targets.arm.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "qnx.targets.arm"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "qnx.targets.arm", Value.DEFAULT, false);
        pkgV.bind("$name", "qnx.targets.arm");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "qnx.targets.arm.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.bld", Global.newArray()));
        imports.add(Global.newArray("gnu.targets", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['qnx.targets.arm'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Qnx$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("qnx.targets.arm.Qnx", "qnx.targets.arm");
        po = (Proto.Obj)om.findStrict("qnx.targets.arm.Qnx.Module", "qnx.targets.arm");
        vo.init2(po, "qnx.targets.arm.Qnx", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("qnx.targets.arm.Qnx$$capsule", "qnx.targets.arm"));
        vo.bind("$package", om.findStrict("qnx.targets.arm", "qnx.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "qnx.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "qnx.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "qnx.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "qnx.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "qnx.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "qnx.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "qnx.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "qnx.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "qnx.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "qnx.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "qnx.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "qnx.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "qnx.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "qnx.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "qnx.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "qnx.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("qnx.targets.arm.Qnx$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("Qnx", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Qnx");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("qnx.targets.arm.Qnx", "qnx.targets.arm"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("qnx.targets.arm.Qnx")).bless();
        ((Value.Arr)om.findStrict("$packages", "qnx.targets.arm")).add(pkgV);
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
        Qnx$$OBJECTS();
        Qnx$$CONSTS();
        Qnx$$CREATES();
        Qnx$$FUNCTIONS();
        Qnx$$SIZES();
        Qnx$$TYPES();
        if (isROV) {
            Qnx$$ROV();
        }//isROV
        $$SINGLETONS();
        Qnx$$SINGLETONS();
        $$INITIALIZATION();
    }
}
